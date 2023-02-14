/**
 * @file MipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

// standard library
#include <cstdio> // cout, endl
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
    }
  }
  verify(timeout > 0, "A line beginning with [TIMEOUT] must have value > 0 in .test file.");

} /* Constructor */
