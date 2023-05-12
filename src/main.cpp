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

  // executable, testfile, benchmark, time multiple, and disjunctive terms should be only arguments
  assert(argc == 5);

  // create the test runner and solve the provided series
  std::string filePathStr(argv[1]);
  fs::path filePath(filePathStr);
  MipComp testRunner(filePath.string(), "../experiments/solutions",
                     "../experiments/" + filePath.stem().string() + ".csv",
                     std::stoi(argv[2]), std::stod(argv[3]), std::stoi(argv[4]));
  testRunner.solveSeries();

} /* main */

// a function to unit test the MipComp class


