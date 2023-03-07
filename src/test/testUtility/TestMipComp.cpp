/**
 * @file TestMipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

#define CATCH_CONFIG_MAIN

// standard library
#include <cstdlib> // abs
#include <filesystem> // path

// unit test library
#include "catch.hpp"

// project modules
#include "MipComp.hpp"

// namespaces
namespace fs = std::__fs::filesystem;


TEST_CASE( "Test Simple") {

  // shared setup
  fs::path filePath = "../src/test/datasets/testfiles/bm23.test";
  fs::path solutionDirectory = filePath.parent_path().parent_path() / "solutions";
  fs::path csvPath = filePath.parent_path().parent_path() / "run_data.csv";

  // clear out any test files
  fs::remove_all(solutionDirectory);
  fs::remove_all(csvPath);

  MipComp testRunner(filePath.string().c_str(), solutionDirectory.string().c_str(),
                     csvPath.string().c_str(), true);

  SECTION("MipComp::MipComp"){
    REQUIRE( testRunner.usePreprocessing );
    REQUIRE( testRunner.timeout == 60 );
    REQUIRE( testRunner.instanceSolvers.size() == 3 );
    REQUIRE( testRunner.instanceNames[0] == "bm23_i01" );
    REQUIRE( testRunner.instanceNames[1] == "bm23_i02" );
    REQUIRE( testRunner.instanceNames[2] == "bm23_i03" );
    REQUIRE( fs::exists(solutionDirectory) );
    REQUIRE( testRunner.seriesSolver.maxExtraSavedSolutions == 100 );
    REQUIRE( testRunner.seriesSolver.maxRunTime == 50 );
  }

  SECTION("MipComp::solveSeries"){
    testRunner.solveSeries();

    // we should have one runData entry for each instance
    REQUIRE( testRunner.runData.size() == 3 );

    for (int i = 0; i < testRunner.instanceSolvers.size(); i++){

      std::string str;
      int lineIndex = 0;
      std::regex re("([a-zA-Z0-9_]+)\\s([0-9\\.]+)");
      std::smatch match;

      // make sure we saved the best solution
      fs::path solutionFile = solutionDirectory / (testRunner.instanceNames[i] + ".sol");
      REQUIRE(fs::exists(solutionFile));

      // the further we solve, the better the dual bound should be
      REQUIRE(std::abs(testRunner.runData[i].lpBound - 20.57) < .01);
      REQUIRE(std::abs(testRunner.runData[i].rootDualBound - 25.39) < .01);
      REQUIRE(testRunner.runData[i].dualBound == 34);

      // we minimize so primal bound should be greater than or equal to dual bound
      REQUIRE(testRunner.runData[i].primalBound == 34);

      // the further we solve, the longer it should take
      REQUIRE(0 <= testRunner.runData[i].heuristicTime);
      REQUIRE(testRunner.runData[i].heuristicTime < testRunner.runData[i].rootDualBoundTime);
      REQUIRE(testRunner.runData[i].rootDualBoundTime < testRunner.runData[i].terminationTime);
      REQUIRE(testRunner.runData[i].terminationTime < 1);

      // make sure the solution saved correctly
      std::ifstream file(solutionFile.string());
      while (std::getline(file, str)){
        if (lineIndex < testRunner.instanceSolvers[i].getNumCols()){
          // all rows except the last should match the pattern
          REQUIRE(std::regex_search(str, match, re));
          // each entry's name should match the corresponding column
          REQUIRE(match[1].str() == testRunner.seriesSolver.variableNames[i][lineIndex]);
          // all variables are binary
          REQUIRE((std::stoi(match[2].str()) == 0 || std::stoi(match[2].str()) == 1));
          // each entry's value should match the corresponding solution
          REQUIRE(std::stoi(match[2].str()) == testRunner.seriesSolver.solutions[i][0][lineIndex]);
        }
        lineIndex++;
      }
      file.close();
    }

    // make sure we saved the run data correctly
    std::string str;
    int lineIndex = 0;
    std::regex re("([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+)");
    std::smatch match;

    // the file should exist
    REQUIRE(fs::exists(csvPath));
    std::ifstream file(csvPath.string());

    while (std::getline(file, str)){
      if (0 < lineIndex && lineIndex <= testRunner.instanceSolvers.size() + 1){
        // all rows except the last should match the pattern
        REQUIRE(std::regex_search(str, match, re));
        // each entry should match the expected value
        REQUIRE(std::abs(std::stod(match[1].str()) - 20.57) < .01);
        REQUIRE(std::abs(std::stod(match[2].str()) - 25.39) < .01);
        REQUIRE(std::stod(match[3].str()) == 34);
        REQUIRE(std::stod(match[4].str()) == 34);
        REQUIRE(std::stod(match[5].str()) == 0);
        REQUIRE(0 < std::stod(match[6].str()));
        REQUIRE(std::stod(match[6].str()) < std::stod(match[7].str()));
        REQUIRE(std::stod(match[7].str()) < 1);
      }
      lineIndex++;
    }
    file.close();
  }

  // clear out any test files
  fs::remove_all(solutionDirectory);
  fs::remove_all(csvPath);
}