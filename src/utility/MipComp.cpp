/**
 * @file MipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

// standard library
#include <cstdio> // cout, endl
#include <ctime> // time_t, localtime, put_time
#include <filesystem> // path
#include <fstream> // ofstream
#include <regex> // regex
#include <string> // string

// coin-or modules
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface

// project modules
#include "MipComp.hpp" // MipComp
#include "VwsUtility.hpp" // verify, extractModelFromGunzip, writeSolution

// namespaces
namespace fs = std::__fs::filesystem;

/** Reads the data in file path into MipComp instance. Captures run time limit of
 * each instance and creates a CbcModel instance for each mps file. Assumes
 * filePath is a .test file and is located in same directory structure as MIPcc repo. */
MipComp::MipComp(const char * filePathChars, const char * solutionDirectoryChars,
                 const char * csvPathChars):
  solutionDirectory(solutionDirectoryChars),
  csvPath(csvPathChars) {

  // validate file paths
  fs::path filePath(filePathChars);
  verify(fs::exists(filePath), "The file path " + filePath.string() + " does not exist.");
  verify(filePath.extension() == ".test", "MipComp::MipComp: filePath must be a .test file");
  verify(!fs::exists(solutionDirectory),
         "The directory " + solutionDirectory.string() + " should not exist.");
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
      timeout = std::stoi(timeoutMatch[1].str());
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
  seriesSolver = VwsSolverInterface(100, timeout-10);
} /* Constructor */

/** Solves each instance in the series and prints each's run metadata to stdout */
void MipComp::solveSeries() {

  // solve each instance in the series
  for (int i = 0; i < instanceSolvers.size(); i++) {

    std::time_t t = std::time(nullptr);
    OsiClpSolverInterface instanceSolver = instanceSolvers[i];
    fs::path solutionPath = solutionDirectory / (instanceNames[i] + ".sol");

    // print instance name and start time
    std::cout << "[INSTANCE] " << instanceNames[i] << ".mps" << std::endl;
    std::cout << "[START] " << std::put_time(std::localtime(&t), "%FT%T") << std::endl;

    // solve the instance
    seriesSolver.solve(instanceSolver);

    // write the best solution if it exists
    if (seriesSolver.solutions[i].size() > 0){
      writeSolution(seriesSolver.solutions[i][0], seriesSolver.variableNames[i], solutionPath);
    }

    // print end time and dual bound
    t = std::time(nullptr);
    std::cout << "[END] " << std::put_time(std::localtime(&t), "%FT%T") << std::endl;
    std::cout << "[DUAL BOUND] " << seriesSolver.dualBounds[i]	<< std::endl;
  }

} /* solveSeries */

///** writes the data collected from a test run to a csv */
//void writeCsvData(){
//  // check that the inputs meet expectations
//  verify(fs::exists(csvPath.parent_path()),
//         "The directory " + csvPath.parent_path().string() + " does not exist.");
//
//  std::ofstream file(csvPath.string());
//  // write the header if the file doesn't exist
//  if (!fs::exists(csvPath)) {
//    file << "Instance Name,LP Bound,Root Dual Bound,Dual Bound,Primal Bound,"
//            "LP Bound Time,Root Dual Bound Time,Termination Time" << std::endl;
//  }
//  for (int i = 0; i < instanceSolvers.size(); i++) {
//    // write each line
//    file << instanceNames[i] + "," + std::to_string(lpBounds[i]) + "," +
//            std::to_string(rootDualBounds[i]) + "," + std::to_string(dualBounds[i]) + "," +
//            std::to_string(primalBounds[i]) + "," + std::to_string(lpBoundTimes[i]) + "," +
//            std::to_string(rootDualBoundTimes[i]) + "," + std::to_string(terminationTimes[i]) << std::endl;
//  }
//  file.close();
//}
