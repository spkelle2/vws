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
    this->instanceIndex = rhs->instanceIndex;
    this->lpBound = rhs->lpBound;
    this->disjunctiveDualBound = rhs->disjunctiveDualBound;
    this->lpBoundPostVpc = rhs->lpBoundPostVpc;
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
    this->actualTerms = rhs->actualTerms;
    this->primalBounds = rhs->primalBounds;
    this->dualBounds = rhs->dualBounds;
    this->times = rhs->times;
  } else {
    this->instanceIndex = 0;
    this->lpBound = 0.0;
    this->disjunctiveDualBound = 0.0;
    this->lpBoundPostVpc = 0.0;
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
    this->actualTerms = 0;
    this->primalBounds = std::vector<double>();
    this->dualBounds = std::vector<double>();
    this->times = std::vector<double>();
  }
}

/** Get a comma-separated string of the names of RunData's attributes */
std::string RunData::getHeader(){
  return "instanceIndex,lpBound,disjunctiveDualBound,lpBoundPostVpc,rootDualBound,"
         "dualBound,heuristicPrimalBound,primalBound,vpcGenerationTime,heuristicTime,"
         "rootDualBoundTime,firstSolutionTime,bestSolutionTime,terminationTime,maxTime,"
         "vpcGenerator,terms,iterations,nodes,actualTerms";
}

/** Get a comma-separated string of the values of RunData's attributes */
std::string RunData::getValues(){
  return std::to_string(instanceIndex) + "," + std::to_string(lpBound) + "," +
    std::to_string(disjunctiveDualBound) + "," + std::to_string(lpBoundPostVpc) + "," +
    std::to_string(rootDualBound) + "," + std::to_string(dualBound) + "," +
    std::to_string(heuristicPrimalBound) + "," + std::to_string(primalBound) + "," +
    std::to_string(vpcGenerationTime) + "," + std::to_string(heuristicTime) + "," +
    std::to_string(rootDualBoundTime) + "," + std::to_string(firstSolutionTime) + "," +
    std::to_string(bestSolutionTime) + "," + std::to_string(terminationTime) + "," +
    std::to_string(maxTime) + "," + vpcGenerator + "," + std::to_string(terms) + "," +
    std::to_string(iterations) + "," + std::to_string(nodes) + "," + std::to_string(actualTerms);
}

/** writes this struct's attributes to the given csv file */
void RunData::writeData(fs::path filePath, std::string kind){

  // check that the inputs meet expectations
  verify(kind == "meta" or kind == "bound", "kind must be either meta or bound");
  verify(fs::exists(filePath.parent_path()),
         "The directory " + filePath.parent_path().string() + " does not exist.");
  verify(filePath.extension() == ".csv", "output must be a .csv file");

  bool alreadyExists = fs::exists(filePath);
  std::ofstream file;
  if (!alreadyExists) {  // start with the header if the file doesn't exist
    file.open(filePath.string());
    if (kind == "meta"){
      file << getHeader() << std::endl;
    } else {
      file << getBoundHeader() << std::endl;
    }
  } else {  // append to the file otherwise
    file.open(filePath.string(), std::ios::app);
  }
  if (kind == "meta"){
    file << getValues() << std::endl;
  } else {
    file << getBoundValues() << std::endl;
  }
}

/** Get a comma-separated string of the names of RunData's attributes tracking bound closure */
std::string RunData::getBoundHeader(){
  return "instanceIndex,primalBounds,dualBounds,times";
}

/** Get a comma-separated string of the values of RunData's attributes tracking bound closure */
std::string RunData::getBoundValues(){
  std::string csvString;
  // Sample the times to get a reasonable number of points
  int numSamples = 1000;
  int spacing = 1 + (int) times.size() / numSamples;

  // record the bounds and times at each sample
  for (int i = 0; i < times.size() - 1; i += spacing) {
    // Concatenate elements with commas and add a newline character
    csvString += std::to_string(instanceIndex) + "," +
                 std::to_string(primalBounds[i]) + "," +
                 std::to_string(dualBounds[i]) + "," +
                 std::to_string(times[i]) + "\n";
  }
  // add the last entry without a newline
  csvString += std::to_string(instanceIndex) + "," +
               std::to_string(primalBounds[times.size() - 1]) + "," +
               std::to_string(dualBounds[times.size() - 1]) + "," +
               std::to_string(times[times.size() - 1]);
  return csvString;
}
