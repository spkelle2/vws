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
#include "BBHelper.hpp" // BBInfo
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
RunData VwsSolverInterface::solve(
    const OsiClpSolverInterface& instanceSolver, const std::string vpcGenerator,
    double primalBound, bool tighten_disjunction, bool tighten_matrix_perturbation,
    bool tighten_infeasible_to_feasible_term, bool tighten_feasible_to_infeasible_basis){

  verify(solvers.size() == 0 || (solvers[0]->getNumCols() == instanceSolver.getNumCols() &&
                                 solvers[0]->getNumRows() == instanceSolver.getNumRows()),
         "VwsSolverInterface::solve: problem dimension must stay fixed");

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
  verify(primalBound >= si->getObjValue() - 1e-4,
         "must have primalBound >= root LP objective value when minimizing");

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
    disjCuts = createVpcsFromOldDisjunctionPRLP(si, data, tighten_disjunction);
  } else if (vpcGenerator == "None") {
    data.disjunctiveDualBound = si->getObjValue();
  } else {
    // assume we are using the Farkas multipliers
    verify(cutCertificates.size() > 0,
           "Assuming parametric cut generation but no previous certificates found");
    disjCuts = createVpcsFromFarkasMultipliers(
        si, data, tighten_disjunction, tighten_matrix_perturbation,
        tighten_infeasible_to_feasible_term, tighten_feasible_to_infeasible_basis);
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
      std::vector<std::vector<double>> pool = std::vector<std::vector<double>>();
      doBranchAndBoundWithUserCutsGurobi(
          params, params.get(VPCParametersNamespace::BB_STRATEGY), si,
          disjCuts.get(), info, primalBound, false, &pool);
      // merge solution pools
      for (const auto& sol : pool) {
        solutionPool.insert(sol);
      }
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
  data.rootDualBound = info.last_cut_pass > 1e99 ? si->getObjValue() : info.last_cut_pass;
  data.dualBound = info.bound;
  data.primalBound = min(info.obj, primalBound);

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

  // get the solver for later
  std::shared_ptr<OsiClpSolverInterface> si_copy =
      std::make_shared<OsiClpSolverInterface>(*dynamic_cast<OsiClpSolverInterface*>(si->clone()));

  params.set(VPCParametersNamespace::RECYCLED_DISJUNCTION, 0);

  // create cuts
  CglVPC gen = CglVPC(params);
  gen.generateCuts(*si, *disjCuts);

  // return if we have no disjunction
  if (!gen.disj()){
    printf("No disjunction was found. Skipping this iteration\n");
    return disjCuts;
  }

  // get the disjunction for later
  std::shared_ptr<PartialBBDisjunction> disj =
      std::make_shared<PartialBBDisjunction>(*dynamic_cast<PartialBBDisjunction*>(gen.disj()));

  // record the disjunctive metadata
  data.disjunctiveDualBound = disj->best_obj;
  data.actualTerms = disj->num_terms;
  data.infeasibleTerms = disj->num_infeasible_terms;

  // if we have cuts and no solution, save the cut generator the Farkas multipliers, and the solver
  if (disj->terms.size() > 0 && disjCuts->sizeCuts() > 0 && disj->integer_sol.size() == 0){
    disjunctions.push_back(disj);
    cutCertificates.push_back(getFarkasMultipliers(*si, *disj.get(), *disjCuts));
    solvers.push_back(si_copy);
    return disjCuts;
  }
  else {
    if (disj->integer_sol.size() > 0){
      printf("Disjunction has integer solution. Skipping this iteration since we can't handle this\n");
    }
    std::shared_ptr<OsiCuts> emptyCuts = std::make_shared<OsiCuts>();
    return emptyCuts;
  }
}

/** Parameterize each previously created disjunctive cut with its disjunction
 *  and Farkas multipliers applied to the given solver. Borrowed from Strengthening's
 *  main.cpp */
