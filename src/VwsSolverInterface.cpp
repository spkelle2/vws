/**
 * @file VwsSolverInterface.cpp
 * @author Sean Kelley
 * @date 2023-01-31
 */

// standard library
#include <iostream> // cout
#include <memory> // shared_ptr
#include <string> // to_string
#include <vector> // vector

// coin-or modules
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface

// vpc modules
#include "CbcHelper.hpp" // CBC solve functions
#include "GurobiHelper.hpp" // gurobi solve functions
#include "CglVPC.hpp" // CglVPC
#include "PartialBBDisjunction.hpp" // PartialBBDisjunction
#include "VPCParameters.hpp" // VPCParameters
#include "utility.hpp"  // max, verify, ensureMinimizationObjective

// project modules
#include "RunData.hpp" // RunData
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp" // findNonZero


/** Default constructor */
VwsSolverInterface::VwsSolverInterface(){
  mipSolver = "CBC";
} /* Default constructor */

/** constructor */
VwsSolverInterface::VwsSolverInterface(VPCParametersNamespace::VPCParameters params,
                                       std::string mipSolver):
  params(params), mipSolver(mipSolver){

  verify(mipSolver == "CBC" || mipSolver == "GUROBI", "mipSolver must be CBC or GUROBI");

} /* constructor */

/** Solve a MIP with VPCs added and add the provided eventHandler to the solve */
RunData VwsSolverInterface::solve(const OsiClpSolverInterface& instanceSolver,
                                  const std::string vpcGenerator, double primalBound){

  // todo: enforce that we have a MIP with same number of variables and constraints
  // todo: enforce that we have a MIP with single sided constraints

  // create a container to track run stats
  RunData data;

  // set up the solver - make sure we minimize and that we have a solution
  OsiClpSolverInterface * si = dynamic_cast<OsiClpSolverInterface*>(instanceSolver.clone());
  ensureMinimizationObjective(si);
  si->initialSolve();

  // get the number of fractional integer variables
  int fractional_int_vars = 0;
  for (int i=0; i < si->getNumCols(); i++){
    if (si->isInteger(i) && !isInteger(si->getColSolution()[i])){
      fractional_int_vars++;
    }
  }

  // make sure primalBound is somewhat reasonable
  verify(primalBound >= si->getObjValue(),
         "must have primalBound > root LP objective value when minimizing");

  // start the timer for cut generation
  TimeStats timer;
  timer.register_name("vpcGenerationTime");
  timer.start_timer("vpcGenerationTime");

  // create cuts - either by PRLP or by previous farkas multipliers
  std::shared_ptr<OsiCuts> disjCuts;
  if (vpcGenerator == "New") {
    disjCuts = createVpcsFromNewDisjunctionPRLP(si, data);
    data.cutLimit = params.get(VPCParametersNamespace::CUTLIMIT) > 0 ?
                    params.get(VPCParametersNamespace::CUTLIMIT) :
                    -1 * params.get(VPCParametersNamespace::CUTLIMIT) * fractional_int_vars;
  } else if (vpcGenerator == "Old") {
    disjCuts = createVpcsFromOldDisjunctionPRLP(si, data);
  } else if (vpcGenerator == "Farkas") {
    disjCuts = createVpcsFromFarkasMultipliers(si, data);
  } else if (vpcGenerator == "None") {
    data.disjunctiveDualBound = si->getObjValue();
  } else {
    verify(false, "vpcGenerator must be one of New, Old, Farkas, or None");
  }

  // stop the timer for cut generation
  timer.end_timer("vpcGenerationTime");

  // configure args for solver helpers
  BBInfo info; // container for stats that vpc tracks for us

  // call the appropriate solver helper subroutine
  if (mipSolver == "CBC") {
    if (vpcGenerator == "None") {
      doBranchAndBoundWithCbc(params, params.get(VPCParametersNamespace::BB_STRATEGY),
                              si, info, primalBound);
    } else {
      doBranchAndBoundWithUserCutsCbc(
          params, params.get(VPCParametersNamespace::BB_STRATEGY), si,
          disjCuts.get(), info, primalBound);
    }
  } else {
    if (vpcGenerator == "None") {
      doBranchAndBoundWithGurobi(params, params.get(VPCParametersNamespace::BB_STRATEGY),
                                 si, info, primalBound);
    } else {
      doBranchAndBoundWithUserCutsGurobi(
          params, params.get(VPCParametersNamespace::BB_STRATEGY), si,
          disjCuts.get(), info, primalBound);
    }
  }

  // fill in remaining primary keys
  data.vpcGenerator = vpcGenerator;
  data.terms = params.get(VPCParametersNamespace::DISJ_TERMS);

  // get remaining bound data
  data.lpBound = si->getObjValue();
  if (disjCuts){
    si->applyCuts(*disjCuts);
    si->resolve();
  }
  data.lpBoundPostVpc = si->getObjValue();
  data.rootDualBound = info.last_cut_pass;
  data.dualBound = info.bound;
  data.primalBound = info.obj;

  // get remaining time stats
  data.vpcGenerationTime = timer.get_time("vpcGenerationTime");
  data.rootDualBoundTime = info.root_time + data.vpcGenerationTime;
  data.bestSolutionTime = info.last_sol_time + data.vpcGenerationTime;
  data.terminationTime = info.time + data.vpcGenerationTime;

  // get remaining performance stats
  data.nodes = info.nodes;
  data.iterations = info.iters;

  // get remaining misc statistics
  data.maxTime = params.get(VPCParametersNamespace::TIMELIMIT);
  data.numCuts = disjCuts ? disjCuts->sizeCuts() : 0;
  data.mipSolver = mipSolver;
  data.providePrimalBound = primalBound < std::numeric_limits<double>::max();

  return data;
} /* solve */

