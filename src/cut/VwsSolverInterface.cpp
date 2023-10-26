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
#include "CbcModel.hpp"
#include "CbcSolver.hpp" // cbcMain0
#include "CbcSolverHeuristics.hpp" // doHeuristics
#include "CbcStrategy.hpp" // CbcStrategy
#include "CglPreProcess.hpp" // CglPreProcess
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface
#include <OsiCuts.hpp> // OsiCuts

// Eigen library
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>

// vpc modules
#include "CglVPC.hpp" // CglVPC
#include "PartialBBDisjunction.hpp" // PartialBBDisjunction
#include "SolverInterface.hpp" // SolverInterface
#include "VPCParameters.hpp" // VPCParameters
#include "utility.hpp"

// project modules
#include "CglStoredVpc.hpp" // CglStoredVpc
#include "VwsEventHandler.hpp" // VwsEventHandler
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp" // getVariableNames, getConstraintNames, putBackSolutions, findNonZero


/** constructor */
VwsSolverInterface::VwsSolverInterface(int maxExtraSavedSolutions, double maxRunTime,
                                       int disjunctiveTerms, double vpcGenTimeRatio):
  maxExtraSavedSolutions(maxExtraSavedSolutions),
  maxRunTime(maxRunTime),
  disjunctiveTerms(disjunctiveTerms),
  vpcGenTimeRatio(vpcGenTimeRatio)
  {
} /* constructor */

/** Solve a MIP with VPCs added. Provides an eventHandler that just gets tossed */
CbcModel VwsSolverInterface::solve(const OsiClpSolverInterface& instanceSolver,
                                   const std::string vpcGenerator){
  VwsEventHandler h = VwsEventHandler();
  return solve(instanceSolver, vpcGenerator, h);
} /* solve */

/** Solve a MIP with VPCs added and add the provided eventHandler to the solve */
CbcModel VwsSolverInterface::solve(const OsiClpSolverInterface& instanceSolver,
                                   const std::string vpcGenerator, VwsEventHandler& eventHandler){

  // todo: enforce that we have a MIP with same number of variables and constraints
  // todo: enforce that we have a MIP with expected constraint format
  // todo: create a parameters file for vws that can set up parameters for VPC and CBC

  // start the timers
  for (auto name : eventHandler.timer_names){
    eventHandler.timer.register_name(name);
    eventHandler.timer.start_timer(name);
  }

  // set up the solver - make sure we minimize and that we have a solution
  OsiClpSolverInterface * si = dynamic_cast<OsiClpSolverInterface*>(instanceSolver.clone());
  ensureMinimizationObjective(si);

  // instanceSolver passed by reference since names aren't carried over when CbcModel constructor copies it
  variableNames.push_back(getVariableNames(instanceSolver));
  constraintNames.push_back(getConstraintNames(instanceSolver));

  // create cuts - either by PRLP or by previous farkas multipliers
  std::shared_ptr<OsiCuts> disjCuts;
  if (vpcGenerator == "New Disjunction") {
    disjCuts = createVpcsFromNewDisjunctionPRLP(si, eventHandler);
  } else if (vpcGenerator == "Old Disjunction") {
    disjCuts = createVpcsFromOldDisjunctionPRLP(si, eventHandler);
  } else if (vpcGenerator == "Farkas") {
    disjCuts = createVpcsFromFarkasMultipliers(si, eventHandler);
  } else if (vpcGenerator == "None") {
    si->initialSolve();
    eventHandler.data.disjunctiveDualBound = si->getObjValue();
  } else {
    verify(false, "vpcGenerator must be one of New Disjunction, Old Disjunction, Farkas, or None");
  }

  // update the event handler from cut creation
  eventHandler.timer.end_timer("vpcGenerationTime");
  eventHandler.data.vpcGenerationTime = eventHandler.timer.get_time("vpcGenerationTime");
  eventHandler.cuts = disjCuts.get();

  // instantiate the MIP model
  CbcModel model(*si);
  model.passInEventHandler(&eventHandler);

  // turn on heuristics
  CbcSolverUsefulData cbcData;
  CbcMain0(model, cbcData);
  cbcData[CbcParam::DIVEOPT]->setVal(2);
  cbcData[CbcParam::FPUMPITS]->setVal(30);
  cbcData[CbcParam::FPUMPTUNE]->setVal(1005043);
  cbcData[CbcParam::DIVINGC]->setVal("on");
  cbcData[CbcParam::RINS]->setVal("on");
  doHeuristics(&model, 1, cbcData, cbcData.noPrinting(), 1005043);

  // turn on strong branching and cut generation
  CbcStrategyDefault strategy(false, 5, 0);
  model.setStrategy(strategy);
  // model.setLogLevel(2);

  // set miscellaneous parameters
  model.setMaximumSavedSolutions(maxExtraSavedSolutions);
  model.setDblParam(CbcModel::CbcMaximumSeconds,
                     maxRunTime - eventHandler.timer.get_time("time"));
//  model.setLogLevel(0);

  // add the cuts
  std::shared_ptr<CglStoredVpc> store = std::make_shared<CglStoredVpc>();
  if (eventHandler.cuts) {
    for (int i = 0; i < eventHandler.cuts->sizeRowCuts(); i++) {
      store->addCut(eventHandler.cuts->rowCut(i));
    }
    model.addCutGenerator(store.get(), 1, "StoredVPCs");
  }

  // run the solver
  model.branchAndBound();

  // save the solutions in memory
  std::vector< std::vector <double>> problemSolutions;
  for (int j = 0; j < model.numberSavedSolutions(); j++) {
    std::vector<double> solution(model.savedSolution(j),
                                 model.savedSolution(j) + model.getNumCols());
    problemSolutions.push_back(solution);
  }
  solutions.push_back(problemSolutions);

  // finish filling out the eventHandler
  eventHandler = *dynamic_cast<VwsEventHandler*>(model.getEventHandler());
  eventHandler.data.vpcGenerator = vpcGenerator;
  eventHandler.data.dualBound = model.getBestPossibleObjValue();
  eventHandler.data.maxTime = maxRunTime;
  eventHandler.data.vpcGenerator = vpcGenerator;
  eventHandler.data.terms = disjunctiveTerms;

  // return the model for any further analysis by user
  return model;
} /* solve */

