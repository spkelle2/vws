/**
 * @file TestMipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

#define CATCH_CONFIG_MAIN

// standard library
#include <cstdlib> // abs
#include <filesystem.hpp> // path
#include <string> // string, stod, stoi

// unit test library
#include "catch.hpp"

// vpc modules
#include "utility.hpp"
#include "VPCParameters.hpp" // VPCParameters
using namespace VPCParametersNamespace;

// project modules
#include "MipComp.hpp"

// namespaces
namespace fs = ghc::filesystem;


TEST_CASE( "Test Simple") {

  // shared setup
  fs::path inputFolder = "../src/test/test_instances/bm23";
  fs::path csvPath = inputFolder.parent_path().parent_path() / "run_data.csv";
  fs::path csvPath2 = inputFolder.parent_path().parent_path() / "run_data2.csv";

  // clear out any test files
  fs::remove_all(csvPath);

  SECTION("MipComp::MipComp"){
    MipComp testRunner(inputFolder.string(), csvPath.string(), 60, "Old", 69, "CBC", true, 0);

    REQUIRE( testRunner.vpcGenerator == "Old" );
    REQUIRE( testRunner.instanceSolvers.size() == 3 );
    REQUIRE( testRunner.instanceNames[0] == "bm23_i01" );
    REQUIRE( testRunner.instanceNames[1] == "bm23_i02" );
    REQUIRE( testRunner.instanceNames[2] == "bm23_i03" );
    REQUIRE( testRunner.primalBounds[0] == 34 );
    REQUIRE( testRunner.primalBounds[1] == 34 );
    REQUIRE( testRunner.primalBounds[2] == 34 );
    REQUIRE( testRunner.seriesSolver.params.get(DISJ_TERMS) == 69 );
    REQUIRE( testRunner.seriesSolver.params.get(TIMELIMIT) == 60 );
    REQUIRE( testRunner.seriesSolver.params.get(PARTIAL_BB_KEEP_PRUNED_NODES) == 1 );
    REQUIRE( testRunner.mipSolver == "CBC" );
    REQUIRE( testRunner.seedIndex == 0 );
    REQUIRE( testRunner.tighten_disjunction == false );
    REQUIRE( testRunner.tighten_matrix_perturbation == false );
    REQUIRE( testRunner.tighten_infeasible_to_feasible_term == false );
    REQUIRE( testRunner.tighten_feasible_to_infeasible_basis == false );
  }

  SECTION("MipComp::solveSeries quits if no cuts on first instance"){
    // test that series cancels after first instance doesn't generate cuts
    MipComp testRunner(inputFolder.string(), csvPath.string(), 1, "Old", 128, "CBC", false, 0);
    testRunner.solveSeries();
    REQUIRE(testRunner.runData.size() == 0);
  }

  SECTION("MipComp::solveSeries"){
    MipComp testRunner(inputFolder.string(), csvPath.string(), 60, "Old", 69, "CBC", true, 0);
    testRunner.solveSeries();

    // we should have one runData entry for each instance
    REQUIRE( testRunner.runData.size() == 3 );

    // make sure we saved the run data correctly
    std::string str;
    int lineIndex = 0;
    std::regex re("([0-9\\.]+),([0-9\\.]+),([a-zA-Z ]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),"
                  "([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),"
                  "([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),"
                  "([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([a-zA-Z ]+),"
                  "([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),([0-9\\.]+),"
                  "([0-9\\.]+),([0-9\\.]+),([0-1]),([0-1]),([0-1]),([0-1])");
    std::smatch match;

    // the file should exist
    REQUIRE(fs::exists(csvPath));
    std::ifstream file(csvPath.string());

    while (std::getline(file, str)){
      if (0 < lineIndex && lineIndex <= testRunner.instanceSolvers.size() + 1){
        // all rows except the last should match the pattern
        REQUIRE(std::regex_search(str, match, re));
        // instanceIndex
        REQUIRE(isVal(std::stoi(match[1].str()), testRunner.runData[lineIndex - 1].instanceIndex, 1e-4));
        // seedIndex
        REQUIRE(isVal(std::stoi(match[2].str()), testRunner.runData[lineIndex - 1].seedIndex, 1e-4));
        // VPC generator
        REQUIRE(match[3].str() == (lineIndex == 1 ? "New" : testRunner.vpcGenerator));
        // terms
        REQUIRE(isVal(std::stoi(match[4].str()), testRunner.runData[lineIndex - 1].terms, 1e-4));
        // lpBound
        REQUIRE(isVal(std::stod(match[5].str()), testRunner.runData[lineIndex - 1].lpBound, 1e-4));
        // disjunctiveDualBound
        REQUIRE(isVal(std::stod(match[6].str()), testRunner.runData[lineIndex - 1].disjunctiveDualBound, 1e-4));
        // lpBoundPostVpc
        REQUIRE(isVal(std::stod(match[7].str()), testRunner.runData[lineIndex - 1].lpBoundPostVpc, 1e-4));
        // rootDualBound
        REQUIRE(isVal(std::stod(match[8].str()), testRunner.runData[lineIndex - 1].rootDualBound, 1e-4));
        // dualBound
        REQUIRE(isVal(std::stod(match[9].str()), testRunner.runData[lineIndex - 1].dualBound, 1e-4));
        // primalBound
        REQUIRE(isVal(std::stod(match[10].str()), testRunner.runData[lineIndex - 1].primalBound, 1e-4));
        // vpcGenerationTime
        REQUIRE(isVal(std::stod(match[11].str()), testRunner.runData[lineIndex - 1].vpcGenerationTime, 1e-4));
        // rootDualBoundTime
        REQUIRE(isVal(std::stod(match[12].str()), testRunner.runData[lineIndex - 1].rootDualBoundTime, 1e-4));
        // bestSolutionTime
        REQUIRE(isVal(std::stod(match[13].str()), testRunner.runData[lineIndex - 1].bestSolutionTime, 1e-4));
        // terminationTime
        REQUIRE(isVal(std::stod(match[14].str()), testRunner.runData[lineIndex - 1].terminationTime, 1e-4));
        // nodes
        REQUIRE(isVal(std::stoi(match[15].str()), testRunner.runData[lineIndex - 1].nodes, 1e-4));
        // iterations
        REQUIRE(isVal(std::stoi(match[16].str()), testRunner.runData[lineIndex - 1].iterations, 1e-4));
        // maxTime
        REQUIRE(isVal(std::stod(match[17].str()), testRunner.runData[lineIndex - 1].maxTime, 1e-4));
        // actualTerms
        REQUIRE(isVal(std::stoi(match[18].str()), testRunner.runData[lineIndex - 1].actualTerms, 1e-4));
        // numCuts
        REQUIRE(isVal(std::stod(match[19].str()), testRunner.runData[lineIndex - 1].numCuts, 1e-4));
        // cutLimit
        REQUIRE(isVal(std::stod(match[20].str()), testRunner.runData[lineIndex - 1].cutLimit, 1e-4));
        // mipSolver
        REQUIRE(match[21].str() == testRunner.runData[lineIndex - 1].mipSolver);
        // providePrimalBound
        REQUIRE(isVal(std::stod(match[22].str()), testRunner.runData[lineIndex - 1].providePrimalBound, 1e-4));
        // infeasibleTerms
        REQUIRE(isVal(std::stoi(match[23].str()), testRunner.runData[lineIndex - 1].infeasibleTerms, 1e-4));
        // feasibleToInfeasibleTerms
        REQUIRE(isVal(std::stoi(match[24].str()), testRunner.runData[lineIndex - 1].feasibleToInfeasibleTerms, 1e-4));
        // infeasibleToFeasibleTerms
        REQUIRE(isVal(std::stoi(match[25].str()), testRunner.runData[lineIndex - 1].infeasibleToFeasibleTerms, 1e-4));
        // termRemainsFeasibleBasisInfeasible
        REQUIRE(isVal(std::stoi(match[26].str()), testRunner.runData[lineIndex - 1].termRemainsFeasibleBasisInfeasible, 1e-4));
        // cutsChangedCoefficients
        REQUIRE(isVal(std::stoi(match[27].str()), testRunner.runData[lineIndex - 1].cutsChangedCoefficients, 1e-4));
        // feasibleTermsPrunedByBound
        REQUIRE(isVal(std::stoi(match[28].str()), testRunner.runData[lineIndex - 1].feasibleTermsPrunedByBound, 1e-4));
        // tighten_disjunction
        REQUIRE(std::stoi(match[29].str()) == 0);
        // tighten_matrix_perturbation
        REQUIRE(std::stoi(match[30].str()) == 0);
        // tighten_infeasible_to_feasible_term
        REQUIRE(std::stoi(match[31].str()) == 0);
        // tighten_feasible_to_infeasible_basis
        REQUIRE(std::stoi(match[32].str()) == 0);
      }
      lineIndex++;
    }
    file.close();
  }

  SECTION("VwsUtility::getCertificate"){

    // read in data
    fs::path inputFolder = "../src/test/test_instances/f2gap801600";

    // create test runners
    MipComp testRunnerFarkas(inputFolder.string(), csvPath.string(), 30, "Farkas", 16, "CBC", true, 0);
    MipComp testRunnerNone(inputFolder.string(), csvPath2.string(), 30, "None", 16, "CBC", true, 0);

    // create rundata containers
    RunData runDataFarkas;
    RunData runDataNone;

    // check to make sure we get the same dual bounds
    // i.e. make sure we didn't
    for (int i = 0; i < testRunnerFarkas.instanceSolvers.size(); i++){
      RunData runDataFarkas = testRunnerFarkas.seriesSolver.solve(
          testRunnerFarkas.instanceSolvers[i], i < 1 ? "New" : "Farkas", 1e10);
      RunData runDataNone = testRunnerNone.seriesSolver.solve(
          testRunnerFarkas.instanceSolvers[i], "None", 1e10);
      REQUIRE(isZero(runDataFarkas.dualBound - runDataNone.dualBound, 1e-3));
    }
  }

  // clear out any test files
  fs::remove_all(csvPath);
  fs::remove_all(csvPath2);
}