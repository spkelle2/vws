/**
 * @file TestMipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

#define CATCH_CONFIG_MAIN

// standard library
#include <filesystem> // path

// unit test library
#include "catch.hpp"

// project modules
#include "MipComp.hpp"

// namespaces
namespace fs = std::__fs::filesystem;


TEST_CASE( "MipComp::MipComp", "[MipComp]" ) {

  MipComp testRunner("../src/test/datasets/testfiles/rhs_series_2_short.test",
                     "../src/test/datasets/solutions");

  REQUIRE( testRunner.timeout == 20 );
  REQUIRE( testRunner.mipModels.size() == 3 );
  REQUIRE( fs::exists("../src/test/datasets/solutions") );
}

TEST_CASE( "MipComp::solveSeriesSavesSolution", "[MipComp]" ) {

  std::string solutionDirectoryStr = "../src/test/datasets/solutions";
  fs::path solutionFile = solutionDirectoryStr + "/bm23.sol";
  std::string str, variableName;
  int lineIndex = 0;
  std::regex re("([a-zA-Z0-9_]+)\\s([0-9\\.]+)");
  std::smatch match;

  // run the simple .test file
  MipComp testRunner("../src/test/datasets/testfiles/bm23.test", solutionDirectoryStr.c_str());
  testRunner.solveSeries();

  // make sure we found a solution and saved it
  REQUIRE(testRunner.dualBounds[0] <= testRunner.primalBounds[0] + .0001);
  REQUIRE(fs::exists(solutionFile));

  // make sure the solution saved correctly
  std::ifstream file(solutionFile.string());
  while (std::getline(file, str)){
    if (std::regex_search(str, match, re)){
      REQUIRE(match[1].str() == testRunner.mipModels[0].solver()->getColName(lineIndex, 0));
      REQUIRE((std::stoi(match[2].str()) == 0 || std::stoi(match[2].str()) == 1));
    }
    lineIndex++;
  }
}

TEST_CASE( "MipComp::solveSeries", "[MipComp]" ) {

  MipComp testRunner("../src/test/datasets/testfiles/rhs_series_2_short.test",
                     "../src/test/datasets/solutions");
  testRunner.solveSeries();

  for (int i = 0; i < testRunner.mipModels.size(); i++){
    // the further we solve, the better the dual bound should be
    REQUIRE(testRunner.lpBounds[i] <= testRunner.rootDualBounds[i] + .0001);
    REQUIRE(testRunner.rootDualBounds[i] <= testRunner.dualBounds[i] + .0001);

    // we minimize so primal bound should be greater than or equal to dual bound
    REQUIRE(testRunner.dualBounds[i] <= testRunner.primalBounds[i] + .0001);

    // the further we solve, the longer it should take
    REQUIRE(testRunner.rootDualBoundTimes[i] <= testRunner.terminationTimes[i]);
  }

}