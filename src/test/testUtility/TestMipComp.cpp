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
  fs::path csvPath = inputFolder.parent_path().parent_path() / "run_data.csv";

  // clear out any test files
  fs::remove_all(csvPath);

  SECTION("MipComp::MipComp"){
    MipComp testRunner(inputFolder.string(), csvPath.string(), 60, "Old Disjunction", 64);

    REQUIRE( testRunner.timeout == 60 );
    REQUIRE( testRunner.terms == 64 );
    REQUIRE( testRunner.vpcGenerator == "Old Disjunction" );
    REQUIRE( testRunner.instanceSolvers.size() == 3 );
    REQUIRE( testRunner.instanceNames[0] == "bm23_i01" );
    REQUIRE( testRunner.instanceNames[1] == "bm23_i02" );
    REQUIRE( testRunner.instanceNames[2] == "bm23_i03" );
    REQUIRE( testRunner.seriesSolver.maxExtraSavedSolutions == 0 );
    REQUIRE( testRunner.seriesSolver.maxRunTime == 60 );
  }

  SECTION("MipComp::solveSeries quits if no cuts on first instance"){
    // test that series cancels after first instance doesn't generate cuts
    MipComp testRunner(inputFolder.string(), csvPath.string(), 1, "Old Disjunction", 128);
    testRunner.solveSeries();
    REQUIRE(testRunner.runData.size() == 1);
  }

  SECTION("MipComp::solveSeries"){
    MipComp testRunner(inputFolder.string(), csvPath.string(), 60, "Old Disjunction", 64);
    testRunner.solveSeries();

    // we should have one runData entry for each instance
    REQUIRE( testRunner.runData.size() == 3 );

    // make sure we saved the run data correctly
    std::string str;
    int lineIndex = 0;
    std::regex re("([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),"
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
        // rootDualBound
        REQUIRE(isVal(std::stod(match[4].str()), testRunner.runData[lineIndex - 1].rootDualBound, 1e-4));
        // dualBound
        REQUIRE(isVal(std::stod(match[5].str()), testRunner.runData[lineIndex - 1].dualBound, 1e-4));
        // heuristicPrimalBound
        REQUIRE(isVal(std::stod(match[6].str()), testRunner.runData[lineIndex - 1].heuristicPrimalBound, 1e-4));
        // primalBound
        REQUIRE(isVal(std::stod(match[7].str()), testRunner.runData[lineIndex - 1].primalBound, 1e-4));
        // vpcGenerationTime
        REQUIRE(isVal(std::stod(match[8].str()), testRunner.runData[lineIndex - 1].vpcGenerationTime, 1e-4));
        // heuristicTime
        REQUIRE(isVal(std::stod(match[9].str()), testRunner.runData[lineIndex - 1].heuristicTime, 1e-4));
        // rootDualBoundTime
        REQUIRE(isVal(std::stod(match[10].str()), testRunner.runData[lineIndex - 1].rootDualBoundTime, 1e-4));
        // firstSolutionTime
        REQUIRE(isVal(std::stod(match[11].str()), testRunner.runData[lineIndex - 1].firstSolutionTime, 1e-4));
        // bestSolutionTime
        REQUIRE(isVal(std::stod(match[12].str()), testRunner.runData[lineIndex - 1].bestSolutionTime, 1e-4));
        // terminationTime
        REQUIRE(isVal(std::stod(match[13].str()), testRunner.runData[lineIndex - 1].terminationTime, 1e-4));
        // maxTime
        REQUIRE(isVal(std::stod(match[14].str()), testRunner.runData[lineIndex - 1].maxTime, 1e-4));
        // VPC generator
        REQUIRE(match[15].str() == (lineIndex == 1 ? "New Disjunction" : testRunner.vpcGenerator));
        // terms
        REQUIRE(isVal(std::stoi(match[16].str()), testRunner.runData[lineIndex - 1].terms, 1e-4));
        // iterations
        REQUIRE(isVal(std::stoi(match[17].str()), testRunner.runData[lineIndex - 1].iterations, 1e-4));
        // nodes
        REQUIRE(isVal(std::stoi(match[18].str()), testRunner.runData[lineIndex - 1].nodes, 1e-4));
      }
      lineIndex++;
    }
    file.close();
  }

  // clear out any test files
  fs::remove_all(csvPath);
}