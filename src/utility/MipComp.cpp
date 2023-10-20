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
MipComp::MipComp(std::string inputFolderStr, std::string solutionDirectoryStr,
                 std::string csvPathStr, double maxRunTime, std::string vpcGenerator,
                 int terms):
  solutionDirectory(solutionDirectoryStr), csvPath(csvPathStr), terms(terms),
  timeout(maxRunTime), vpcGenerator(vpcGenerator) {

  // container for sorting input files
  std::vector<fs::path> inputFiles;

  // validate file paths
  fs::path inputFolder(inputFolderStr);
  verify(fs::is_directory(inputFolder), "The path " + inputFolder.string() + " must exist and be a folder.");
  verify(!fs::exists(csvPath), "The file " + csvPath.string() + " should not exist.");

  // create the solution directory if it doesn't exist
  if (!fs::exists(solutionDirectory)){
    fs::create_directory(solutionDirectory);
  }

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

  // set the solver interface
  seriesSolver = VwsSolverInterface(0, timeout, terms, 1);
} /* Constructor */

/** Solves each instance in the series and prints each's run metadata to stdout */
void MipComp::solveSeries() {

  // solve each instance in the series
  for (int i = 0; i < instanceSolvers.size(); i++) {

    std::time_t startTime = std::time(nullptr);
    OsiClpSolverInterface instanceSolver = instanceSolvers[i];
    fs::path solutionPath = solutionDirectory / (instanceNames[i] + ".sol");

    // print instance name and start time
    std::cout << "[INSTANCE] " << instanceNames[i] << ".mps" << std::endl;
    std::cout << "[START] " << std::put_time(std::localtime(&startTime), "%FT%T") << std::endl;

    // set up event handler for data collection
    std::shared_ptr<VwsEventHandler> handler = std::make_shared<VwsEventHandler>();

    // first iteration should always use New Disjunction if it makes VPCs - otherwise use requested generator
    std::string genType = i < 1 && vpcGenerator != "None" ? "New Disjunction" : vpcGenerator;

    // solve the instance with the given generator
    CbcModel model = seriesSolver.solve(instanceSolver, genType, *handler);

    // write the best solution if it exists
    if (seriesSolver.solutions[i].size() > 0){
      writeSolution(seriesSolver.solutions[i][0], seriesSolver.variableNames[i], solutionPath);
    }

    // save the data collected by the event handler
    handler->data.writeData(csvPath);
    runData.push_back(handler->data);

    // print end time
    std::time_t endTime = std::time(nullptr);
    std::cout << "[END] " << std::put_time(std::localtime(&endTime), "%FT%T") << std::endl;

  }
} /* solveSeries */
