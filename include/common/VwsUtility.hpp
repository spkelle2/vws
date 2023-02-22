/**
 * @file VwsUtility.hpp
 * @author Sean Kelley
 * @date 2023-02-13
 */

// standard library
#include <filesystem> // path
#include <string> // string

// coin-or modules
#include "CbcModel.hpp"

// namespaces
namespace fs = std::__fs::filesystem;

/** Asserts that the condition is true. If not, msg is printed and program terminates */
void verify(bool condition, const std::string& msg);

/** Unzips the .gz file at instancePath and reads it into a CbcModel */
CbcModel extractModelFromGunzip(fs::path instancePath);

/** get the variable names from a CbcModel */
std::vector<std::string> getVariableNames(CbcModel &model);

/** get the constraint names from a CbcModel */
std::vector<std::string> getConstraintNames(CbcModel &model);

/** writes the best solution from <model> to <solutionPath> with the names <variableNames> */
void writeSolution(CbcModel &model, std::vector<std::string> variableNames, fs::path solutionPath);