/** Creates cuts from a PRLP relaxation of the disjunctive terms found from
 *  partially solving the given problem encoded in the solver interface.
 *  Simplified from Strengthening's CglAdvCut::generateCuts */
std::shared_ptr<OsiCuts> VwsSolverInterface::createVpcsFromNewDisjunctionPRLP(
    OsiClpSolverInterface * si, VwsEventHandler& eventHandler){

  si->initialSolve();
  verify(si->isProvenOptimal(), "Solver must be optimal to create disjunctive cuts");
  std::shared_ptr<OsiCuts> disjCuts = std::make_shared<OsiCuts>();

  //==================================================//
  // Set up VPC generation
  VPCParametersNamespace::VPCParameters vpc_params;
  vpc_params.set(VPCParametersNamespace::DISJ_TERMS, disjunctiveTerms);
  vpc_params.set(VPCParametersNamespace::TIMELIMIT, maxRunTime);
  vpc_params.set(VPCParametersNamespace::PARTIAL_BB_TIMELIMIT, maxRunTime);
  vpc_params.set(VPCParametersNamespace::PARTIAL_BB_KEEP_PRUNED_NODES, 1);
  vpc_params.set(VPCParametersNamespace::MODE, 0);

  // create cuts
  CglVPC gen = CglVPC(vpc_params);
  gen.generateCuts(*si, *disjCuts);

  // get the disjunction for later
  std::shared_ptr<PartialBBDisjunction> disj =
      std::make_shared<PartialBBDisjunction>(*dynamic_cast<PartialBBDisjunction*>(gen.disj()));

  // record the disjunctive lower bound
  eventHandler.data.disjunctiveDualBound = disj->best_obj;

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
    OsiClpSolverInterface * si, VwsEventHandler& eventHandler) {

  verify(cutCertificates.size() > 0, "No certificates to create disjunctive cuts");

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
    eventHandler.data.disjunctiveDualBound = probIdx == 0 ? param_disj.best_obj :
        max(param_disj.best_obj, eventHandler.data.disjunctiveDualBound);
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
    OsiClpSolverInterface * si, VwsEventHandler& eventHandler){

  // make sure we have at least one disjunction already existing
  verify(disjunctions.size() > 0, "We need previous disjunctions for this method");

  // CglVPC expects si to be optimal
  si->initialSolve();
  verify(si->isProvenOptimal(), "Solver must be optimal to create disjunctive cuts");

  // create a container for the cuts
  std::shared_ptr<OsiCuts> disjCuts = std::make_shared<OsiCuts>();

  // Set VPC parameters
  VPCParametersNamespace::VPCParameters vpc_params;
  vpc_params.set(VPCParametersNamespace::DISJ_TERMS, disjunctiveTerms);
  vpc_params.set(VPCParametersNamespace::TIMELIMIT, maxRunTime);
  vpc_params.set(VPCParametersNamespace::RECYCLED_DISJUNCTION, 1);

  // create vpcs for each disjunction
  for (const auto& disj : disjunctions){

    // parameterize the disjunction
    PartialBBDisjunction param_disj = disj.get()->parameterize(si);

    // create the cut generator and generate cuts
    CglVPC gen = CglVPC(vpc_params);
    gen.setDisjunction(&param_disj);
    gen.generateCuts(*si, *disjCuts);

    // update the event handler with the disjunctive lower bound
    // each disjunction forms a dual so strongest dual should be max since we minimize
    eventHandler.data.disjunctiveDualBound = &disj == &disjunctions[0] ? param_disj.best_obj :
        max(param_disj.best_obj, eventHandler.data.disjunctiveDualBound);
  }

  return disjCuts;
}