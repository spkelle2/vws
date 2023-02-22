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
    assert(false);  // cause failure with a stack trace
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
  solver->readMps(instancePath.c_str(), true, false);
  CbcModel model(*solver);
  // model.setLogLevel(0);
  return model;
}

// get the variable names from a CbcModel
std::vector<std::string> getVariableNames(CbcModel &model) {
  std::vector<std::string> variableNames;
  for (int i = 0; i < model.getNumCols(); i++) {
    variableNames.push_back(model.solver()->getColName(i, 0));
  }
  return variableNames;
}

// get the constraint names from a CbcModel
std::vector<std::string> getConstraintNames(CbcModel &model) {
  std::vector<std::string> constraintNames;
  for (int i = 0; i < model.getNumRows(); i++) {
    constraintNames.push_back(model.solver()->getRowName(i, 0));
  }
  return constraintNames;
}

/** writes solution from <model> to <solutionPath> if one exists. In either case
 * previous solution recorded at <solutionPath> is removed. */
void writeSolution(CbcModel &model, std::vector<std::string> variableNames, fs::path solutionPath){

  // check that the inputs meet expectations
  verify(fs::exists(solutionPath.parent_path()),
         "The directory " + solutionPath.parent_path().string() + " does not exist.");
  verify(variableNames.size() == model.getNumCols(),
         "The number of variable names must match the number of columns in the model. " +
         std::to_string(variableNames.size()) + " != " + std::to_string(model.getNumCols()));

  // get rid of the old solution if there is one
  if (fs::exists(solutionPath)) {
    fs::remove(solutionPath);
  }

  // write the solution if it exists
  if (model.numberSavedSolutions() > 0){
    std::ofstream file(solutionPath);
    for (int i = 0; i < model.getNumCols(); i++) {
      file << variableNames[i] + " " +
              std::to_string(model.bestSolution()[i]) << std::endl;
    }
    file.close();
  }
}
