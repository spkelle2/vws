/**
 * @file VwsUtility.hpp
 * @author Sean Kelley
 * @date 2023-02-13
 */

// standard library
#include <cstdio> // cerr
#include <cstdlib> // source
#include <ghc/filesystem.hpp> // path
#include <fstream> // ofstream
#include <iostream> // endl
#include <string>
#include <vector>

// coin-or modules
#include "CbcModel.hpp" // CbcModel
#include "CglPreProcess.hpp" // CglPreProcess
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface

// project modules
#include "VwsUtility.hpp"

// namespaces
namespace fs = ghc::filesystem;

void verify(bool condition, const std::string& msg) {
  if (!condition) {
    std::cerr << msg << std::endl;
    assert(false);  // cause failure with a stack trace
  }
}

OsiClpSolverInterface extractSolverInterfaceFromGunzip(fs::path instancePath) {
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
  OsiClpSolverInterface instanceSolver;
  instanceSolver.readMps(instancePath.c_str(), true, false);
  return instanceSolver;
}

// get the variable names from an OsiSolverInterface
std::vector<std::string> getVariableNames(OsiSolverInterface& instanceSolver) {
  std::vector<std::string> variableNames;
  for (int i = 0; i < instanceSolver.getNumCols(); i++) {
    variableNames.push_back(instanceSolver.getColName(i, 0));
  }
  return variableNames;
}

// get the constraint names from an OsiSolverInterface
std::vector<std::string> getConstraintNames(OsiSolverInterface& instanceSolver) {
  std::vector<std::string> constraintNames;
  for (int i = 0; i < instanceSolver.getNumRows(); i++) {
    constraintNames.push_back(instanceSolver.getRowName(i, 0));
  }
  return constraintNames;
}

/** writes solution in <variableValue> to <solutionPath>. Removes previous
 * solution recorded at <solutionPath>. */
void writeSolution(std::vector<double> variableValues, std::vector<std::string> variableNames,
                   fs::path solutionPath){

  // check that the inputs meet expectations
  verify(fs::exists(solutionPath.parent_path()),
         "The directory " + solutionPath.parent_path().string() + " does not exist.");
  verify(variableNames.size() == variableValues.size(),
         "The number of variable names must match the number of columns in the model. " +
         std::to_string(variableNames.size()) + " != " + std::to_string(variableValues.size()));

  // get rid of the old solution if there is one
  if (fs::exists(solutionPath)) {
    fs::remove(solutionPath);
  }

  // write the current solution
  std::ofstream file(solutionPath);
  for (int i = 0; i < variableValues.size(); i++) {
    file << variableNames[i] + " " + std::to_string(variableValues[i]) << std::endl;
  }
  file.close();
}

/** transform solutions from the presolved model to the original instance */
void putBackSolutions(CbcModel *presolvedModel, CbcModel *model, CglPreProcess *preProcess){
  int numberSolutions = presolvedModel->numberSavedSolutions();
  int numberColumns = presolvedModel->getNumCols();
  if (numberSolutions >= 1) {
    model->deleteSolutions();
    double *bestSolution =
        CoinCopyOfArray(presolvedModel->bestSolution(), numberColumns);
    // double cutoff = presolvedModel->getCutoff();
    double objectiveValue = presolvedModel->getObjValue();
    // model->createSpaceForSavedSolutions(numberSolutions-1);
    for (int iSolution = numberSolutions - 1; iSolution >= 0; iSolution--) {
      presolvedModel->setCutoff(COIN_DBL_MAX);
      presolvedModel->solver()->setColSolution(
          presolvedModel->savedSolution(iSolution));
      // presolvedModel->savedSolutionObjective(iSolution));
      preProcess->postProcess(*presolvedModel->solver(), false);
      model->setBestSolution(preProcess->originalModel()->getColSolution(),
                             model->solver()->getNumCols(),
                             presolvedModel->savedSolutionObjective(iSolution));
    }
    presolvedModel->setBestObjectiveValue(objectiveValue);
    presolvedModel->solver()->setColSolution(bestSolution);
    // presolvedModel->setBestSolution(bestSolution,numberColumns,objectiveValue);
  }
}
