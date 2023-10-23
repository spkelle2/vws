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
    this->lpBound = rhs->lpBound;
    this->disjunctiveDualBound = rhs->disjunctiveDualBound;
    this->lpBoundPostVpc = rhs->lpBoundPostVpc;
    this->rootDualBoundPreVpc = rhs->rootDualBoundPreVpc;
    this->rootDualBound = rhs->rootDualBound;
    this->dualBound = rhs->dualBound;
    this->heuristicPrimalBound = rhs->heuristicPrimalBound;
    this->primalBound = rhs->primalBound;
    this->vpcGenerationTime = rhs->vpcGenerationTime;
    this->heuristicTime = rhs->heuristicTime;
    this->rootDualBoundTime = rhs->rootDualBoundTime;
    this->firstSolutionTime = rhs->firstSolutionTime;
    this->bestSolutionTime = rhs->bestSolutionTime;
    this->terminationTime = rhs->terminationTime;
    this->maxTime = rhs->maxTime;
    this->vpcGenerator = rhs->vpcGenerator;
    this->terms = rhs->terms;
    this->iterations = rhs->iterations;
    this->nodes = rhs->nodes;
  } else {
    this->lpBound = 0.0;
    this->disjunctiveDualBound = 0.0;
    this->lpBoundPostVpc = 0.0;
    this->rootDualBoundPreVpc = 0.0;
    this->rootDualBound = 0.0;
    this->dualBound = 0.0;
    this->heuristicPrimalBound = 0.0;
    this->primalBound = 0.0;
    this->vpcGenerationTime = 0.0;
    this->heuristicTime = 0.0;
    this->rootDualBoundTime = 0.0;
    this->firstSolutionTime = 0.0;
    this->bestSolutionTime = 0.0;
    this->terminationTime = 0.0;
    this->maxTime = 0.0;
    this->vpcGenerator = "";
    this->terms = 0;
    this->iterations = 0;
    this->nodes = 0;
  }
}

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
  verify(filePath.extension() == ".csv", "output must be a .csv file");

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
