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
#include "MipCompEventHandler.hpp" // MipCompEventHandler
#include "VwsUtility.hpp" // verify, extractModelFromGunzip, writeSolution

// namespaces
namespace fs = ghc::filesystem;

/** Reads the data in file path into MipComp instance. Captures run time limit of
 * each instance and creates a CbcModel instance for each mps file. Assumes
 * filePath is a .test file and is located in same directory structure as MIPcc repo. */
MipComp::MipComp(std::string filePathStr, std::string solutionDirectoryStr,
                 std::string csvPathStr, bool usePreprocessing, double timeMultiplier):
  solutionDirectory(solutionDirectoryStr), csvPath(csvPathStr),
  usePreprocessing(usePreprocessing) {

  // validate file paths
  fs::path filePath(filePathStr);
  verify(fs::exists(filePath), "The file path " + filePath.string() + " does not exist.");
  verify(filePath.extension() == ".test", "filePath must be a .test file");
  verify(!fs::exists(csvPath), "The file " + csvPath.string() + " should not exist.");

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
      timeout = std::stod(timeoutMatch[1].str()) * timeMultiplier;
      timeoutBuffer = timeout * 0.3;
    }

    // read each .gz file to a CbcModel
    else if (!std::regex_search(str, metadataPattern))
    {
      // for each model, save its solver interface and its name
      fs::path instancePath = datasetRootDirectory / str;
      OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip(instancePath);
      instanceSolvers.push_back(instanceSolver);
      instanceNames.push_back(instancePath.stem().replace_extension("").string());
    }
  }
  file.close();

  // check to make sure the file was structured correctly
  verify(timeout > 0, "A line beginning with [TIMEOUT] must have value > 0 in .test file.");

  // set the solver interface
  seriesSolver = VwsSolverInterface(100, timeout-timeoutBuffer);
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
    std::shared_ptr<MipCompEventHandler> handler = std::make_shared<MipCompEventHandler>();

    // solve the instance
    CbcModel model = seriesSolver.solve(instanceSolver, usePreprocessing, handler.get());

    // correct the recorded dual bound and save the data collected by the event handler
    *handler = *dynamic_cast<MipCompEventHandler*>(model.getEventHandler());
    handler->data.dualBound = model.getBestPossibleObjValue();
    handler->data.maxTerminationTime = timeout - timeoutBuffer;
    handler->data.maxCompletionTime = timeout;
    handler->data.usePreprocessing = usePreprocessing;

    // write the best solution if it exists
    if (seriesSolver.solutions[i].size() > 0){
      writeSolution(seriesSolver.solutions[i][0], seriesSolver.variableNames[i], solutionPath);
    }

    // print end time and dual bound
    std::time_t endTime = std::time(nullptr);
    std::cout << "[END] " << std::put_time(std::localtime(&endTime), "%FT%T") << std::endl;
    std::cout << "[DUAL BOUND] " << handler->data.dualBound << std::endl;

    // record total time and save the data collected by the event handler
    handler->data.completionTime = std::difftime(endTime, startTime);
    runData.push_back(handler->data);
  }
  writeCsvData();
} /* solveSeries */

/** writes the data collected from a test run to a csv */
void MipComp::writeCsvData(){
  // check that the inputs meet expectations
  verify(fs::exists(csvPath.parent_path()),
         "The directory " + csvPath.parent_path().string() + " does not exist.");

  std::ofstream file(csvPath.string());
  file << runData[0].getHeader() << std::endl;
  for (int i = 0; i < instanceSolvers.size(); i++) {
    file << runData[i].getValues() << std::endl;
  }
  file.close();
}
