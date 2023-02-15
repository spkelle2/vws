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

// project modules
#include "MipComp.hpp"
#include "VwsUtility.hpp" // verify, extractModelFromGunzip

// namespaces
namespace fs = std::__fs::filesystem;

/** Reads the data in file path into MipComp instance. Captures run time limit of
 * each instance and creates a CbcModel instance for each mps file. Assumes
 * filePath is a .test file and is located in same directory structure as MIPcc repo */
MipComp::MipComp(char * filePathChars){

  // validate function input
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

  // try to read in the instances and needed metadata in the file
  while (std::getline(file, str)){

    // get the timeout amount
    if (std::regex_search(str, timeoutMatch, timeoutPattern)) {
      timeout = std::stoi(timeoutMatch[1].str());
    }

    // read each .gz file to a CbcModel
    else if (!std::regex_search(str, metadataPattern))
    {
      fs::path instancePath(datasetRootDirectory.string() + "/" + str);
      mipModels.push_back(extractModelFromGunzip(instancePath));
      names.push_back(instancePath.stem());
    }
  }

  // set the solver interface
  VwsSolverInterface solver = VwsSolverInterface();

  // check to make sure the file was structured correctly
  verify(timeout > 0, "A line beginning with [TIMEOUT] must have value > 0 in .test file.");

} /* Constructor */

/** Solves each instance in the series and prints each's run metadata to stdout */
void MipComp::solveSeries() {

  // solve each instance in the series
  for (int i = 0; i < mipModels.size(); i++) {

    std::time_t t = std::time(nullptr);

    // print instance name and start time
    std::cout << "[INSTANCE] " << names[i] << std::endl;
    std::cout << "[START] " << std::put_time(std::localtime(&t), "%FT%T") << std::endl;

    // solve the instance
    solver.solve(mipModels[i], 1);

    // print end time and dual bound
    std::cout << "[END] " << std::put_time(std::localtime(&t), "%FT%T") << std::endl;
    std::cout << "[DUAL BOUND] " << mipModels[i].getBestPossibleObjValue()	<< std::endl;
  }

} /* solveSeries */
