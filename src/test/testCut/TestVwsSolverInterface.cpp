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
#include "CbcEventHandler.hpp" // CbcEventHandler
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface
#include <OsiCuts.hpp> // OsiCuts

// vpc modules
#include "Disjunction.hpp" // DisjExitReason
#include "VPCParameters.hpp" // VPCParameters

// project unit test modules
#include "VwsEventHandler.hpp"
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp"


// --------------------------- constructor tests -------------------------------
TEST_CASE("Test default constructor", "[VwsSolverInterface::VwsSolverInterface]") {

  // Ensure we set up as expected
  VwsSolverInterface seriesSolver(5, 50, 16);
  REQUIRE(seriesSolver.maxExtraSavedSolutions == 5);
  REQUIRE(seriesSolver.maxRunTime == 50);
  REQUIRE(seriesSolver.disjunctiveTerms == 16);
  REQUIRE(seriesSolver.vpcGenTimeRatio == 0.1);
}


// ------------------------------ solve tests ----------------------------------

void checkNames(VwsSolverInterface &seriesSolver, int probIdx){
  // check that row and column names are correct
  REQUIRE(seriesSolver.variableNames.size() == probIdx + 1);
  REQUIRE(seriesSolver.variableNames[probIdx].size() == 27);
  for (int i = 0; i < seriesSolver.variableNames[probIdx].size(); i++){
    std::string name = i < 9 ? "C10" + std::to_string(i + 1) : "C1" + std::to_string(i + 1);
    REQUIRE(seriesSolver.variableNames[probIdx][i] == name);
  }
  REQUIRE(seriesSolver.constraintNames.size() == probIdx + 1);
  REQUIRE(seriesSolver.constraintNames[probIdx].size() == 20);
  for (int i = 0; i < seriesSolver.constraintNames[probIdx].size(); i++){
    std::string name = i < 9 ? "R10" + std::to_string(i + 1) : "R1" + std::to_string(i + 1);
    REQUIRE(seriesSolver.constraintNames[probIdx][i] == name);
  }
}

void checkSolutions(VwsSolverInterface& seriesSolver, const OsiClpSolverInterface& instanceSolver,
                    const CbcModel& model, int probIdx){

  for (int i = 0; i < seriesSolver.solutions[probIdx].size(); i++){
    // the solution should be feasible
    REQUIRE(isFeasible(instanceSolver, seriesSolver.solutions[probIdx][i]));
    // the solution should match that found by the CBC model
    for (int j = 0; j < seriesSolver.solutions[probIdx][i].size(); j++){
      REQUIRE(std::abs(seriesSolver.solutions[probIdx][i][j] - model.savedSolution(i)[j]) < .0001);
    }
  }
}

void checkCuts(OsiCuts* cuts, std::vector<double> solution){
  for (int i=0; i<cuts->sizeRowCuts(); i++){

    // each cut should be of form a^T x >= b
    REQUIRE(cuts->rowCutPtr(i)->lb() > -COIN_DBL_MAX);
    REQUIRE(cuts->rowCutPtr(i)->ub() == COIN_DBL_MAX);

    // each cut should be valid for the optimal solution
    REQUIRE(isFeasible(cuts->rowCut(i), solution));
  }
}

int compareCutGenerators(OsiClpSolverInterface tmpSolver, VwsSolverInterface seriesSolver){

  // get temporary solvers for this test - one for each way to make cuts
  OsiClpSolverInterface newDisjSolver = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());
  OsiClpSolverInterface farkasSolver = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());
  OsiClpSolverInterface oldDisjSolver = *dynamic_cast<OsiClpSolverInterface*>(tmpSolver.clone());

  // get the optimal solution to the new problem
  CbcModel tmpModel = seriesSolver.solve(tmpSolver, "None");
  std::vector<double> solution(tmpModel.bestSolution(),
                               tmpModel.bestSolution() + tmpModel.getNumCols());

  // get the cuts from solving the PRLP with a new disjunction - conduct on new
  // series solver to not add additional disjunctions to what subsequent tests use
  VwsSolverInterface tmpSeriesSolver(10, 600, seriesSolver.disjunctiveTerms, .8);
  VwsEventHandler eventHandler;
  std::shared_ptr<OsiCuts> newDisjCuts =
      tmpSeriesSolver.createVpcsFromNewDisjunctionPRLP(&newDisjSolver, eventHandler);
  REQUIRE(newDisjCuts->sizeRowCuts() > 0);
  checkCuts(newDisjCuts.get(), solution);

  // get the cuts from parameterizing previous ones via farkas multipliers
  std::shared_ptr<OsiCuts> farkasCuts =
      seriesSolver.createVpcsFromFarkasMultipliers(&farkasSolver, eventHandler);
  REQUIRE(farkasCuts->sizeRowCuts() == 6);
  checkCuts(farkasCuts.get(), solution);

  // get the cuts from solving the PRLP with an old disjunction
  std::shared_ptr<OsiCuts> oldDisjCuts =
      seriesSolver.createVpcsFromOldDisjunctionPRLP(&oldDisjSolver, eventHandler);
  REQUIRE(oldDisjCuts->sizeRowCuts() > 0);
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

