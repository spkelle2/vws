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
  REQUIRE(seriesSolver.maxSavedSolutions == 5);
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
    REQUIRE(seriesSolver.variableNames[0][i] == "C10" + std::to_string(i + 1));
  }
  REQUIRE(seriesSolver.constraintNames.size() == 1);
  REQUIRE(seriesSolver.constraintNames[0].size() == 20);
  for (int i = 0; i < seriesSolver.constraintNames[0].size(); i++){
    REQUIRE(seriesSolver.variableNames[0][i] == "R10" + std::to_string(i + 1));
  }

  // check that we have an optimal solution
  REQUIRE(instanceSolver.getObjValue() == 34);
  REQUIRE(seriesSolver.dualBounds[0] == 34);

  // we only solve one problem
  REQUIRE(seriesSolver.solutions.size() == 1);

  // the problem should have one solution right now
  REQUIRE(seriesSolver.solutions[0].size() == 2);

  // the solution should be the same dimension as the number of variables
  REQUIRE(seriesSolver.solutions[0][0].size() == 27);

  for (int i = 0; i < seriesSolver.solutions[0].size(); i++){
    for (int j = 0; j < seriesSolver.solutions[0][0].size(); i++){
      // the solution should be integer
      REQUIRE((seriesSolver.solutions[1][i][j] == 0 || seriesSolver.solutions[1][i][j] == 1));
      if (i == 0){
        // the solution should match that placed back in the instanceSolver
        REQUIRE(seriesSolver.solutions[1][i][j] == instanceSolver.getColSolution()[j]);
      }
    }
  }

}