/** Creates cuts from a PRLP relaxation of the disjunctive terms found from
 *  partially solving the given problem encoded in the solver interface.
 *  Simplified from Strengthening's CglAdvCut::generateCuts */
std::shared_ptr<OsiCuts> VwsSolverInterface::createVpcsFromNewDisjunctionPRLP(
    OsiClpSolverInterface * si, RunData& data){

  verify(si->isProvenOptimal(), "Solver must be optimal to create disjunctive cuts");
  std::shared_ptr<OsiCuts> disjCuts = std::make_shared<OsiCuts>();

  params.set(VPCParametersNamespace::RECYCLED_DISJUNCTION, 0);

  // create cuts
  CglVPC gen = CglVPC(params);
  gen.generateCuts(*si, *disjCuts);

  // get the disjunction for later
  std::shared_ptr<PartialBBDisjunction> disj =
      std::make_shared<PartialBBDisjunction>(*dynamic_cast<PartialBBDisjunction*>(gen.disj()));

  // record the disjunctive metatdata
  data.disjunctiveDualBound = disj->best_obj;
  data.actualTerms = disj->num_terms;

  // if we have cuts and a full tree, save the cut generator and the Farkas multipliers
  if (disj && disj->terms.size() > 0 && disjCuts->sizeCuts() > 0){
    disjunctions.push_back(disj);
    cutCertificates.push_back(getFarkasMultipliers(*si, *disj.get(), *disjCuts));
    return disjCuts;
  }
  else {
    std::shared_ptr<OsiCuts> emptyCuts;
    return emptyCuts;
  }
}

/** Parameterize each previously created disjunctive cut with its disjunction
 *  and Farkas multipliers applied to the given solver. Borrowed from Strengthening's
 *  main.cpp */