TEST_CASE("Test a single solve", "[VwsSolverInterface::solve]") {

  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  fs::path inputPath("../src/test/datasets/bm23/bm23_i01.mps");
  OsiClpSolverInterface instanceSolver;
  instanceSolver.readMps(inputPath.c_str(), true, false);
  VwsSolverInterface seriesSolver;
  CbcModel model = seriesSolver.solve(instanceSolver, "None");

  // check that row and column names are correct
  checkNames(seriesSolver, 0);

  // check that we have an optimal solution
  REQUIRE(model.getObjValue() == 34);

  // we only solve one problem
  REQUIRE(seriesSolver.solutions.size() == 1);

  // the problem should have one solution with heuristics turned on now
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model, 0);
}

TEST_CASE( "Solve a second time", "[VwsSolverInterface::solve]" ){

  // solve a model with the VwsSolverInterface
  fs::path inputPath("../src/test/datasets/bm23/bm23_i01.mps");
  OsiClpSolverInterface instanceSolver;
  instanceSolver.readMps(inputPath.c_str(), true, false);
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver, "None");

  // check on rerun that we get acceptable duplication
  CbcModel model = seriesSolver.solve(instanceSolver, "None");

  // check that row and column names are correct
  checkNames(seriesSolver, 1);

  // check that we have an optimal solution
  REQUIRE(model.getObjValue() == 34);

  // we've solved two problems now
  REQUIRE(seriesSolver.solutions.size() == 2);

  // the problem should have one solution
  REQUIRE(seriesSolver.solutions[1].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model, 1);
}

TEST_CASE( "Solve without preprocessing", "[VwsSolverInterface::solve]" ) {
  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  fs::path inputPath("../src/test/datasets/bm23/bm23_i01.mps");
  OsiClpSolverInterface instanceSolver;
  instanceSolver.readMps(inputPath.c_str(), true, false);
  VwsSolverInterface seriesSolver;
  CbcModel model = seriesSolver.solve(instanceSolver, "None");

  // check that row and column names are correct
  checkNames(seriesSolver, 0);

  // check that we have the same optimal solution
  REQUIRE(model.getObjValue() == 34);

  // we only solve one problem
  REQUIRE(seriesSolver.solutions.size() == 1);

  // the problem should have one solution now that heuristics work
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model, 0);
}

// checks that we get optimal solution when adding vpcs from PRLP and Farkas Multipliers
// well currently just checks that root node is correct, but we'll get back to this
TEST_CASE( "Check solve with VPCs added", "[VwsSolverInterface::solve]" ) {

  // solve a model with the VwsSolverInterface
  fs::path inputPath("../src/test/datasets/bm23/bm23_i01.mps");
  OsiClpSolverInterface instanceSolver;
  instanceSolver.readMps(inputPath.c_str(), true, false);
  VwsSolverInterface seriesSolver(10, 60, 64);
  VwsEventHandler eventHandler;
  CbcModel model = seriesSolver.solve(instanceSolver, "New Disjunction", eventHandler);

  // we should have one vpc generator and set of farkas multipliers
  REQUIRE(seriesSolver.disjunctions.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);

  // we should have found 6 cuts (for the number of fractional variables in the root LP relaxation)
  REQUIRE(seriesSolver.cutCertificates[0].size() == 6);

  // each cut should have at least 66 disjunctive terms (64 unpruned leaves, 2 pruned leaves)
  for (int cutIdx=0; cutIdx<seriesSolver.cutCertificates[0].size(); cutIdx++) {
    REQUIRE(seriesSolver.cutCertificates[0][cutIdx].size() >= 66);
    // each disjunctive term should have more than 47 coefficients (27 variables, 20 constraints, n branching decisions)
    for (int termIdx=0; termIdx<seriesSolver.cutCertificates[0][cutIdx].size(); termIdx++) {
      REQUIRE(47 < seriesSolver.cutCertificates[0][cutIdx][termIdx].size());
    }
  }

  // check the primal solutions
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model, 0);

  // check the bounds - root LP relaxation should be much improved from VPCs
  REQUIRE(model.getObjValue() == 34);
  REQUIRE(isVal(eventHandler.data.rootDualBound, 30, 1));

  // but the underlying model unchanged
  REQUIRE((20 < model.getContinuousObjective() && model.getContinuousObjective() < 21));
  REQUIRE(model.solver()->getNumRows() == 20);

  // check vpcs when parameterized
  eventHandler = VwsEventHandler();
  CbcModel model2 = seriesSolver.solve(instanceSolver, "Farkas", eventHandler);

  // Farkas does not create an additional VPC generator or set of multipliers
  REQUIRE(seriesSolver.disjunctions.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);

  // The underlying model should not have changed
  REQUIRE(model2.solver()->getNumRows() == 20);
  REQUIRE((20 < model2.getContinuousObjective() && model2.getContinuousObjective() < 21));

  // check the primal solutions
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model2, 1);

  // check the bounds - root LP relaxation should be much improved from VPCs
  REQUIRE(model2.getObjValue() == 34);
  REQUIRE(isVal(eventHandler.data.rootDualBound, 30, 1));

}

