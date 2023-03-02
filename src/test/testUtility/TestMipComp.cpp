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

fs::path filePath = "../src/test/datasets/testfiles/rhs_series_2_short.test";
fs::path solutionDirectory = filePath.parent_path().parent_path() / "solutions";
fs::path csvPath = filePath.parent_path().parent_path() / "run_data/test.csv";

TEST_CASE( "MipComp::MipComp", "[MipComp]" ) {

  MipComp testRunner(filePath.string().c_str(), solutionDirectory.string().c_str(),
                     csvPath.string().c_str());

  REQUIRE( testRunner.timeout == 60 );
  REQUIRE( testRunner.mipModels.size() == 3 );
  REQUIRE( testRunner.instanceNames[0] == "rhs_s2_i01" );
  REQUIRE( testRunner.instanceNames[1] == "rhs_s2_i02" );
  REQUIRE( testRunner.instanceNames[2] == "rhs_s2_i03" );
  REQUIRE( fs::exists("../src/test/datasets/solutions/rhs_series_2_short") );
  REQUIRE( testRunner.solver.maxExtraSavedSolutions == 100 );
  REQUIRE( testRunner.solver.maxRunTime == 50 );
}

TEST_CASE( "MipComp::solveSeries", "[MipComp]" ) {

  std::string str, variableName;
  int lineIndex = 0;
  std::regex re("([a-zA-Z0-9_]+)\\s([0-9\\.]+)");
  std::smatch match;

  // run the simple .test file
  MipComp testRunner(filePath.string().c_str(), solutionDirectory.string().c_str(),
                     csvPath.string().c_str());
  testRunner.solveSeries();

  for (int i = 0; i < testRunner.mipModels.size(); i++){

    fs::path solutionFile = solutionDirectory / (testRunner.instanceNames[i] + ".sol");

    // check to make sure the model was run
    REQUIRE(testRunner.model[i].status == 1);

    // the further we solve, the better the dual bound should be
    REQUIRE(-18000 <= testRunner.lpBounds[i]);
    REQUIRE(testRunner.lpBounds[i] <= testRunner.rootDualBounds[i] + .0001);
    REQUIRE(testRunner.rootDualBounds[i] <= testRunner.dualBounds[i] + .0001);

    // we minimize so primal bound should be greater than or equal to dual bound
    REQUIRE(testRunner.dualBounds[i] <= testRunner.primalBounds[i] + .0001);

    // the further we solve, the longer it should take
    REQUIRE(0 < testRunner.rootDualBoundTimes[i]);
    REQUIRE(testRunner.rootDualBoundTimes[i] <= testRunner.terminationTimes[i]);
    REQUIRE(testRunner.rootDualBoundTimes[i] <= testRunner.terminationTimes[i]);
    REQUIRE(testRunner.terminationTimes[i] <= 60);

    // make sure we found both solutions and saved the best one
    REQUIRE(testRunner.dualBounds[i] <= testRunner.primalBounds[i] + .0001);
    REQUIRE(testRunner.primalBounds[i] <= -16000);
    REQUIRE(fs::exists(solutionFile));

    // make sure the solution saved correctly
    std::ifstream file(solutionFile.string());
    while (std::getline(file, str)){
      if (lineIndex < testRunner.mipModels[i].solver()->getNumCols()){
        // all rows except the last should match the pattern
        REQUIRE(std::regex_search(str, match, re));
        // each entry's name should match the corresponding column
        REQUIRE(match[1].str() == testRunner.mipModels[0].solver()->getColName(lineIndex, 0));
        // if the entry is an integer, it should be 0 or 1
        if testRunner.mipModels[i].isInteger(lineIndex){
          REQUIRE((std::stoi(match[2].str()) == 0 || std::stoi(match[2].str()) == 1));
        }
      }
      lineIndex++;
    }
  }
}