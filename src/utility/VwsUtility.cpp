/**
 * @file VwsUtility.hpp
 * @author Sean Kelley
 * @date 2023-02-13
 */

// standard library
#include <cstdio> // cerr
#include <cstdlib> // source
#include <filesystem> // path
#include <iostream> // endl
#include <string>
#include <vector>

// coin-or modules
#include "OsiClpSolverInterface.hpp"
#include "CbcModel.hpp"

// project modules
#include "VwsUtility.hpp"

// namespaces
namespace fs = std::__fs::filesystem;

void verify(bool condition, const std::string& msg) {
  if (!condition) {
    std::cerr << msg << std::endl;
    exit(1);
  }
}

CbcModel extractModelFromGunzip(fs::path instancePath) {
  // check that the file meets expectations
  verify(fs::exists(instancePath),
       "The file path " + instancePath.string() + " does not exist.");
  verify(instancePath.extension() == ".gz",
         "The file path " + instancePath.string() + " must be .gz file");

  // unzip and read the file
  std::string unzipCommand = "gunzip -k -f " + instancePath.string();
  std::system(unzipCommand.c_str());
  instancePath.replace_extension("");

  // read instancePath into a CbcModel
  OsiClpSolverInterface * solver = new OsiClpSolverInterface();
  solver->readMps(instancePath.c_str(), "mps");
  CbcModel model(*solver);
  model.setLogLevel(0);
  return model;
}

