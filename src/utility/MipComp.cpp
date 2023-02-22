/**
 * @file MipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

// standard library
#include <cstdio> // cout, endl
#include <ctime> // time_t, localtime, put_time
#include <filesystem> // path
#include <regex> // regex
#include <string> // string

// coin-or modules
#include "CbcModel.hpp" // CbcModel

// project modules
#include "MipComp.hpp"
#include "MipCompEventHandler.hpp" // MipCompEventHandler
#include "VwsUtility.hpp" // verify, extractModelFromGunzip, writeSolution

// namespaces
namespace fs = std::__fs::filesystem;

/** Reads the data in file path into MipComp instance. Captures run time limit of
 * each instance and creates a CbcModel instance for each mps file. Assumes
 * filePath is a .test file and is located in same directory structure as MIPcc repo. */
MipComp::MipComp(const char * filePathChars, const char * solutionDirectoryChars):
  solutionDirectory(solutionDirectoryChars) {

  // validate file paths
  fs::path filePath(filePathChars);
  verify(fs::exists(filePath), "The file path " + filePath.string() + " does not exist.");
  verify(filePath.extension() == ".test", "MipComp::MipComp: filePath must be a .test file");

  // instantiate variables we'll need for this function
  std::ifstream file(filePath.string());
  fs::path datasetRootDirectory = filePath.parent_path().parent_path().parent_path();
  std::regex metadataPattern("\\[.*\\]");
  std::regex timeoutPattern("\\[TIMEOUT\\]\\s*(\\d+)");
  std::string str;
  std::smatch timeoutMatch;

  // create the solution directory if it doesn't exist
  if (!fs::exists(solutionDirectory)){
    fs::create_directory(solutionDirectory);
  }

  // try to read in the instances and needed metadata in the file
  while (std::getline(file, str)){

    // get the timeout amount
    if (std::regex_search(str, timeoutMatch, timeoutPattern)) {
      timeout = std::stoi(timeoutMatch[1].str());
    }

    // read each .gz file to a CbcModel
    else if (!std::regex_search(str, metadataPattern))
    {
      // for each model, save it, its name, and add an event handler for data tracking
      fs::path instancePath = datasetRootDirectory / str;
      CbcModel model = extractModelFromGunzip(instancePath);
      model.setDblParam(CbcModel::CbcMaximumSeconds, timeout);
      MipCompEventHandler handler;
      model.passInEventHandler(&handler);
      mipModels.push_back(model);
      instanceNames.push_back(instancePath.stem().replace_extension("").string());
    }
  }

  // check to make sure the file was structured correctly
  verify(timeout > 0, "A line beginning with [TIMEOUT] must have value > 0 in .test file.");

  // set the solver interface
  VwsSolverInterface solver = VwsSolverInterface(100);

} /* Constructor */

/** Solves each instance in the series and prints each's run metadata to stdout */
void MipComp::solveSeries() {

  // solve each instance in the series
  for (int i = 0; i < mipModels.size(); i++) {

    std::time_t t = std::time(nullptr);
    CbcModel model = mipModels[i];
    fs::path solutionPath = solutionDirectory / (instanceNames[i] + ".sol");

    // print instance name and start time
    std::cout << "[INSTANCE] " << instanceNames[i] << ".mps" << std::endl;
    std::cout << "[START] " << std::put_time(std::localtime(&t), "%FT%T") << std::endl;

    // solve the root LP relaxation and record related stats
    const auto startTime = std::chrono::system_clock::now();
    model.initialSolve();
    lpBounds.push_back(model.getBestPossibleObjValue());
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count();
    lpBoundTimes.push_back(ms/1000.0);

    // todo: set hot start solutions

    // solve the instance
    solver.solve(model);

    // record stats
    MipCompEventHandler* eventHandler = dynamic_cast<MipCompEventHandler*>(model.getEventHandler());
    rootDualBounds.push_back(eventHandler->rootDualBound);
    rootDualBoundTimes.push_back(eventHandler->rootDualBoundTime);
    dualBounds.push_back(model.getBestPossibleObjValue());
    primalBounds.push_back(model.getObjValue());
    terminationTimes.push_back(model.getCurrentSeconds());

    // write the best solution if it exists
    writeSolution(model, solver.variableNames[i], solutionPath);

    // print end time and dual bound
    std::cout << "[END] " << std::put_time(std::localtime(&t), "%FT%T") << std::endl;
    std::cout << "[DUAL BOUND] " << model.getBestPossibleObjValue()	<< std::endl;
  }

} /* solveSeries */
