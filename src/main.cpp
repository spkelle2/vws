/**
 * @file main.cpp
 * @author Sean Kelley
 * @date 2023-02-01
 */

#include <ghc/filesystem.hpp> // path
#include <string> // string


// Project files
#include "MipComp.hpp"
#include "VwsUtility.hpp"

// namespaces
namespace fs = ghc::filesystem;

/****************** MAIN FUNCTION **********************/
int main(int argc, char** argv) {

  // executable, input folder, output file, max run time, cut generator, and disjunctive terms
  assert(argc == 6);

  // create the test runner and solve the provided series
  std::string inputFolderStr(argv[1]);
  std::string outputFileStr(argv[2]);
  std::string mipSolver(argv[6]);
  MipComp testRunner(inputFolderStr, outputFileStr, std::stod(argv[3]),
                     std::string(argv[4]), std::stoi(argv[5]), mipSolver);
  testRunner.solveSeries();

} /* main */

// a function to unit test the MipComp class


