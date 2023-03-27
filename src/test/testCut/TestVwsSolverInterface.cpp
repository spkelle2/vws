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

// project unit test modules
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

void checkSolutions(VwsSolverInterface &seriesSolver, OsiClpSolverInterface &instanceSolver,
                    int probIdx){

  for (int i = 0; i < seriesSolver.solutions[probIdx].size(); i++){

    // the solution should be the same dimension as the number of variables
    REQUIRE(seriesSolver.solutions[probIdx][i].size() == instanceSolver.getNumCols());
    for (int j = 0; j < seriesSolver.solutions[0][i].size(); j++){
      // the solution should be integer if column requires it
      if (instanceSolver.isInteger(j)){
        REQUIRE((seriesSolver.solutions[probIdx][i][j] == 0 ||
                 seriesSolver.solutions[probIdx][i][j] == 1));
      }
      if (i == 0){
        // the solution should match that placed back in the instanceSolver
        REQUIRE(std::abs(seriesSolver.solutions[probIdx][i][j] - instanceSolver.getColSolution()[j]) < .0001);
      }
    }
  }
}

TEST_CASE("Test a single solve", "[VwsSolverInterface::solve]") {

  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver);

  // check that row and column names are correct
  checkNames(seriesSolver, 0);

  // check that we have an optimal solution
  REQUIRE(instanceSolver.getObjValue() == 34);

  // we only solve one problem
  REQUIRE(seriesSolver.solutions.size() == 1);

  // the problem should have one solution with heuristics turned on now
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, 0);
}

TEST_CASE( "Solve a second time", "[VwsSolverInterface::solve]" ){

  // solve a model with the VwsSolverInterface
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver);

  // check on rerun that we get acceptable duplication
  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  seriesSolver.solve(instanceSolver);

  // check that row and column names are correct
  checkNames(seriesSolver, 1);

  // check that we have an optimal solution
  REQUIRE(instanceSolver.getObjValue() == 34);

  // we've solved two problems now
  REQUIRE(seriesSolver.solutions.size() == 2);

  // the problem should have one solution
  REQUIRE(seriesSolver.solutions[1].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, 1);
}

TEST_CASE( "Solve without preprocessing", "[VwsSolverInterface::solve]" ) {
  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver, "None", false);

  // check that row and column names are correct
  checkNames(seriesSolver, 0);

  // check that we have the same optimal solution
  REQUIRE(instanceSolver.getObjValue() == 34);

  // we only solve one problem
  REQUIRE(seriesSolver.solutions.size() == 1);

  // the problem should have one solution now that heuristics work
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, 0);
}

// make sure the above wasn't a fluke because preprocessing had no effect on small problem
TEST_CASE( "Check larger instance preprocessed", "[VwsSolverInterface::solve][long]" ) {
  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver =
      extractSolverInterfaceFromGunzip("../src/test/datasets/vary_rhs/series_2/rhs_s2_i01.mps.gz");
  VwsSolverInterface seriesSolver(10, 60, 16);
  seriesSolver.solve(instanceSolver);

  // check that we have valid bounds
  REQUIRE((-17170 < instanceSolver.getObjValue() && instanceSolver.getObjValue() < -17150));

  // the problem should have two solutions since we didn't specify a max
  REQUIRE(seriesSolver.solutions[0].size() == 2);
  checkSolutions(seriesSolver, instanceSolver, 0);
}

TEST_CASE( "Check larger instance unprocessed", "[VwsSolverInterface::solve][long]" ) {
  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver =
      extractSolverInterfaceFromGunzip("../src/test/datasets/vary_rhs/series_2/rhs_s2_i01.mps.gz");
  VwsSolverInterface seriesSolver(10, 60, 16);
  seriesSolver.maxExtraSavedSolutions = 0;
  seriesSolver.solve(instanceSolver, "None", false);

  // check that we have valid bounds
  REQUIRE((-17170 < instanceSolver.getObjValue() && instanceSolver.getObjValue() < -17150));

  // the problem should have one solution since we don't save extras
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, 0);
}