std::shared_ptr<OsiCuts> VwsSolverInterface::createVpcsFromFarkasMultipliers(
    OsiClpSolverInterface * si, RunData& data, bool tighten_disjunction,
    bool tighten_matrix_perturbation, bool tighten_infeasible_to_feasible_term,
    bool tighten_feasible_to_infeasible_basis){

  verify(cutCertificates.size() > 0, "No certificates to create disjunctive cuts");

  // LP relaxation will have to be feasible for any of the following to matter
  verify(si->isProvenOptimal(), "Solver must be optimal to create disjunctive cuts");

  std::shared_ptr<OsiCuts> disjCuts = std::make_shared<OsiCuts>();
  bool unprunedTermSolver;
  
  // get a bound on best known solution if we are tightening
  double empiricalBound = tighten_disjunction ? findPrimalBound(si, solutionPool) :
      std::numeric_limits<double>::max();

  // record parameters
  data.tighten_disjunction = tighten_disjunction;
  data.tighten_matrix_perturbation = tighten_matrix_perturbation;
  data.tighten_feasible_to_infeasible_basis = tighten_feasible_to_infeasible_basis;
  data.tighten_infeasible_to_feasible_term = tighten_infeasible_to_feasible_term;

  // set up vectors of disjunctive cuts
  // [problemIdx][cutIdx][termIdx][variableIdx]
  std::vector< std::vector< std::vector< std::vector<double> > > > a;
  // [problemIdx][cutIdx][termIdx]
  std::vector< std::vector< std::vector<double> > > b;
  a.resize(cutCertificates.size());
  b.resize(cutCertificates.size());
  for (int probIdx=0; probIdx < cutCertificates.size(); probIdx++){

    // get the original solver parameterized for each term of this problem
    std::vector<std::unique_ptr<OsiSolverInterface>> term_solvers;
    PartialBBDisjunction param_disj = disjunctions[probIdx].get()->parameterize(si, &term_solvers);

    // set up containers for filters we'll use throughout
    // [termIdx] if an originally infeasible term becomes feasible
    std::vector<bool> infeasible_term_becomes_feasible;
    // [termIdx] if an initially feasible term remains feasible but initial basis becomes infeasible
    std::vector<bool> term_remains_feasible_basis_infeasible;
    // [termIdx] if the original basis remains feasible for this term after parameterization
    std::vector<bool> original_basis_feasible;
    // [cutIdx][termIdx] if cut coefficients will change with parameterization
    std::vector<std::vector<bool>> cuts_changed_coefficients(
        cutCertificates[probIdx].size(), std::vector<bool>(param_disj.num_terms, false));
    // [termIdx] if we keep termIdx instead of pruning it
    std::vector<bool> keep;


    // fill out these containers and do data tracking
    for (int termIdx=0; termIdx < param_disj.num_terms; termIdx++) {

      // get pruning information
      // check if we prune this term - infeasible or won't improve primal bound
      keep.push_back(param_disj.terms[termIdx].is_feasible &&
                     !lessThanVal(empiricalBound, param_disj.terms[termIdx].obj));
      // track terms that were feasible but just pruned by bound
      if (param_disj.terms[termIdx].is_feasible && !keep[termIdx]){
        data.feasibleTermsPrunedByBound++;
      }

      // check if we changed feasibility states for this term by perturbing
      infeasible_term_becomes_feasible.push_back(
          !disjunctions[probIdx].get()->terms[termIdx].is_feasible &&
          param_disj.terms[termIdx].is_feasible);
      if (infeasible_term_becomes_feasible[termIdx] && keep[termIdx]){
        data.infeasibleToFeasibleTerms++;  // only worried about terms remaining after pruning
      }
      if (!param_disj.terms[termIdx].is_feasible){
        data.infeasibleTerms++;
        if (disjunctions[probIdx].get()->terms[termIdx].is_feasible){
          data.feasibleToInfeasibleTerms++;
        }
      }

      // check if the original basis is feasible for the new solver
      OsiSolverInterface* termSolver = term_solvers[termIdx].get();
      const CoinWarmStartBasis* basis_extended =
          dynamic_cast<const CoinWarmStartBasis*>(disjunctions[probIdx]->terms[termIdx].basis_extended);
      bool basis_extended_feasible = isFeasible(termSolver, basis_extended);
      original_basis_feasible.push_back(basis_extended_feasible);
      term_remains_feasible_basis_infeasible.push_back(
          disjunctions[probIdx]->terms[termIdx].is_feasible &&
          param_disj.terms[termIdx].is_feasible && !basis_extended_feasible);
      if (term_remains_feasible_basis_infeasible[termIdx] && keep[termIdx]) {
        data.termRemainsFeasibleBasisInfeasible++;  // only worried about terms remaining after pruning
      }

      // check if term used and coefficients in constraints to be combined for parameterization changed
      for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++){
        cuts_changed_coefficients[cutIdx][termIdx] =
            !sameCoefficientMatrix(solvers[probIdx].get(), si,
                                   &cutCertificates[probIdx][cutIdx][termIdx]);
      }
    }

    // track the number of parametric disjunctive cuts that had their coefficients changed
    for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++){
      if (anyBothTrue(cuts_changed_coefficients[cutIdx], keep)){
        data.cutsChangedCoefficients++;  // only worried about terms remaining after pruning
      }
    }

    // set up cut coefficient storage
    a[probIdx].resize(cutCertificates[probIdx].size());
    b[probIdx].resize(cutCertificates[probIdx].size());
    for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++) {
      a[probIdx][cutIdx].resize(param_disj.num_terms);
      b[probIdx][cutIdx].resize(param_disj.num_terms);
    }

    // calculate a cut for each disjunctive term
    unprunedTermSolver = false;
    for (int termIdx=0; termIdx < param_disj.num_terms; termIdx++) {
      OsiSolverInterface* termSolver = term_solvers[termIdx].get();

      // get each cut for this term
      for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++){
        a[probIdx][cutIdx][termIdx].resize(si->getNumCols());
        // if the term is feasible and isn't safely prunable by bound, get the cut
        if (keep[termIdx]){
          unprunedTermSolver = true;
          getCutFromCertificate(a[probIdx][cutIdx][termIdx], b[probIdx][cutIdx][termIdx],
                                cutCertificates[probIdx][cutIdx][termIdx], termSolver);
        } else {
          // pruned - no cuts will violate the optimal solution
          // so just make its cut to where it won't be chosen
          std::fill(a[probIdx][cutIdx][termIdx].begin(), a[probIdx][cutIdx][termIdx].end(), -1e50);
          b[probIdx][cutIdx][termIdx] = 1e50;
        }
      }
    }

    // if we are unsure the status of a solver or all terms were infeasible, move on
    if (!unprunedTermSolver) {
      continue;
    }

    // create a valid disjunctive cut given all the valid term cuts
    for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++){

      // collect the cut coefficients for the unpruned, initially feasible terms
      // with still feasible generating bases - allows for tightening to occur
      std::vector<std::vector<double>> a_feasible;
      std::vector<double> b_feasible;
      bool term_exists = false;
      for (int termIdx=0; termIdx < param_disj.num_terms; termIdx++){
        if (disjunctions[probIdx].get()->terms[termIdx].is_feasible &&
            original_basis_feasible[termIdx] && keep[termIdx]){
          a_feasible.push_back(a[probIdx][cutIdx][termIdx]);
          b_feasible.push_back(b[probIdx][cutIdx][termIdx]);
          term_exists = true;
        }
      }

      // if we want to try tightening the cut for changes in coefficient matrix or feasibility status
      // (assuming we find an initial cut for the feasible terms)
      bool tighten_cuts = tighten_matrix_perturbation || tighten_infeasible_to_feasible_term || tighten_feasible_to_infeasible_basis;
      if (term_exists && tighten_cuts){

        // get an initial parametric disjunctive cut (all tightened cuts will be parallel to this)
        std::vector<double> alpha_feasible = elementWiseMax(a_feasible);
        double beta_feasible = min(b_feasible);
        std::vector<int> indices_feasible;
        std::vector<double> elements_feasible;
        findNonZero(alpha_feasible, indices_feasible, elements_feasible);
        OsiRowCut cut_feasible;
        cut_feasible.setRow(indices_feasible.size(), indices_feasible.data(), elements_feasible.data());
        cut_feasible.setLb(beta_feasible);
        const CoinPackedVector lhs_feasible = cut_feasible.row();
        const double rhs_feasible = cut_feasible.lb();

        // try tightening terms initially infeasible or with perturbed coefficient matrix
        for (int termIdx=0; termIdx < param_disj.num_terms; termIdx++){

          // move on if we can safely prune the term
          if (!keep[termIdx]){
            continue;
          }

          // move on if the coefficient matrix didn't change, the term was originally feasible
          // and the basis after perturbation stayed feasible
          bool need_matrix_tightening = anyBothTrue(cuts_changed_coefficients[cutIdx], keep)
              && tighten_matrix_perturbation;
          bool need_infeasible_term_tightening = infeasible_term_becomes_feasible[termIdx]
              && tighten_infeasible_to_feasible_term;
          bool need_infeasible_basis_tightening = term_remains_feasible_basis_infeasible[termIdx]
              && tighten_feasible_to_infeasible_basis;
          if (!(need_matrix_tightening || need_infeasible_term_tightening ||
                need_infeasible_basis_tightening)){
            continue;
          }

          // we have a candidate for tightening - give it a try
          try {

            // find the supporting basis for this cut and the certificate that
            // generates the cut from the term solver at the given basis
            std::vector<double> new_v;
            getCertificate(new_v, lhs_feasible.getNumElements(), lhs_feasible.getIndices(),
                           lhs_feasible.getElements(), rhs_feasible, term_solvers[termIdx].get());

            // we already filtered out infeasible terms, so only remaining issue could be numerical
            // continue if we failed to find a certificate for the cut due to numerical error
            if (min(new_v) == 0.0 && max(new_v) == 0.0) {
              continue;
            }

            // get the new rhs
            std::vector<double> new_a(term_solvers[termIdx]->getNumCols());
            double new_b = 0.0;
            getCutFromCertificate(new_a, new_b, new_v, term_solvers[termIdx].get());

            // check the coefs match. should be guarantee at this point, but you never know
            for (int idx = 0; idx < new_a.size(); idx++){
              if (!isVal(alpha_feasible[idx], new_a[idx])){
                continue;
              }
            }

            // if cut coefs changed from a[probIdx][cutIdx][termIdx] to alpha_feasible,
            // then we want to keep the new cut whatever the rhs
            // if cut coefs didn't change, then rhs will be at least as good as before
            // tl;dr save the tightened cut
            a[probIdx][cutIdx][termIdx] = alpha_feasible;
            b[probIdx][cutIdx][termIdx] = new_b;

          } catch (const std::runtime_error& e) {

            // don't terminate if tightening fails. just give the user the heads up
            // and move on since we already had a certificate for the term to begin with
            std::cerr << "warning: cut tightening failed" << std::endl;
          }
        }
      }

      std::vector<double> alpha = elementWiseMax(a[probIdx][cutIdx]);
      double beta = min(b[probIdx][cutIdx]);
      std::vector<int> indices;
      std::vector<double> elements;
      findNonZero(alpha, indices, elements);
      OsiRowCut cut;
      cut.setRow(indices.size(), indices.data(), elements.data());
      cut.setLb(beta);

      // add the cut to the disjunctive cuts
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
    OsiClpSolverInterface * si, RunData& data, bool tighten_disjunction){

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

    // parameterize the disjunction  todo: give bound which will drop all prunable terms
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

    // record term statuses and changes to term statuses
    for (int i = 0; i < param_disj.num_terms; i++){
      if (!param_disj.terms[i].is_feasible){
        data.infeasibleTerms++;
      }
      if (disj->terms[i].is_feasible && !param_disj.terms[i].is_feasible) {
        data.feasibleToInfeasibleTerms++;
      }
      if (!disj->terms[i].is_feasible && param_disj.terms[i].is_feasible) {
        data.infeasibleToFeasibleTerms++;
      }
    }
  }

  return disjCuts;
}
