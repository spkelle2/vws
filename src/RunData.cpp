/**
 * @file RunData.cpp
 * @author Sean Kelley
 * @date 2023-03-06
 */


// standard modules
#include <filesystem.hpp> // path
#include <string> // string

// project modules
#include "RunData.hpp"
#include "VwsUtility.hpp" // verify

// namespaces
namespace fs = ghc::filesystem;

/** constructor (default) */
RunData::RunData(){
  initialize(NULL);
}

/** copy constructor */
RunData::RunData(const RunData& rhs){
  initialize(&rhs);
}

/** assignment operator */
RunData& RunData::operator=(const RunData& rhs) {
  initialize(&rhs);
  return *this;
}

/** clone */
RunData* RunData::clone() const {
  return new RunData(*this);
}

/** Copy our stuff */
void RunData::initialize(const RunData* const rhs) {
  if (rhs) {
    this->instanceIndex = rhs->instanceIndex;
    this->vpcGenerator = rhs->vpcGenerator;
    this->terms = rhs->terms;
    this->lpBound = rhs->lpBound;
    this->disjunctiveDualBound = rhs->disjunctiveDualBound;
    this->lpBoundPostVpc = rhs->lpBoundPostVpc;
    this->rootDualBound = rhs->rootDualBound;
    this->dualBound = rhs->dualBound;
    this->primalBound = rhs->primalBound;
    this->vpcGenerationTime = rhs->vpcGenerationTime;
    this->rootDualBoundTime = rhs->rootDualBoundTime;
    this->bestSolutionTime = rhs->bestSolutionTime;
    this->terminationTime = rhs->terminationTime;
    this->nodes = rhs->nodes;
    this->iterations = rhs->iterations;
    this->maxTime = rhs->maxTime;
    this->actualTerms = rhs->actualTerms;
    this->numCuts = rhs->numCuts;
    this->mipSolver = rhs->mipSolver;
    this->providePrimalBound = rhs->providePrimalBound;
  } else {
    this->instanceIndex = 0;
    this->vpcGenerator = "";
    this->terms = 0;
    this->lpBound = 0.0;
    this->disjunctiveDualBound = 0.0;
    this->lpBoundPostVpc = 0.0;
    this->rootDualBound = 0.0;
    this->dualBound = 0.0;
    this->primalBound = 0.0;
    this->vpcGenerationTime = 0.0;
    this->rootDualBoundTime = 0.0;
    this->bestSolutionTime = 0.0;
    this->terminationTime = 0.0;
    this->nodes = 0;
    this->iterations = 0;
    this->maxTime = 0.0;
    this->actualTerms = 0;
    this->numCuts = 0;
    this->mipSolver = "";
    this->providePrimalBound = false;
  }
}

/** Get a comma-separated string of the names of RunData's attributes */
std::string RunData::getHeader(){
  return "instanceIndex,vpcGenerator,terms,lpBound,disjunctiveDualBound,"
         "lpBoundPostVpc,rootDualBound,dualBound,primalBound,vpcGenerationTime,"
         "rootDualBoundTime,bestSolutionTime,terminationTime,nodes,iterations,"
         "maxTime,actualTerms,numCuts,mipSolver,providePrimalBound";
}

/** Get a comma-separated string of the values of RunData's attributes */
std::string RunData::getValues(){
  return std::to_string(instanceIndex) + "," + vpcGenerator + "," +
    std::to_string(terms) + "," + std::to_string(lpBound) + "," +
    std::to_string(disjunctiveDualBound) + "," + std::to_string(lpBoundPostVpc) + "," +
    std::to_string(rootDualBound) + "," + std::to_string(dualBound) + "," +
    std::to_string(primalBound) + "," + std::to_string(vpcGenerationTime) + "," +
    std::to_string(rootDualBoundTime) + "," + std::to_string(bestSolutionTime) + "," +
    std::to_string(terminationTime) + "," + std::to_string(nodes) + "," +
    std::to_string(iterations) + "," + std::to_string(maxTime) + "," +
    std::to_string(actualTerms) + "," + std::to_string(numCuts) + "," +
    mipSolver + "," + std::to_string(providePrimalBound);
}

/** writes this struct's attributes to the given csv file */
void RunData::writeData(fs::path filePath){

  // check that the inputs meet expectations
  verify(fs::exists(filePath.parent_path()),
         "The directory " + filePath.parent_path().string() + " does not exist.");
  verify(filePath.extension() == ".csv", "output must be a .csv file");

  // object to represent the file in memory
  std::ofstream file;

  if (!fs::exists(filePath)) {
    // create the file and write the header if it doesn't exist
    file.open(filePath.string());
    file << getHeader() << std::endl;
  } else {
    // configure the file to be appended to otherwise
    file.open(filePath.string(), std::ios::app);
  }

  // write the data
  file << getValues() << std::endl;
}