TEST_CASE( "Check solve with VPCs added", "[VwsSolverInterface::solve]" ) {

  // todo check this works for both PRLP and Farkas

  // solve a model with the VwsSolverInterface
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  VwsSolverInterface seriesSolver(10, 60, 4);
  CbcModel model = seriesSolver.solve(instanceSolver, "PRLP");

  // we should have one vpc generator and set of farkas multipliers
  REQUIRE(seriesSolver.vpcGenerators.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);

  // we should have added 6 cuts (for the number of fractional variables in the root LP relaxation)
  REQUIRE(instanceSolver.getNumRows() == 26);
  REQUIRE(seriesSolver.cutCertificates[0].size() == 6);

  // each cut should have 4 disjunctive terms
  for (int cutIdx=0; cutIdx<seriesSolver.cutCertificates[0].size(); cutIdx++) {
    REQUIRE(seriesSolver.cutCertificates[0][cutIdx].size() == 4);
    // each disjunctive term should have 49 coefficients (27 variables, 20 constraints, 2 branching decisions)
    for (int termIdx=0; termIdx<seriesSolver.cutCertificates[0][cutIdx].size(); termIdx++) {
      REQUIRE(seriesSolver.cutCertificates[0][cutIdx][termIdx].size() == 49);
    }
  }

  // check the primal solutions
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, 0);

  // check the bounds - root LP relaxation should be much improved from VPCs
  REQUIRE(instanceSolver.getObjValue() == 34);
  REQUIRE(model.getObjValue() == 34);
  REQUIRE((22 < model.getContinuousObjective() && model.getContinuousObjective() < 23));

}

TEST_CASE( "Check solve longer with VPCs added", "[VwsSolverInterface::solve][long]" ) {
  // todo check that expanded API works
  // todo check that we see an improved dual bound at the root and an otherwise same solution
  // todo check this works for both PRLP and Farkas

  // solve a model with the VwsSolverInterface
  OsiClpSolverInterface instanceSolver =
      extractSolverInterfaceFromGunzip("../src/test/datasets/vary_rhs/series_2/rhs_s2_i01.mps.gz");
  VwsSolverInterface seriesSolver(10, 60, 4);
  CbcModel model = seriesSolver.solve(instanceSolver, "PRLP");

  // we should have one vpc generator and set of farkas multipliers
  REQUIRE(seriesSolver.vpcGenerators.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);

  // we should have added 6 cuts (for the number of fractional variables in the root LP relaxation)
  REQUIRE(instanceSolver.getNumRows() > 1250);
  REQUIRE(seriesSolver.cutCertificates[0].size() > 0);

  // each cut should have 4 disjunctive terms
  for (int cutIdx=0; cutIdx<seriesSolver.cutCertificates[0].size(); cutIdx++) {
    REQUIRE(seriesSolver.cutCertificates[0][cutIdx].size() == 4);
    // each disjunctive term should have 2252 coefficients (1000 variables, 1250 constraints, 2 branching decisions)
    for (int termIdx=0; termIdx<seriesSolver.cutCertificates[0][cutIdx].size(); termIdx++) {
      REQUIRE(seriesSolver.cutCertificates[0][cutIdx][termIdx].size() == 2252);
    }
  }

  // check the primal solutions
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, 0);

  // check the bounds are valid
  REQUIRE((-17170 < instanceSolver.getObjValue() && instanceSolver.getObjValue() < -17150));

}

TEST_CASE( "Check VPCs created successfully from PRLP",
           "[VwsSolverInterface::createDisjunctiveCutsFromPRLP]" ) {
  // todo check that we get a set of valid cuts returned
}

TEST_CASE( "Check VPCs created successfully from Farkas",
           "[VwsSolverInterface::createDisjunctiveCutsFromFarkasMultipliers]" ) {
  // todo check that we get a set of valid cuts returned
}