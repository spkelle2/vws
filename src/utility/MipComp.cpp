/**
 * @file MipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

// standard library
#include <cstdio> // cout, endl
#include <ctime> // time_t, localtime
#include <ghc/filesystem.hpp> // path
#include <fstream> // ofstream
#include <iomanip> // put_time
#include <memory> // shared_ptr
#include <regex> // regex
#include <string> // string

// coin-or modules
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface

// project modules
#include "MipComp.hpp" // MipComp
#include "VwsEventHandler.hpp" // VwsEventHandler
#include "VwsUtility.hpp" // verify, extractModelFromGunzip, writeSolution

// namespaces
namespace fs = ghc::filesystem;

/** Reads the data in file path into MipComp instance. Captures run time limit of
 * each instance and creates a CbcModel instance for each mps file. Assumes
 * filePath is a .test file and is located in same directory structure as MIPcc repo. */
MipComp::MipComp(std::string inputFolderStr, std::string csvPathStr, double maxRunTime,
                 std::string vpcGenerator, int terms, std::string mipSolver):
  csvPath(csvPathStr), vpcGenerator(vpcGenerator) {

  // container for sorting input files
  std::vector<fs::path> inputFiles;

  // validate file paths
  fs::path inputFolder(inputFolderStr);
  verify(fs::is_directory(inputFolder), "The path " + inputFolder.string() + " must exist and be a folder.");
  verify(!fs::exists(csvPath), "The file " + csvPath.string() + " should not exist.");

  // get each .mps file in the input folder
  for (const auto& entry : fs::directory_iterator(inputFolder.string())) {
    if (fs::is_regular_file(entry) && entry.path().extension() == ".mps") {
      inputFiles.push_back(entry.path());
    }
  }

  // Sort the file names alphabetically
  std::sort(inputFiles.begin(), inputFiles.end());

  // read in each instance in alphabetical order
  for (const auto& inputPath : inputFiles) {
    OsiClpSolverInterface instanceSolver;
    instanceSolver.readMps(inputPath.c_str(), true, false);
    instanceSolvers.push_back(instanceSolver);
    instanceNames.push_back(inputPath.stem().replace_extension("").string());
  }
  
  // set parameters
  VPCParametersNamespace::VPCParameters params;
  params.set(VPCParametersNamespace::DISJ_TERMS, terms); // how many active leaves in the disjunction
  params.set(VPCParametersNamespace::TIMELIMIT, maxRunTime); // max time for vpc generation
  params.set(VPCParametersNamespace::PARTIAL_BB_TIMELIMIT, maxRunTime); // max time for creating partial bb tree
  params.set(VPCParametersNamespace::PARTIAL_BB_KEEP_PRUNED_NODES, 1); // get the entirety of the partial bb tree
  params.set(VPCParametersNamespace::MODE, 0); // create a partial bb tree to get the disjunction
  // todo: check these are applied as expected
  params.set(VPCParametersNamespace::BB_STRATEGY, VPCParametersNamespace::get_bb_option_value({
    VPCParametersNamespace::BB_Strategy_Options::user_cuts, // to allow VPCs
    VPCParametersNamespace::BB_Strategy_Options::all_cuts_on, // to see effects "in practice"
    VPCParametersNamespace::BB_Strategy_Options::presolve_off, // to compare disjunctive dual bound to root dual bound
    VPCParametersNamespace::BB_Strategy_Options::heuristics_on, // to see effects "in practice"
    VPCParametersNamespace::BB_Strategy_Options::strong_branching_on // to see effects "in practice"
  }));

  // set the solver interface
  seriesSolver = VwsSolverInterface(params, mipSolver);
} /* Constructor */

/** Solves each instance in the series and prints each's run metadata to stdout */
void MipComp::solveSeries() {

  // solve each instance in the series
  for (int i = 0; i < instanceSolvers.size(); i++) {

    std::time_t startTime = std::time(nullptr);
    OsiClpSolverInterface instanceSolver = instanceSolvers[i];

    // print instance name and start time
    std::cout << "[INSTANCE] " << instanceNames[i] << ".mps" << std::endl;
    std::cout << "[GENERATOR] " << vpcGenerator << std::endl;
    std::cout << "[START] " << std::put_time(std::localtime(&startTime), "%FT%T") << std::endl;

    // first iteration should always use New if it makes VPCs - otherwise use requested generator
    std::string genType = i < 1 && vpcGenerator != "None" ? "New" : vpcGenerator;

    // solve the instance with the given generator
    RunData data = seriesSolver.solve(instanceSolver, genType);

    // mark the instance index
    data.instanceIndex = i;

    // stop the series if we sought vpcs on the first iteration but didn't get any.
    // Need to generate cuts so we have a disjunction/multipliers to reuse and an ideal bound to compare
    verify(vpcGenerator == "None" or i > 0 or data.numCuts,
           "No vpcs were made from a new disjunction in first iteration. Stopping series.");

    // if generating vpcs didn't work, just skip recording this instance
    // since bound comparisons will be off or incomplete
    if (data.numCuts or vpcGenerator == "None"){
      // save the data collected by the event handler
      data.writeData(csvPath);
      runData.push_back(data);
    }

    // print end time
    std::time_t endTime = std::time(nullptr);
    std::cout << "[END] " << std::put_time(std::localtime(&endTime), "%FT%T") << std::endl;
  }
} /* solveSeries */
