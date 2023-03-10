/**
 * @file VwsUtility.hpp
 * @author Sean Kelley
 * @date 2023-02-13
 */

// standard library
#include <ghc/filesystem.hpp> // path
#include <string> // string

// coin-or modules
#include "CbcModel.hpp"

// namespaces
namespace fs = ghc::filesystem;

/** Asserts that the condition is true. If not, msg is printed and program terminates */
void verify(bool condition, const std::string& msg);

/** Unzips the .gz file at instancePath and reads it into an OsiClpSolverInterface */
OsiClpSolverInterface extractSolverInterfaceFromGunzip(fs::path instancePath);

/** get the variable names from a CbcModel */
std::vector<std::string> getVariableNames(OsiSolverInterface& solverInterface);

/** get the constraint names from a CbcModel */
std::vector<std::string> getConstraintNames(OsiSolverInterface& solverInterface);

/** writes solution in <variableValue> to <solutionPath>. Removes previous
 * solution recorded at <solutionPath>. */
void writeSolution(std::vector<double> variableValues, std::vector<std::string> variableNames,
                   fs::path solutionPath);

/** transform solutions from the presolved model to the original instance */
void putBackSolutions(CbcModel *presolvedModel, CbcModel *model, CglPreProcess *preProcess);