// --------------------- create disjunctive cut tests --------------------------

TEST_CASE( "Check VPCs from both PRLP and Farkas are valid",
           "[VwsSolverInterface::createVpcsFromNewDisjunctionPRLP]"
           "[VwsSolverInterface::createVpcsFromFarkasMultipliers]"
           "[VwsSolverInterface::createVpcsFromOldDisjunctionPRLP]") {

  // solve a model with the VwsSolverInterface to get the solution
  fs::path inputPath("../src/test/datasets/bm23/bm23_i01.mps");
  OsiClpSolverInterface instanceSolver;
  instanceSolver.readMps(inputPath.c_str(), true, false);
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver, "None");

  // check that the vpcs we get are valid when solving the PRLP with a new disjunction
  VwsEventHandler eventHandler;
  std::shared_ptr<OsiCuts> disjCuts =
      seriesSolver.createVpcsFromNewDisjunctionPRLP(&instanceSolver, eventHandler);

  // we should have added some cuts
  REQUIRE(disjCuts->sizeRowCuts() == 6);
  for (int i=0; i<disjCuts->sizeRowCuts(); i++){

    // each cut should be of form a^T x >= b
    REQUIRE(disjCuts->rowCutPtr(i)->lb() > -COIN_DBL_MAX);
    REQUIRE(disjCuts->rowCutPtr(i)->ub() == COIN_DBL_MAX);

    // each cut should be valid for the optimal solution
    REQUIRE(isFeasible(disjCuts->rowCut(i), seriesSolver.solutions[0][0]));
  }

  // solve the PRLP with a new disjunction again so we have two disjunctions to use in our following tests
  disjCuts = seriesSolver.createVpcsFromNewDisjunctionPRLP(&instanceSolver, eventHandler);

  // check that the vpcs we get are valid when we use Farkas Multipliers
  disjCuts = seriesSolver.createVpcsFromFarkasMultipliers(&instanceSolver, eventHandler);

  REQUIRE(disjCuts->sizeRowCuts() == 12);
  checkCuts(disjCuts.get(), seriesSolver.solutions[0][0]);

  // check that the vpcs we get are valid when we use the old disjunctions in the PRLP
  disjCuts = seriesSolver.createVpcsFromOldDisjunctionPRLP(&instanceSolver, eventHandler);

  REQUIRE(disjCuts->sizeRowCuts() == 12);
  checkCuts(disjCuts.get(), seriesSolver.solutions[0][0]);
}

