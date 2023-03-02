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
    REQUIRE(seriesSolver.solutions[probIdx][i].size() == 27);

    for (int j = 0; j < seriesSolver.solutions[0][i].size(); j++){
      // the solution should be integer
      REQUIRE((seriesSolver.solutions[probIdx][i][j] == 0 || seriesSolver.solutions[probIdx][i][j] == 1));
      if (i == 0){
        // the solution should match that placed back in the instanceSolver
        REQUIRE(seriesSolver.solutions[probIdx][i][j] == instanceSolver.getColSolution()[j]);
      }
    }
  }
}

TEST_CASE("Test a single solve", "[VwsSolverInterface::solve]") {

  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/bm23.mps.gz");
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver);

  // check that row and column names are correct
  checkNames(seriesSolver, 0);

  // check that we have an optimal solution
  REQUIRE(instanceSolver.getObjValue() == 34);
  REQUIRE(seriesSolver.dualBounds[0] == 34);

  // we only solve one problem
  REQUIRE(seriesSolver.solutions.size() == 1);

  // the problem should have five solutions
  REQUIRE(seriesSolver.solutions[0].size() == 5);
  checkSolutions(seriesSolver, instanceSolver, 0);
}

TEST_CASE( "Solve a second time with one solution", "[VwsSolverInterface::solve]" ){

  // solve a model with the VwsSolverInterface
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/bm23.mps.gz");
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver);

  // check on rerun that we get acceptable duplication
  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/bm23.mps.gz");
  seriesSolver.maxExtraSavedSolutions = 0;
  seriesSolver.solve(instanceSolver);

  // check that row and column names are correct
  checkNames(seriesSolver, 1);

  // check that we have an optimal solution
  REQUIRE(instanceSolver.getObjValue() == 34);
  REQUIRE(seriesSolver.dualBounds[1] == 34);

  // we've solved two problems now
  REQUIRE(seriesSolver.solutions.size() == 2);

  // the problem should have one solution
  REQUIRE(seriesSolver.solutions[1].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, 1);
}

TEST_CASE( "Solve without preprocessing", "[VwsSolverInterface::solve]" ) {
  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/bm23.mps.gz");
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver, false);

  // check that row and column names are correct
  checkNames(seriesSolver, 0);

  // check that we have the same optimal solution
  REQUIRE(instanceSolver.getObjValue() == 34);
  REQUIRE(seriesSolver.dualBounds[0] == 34);

  // we only solve one problem
  REQUIRE(seriesSolver.solutions.size() == 1);

  // the problem should have five solutions
  REQUIRE(seriesSolver.solutions[0].size() == 5);
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
  REQUIRE((-17161 < instanceSolver.getObjValue() && instanceSolver.getObjValue() < -17160));
  REQUIRE((-17226 < seriesSolver.dualBounds[0] && seriesSolver.dualBounds[0] < -17225));

  // check that best solution is recorded in instanceSolver
  for (int j = 0; j < seriesSolver.solutions[0][0].size(); j++){
    REQUIRE(std::abs(seriesSolver.solutions[0][0][j] - instanceSolver.getColSolution()[j]) < .0001);
  }
}

TEST_CASE( "Check larger instance unprocessed", "[VwsSolverInterface::solve][long]" ) {
  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver =
      extractSolverInterfaceFromGunzip("../src/test/datasets/vary_rhs/series_2/rhs_s2_i01.mps.gz");
  VwsSolverInterface seriesSolver(10, 60, 16);
  seriesSolver.solve(instanceSolver, false);

  // check that we have valid bounds
  REQUIRE((-17151 < instanceSolver.getObjValue() && instanceSolver.getObjValue() < -17150));
  REQUIRE((-17227 < seriesSolver.dualBounds[0] && seriesSolver.dualBounds[0] < -17226));

  // check that best solution is recorded in instanceSolver
  for (int j = 0; j < seriesSolver.solutions[0][0].size(); j++){
    REQUIRE(std::abs(seriesSolver.solutions[0][0][j] - instanceSolver.getColSolution()[j]) < .0001);
  }
}