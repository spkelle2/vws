/**
 * @file main.cpp
 * @author Sean Kelley
 * @date 2023-02-01
 */

#include <filesystem.hpp> // path
#include <string> // string


// Project files
#include "MipComp.hpp"
#include "VwsUtility.hpp"

// namespaces
namespace fs = ghc::filesystem;

/****************** MAIN FUNCTION **********************/
int main(int argc, char** argv) {

  // executable, input folder, output file, max run time,
  // cut generator, disjunctive terms, solver,
  // primal bound, seed index,
  // tighten disjunction, tighten matrix perturbation,
  // tighten infeasible to feasible term, tighten feasible to infeasible basis
  assert(argc == 13);

  // create the test runner and solve the provided series
  std::string inputFolderStr(argv[1]);
  std::string outputFileStr(argv[2]);
  std::string mipSolver(argv[6]);
  MipComp testRunner(inputFolderStr, outputFileStr, std::stod(argv[3]),
                     std::string(argv[4]), std::stoi(argv[5]), mipSolver,
                     std::stoi(argv[7]), std::stoi(argv[8]),
                     std::stoi(argv[9]), std::stoi(argv[10]),
                     std::stoi(argv[11]), std::stoi(argv[12]));
  testRunner.solveSeries();

} /* main */

// a function to unit test the MipComp class


