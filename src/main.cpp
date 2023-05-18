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

  // executable, testfile, output folder, benchmark, time multiple, and disjunctive terms
  assert(argc == 6);

  // create the test runner and solve the provided series
  std::string filePathStr(argv[1]);
  fs::path filePath(filePathStr);
  std::string outputFolderStr(argv[2]);
  fs::path outputFolder(outputFolderStr);
  MipComp testRunner(filePath.string(), outputFolder.string() + "/solutions",
                     outputFolder.string() + "/" + filePath.stem().string() + ".csv",
                     std::stoi(argv[3]), std::stod(argv[4]), std::stoi(argv[5]));
  testRunner.solveSeries();

} /* main */

// a function to unit test the MipComp class


