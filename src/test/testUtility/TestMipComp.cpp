/**
 * @file TestMipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

#define CATCH_CONFIG_MAIN

// standard library
#include <cstdlib> // abs
#include <ghc/filesystem.hpp> // path
#include <string> // string, stod, stoi

// unit test library
#include "catch.hpp"

// vpc modules
#include "utility.hpp"

// project modules
#include "MipComp.hpp"

// namespaces
namespace fs = ghc::filesystem;


TEST_CASE( "Test Simple") {

  // shared setup
  fs::path inputFolder = "../src/test/datasets/bm23";
  fs::path solutionDirectory = inputFolder.parent_path().parent_path() / "solutions";
  fs::path csvPath = inputFolder.parent_path().parent_path() / "run_data.csv";

  // clear out any test files
  fs::remove_all(solutionDirectory);
  fs::remove_all(csvPath);

  MipComp testRunner(inputFolder.string(), solutionDirectory.string(),
                     csvPath.string(), 60, "Old Disjunction", 64);

  SECTION("MipComp::MipComp"){
    REQUIRE( testRunner.timeout == 60 );
    REQUIRE( testRunner.terms == 64 );
    REQUIRE( testRunner.vpcGenerator == "Old Disjunction" );
    REQUIRE( testRunner.instanceSolvers.size() == 3 );
    REQUIRE( testRunner.instanceNames[0] == "bm23_i01" );
    REQUIRE( testRunner.instanceNames[1] == "bm23_i02" );
    REQUIRE( testRunner.instanceNames[2] == "bm23_i03" );
    REQUIRE( fs::exists(solutionDirectory) );
    REQUIRE( testRunner.seriesSolver.maxExtraSavedSolutions == 0 );
    REQUIRE( testRunner.seriesSolver.maxRunTime == 60 );
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
    std::regex re("([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),"
                  "([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),"
                  "([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),"
                  "([a-zA-Z ]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+)");
    std::smatch match;

    // the file should exist
    REQUIRE(fs::exists(csvPath));
    std::ifstream file(csvPath.string());

    while (std::getline(file, str)){
      if (0 < lineIndex && lineIndex <= testRunner.instanceSolvers.size() + 1){
        // all rows except the last should match the pattern
        REQUIRE(std::regex_search(str, match, re));
        // lpBound
        REQUIRE(isVal(std::stod(match[1].str()), testRunner.runData[lineIndex - 1].lpBound, 1e-4));
        // disjunctiveDualBound
        REQUIRE(isVal(std::stod(match[2].str()), testRunner.runData[lineIndex - 1].disjunctiveDualBound, 1e-4));
        // lpBoundPostVpc
        REQUIRE(isVal(std::stod(match[3].str()), testRunner.runData[lineIndex - 1].lpBoundPostVpc, 1e-4));
        // rootDualBoundPreVpc
        REQUIRE(isVal(std::stod(match[4].str()), testRunner.runData[lineIndex - 1].rootDualBoundPreVpc, 1e-4));
        // rootDualBound
        REQUIRE(isVal(std::stod(match[5].str()), testRunner.runData[lineIndex - 1].rootDualBound, 1e-4));
        // dualBound
        REQUIRE(isVal(std::stod(match[6].str()), testRunner.runData[lineIndex - 1].dualBound, 1e-4));
        // heuristicPrimalBound
        REQUIRE(isVal(std::stod(match[7].str()), testRunner.runData[lineIndex - 1].heuristicPrimalBound, 1e-4));
        // primalBound
        REQUIRE(isVal(std::stod(match[8].str()), testRunner.runData[lineIndex - 1].primalBound, 1e-4));
        // vpcGenerationTime
        REQUIRE(isVal(std::stod(match[9].str()), testRunner.runData[lineIndex - 1].vpcGenerationTime, 1e-4));
        // heuristicTime
        REQUIRE(isVal(std::stod(match[10].str()), testRunner.runData[lineIndex - 1].heuristicTime, 1e-4));
        // rootDualBoundTime
        REQUIRE(isVal(std::stod(match[11].str()), testRunner.runData[lineIndex - 1].rootDualBoundTime, 1e-4));
        // firstSolutionTime
        REQUIRE(isVal(std::stod(match[12].str()), testRunner.runData[lineIndex - 1].firstSolutionTime, 1e-4));
        // bestSolutionTime
        REQUIRE(isVal(std::stod(match[13].str()), testRunner.runData[lineIndex - 1].bestSolutionTime, 1e-4));
        // terminationTime
        REQUIRE(isVal(std::stod(match[14].str()), testRunner.runData[lineIndex - 1].terminationTime, 1e-4));
        // maxTime
        REQUIRE(isVal(std::stod(match[15].str()), testRunner.runData[lineIndex - 1].maxTime, 1e-4));
        // VPC generator
        REQUIRE(match[16].str() == (lineIndex == 1 ? "New Disjunction" : testRunner.vpcGenerator));
        // terms
        REQUIRE(isVal(std::stoi(match[17].str()), testRunner.runData[lineIndex - 1].terms, 1e-4));
        // iterations
        REQUIRE(isVal(std::stoi(match[18].str()), testRunner.runData[lineIndex - 1].iterations, 1e-4));
        // nodes
        REQUIRE(isVal(std::stoi(match[19].str()), testRunner.runData[lineIndex - 1].nodes, 1e-4));
      }
      lineIndex++;
    }
    file.close();
  }

  // clear out any test files
  fs::remove_all(solutionDirectory);
  fs::remove_all(csvPath);
}