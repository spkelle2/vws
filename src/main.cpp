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

  // executable, testfile, preprocessing, and time multiple should be only arguments
  assert(argc == 4);

  // create the test runner and solve the provided series
  std::string filePathStr(argv[1]);
  fs::path filePath(filePathStr);
  MipComp testRunner(filePath.string(), "../experiments/solutions",
                     "../experiments/" + filePath.stem().string() + ".csv",
                     std::stoi(argv[2]), std::stod(argv[3]));
  testRunner.solveSeries();

} /* main */

// a function to unit test the MipComp class