std::shared_ptr<OsiCuts> VwsSolverInterface::createVpcsFromFarkasMultipliers(
    OsiClpSolverInterface * si, RunData& data) {

  verify(cutCertificates.size() > 0, "No certificates to create disjunctive cuts");

  // LP relaxation will have to be feasible for any of the following to matter
  verify(si->isProvenOptimal(), "Solver must be optimal to create disjunctive cuts");

  std::shared_ptr<OsiCuts> disjCuts = std::make_shared<OsiCuts>();
  bool feasibleTermSolver;
  bool ambiguousTermSolver;

  // set up vectors of disjunctive cuts
  // [problemIdx][cutIdx][termIdx][variableIdx]
  std::vector< std::vector< std::vector< std::vector<double> > > > a;
  // [problemIdx][cutIdx][termIdx]
  std::vector< std::vector< std::vector<double> > > b;
  a.resize(cutCertificates.size());
  b.resize(cutCertificates.size());
  for (int probIdx=0; probIdx < cutCertificates.size(); probIdx++){
    a[probIdx].resize(cutCertificates[probIdx].size());
    b[probIdx].resize(cutCertificates[probIdx].size());
    PartialBBDisjunction param_disj = disjunctions[probIdx].get()->parameterize(si);
    for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++) {
      a[probIdx][cutIdx].resize(param_disj.num_terms);
      b[probIdx][cutIdx].resize(param_disj.num_terms);
    }

    // calculate a cut for each disjunctive term
    ambiguousTermSolver = false;
    feasibleTermSolver = false;
    for (int termIdx=0; termIdx < param_disj.num_terms; termIdx++) {
      OsiSolverInterface* termSolver;
      param_disj.getSolverForTerm(termSolver, termIdx, si, false, .001, NULL, true);
      // if we don't know if we're optimal or primal infeasible,
      // we can't safely use these cuts, so skip to next problem
      if (!termSolver->isProvenOptimal() && !termSolver->isProvenPrimalInfeasible()){
        ambiguousTermSolver = true;
        break;
      }
      for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++){
        a[probIdx][cutIdx][termIdx].resize(si->getNumCols());
        if (termSolver->isProvenOptimal()) {
          feasibleTermSolver = true;
          getCutFromCertificate(a[probIdx][cutIdx][termIdx], b[probIdx][cutIdx][termIdx],
                                cutCertificates[probIdx][cutIdx][termIdx], termSolver);
        } else {
          // primal infeasible - no cuts will violate this term,
          // so just make its cut to where it won't be chosen
          std::fill(a[probIdx][cutIdx][termIdx].begin(), a[probIdx][cutIdx][termIdx].end(), -1e50);
          b[probIdx][cutIdx][termIdx] = 1e50;
        }
      }
    }

    // if we are unsure the status of a solver or all terms were infeasible, move on
    if (ambiguousTermSolver || !feasibleTermSolver) {
      continue;
    }

    // create a valid disjunctive cut given all the valid term cuts
    for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++){
      std::vector<double> alpha = elementWiseMax(a[probIdx][cutIdx]);
      double beta = min(b[probIdx][cutIdx]);
      std::vector<int> indices;
      std::vector<double> elements;
      findNonZero(alpha, indices, elements);
      OsiRowCut cut;
      cut.setRow(indices.size(), indices.data(), elements.data());
      cut.setLb(beta);
      disjCuts->insert(cut);
    }

    // update the event handler with the disjunctive lower bound
    if (probIdx == 0 || data.disjunctiveDualBound < param_disj.best_obj){
      data.disjunctiveDualBound = param_disj.best_obj;
      data.actualTerms = param_disj.num_terms;
    }
  }

  return disjCuts;
}

/**
 * @details create VPCs by solving each PRLP resulting from applying each
 * disjunction in <disjunctions> to <si>
 *
 * @param si the solver interface to which we apply each disjunction
 *
 * @return the cuts generated by applying previous disjunctions to the solver
 * interface and resolving the PRLP
 */
std::shared_ptr<OsiCuts> VwsSolverInterface::createVpcsFromOldDisjunctionPRLP(
    OsiClpSolverInterface * si, RunData& data){

  // make sure we have at least one disjunction already existing
  verify(disjunctions.size() > 0, "We need previous disjunctions for this method");

  // CglVPC expects si to be optimal
  verify(si->isProvenOptimal(), "Solver must be optimal to create disjunctive cuts");

  // create a container for the cuts
  std::shared_ptr<OsiCuts> disjCuts = std::make_shared<OsiCuts>();

  // Set VPC parameters
  params.set(VPCParametersNamespace::RECYCLED_DISJUNCTION, 1);

  // create vpcs for each disjunction
  for (const auto& disj : disjunctions){

    // parameterize the disjunction
    PartialBBDisjunction param_disj = disj.get()->parameterize(si);

    // create the cut generator and generate cuts
    CglVPC gen = CglVPC(params);
    gen.setDisjunction(&param_disj);
    gen.generateCuts(*si, *disjCuts);

    // update the event handler with the disjunctive lower bound
    // each disjunction forms a dual so strongest dual should be max since we minimize
    if (&disj == &disjunctions[0] || data.disjunctiveDualBound < param_disj.best_obj){
      data.disjunctiveDualBound = param_disj.best_obj;
      data.actualTerms = param_disj.num_terms;
    }
  }

  return disjCuts;
}