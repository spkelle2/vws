/**
 * @file TestVwsSolverInterface.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

#define CATCH_CONFIG_MAIN

// unit test library
#include "catch.hpp"

// project unit test modules
#include "VwsSolverInterface.hpp"


TEST_CASE( "VwsSolverInterface::VwsSolverInterface", "[VwsSolverInterface]" ) {

  VwsSolverInterface solverInterface();

  REQUIRE(false);
}
