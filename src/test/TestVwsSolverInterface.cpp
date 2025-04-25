/**
 * @file TestVwsSolverInterface.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

#define CATCH_CONFIG_MAIN

// standard library
#include <cstdlib> // abs

// unit test library
#include "catch.hpp"

// coin-or modules
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface
#include <OsiCuts.hpp> // OsiCuts

// vpc modules
#include "Disjunction.hpp" // DisjExitReason
#include "VPCParameters.hpp" // VPCParameters
using namespace VPCParametersNamespace;

// project unit test modules
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp"

// utility function for getting default vpc parameters
VPCParameters getParams(int terms=69){
  VPCParameters params;
  params.set(DISJ_TERMS, 69); // how many total leaves in the disjunction
  params.set(TIMELIMIT, 10); // max time for vpc generation
  params.set(PARTIAL_BB_TIMELIMIT, 10); // max time for creating partial bb tree
  params.set(PARTIAL_BB_KEEP_PRUNED_NODES, 1); // get the entirety of the partial bb tree
  params.set(MODE, 0); // create a partial bb tree to get the disjunction
  params.set(BB_STRATEGY, get_bb_option_value({
      BB_Strategy_Options::user_cuts,
      BB_Strategy_Options::all_cuts_on,
      BB_Strategy_Options::heuristics_on
  }));
  return params;
}

// utility function for getting a solution to a solver
std::vector<double> getSolution(OsiClpSolverInterface& solver){
  CbcModel model(solver);
  model.branchAndBound();
  return std::vector<double>(model.bestSolution(),
                             model.bestSolution() + model.getNumCols());
}

// utility function for checking if a cut is valid for a given solution
void checkCuts(OsiCuts* cuts, std::vector<double> solution){
  for (int i=0; i<cuts->sizeRowCuts(); i++){

    // each cut should be of form a^T x >= b
    REQUIRE(cuts->rowCutPtr(i)->lb() > -COIN_DBL_MAX);
    REQUIRE(cuts->rowCutPtr(i)->ub() == COIN_DBL_MAX);

    // each cut should be valid for the optimal solution
    REQUIRE(isFeasible(cuts->rowCut(i), solution));
  }
}

// checks if new VPCs are roughly better than old VPCs are roughly better than Farkas VPCs
int compareCutGenerators(OsiClpSolverInterface tmpSolver, VwsSolverInterface seriesSolver){

  // get temporary solvers for this test - one for each way to make cuts
  OsiClpSolverInterface newDisjSolver = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());
  OsiClpSolverInterface farkasSolver = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());
  OsiClpSolverInterface oldDisjSolver = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());
  OsiClpSolverInterface farkasSolverTight = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());
  OsiClpSolverInterface oldDisjSolverTight = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());

  // get the optimal solution to the new problem
  std::vector<double> solution = getSolution(tmpSolver);

  // get the cuts from solving the PRLP with a New - conduct on new
  // series solver to not add additional disjunctions to what subsequent tests use
  VwsSolverInterface tmpSeriesSolver(getParams(), "CBC");
  RunData data;
  std::shared_ptr<OsiCuts> newDisjCuts =
      tmpSeriesSolver.createVpcsFromNewDisjunctionPRLP(&newDisjSolver, data);
  std::cout << "hi 1" << std::endl;
  checkCuts(newDisjCuts.get(), solution);

  // get the cuts from parameterizing previous ones via farkas multipliers
  std::shared_ptr<OsiCuts> farkasCuts =
      seriesSolver.createVpcsFromFarkasMultipliers(&farkasSolver, data);
  std::cout << "hi 2" << std::endl;
  REQUIRE(farkasCuts->sizeRowCuts() == 6);
  checkCuts(farkasCuts.get(), solution);

  // get the cuts from solving the PRLP with an Old
  std::shared_ptr<OsiCuts> oldDisjCuts =
      seriesSolver.createVpcsFromOldDisjunctionPRLP(&oldDisjSolver, data);
  std::cout << "hi 3" << std::endl;
  checkCuts(oldDisjCuts.get(), solution);

  // get the cuts from parameterizing previous ones via farkas multipliers and tightening
  RunData data_tight;
  std::shared_ptr<OsiCuts> farkasCutsTight =
      seriesSolver.createVpcsFromFarkasMultipliers(&farkasSolverTight, data_tight, true);
  std::cout << "hi 4" << std::endl;
  REQUIRE(farkasCutsTight->sizeRowCuts() == 6);
  checkCuts(farkasCutsTight.get(), solution);

  // get the cuts from solving the PRLP with an Old and tightening
  std::shared_ptr<OsiCuts> oldDisjCutsTight =
      seriesSolver.createVpcsFromOldDisjunctionPRLP(&oldDisjSolverTight, data, true);
  std::cout << "hi 5" << std::endl;
  REQUIRE(oldDisjCutsTight->sizeRowCuts() > 0);
  checkCuts(oldDisjCutsTight.get(), solution);

  // add the cuts to each respective solver
  newDisjSolver.applyCuts(*newDisjCuts);
  newDisjSolver.resolve();
  farkasSolver.applyCuts(*farkasCuts);
  farkasSolver.resolve();
  oldDisjSolver.applyCuts(*oldDisjCuts);
  oldDisjSolver.resolve();
  farkasSolverTight.applyCuts(*farkasCutsTight);
  farkasSolverTight.resolve();
  oldDisjSolverTight.applyCuts(*oldDisjCutsTight);
  oldDisjSolverTight.resolve();

  // check the bounds when cuts exist - greater value should be better as that means more bound tightened
  // farkas should never be significantly better than the other two since they tailor their cuts to the problem
  if (oldDisjCuts->sizeRowCuts()){
    REQUIRE(oldDisjSolver.getObjValue() - farkasSolver.getObjValue() > -1);
  }
  if (newDisjCuts->sizeRowCuts()){
    REQUIRE(newDisjSolver.getObjValue() - farkasSolver.getObjValue() > -1);
  }

  // tightened cuts should at least be as strong as non-tightened cuts when they exist
  REQUIRE(farkasSolver.getObjValue() < farkasSolverTight.getObjValue() + 1e-4);
  if (oldDisjCutsTight->sizeRowCuts()){
    REQUIRE(oldDisjSolver.getObjValue() < oldDisjSolverTight.getObjValue() + 1e-4);
  }

  int separated = 0;
  // track when tightened is significantly better than non-tightened
  if (farkasSolver.getObjValue() < farkasSolverTight.getObjValue() - 1e-4){
    separated++;
    REQUIRE(data_tight.feasibleTermsPrunedByBound > 0);
  }
  if (oldDisjSolver.getObjValue() < oldDisjSolverTight.getObjValue() - 1e-4){
    separated++;
  }

  return separated;
}

// make sure we get the correct solution to the original problem
void check_orig(RunData& data){
  // the further we solve, the better the dual bound should be
  REQUIRE(isVal(data.lpBound, 20.57, .01));
  REQUIRE(data.dualBound == 34);

  // we minimize so primal bound should be greater than or equal to dual bound
  REQUIRE(data.primalBound == 34);
}

// make sure we get the correct solution to the parameterized problem
void check_param(RunData& data){
  // the further we solve, the better the dual bound should be
  REQUIRE(isVal(data.lpBound, 19.09, .01));
  REQUIRE(data.dualBound == 31);

  // we minimize so primal bound should be greater than or equal to dual bound
  REQUIRE(data.primalBound == 31);
}

// make sure we capture the correct data for any perturbation of bm23
void check_bm23_data(RunData& data){
  // algorithm step time comparisons
  REQUIRE(0 <= data.vpcGenerationTime);
  REQUIRE(data.vpcGenerationTime < data.rootDualBoundTime);
  REQUIRE(data.rootDualBoundTime < data.terminationTime);
  REQUIRE(data.terminationTime < 2);

  // solution time comparisons
  REQUIRE(data.bestSolutionTime < data.terminationTime);

  // check the run parameters
  REQUIRE(data.maxTime == 10);
  REQUIRE(data.terms == 69);
  if (data.vpcGenerator == "None"){
    REQUIRE(data.actualTerms == 0);
  } else {
    REQUIRE(data.actualTerms == 69);
  }
  REQUIRE(data.iterations > 100);
  REQUIRE(data.nodes > 10);
  if (data.vpcGenerator == "New"){
    REQUIRE(data.cutLimit == 6);
  } else {
    REQUIRE(data.cutLimit == 0);
  }
}

TEST_CASE("Test default constructor", "[VwsSolverInterface::VwsSolverInterface]") {
  // Ensure we set up as expected
  VwsSolverInterface seriesSolver(getParams(), "CBC");
  REQUIRE(seriesSolver.mipSolver == "CBC");
  REQUIRE(seriesSolver.params.get(DISJ_TERMS) == 69);
  REQUIRE(seriesSolver.params.get(TIMELIMIT) == 10);
}

TEST_CASE( "test solve", "[VwsSolverInterface::solve]" ){

  // instance solver
  fs::path inputPath("../src/test/test_instances/bm23/bm23_i01.mps");
  OsiClpSolverInterface si;
  si.readMps(inputPath.c_str(), true, false);
  si.initialSolve();

  // series solver
  VwsSolverInterface seriesSolver(getParams(), "CBC");

  // ------------------------ no vpcs ------------------------------------------

  // check running when we swap out primal heuristics
  seriesSolver.params.set(BB_STRATEGY, get_bb_option_value({
    BB_Strategy_Options::heuristics_off,
    BB_Strategy_Options::use_best_bound,
    BB_Strategy_Options::user_cuts
  }));

  // solve
  RunData data = seriesSolver.solve(si, "None", 34);

  // check seriesSolver attributes didn't change
  REQUIRE(seriesSolver.disjunctions.size() == 0);
  REQUIRE(seriesSolver.cutCertificates.size() == 0);
  REQUIRE(seriesSolver.solvers.size() == 0);

  // check the event handler
  REQUIRE(!data.numCuts);
  REQUIRE(isVal(data.disjunctiveDualBound, 20.57, .01));
  REQUIRE(isVal(data.lpBoundPostVpc, 20.57, .01));
  REQUIRE(isVal(data.rootDualBound, 25.25, .01));
  REQUIRE(data.vpcGenerator == "None");
  REQUIRE(data.mipSolver == "CBC");
  REQUIRE(data.providePrimalBound == true);
  check_orig(data);
  check_bm23_data(data);

  // now rerun no VPCs with Gurobi interface
  seriesSolver.mipSolver = "Gurobi";
  data = seriesSolver.solve(si, "None", 34);

  // check seriesSolver attributes didn't change
  REQUIRE(seriesSolver.disjunctions.size() == 0);
  REQUIRE(seriesSolver.cutCertificates.size() == 0);
  REQUIRE(seriesSolver.solvers.size() == 0);

  // check the event handler
  REQUIRE(!data.numCuts);
  REQUIRE(isVal(data.disjunctiveDualBound, 20.57, .01));
  REQUIRE(isVal(data.lpBoundPostVpc, 20.57, .01));
  REQUIRE(isVal(data.rootDualBound, 26.00, .01));
  REQUIRE(data.vpcGenerator == "None");
  REQUIRE(data.mipSolver == "Gurobi");
  REQUIRE(data.providePrimalBound == true);
  check_orig(data);
  check_bm23_data(data);

  // check sensitivity stats
  REQUIRE(data.infeasibleTerms == 0);
  REQUIRE(data.feasibleToInfeasibleTerms == 0);
  REQUIRE(data.infeasibleToFeasibleTerms == 0);

  // ------------------------ New ----------------------------------

  // now run the rest with defaults
  seriesSolver = VwsSolverInterface(getParams(), "CBC");

  // solve
  data = seriesSolver.solve(si, "New");

  // check seriesSolver attributes
  // we should have one vpc generator and set of farkas multipliers
  REQUIRE(seriesSolver.disjunctions.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);
  REQUIRE(seriesSolver.solvers.size() == 1);
  REQUIRE(sameCoefficientMatrix(seriesSolver.solvers[0].get(), &si));

  // we should have found 6 cuts (for the number of fractional variables in the root LP relaxation)
  REQUIRE(seriesSolver.cutCertificates[0].size() == 6);

  // each cut should have at least 69 disjunctive terms
  // (64 unpruned leaves, 2 pruned leaves, 3 strong branching terms)
  for (int cutIdx=0; cutIdx<seriesSolver.cutCertificates[0].size(); cutIdx++) {
    REQUIRE(seriesSolver.cutCertificates[0][cutIdx].size() == 69);
    // each disjunctive term should have more than 47 coefficients (27 variables, 20 constraints, n branching decisions)
    for (int termIdx=0; termIdx<seriesSolver.cutCertificates[0][cutIdx].size(); termIdx++) {
      REQUIRE(47 < seriesSolver.cutCertificates[0][cutIdx][termIdx].size());
    }
  }

  // check data attributes
  REQUIRE(data.numCuts == 6);
  REQUIRE(isVal(data.disjunctiveDualBound, 30.17, .01));
  REQUIRE(isVal(data.lpBoundPostVpc, 29.98, .01));
  REQUIRE(isVal(data.rootDualBound, 30.12, .01));
  REQUIRE(data.vpcGenerator == "New");
  REQUIRE(data.mipSolver == "CBC");
  REQUIRE(data.providePrimalBound == false);
  check_orig(data);
  check_bm23_data(data);

  // check sensitivity stats
  REQUIRE(data.infeasibleTerms == 3);
  REQUIRE(data.feasibleToInfeasibleTerms == 0);
  REQUIRE(data.infeasibleToFeasibleTerms == 0);

  // ------------------------ Old ----------------------------------

  // perturb the problem
  for (int col_idx=0; col_idx < si.getNumCols(); col_idx++){
    bool perturb = col_idx == 25 || col_idx == 0 || col_idx == 13 || col_idx == 22;
    si.setObjCoeff(
        col_idx, perturb ? si.getObjCoefficients()[col_idx] * 5 :
        si.getObjCoefficients()[col_idx]);
  }

  // update the constraint bounds
  for (int row_idx=0; row_idx < si.getNumRows(); row_idx++){
    double b = si.getRowUpper()[row_idx];
    double looser_b = b > 0 ? b * 1.5 : b / 1.5;
    bool perturb = row_idx == 2 || row_idx == 7 || row_idx == 12 || row_idx == 17;
    si.setRowUpper(row_idx, perturb ? looser_b: b);
  }
  si.resolve();

  // solve with Old
  data = seriesSolver.solve(si, "Old");

  // check seriesSolver attributes didn't change
  REQUIRE(seriesSolver.disjunctions.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);
  REQUIRE(seriesSolver.cutCertificates[0].size() == 6);
  REQUIRE(seriesSolver.solvers.size() == 1);

  // check data attributes
  REQUIRE(data.numCuts);
  REQUIRE(isVal(data.disjunctiveDualBound, 22.91, .01));
  REQUIRE(isVal(data.lpBoundPostVpc, 22.77, .01));
  REQUIRE(data.lpBoundPostVpc - .01 < data.rootDualBound);
  REQUIRE(data.vpcGenerator == "Old");
  REQUIRE(data.mipSolver == "CBC");
  REQUIRE(!data.providePrimalBound);
  check_param(data);
  check_bm23_data(data);

  // now rerun with gurobi
  seriesSolver.mipSolver = "Gurobi";
  data = seriesSolver.solve(si, "Old");

  // check seriesSolver attributes didn't change
  REQUIRE(seriesSolver.disjunctions.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);
  REQUIRE(seriesSolver.cutCertificates[0].size() == 6);
  REQUIRE(seriesSolver.solvers.size() == 1);

  // but we should have solution pool now
  REQUIRE(seriesSolver.solutionPool.size() > 0);
  for (const auto& sol : seriesSolver.solutionPool) {
    REQUIRE(isFeasible(si, sol));
  }

  // check data attributes
  REQUIRE(data.numCuts);
  REQUIRE(isVal(data.disjunctiveDualBound, 22.91, .01));
  REQUIRE(isVal(data.lpBoundPostVpc, 22.77, .01));
  REQUIRE(data.lpBoundPostVpc - .01 < data.rootDualBound);
  REQUIRE(data.vpcGenerator == "Old");
  REQUIRE(data.mipSolver == "Gurobi");
  REQUIRE(!data.providePrimalBound);
  check_param(data);
  check_bm23_data(data);

  // check sensitivity stats
  REQUIRE(data.infeasibleTerms == 1);
  REQUIRE(data.feasibleToInfeasibleTerms == 0);
  REQUIRE(data.infeasibleToFeasibleTerms == 2);

  // ------------------------ Farkas ------------------------------------------

  // reset to using CBC
  seriesSolver.mipSolver = "CBC";

  // solve with farkas vpcs
  data = seriesSolver.solve(si, "Farkas");

  // check seriesSolver attributes didn't change
  REQUIRE(seriesSolver.disjunctions.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);
  REQUIRE(seriesSolver.cutCertificates[0].size() == 6);
  REQUIRE(seriesSolver.solvers.size() == 1);

  // check data attributes
  REQUIRE(data.numCuts);
  REQUIRE(isVal(data.disjunctiveDualBound, 22.91, .01));
  REQUIRE(isVal(data.lpBoundPostVpc, 19.1, .01));
  REQUIRE(isVal(data.rootDualBound, 25.3, .01));
  REQUIRE(data.vpcGenerator == "Farkas");
  REQUIRE(data.mipSolver == "CBC");
  REQUIRE(!data.providePrimalBound);
  check_param(data);
  check_bm23_data(data);

  // check sensitivity stats
  REQUIRE(data.infeasibleTerms == 1);
  REQUIRE(data.feasibleToInfeasibleTerms == 0);
  REQUIRE(data.infeasibleToFeasibleTerms == 2);
}

// also checks that the cuts are valid for the given solution
TEST_CASE( "for small perturbations check that New VPCs > Old VPCc > Farkas VPCs",
           "[VwsSolverInterface::createVpcsFromNewDisjunctionPRLP"
           "[VwsSolverInterface::createVpcsFromFarkasMultipliers]"
           "[VwsSolverInterface::createVpcsFromOldDisjunctionPRLP][long][gurobi]" ) {

  // set up reusable stuff
  VwsSolverInterface seriesSolver(getParams(), "GUROBI");
  OsiClpSolverInterface instanceSolver;
  int separated = 0;

  // solve the instance via PRLP to get a disjunction and farkas certificate
  fs::path inputPath("../src/test/test_instances/bm23/bm23_i01.mps");
  instanceSolver.readMps(inputPath.c_str(), true, false);
  RunData data = seriesSolver.solve(instanceSolver, "New");

  // test changing the objective
  for (int i = 0; i < instanceSolver.getNumCols(); i++){

    // create a solver to perturb the original instance
    OsiClpSolverInterface tmpSolver = *dynamic_cast<OsiClpSolverInterface*>(instanceSolver.clone());

    // update the objective in one dimension
    for (int col_idx=0; col_idx < instanceSolver.getNumCols(); col_idx++){
      tmpSolver.setObjCoeff(
          col_idx, i == col_idx ? instanceSolver.getObjCoefficients()[col_idx] * 1.2 :
          instanceSolver.getObjCoefficients()[col_idx]);
    }
    tmpSolver.initialSolve();

    // make sure the cuts are generated and improve the bound as expected
    separated = separated + compareCutGenerators(tmpSolver, seriesSolver);

  }

  // test changing the constraint bounds
  for (int i = 0; i < instanceSolver.getNumRows(); i++){

    // create a solver to perturb the original instance
    OsiClpSolverInterface tmpSolver = *dynamic_cast<OsiClpSolverInterface*>(instanceSolver.clone());

    // update the constraint bound
    for (int row_idx=0; row_idx < instanceSolver.getNumRows(); row_idx++){
      double b = tmpSolver.getRowUpper()[row_idx];
      double looser_b = b > 0 ? b * 1.2 : b / 1.2;
      tmpSolver.setRowUpper(row_idx, i == row_idx ? looser_b: b);
    }
    tmpSolver.initialSolve();

    // make sure the cuts are generated and improve the bound as expected
    // i = 17 we have an infeasible term become feasible (which means we have a term constraint of 0x >= 0),
    // which weakens parameterization. However, tightening finds the term prunable, returning cuts to strength
    separated = separated + compareCutGenerators(tmpSolver, seriesSolver);
  }

  // this number is kinda arbitrary but we would expect at least one improvement here more the point here is that most of our
  // bounds should go in order of (New) > (farkas) and (Old) > (farkas)
  REQUIRE(separated > 0);
}

TEST_CASE( "check that if we perturb the problem a lot that we still get valid cuts",
           "[VwsSolverInterface::createVpcsFromFarkasMultipliers]"
           "[VwsSolverInterface::createVpcsFromOldDisjunctionPRLP][long][alot]" ) {

  // set up reusable stuff
  VwsSolverInterface seriesSolver(getParams(), "GUROBI");
  OsiClpSolverInterface instanceSolver;
  int separated = 0;

  // solve the instance via PRLP to get a disjunction and farkas certificate
  fs::path inputPath("../src/test/test_instances/bm23/bm23_i01.mps");
  instanceSolver.readMps(inputPath.c_str(), true, false);
  RunData data = seriesSolver.solve(instanceSolver, "New");

  // test changing the objective
  for (int i = 0; i < instanceSolver.getNumCols(); i++){

    // get temporary solvers for this test - one for baseline and one for farkas vpcs
    OsiClpSolverInterface tmpSolver = *dynamic_cast<OsiClpSolverInterface*>(instanceSolver.clone());

    // update the objective to push in one dimension
    for (int col_idx=0; col_idx < instanceSolver.getNumCols(); col_idx++){
      tmpSolver.setObjCoeff(col_idx, i == col_idx ? -1.1 : -1);
    }
    tmpSolver.resolve();

    // get the optimal solution to the new problem and check the parameterized cuts for it
    std::vector<double> solution = getSolution(tmpSolver);

    std::shared_ptr<OsiCuts> disjCuts =
        seriesSolver.createVpcsFromFarkasMultipliers(&tmpSolver, data);
    std::shared_ptr<OsiCuts> disjCutsTight =
        seriesSolver.createVpcsFromFarkasMultipliers(&tmpSolver, data, true);

    // sanity check to make sure we didn't make up some extra cuts somewhere
    REQUIRE(disjCuts->sizeRowCuts() == 6);
    checkCuts(disjCuts.get(), solution);
    REQUIRE(disjCutsTight->sizeRowCuts() == 6);
    checkCuts(disjCutsTight.get(), solution);

    OsiClpSolverInterface farkasSolver = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());
    OsiClpSolverInterface farkasSolverTight = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());
    farkasSolver.applyCuts(*disjCuts);
    farkasSolver.resolve();
    farkasSolverTight.applyCuts(*disjCutsTight);
    farkasSolverTight.resolve();
    if (farkasSolver.getObjValue() < farkasSolverTight.getObjValue() - 1e-4){
      separated++;
    }

    // repeat for generating cuts from the previous disjunction
    // under large changes the PRLP may become infeasible and yield no cuts because the LP
    // relaxation solution belongs to a disjunctive term
    disjCuts = seriesSolver.createVpcsFromOldDisjunctionPRLP(&tmpSolver, data);
    checkCuts(disjCuts.get(), solution);
  }

  // test changing the constraint bounds
  for (int i = 0; i < instanceSolver.getNumRows(); i++){

    // get temporary solvers for this test - one for baseline and one for farkas vpcs
    OsiClpSolverInterface tmpSolver = *dynamic_cast<OsiClpSolverInterface*>(instanceSolver.clone());

    // relax the constraint bound (to encourage original vpc infeasibility) to push in one dimension
    for (int row_idx=0; row_idx < instanceSolver.getNumRows(); row_idx++){
      double b = tmpSolver.getRowUpper()[row_idx];
      double loose_b = b > 0 ? b * 1 : b / 1;
      double looser_b = b > 0 ? b * 1.1 : b / 1.1;
      tmpSolver.setRowUpper(row_idx, i == row_idx ? looser_b: loose_b);
    }
    tmpSolver.resolve();

    // get the optimal solution to the new problem and get the parameterized cuts for it
    std::vector<double> solution = getSolution(tmpSolver);

    std::shared_ptr<OsiCuts> disjCuts =
        seriesSolver.createVpcsFromFarkasMultipliers(&tmpSolver, data);
    std::shared_ptr<OsiCuts> disjCutsTight =
        seriesSolver.createVpcsFromFarkasMultipliers(&tmpSolver, data, true);

    // sanity check to make sure we didn't make up some extra cuts somewhere
    REQUIRE(disjCuts->sizeRowCuts() == 6);
    checkCuts(disjCuts.get(), solution);
    REQUIRE(disjCutsTight->sizeRowCuts() == 6);
    checkCuts(disjCutsTight.get(), solution);

    OsiClpSolverInterface farkasSolver = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());
    OsiClpSolverInterface farkasSolverTight = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());
    farkasSolver.applyCuts(*disjCuts);
    farkasSolver.resolve();
    farkasSolverTight.applyCuts(*disjCutsTight);
    farkasSolverTight.resolve();
    if (farkasSolver.getObjValue() < farkasSolverTight.getObjValue() - 1e-4){
      separated++;
    }

    // repeat for generating cuts from the previous disjunction
    // under large changes the PRLP may become infeasible and yield no cuts because the LP
    // relaxation solution belongs to a disjunctive term
    disjCuts = seriesSolver.createVpcsFromOldDisjunctionPRLP(&tmpSolver, data);
    checkCuts(disjCuts.get(), solution);
  }
  // i \in {1, 4} should have infeasible terms become feasible necessitating bound tightening
  REQUIRE(separated > 0);
}

int tighteningHasEffect(OsiClpSolverInterface& tmp_solver, VwsSolverInterface& seriesSolver,
                        bool tighten_matrix_perturbation, bool tighten_infeasible_to_feasible_term,
                        bool tighten_feasible_to_infeasible_basis){
  // set up cut generation for each problem
  tmp_solver.initialSolve();
  RunData tmp_data;
  RunData tmp_data_tight;

  // create untightened and tightened matrix perturbed cuts
  std::shared_ptr<OsiCuts> disjCuts =
      seriesSolver.createVpcsFromFarkasMultipliers(&tmp_solver, tmp_data);
  std::shared_ptr<OsiCuts> disjCutsTight =
      seriesSolver.createVpcsFromFarkasMultipliers(
          &tmp_solver, tmp_data_tight, false, tighten_matrix_perturbation,
          tighten_infeasible_to_feasible_term, tighten_feasible_to_infeasible_basis);

  // check data outputs
  tmp_data.tighten_disjunction = false;
  tmp_data.tighten_matrix_perturbation = false;
  tmp_data.tighten_infeasible_to_feasible_term = false;
  tmp_data.tighten_feasible_to_infeasible_basis = false;
  tmp_data_tight.tighten_disjunction = false;
  tmp_data_tight.tighten_matrix_perturbation = tighten_matrix_perturbation;
  tmp_data_tight.tighten_infeasible_to_feasible_term = tighten_infeasible_to_feasible_term;
  tmp_data_tight.tighten_feasible_to_infeasible_basis = tighten_feasible_to_infeasible_basis;

  if (sameCoefficientMatrix(seriesSolver.solvers[0].get(), &tmp_solver)){
    REQUIRE(tmp_data.cutsChangedCoefficients == 0);
    REQUIRE(tmp_data_tight.cutsChangedCoefficients == 0);
  } else {
    REQUIRE(tmp_data.cutsChangedCoefficients > 0);
    REQUIRE(tmp_data_tight.cutsChangedCoefficients > 0);
  }

  // make sure the cuts are vaild
  checkCuts(disjCuts.get(), getSolution(tmp_solver));
  checkCuts(disjCutsTight.get(), getSolution(tmp_solver));

  // apply the cuts to the original solver and check to make sure bound improves as expected
  OsiClpSolverInterface tight_solver = *dynamic_cast<OsiClpSolverInterface*>(tmp_solver.clone());
  tmp_solver.applyCuts(*disjCuts);
  tmp_solver.resolve();
  tight_solver.applyCuts(*disjCutsTight);
  tight_solver.resolve();

  // the bound shouldn't be worse than before (within tolerance)
  REQUIRE(tmp_solver.getObjValue() < tight_solver.getObjValue() + 1e-4);

  // track when we have a significant improvement
  if (tmp_solver.getObjValue() < tight_solver.getObjValue() - 1e-4){
    // if only infeasible term tightening enabled and we improve, we should have found a term
    if (!tighten_matrix_perturbation && tighten_infeasible_to_feasible_term &&
        !tighten_feasible_to_infeasible_basis) {
      REQUIRE(tmp_data.infeasibleToFeasibleTerms > 0);
      REQUIRE(tmp_data_tight.infeasibleToFeasibleTerms > 0);
    }
    // if exactly both infeasibility tightenings enabled and we don't have an infeasible
    // term becoming feasible, we should have found a basis that became infeasible
    if (!tighten_matrix_perturbation && tighten_infeasible_to_feasible_term &&
        tighten_feasible_to_infeasible_basis && tmp_data.infeasibleToFeasibleTerms == 0) {
      REQUIRE(tmp_data.termRemainsFeasibleBasisInfeasible > 0);
    }
    if (!tighten_matrix_perturbation && tighten_infeasible_to_feasible_term &&
        tighten_feasible_to_infeasible_basis && tmp_data_tight.infeasibleToFeasibleTerms == 0) {
      REQUIRE(tmp_data_tight.termRemainsFeasibleBasisInfeasible > 0);
    }
    return 1;
  } else {
    // if no improvement but we had term tightening enabled, we should have found no terms
    if (tighten_infeasible_to_feasible_term) {
      REQUIRE(tmp_data.infeasibleToFeasibleTerms == 0);
      REQUIRE(tmp_data_tight.infeasibleToFeasibleTerms == 0);
    }
    // if no improvement but we had basis tightening enabled, we should have found no terms
    if (tighten_feasible_to_infeasible_basis) {
      REQUIRE(tmp_data.termRemainsFeasibleBasisInfeasible == 0);
      REQUIRE(tmp_data_tight.termRemainsFeasibleBasisInfeasible == 0);
    }
    return 0;
  }
}

TEST_CASE( "test matrix tightening", "[VwsSolverInterface::createVpcsFromFarkasMultipliers][matrixTightening][tighten]" ){

  // read in and then sort alphabetically all the mps files in the test_instances folder
  fs::path inputFolder("../src/test/test_instances/matrix_1");
  std::vector<fs::path> inputFiles;
  for (const auto& entry : fs::directory_iterator(inputFolder)){
    if (entry.path().extension() == ".mps"){
      inputFiles.push_back(entry.path());
    }
  }
  std::sort(inputFiles.begin(), inputFiles.end());
  
  // create a series solver to track future cut generation
  VwsSolverInterface seriesSolver(getParams(), "CBC");

  // solve the original problem
  OsiClpSolverInterface si;
  si.readMps(inputFiles[0].c_str(), true, false);
  si.initialSolve();

  // solve the instance making disjunctive cuts via PRLP to get a disjunction and farkas certificate
  RunData data = seriesSolver.solve(si, "New");

  // check the event handler
  check_bm23_data(data);

  std::cout << "\n\n\n\n\n\n" << std::endl;

  // iterate over the rest of the files to check to make sure cuts tighten as expected
  int improved = 0;
  for (int i = 1; i < inputFiles.size(); i++){
    OsiClpSolverInterface tmp_solver;
    tmp_solver.readMps(inputFiles[i].c_str(), true, false);
    improved = improved + tighteningHasEffect(tmp_solver, seriesSolver, true, false, false);
  }

  // we should have at least one instance for which we have decent improvements
  REQUIRE(improved > 0);  // 18
}

TEST_CASE( "test term infeasibility tightening", "[VwsSolverInterface::createVpcsFromFarkasMultipliers][infeasibleTermTightening][tighten]" ){

  // read in and then sort alphabetically all the mps files in the test_instances folder
  fs::path inputFolder("../src/test/test_instances/rhs_1");
  std::vector<fs::path> inputFiles;
  for (const auto& entry : fs::directory_iterator(inputFolder)){
    if (entry.path().extension() == ".mps"){
      inputFiles.push_back(entry.path());
    }
  }
  std::sort(inputFiles.begin(), inputFiles.end());

  // create a series solver to track future cut generation
  VwsSolverInterface seriesSolver(getParams(), "CBC");

  // solve the original problem
  OsiClpSolverInterface si;
  si.readMps(inputFiles[0].c_str(), true, false);
  si.initialSolve();

  // solve the instance making disjunctive cuts via PRLP to get a disjunction and farkas certificate
  RunData data = seriesSolver.solve(si, "New");

  // check the event handler
  check_bm23_data(data);

  std::cout << "\n\n\n\n\n\n" << std::endl;

  // iterate over the rest of the files to check to make sure cuts tighten as expected
  int improved = 0;
  int more_improved = 0;
  for (int i = 1; i < inputFiles.size(); i++){
    OsiClpSolverInterface tmp_solver;
    tmp_solver.readMps(inputFiles[i].c_str(), true, false);
    improved = improved + tighteningHasEffect(tmp_solver, seriesSolver, false, true, false);

    OsiClpSolverInterface tmp_solver2;
    tmp_solver2.readMps(inputFiles[i].c_str(), true, false);
    more_improved = more_improved + tighteningHasEffect(tmp_solver2, seriesSolver, false, true, true);
  }

  // we should have at least one instance for which we have decent improvements
  REQUIRE(improved > 0);  // 10
  REQUIRE(more_improved > improved);  // 20
}