TEST_CASE( "Create VPCs from Farkas multipliers and old disjunctions on very different problems",
           "[VwsSolverInterface::createVpcsFromFarkasMultipliers]"
           "[VwsSolverInterface::createVpcsFromOldDisjunctionPRLP][long]" ) {

  // set up reusable stuff
  VwsSolverInterface seriesSolver(10, 600, 64, .8);
  OsiClpSolverInterface instanceSolver;
  VwsEventHandler eventHandler;

  // solve the instance via PRLP to get a disjunction and farkas certificate
  fs::path inputPath("../src/test/datasets/bm23/bm23_i01.mps");
  instanceSolver.readMps(inputPath.c_str(), true, false);
  seriesSolver.solve(instanceSolver, "New Disjunction", eventHandler);

  // test changing the objective
  for (int i = 0; i < instanceSolver.getNumCols(); i++){

    //---------------------------- check dramatic changes ----------------------
    // get temporary solvers for this test - one for baseline and one for farkas vpcs
    OsiClpSolverInterface tmpSolver = *dynamic_cast<OsiClpSolverInterface*>(instanceSolver.clone());

    // update the objective to push in one dimension
    for (int col_idx=0; col_idx < instanceSolver.getNumCols(); col_idx++){
      tmpSolver.setObjCoeff(col_idx, i == col_idx ? -2 : -1);
    }

    // get the optimal solution to the new problem and check the parameterized cuts for it
    CbcModel tmpModel = seriesSolver.solve(tmpSolver, "None");
    std::vector<double> solution(tmpModel.bestSolution(),
                                 tmpModel.bestSolution() + tmpModel.getNumCols());
    std::shared_ptr<OsiCuts> disjCuts =
        seriesSolver.createVpcsFromFarkasMultipliers(&tmpSolver, eventHandler);
    REQUIRE(disjCuts->sizeRowCuts() == 6);
    checkCuts(disjCuts.get(), solution);

    // repeat for generating cuts from the previous disjunction
    // under large changes the PRLP may become infeasible and yield no cuts because the LP
    // relaxation solution belongs to a disjunctive term
    disjCuts = seriesSolver.createVpcsFromOldDisjunctionPRLP(&tmpSolver, eventHandler);
    checkCuts(disjCuts.get(), solution);
  }

  // test changing the constraint bounds
  for (int i = 0; i < instanceSolver.getNumRows(); i++){

    //---------------------------- check dramatic changes ----------------------
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
    CbcModel tmpModel = seriesSolver.solve(tmpSolver, "None");
    std::vector<double> solution(tmpModel.bestSolution(),
                                 tmpModel.bestSolution() + tmpModel.getNumCols());
    std::shared_ptr<OsiCuts> disjCuts =
        seriesSolver.createVpcsFromFarkasMultipliers(&tmpSolver, eventHandler);

    // sanity check to make sure we didn't make up some extra cuts somewhere
    REQUIRE(disjCuts->sizeRowCuts() == 6);
    checkCuts(disjCuts.get(), solution);

    // repeat for generating cuts from the previous disjunction
    // under large changes the PRLP may become infeasible and yield no cuts because the LP
    // relaxation solution belongs to a disjunctive term
    disjCuts = seriesSolver.createVpcsFromOldDisjunctionPRLP(&tmpSolver, eventHandler);
    checkCuts(disjCuts.get(), solution);
  }
}

TEST_CASE( "Create VPCs from Farkas multipliers and old disjunctions less parameterized problems",
           "[VwsSolverInterface::createVpcsFromNewDisjunctionPRLP"
           "[VwsSolverInterface::createVpcsFromFarkasMultipliers]"
           "[VwsSolverInterface::createVpcsFromOldDisjunctionPRLP][long]" ) {

  // set up reusable stuff
  VwsSolverInterface seriesSolver(10, 600, 4, .8);
  OsiClpSolverInterface instanceSolver;
  VwsEventHandler eventHandler;
  int misordered = 0;

  // solve the instance via PRLP to get a disjunction and farkas certificate
  fs::path inputPath("../src/test/datasets/bm23/bm23_i01.mps");
  instanceSolver.readMps(inputPath.c_str(), true, false);
  seriesSolver.solve(instanceSolver, "New Disjunction", eventHandler);

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
  // bounds should go in order of (new disjunction) > (old disjunction) > (farkas)
  REQUIRE(misordered < 10);
}

void check_orig(VwsEventHandler& eventHandler){
  // the further we solve, the better the dual bound should be
  REQUIRE(isVal(eventHandler.data.lpBound, 20.57, .01));
  REQUIRE(eventHandler.data.dualBound == 34);

  // we minimize so primal bound should be greater than or equal to dual bound
  REQUIRE(eventHandler.data.heuristicPrimalBound == 34);
  REQUIRE(eventHandler.data.primalBound == 34);
}

void check_param(VwsEventHandler& eventHandler){
  // the further we solve, the better the dual bound should be
  REQUIRE(isVal(eventHandler.data.lpBound, 19.09, .01));
  REQUIRE(eventHandler.data.dualBound == 31);

  // we minimize so primal bound should be greater than or equal to dual bound
  REQUIRE(eventHandler.data.heuristicPrimalBound == 31);
  REQUIRE(eventHandler.data.primalBound == 31);
}

