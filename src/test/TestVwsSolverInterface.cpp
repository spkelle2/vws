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
VPCParameters getParams(){
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

  // get the optimal solution to the new problem
  std::vector<double> solution = getSolution(tmpSolver);

  // get the cuts from solving the PRLP with a New - conduct on new
  // series solver to not add additional disjunctions to what subsequent tests use
  VwsSolverInterface tmpSeriesSolver(getParams(), "CBC");
  RunData data;
  std::shared_ptr<OsiCuts> newDisjCuts =
      tmpSeriesSolver.createVpcsFromNewDisjunctionPRLP(&newDisjSolver, data);
  REQUIRE(newDisjCuts->sizeRowCuts() > 0);
  REQUIRE(newDisjCuts->sizeRowCuts() == data.numCuts);
  checkCuts(newDisjCuts.get(), solution);

  // get the cuts from parameterizing previous ones via farkas multipliers
  std::shared_ptr<OsiCuts> farkasCuts =
      seriesSolver.createVpcsFromFarkasMultipliers(&farkasSolver, data);
  REQUIRE(farkasCuts->sizeRowCuts() == 6);
  REQUIRE(farkasCuts->sizeRowCuts() == data.numCuts);
  checkCuts(farkasCuts.get(), solution);

  // get the cuts from solving the PRLP with an Old
  std::shared_ptr<OsiCuts> oldDisjCuts =
      seriesSolver.createVpcsFromOldDisjunctionPRLP(&oldDisjSolver, data);
  REQUIRE(oldDisjCuts->sizeRowCuts() > 0);
  REQUIRE(oldDisjCuts->sizeRowCuts() == data.numCuts);
  checkCuts(oldDisjCuts.get(), solution);

  // add the cuts to each respective solver
  newDisjSolver.applyCuts(*newDisjCuts);
  newDisjSolver.resolve();
  farkasSolver.applyCuts(*farkasCuts);
  farkasSolver.resolve();
  oldDisjSolver.applyCuts(*oldDisjCuts);
  oldDisjSolver.resolve();

  // check the bounds - greater value should be better as that means more bound tightened
  REQUIRE(newDisjSolver.getObjValue() - oldDisjSolver.getObjValue() > -1);
  REQUIRE(oldDisjSolver.getObjValue() - farkasSolver.getObjValue() > -1);

  // keep a count of how many were seriously out of bounds
  int misordered = 0;
  if (newDisjSolver.getObjValue() - oldDisjSolver.getObjValue() < -1e-4){
    misordered++;
  }
  if (oldDisjSolver.getObjValue() - farkasSolver.getObjValue() < -1e-4){
    misordered++;
  }
  return misordered;
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

  // check data attributes
  REQUIRE(data.numCuts);
  REQUIRE(isVal(data.disjunctiveDualBound, 22.91, .01));
  REQUIRE(isVal(data.lpBoundPostVpc, 22.77, .01));
  REQUIRE(isVal(data.rootDualBound, 24.73, .01));
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

  // check data attributes
  REQUIRE(data.numCuts);
  REQUIRE(isVal(data.disjunctiveDualBound, 22.91, .01));
  REQUIRE(isVal(data.lpBoundPostVpc, 22.77, .01));
  REQUIRE(isVal(data.rootDualBound, 24.69, .01));
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
           "[VwsSolverInterface::createVpcsFromOldDisjunctionPRLP][long]" ) {

  // set up reusable stuff
  VwsSolverInterface seriesSolver(getParams(), "CBC");
  OsiClpSolverInterface instanceSolver;
  int misordered = 0;

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
    misordered = misordered + compareCutGenerators(tmpSolver, seriesSolver);

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
    misordered = misordered + compareCutGenerators(tmpSolver, seriesSolver);
  }

  // this number is kinda arbitrary but more the point here is that most of our
  // bounds should go in order of (New) > (Old) > (farkas)
  REQUIRE(misordered < 10);
}

TEST_CASE( "check that if we perturb the problem a lot that we still get valid cuts",
           "[VwsSolverInterface::createVpcsFromFarkasMultipliers]"
           "[VwsSolverInterface::createVpcsFromOldDisjunctionPRLP][long]" ) {

  // set up reusable stuff
  VwsSolverInterface seriesSolver(getParams(), "CBC");
  OsiClpSolverInterface instanceSolver;

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
      tmpSolver.setObjCoeff(col_idx, i == col_idx ? -2 : -1);
    }

    // get the optimal solution to the new problem and check the parameterized cuts for it
    std::vector<double> solution = getSolution(tmpSolver);

    std::shared_ptr<OsiCuts> disjCuts =
        seriesSolver.createVpcsFromFarkasMultipliers(&tmpSolver, data);
    REQUIRE(disjCuts->sizeRowCuts() == 6);
    checkCuts(disjCuts.get(), solution);

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
      double loose_b = b > 0 ? b * 1.25 : b / 1.25;
      double looser_b = b > 0 ? b * 2 : b / 2;
      tmpSolver.setRowUpper(row_idx, i == row_idx ? looser_b: loose_b);
    }

    // get the optimal solution to the new problem and get the parameterized cuts for it
    std::vector<double> solution = getSolution(tmpSolver);

    std::shared_ptr<OsiCuts> disjCuts =
        seriesSolver.createVpcsFromFarkasMultipliers(&tmpSolver, data);

    // sanity check to make sure we didn't make up some extra cuts somewhere
    REQUIRE(disjCuts->sizeRowCuts() == 6);
    checkCuts(disjCuts.get(), solution);

    // repeat for generating cuts from the previous disjunction
    // under large changes the PRLP may become infeasible and yield no cuts because the LP
    // relaxation solution belongs to a disjunctive term
    disjCuts = seriesSolver.createVpcsFromOldDisjunctionPRLP(&tmpSolver, data);
    checkCuts(disjCuts.get(), solution);
  }
}
