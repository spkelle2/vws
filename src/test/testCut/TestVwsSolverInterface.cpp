/**
 * @file TestVwsSolverInterface.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

#define CATCH_CONFIG_MAIN

// unit test library
#include "catch.hpp"

// coin-or modules
#include <CbcModel.hpp>

// project unit test modules
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp"

TEST_CASE( "VwsSolverInterface::VwsSolverInterface", "[VwsSolverInterface]" ) {

  // Ensure we set up as expected
  CbcModel model = extractModelFromGunzip("../src/test/bm23.mps.gz");
  VwsSolverInterface solver(5);

  REQUIRE(solver.maxSavedSolutions == 5);
}


TEST_CASE( "VwsSolverInterface::solve", "[VwsSolverInterface]" ) {

  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  CbcModel model = extractModelFromGunzip("../src/test/bm23.mps.gz");
  VwsSolverInterface solver;
  solver.solve(model);

  REQUIRE(model.getBestPossibleObjValue() == model.getObjValue());

  // we only solve one problem
  REQUIRE(solver.solutions.size() == 1);

  // the problem should have more than one solution
  REQUIRE(solver.solutions[1].size() > 1);

}

