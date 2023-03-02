/**
 * @file TestVwsSolverInterface.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

#define CATCH_CONFIG_MAIN

// unit test library
#include "catch.hpp"

// coin-or modules
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface

// project unit test modules
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp"

TEST_CASE( "VwsSolverInterface::VwsSolverInterface", "[VwsSolverInterface]" ) {

  // Ensure we set up as expected
  VwsSolverInterface seriesSolver(5, 50, 16);
  REQUIRE(seriesSolver.maxExtraSavedSolutions == 5);
  REQUIRE(seriesSolver.maxRunTime == 50);
  REQUIRE(seriesSolver.disjunctiveTerms == 16);
}

TEST_CASE( "VwsSolverInterface::solve", "[VwsSolverInterface]" ) {

  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/bm23.mps.gz");
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver);

  // check that row and column names are correct
  REQUIRE(seriesSolver.variableNames.size() == 1);
  REQUIRE(seriesSolver.variableNames[0].size() == 27);
  for (int i = 0; i < seriesSolver.variableNames[0].size(); i++){
    std::string name = i < 9 ? "C10" + std::to_string(i + 1) : "C1" + std::to_string(i + 1);
    REQUIRE(seriesSolver.variableNames[0][i] == name);
  }
  REQUIRE(seriesSolver.constraintNames.size() == 1);
  REQUIRE(seriesSolver.constraintNames[0].size() == 20);
  for (int i = 0; i < seriesSolver.constraintNames[0].size(); i++){
    std::string name = i < 9 ? "R10" + std::to_string(i + 1) : "R1" + std::to_string(i + 1);
    REQUIRE(seriesSolver.constraintNames[0][i] == name);
  }

  // check that we have an optimal solution
  REQUIRE(instanceSolver.getObjValue() == 34);
  REQUIRE(seriesSolver.dualBounds[0] == 34);

  // we only solve one problem
  REQUIRE(seriesSolver.solutions.size() == 1);

  // the problem should have five solutions
  REQUIRE(seriesSolver.solutions[0].size() == 5);

  for (int i = 0; i < seriesSolver.solutions[0].size(); i++){

    // the solution should be the same dimension as the number of variables
    REQUIRE(seriesSolver.solutions[0][i].size() == 27);

    for (int j = 0; j < seriesSolver.solutions[0][i].size(); j++){
      // the solution should be integer
      REQUIRE((seriesSolver.solutions[0][i][j] == 0 || seriesSolver.solutions[0][i][j] == 1));
      if (i == 0){
        // the solution should match that placed back in the instanceSolver
        REQUIRE(seriesSolver.solutions[0][i][j] == instanceSolver.getColSolution()[j]);
      }
    }
  }

  // check on rerun that we get acceptable duplication
  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/bm23.mps.gz");
  seriesSolver.maxExtraSavedSolutions = 0;
  seriesSolver.solve(instanceSolver);

  // check that row and column names are correct
  REQUIRE(seriesSolver.variableNames.size() == 2);
  REQUIRE(seriesSolver.variableNames[1].size() == 27);
  for (int i = 0; i < seriesSolver.variableNames[1].size(); i++){
    std::string name = i < 9 ? "C10" + std::to_string(i + 1) : "C1" + std::to_string(i + 1);
    REQUIRE(seriesSolver.variableNames[1][i] == name);
  }
  REQUIRE(seriesSolver.constraintNames.size() == 2);
  REQUIRE(seriesSolver.constraintNames[1].size() == 20);
  for (int i = 0; i < seriesSolver.constraintNames[1].size(); i++){
    std::string name = i < 9 ? "R10" + std::to_string(i + 1) : "R1" + std::to_string(i + 1);
    REQUIRE(seriesSolver.constraintNames[1][i] == name);
  }

  // check that we have an optimal solution
  REQUIRE(instanceSolver.getObjValue() == 34);
  REQUIRE(seriesSolver.dualBounds[1] == 34);

  // we've solved two problems now
  REQUIRE(seriesSolver.solutions.size() == 2);

  // the problem should have one solution
  REQUIRE(seriesSolver.solutions[1].size() == 1);

  // the solution should be the same dimension as the number of variables
  REQUIRE(seriesSolver.solutions[1][0].size() == 27);

  for (int j = 0; j < seriesSolver.solutions[1][0].size(); j++){
    // the solution should be integer
    REQUIRE((seriesSolver.solutions[1][0][j] == 0 || seriesSolver.solutions[1][0][j] == 1));
    // the solution should match that placed back in the instanceSolver
    REQUIRE(seriesSolver.solutions[1][0][j] == instanceSolver.getColSolution()[j]);
  }
}

