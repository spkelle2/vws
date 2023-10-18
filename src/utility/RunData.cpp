/**
 * @file RunData.cpp
 * @author Sean Kelley
 * @date 2023-03-06
 */


// standard modules
#include <ghc/filesystem.hpp> // path
#include <string> // string

// project modules
#include "RunData.hpp"
#include "VwsUtility.hpp" // verify

// namespaces
namespace fs = ghc::filesystem;


/** Get a comma-separated string of the names of RunData's attributes */
std::string RunData::getHeader(){
  return "lpBound,disjunctiveDualBound,lpBoundPostVpc,rootDualBoundPreVpc,rootDualBound,"
         "dualBound,heuristicPrimalBound,primalBound,vpcGenerationTime,heuristicTime,"
         "rootDualBoundTime,firstSolutionTime,bestSolutionTime,terminationTime,maxTime,"
         "vpcGenerator,terms,iterations,nodes";
}

/** Get a comma-separated string of the values of RunData's attributes */
std::string RunData::getValues(){
  return std::to_string(lpBound) + "," + std::to_string(disjunctiveDualBound) + "," +
    std::to_string(lpBoundPostVpc) + "," + std::to_string(rootDualBoundPreVpc) + "," +
    std::to_string(rootDualBound) + "," + std::to_string(dualBound) + "," +
    std::to_string(heuristicPrimalBound) + "," + std::to_string(primalBound) + "," +
    std::to_string(vpcGenerationTime) + "," + std::to_string(heuristicTime) + "," +
    std::to_string(rootDualBoundTime) + "," + std::to_string(firstSolutionTime) + "," +
    std::to_string(bestSolutionTime) + "," + std::to_string(terminationTime) + "," +
    std::to_string(maxTime) + "," + vpcGenerator + "," + std::to_string(terms) + "," +
    std::to_string(iterations) + "," + std::to_string(nodes);
}


/** writes this struct's attributes to the given csv file */
void RunData::writeData(fs::path filePath){

  // check that the inputs meet expectations
  verify(fs::exists(filePath.parent_path()),
         "The directory " + filePath.parent_path().string() + " does not exist.");

  bool alreadyExists = fs::exists(filePath);
  std::ofstream file;
  if (!alreadyExists) {  // start with the header if the file doesn't exist
    file.open(filePath.string());
    file << getHeader() << std::endl;
  } else {  // append to the file otherwise
    file.open(filePath.string(), std::ios::app);
  }
  file << getValues() << std::endl;
}