void check_bm23_data(VwsEventHandler& eventHandler){
  // algorithm step time comparisons
  REQUIRE(0 < eventHandler.data.vpcGenerationTime);
  REQUIRE(eventHandler.data.vpcGenerationTime < eventHandler.data.heuristicTime);
  REQUIRE(eventHandler.data.heuristicTime < eventHandler.data.rootDualBoundTime);
  REQUIRE(eventHandler.data.rootDualBoundTime < eventHandler.data.terminationTime);
  REQUIRE(eventHandler.data.terminationTime < 2);

  // solution time comparisons
  REQUIRE(eventHandler.data.vpcGenerationTime < eventHandler.data.firstSolutionTime);
  REQUIRE(eventHandler.data.firstSolutionTime < eventHandler.data.bestSolutionTime);
  REQUIRE(eventHandler.data.bestSolutionTime < eventHandler.data.terminationTime);

  // check the run parameters
  // make this was not a benchmark run
  REQUIRE(eventHandler.data.maxTime == 600);
  REQUIRE(eventHandler.data.terms == 64);
  REQUIRE(eventHandler.data.iterations > 1000);
  REQUIRE(eventHandler.data.nodes > 10);
}

TEST_CASE( "Check event handler stats", "[VwsSolverInterface::solve]" ){
  
  // instance solver
  fs::path inputPath("../src/test/datasets/bm23/bm23_i01.mps");
  OsiClpSolverInterface si;
  si.readMps(inputPath.c_str(), true, false);
  si.initialSolve();
  
  // series solver
  VwsSolverInterface seriesSolver(10, 600, 64, .8);

  // ------------------------ no vpcs ------------------------------------------

  // solve
  VwsEventHandler eventHandler;
  seriesSolver.solve(si, "None", eventHandler);
  
  // check the event handler
  REQUIRE(!eventHandler.cuts);
  REQUIRE(isVal(eventHandler.data.disjunctiveDualBound, 20.57, .01));
  REQUIRE(isVal(eventHandler.data.lpBoundPostVpc, 20.57, .01));
  REQUIRE(isVal(eventHandler.data.rootDualBound, 25.25, .01));
  REQUIRE(eventHandler.data.vpcGenerator == "None");
  check_orig(eventHandler);
  check_bm23_data(eventHandler);

  // ------------------------ New Disjunction ----------------------------------

  // solve
  eventHandler = VwsEventHandler();
  seriesSolver.solve(si, "New Disjunction", eventHandler);

  // check the event handler
  REQUIRE(eventHandler.cuts);
  REQUIRE(isVal(eventHandler.data.disjunctiveDualBound, 30.17, .01));
  REQUIRE(isVal(eventHandler.data.lpBoundPostVpc, 29.98, .01));
  REQUIRE(isVal(eventHandler.data.rootDualBound, 30.12, .01));
  REQUIRE(eventHandler.data.vpcGenerator == "New Disjunction");
  check_orig(eventHandler);
  check_bm23_data(eventHandler);

  // ------------------------ Old Disjunction ----------------------------------

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

  // solve with old disjunction
  eventHandler = VwsEventHandler();
  seriesSolver.solve(si, "Old Disjunction", eventHandler);

  // check the event handler
  REQUIRE(eventHandler.cuts);
  REQUIRE(isVal(eventHandler.data.disjunctiveDualBound, 22.91, .01));
  REQUIRE(isVal(eventHandler.data.lpBoundPostVpc, 22.77, .01));
  REQUIRE(isVal(eventHandler.data.rootDualBound, 24.96, .01));
  REQUIRE(eventHandler.data.vpcGenerator == "Old Disjunction");
  check_param(eventHandler);
  check_bm23_data(eventHandler);

  // ------------------------ Farkas ------------------------------------------

  // solve with no old disjunction
  eventHandler = VwsEventHandler();
  seriesSolver.solve(si, "Farkas", eventHandler);

  // check the event handler
  REQUIRE(eventHandler.cuts);
  REQUIRE(isVal(eventHandler.data.disjunctiveDualBound, 22.91, .01));
  REQUIRE(isVal(eventHandler.data.lpBoundPostVpc, 19.1, .01));
  REQUIRE(isVal(eventHandler.data.rootDualBound, 25.3, .01));
  REQUIRE(eventHandler.data.vpcGenerator == "Farkas");
  check_param(eventHandler);
  check_bm23_data(eventHandler);
  
}
