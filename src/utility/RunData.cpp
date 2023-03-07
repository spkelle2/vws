/**
 * @file RunData.cpp
 * @author Sean Kelley
 * @date 2023-03-06
 */


// standard modules
#include <string> // string

// project modules
#include "RunData.hpp"


/** Get a comma-separated string of the names of RunData's attributes */
std::string RunData::getHeader(){
  return "lpBound,rootDualBound,dualBound,primalBound,heuristicTime,rootDualBoundTime,terminationTime";
}

/** Get a comma-separated string of the values of RunData's attributes */
std::string RunData::getValues(){
  return std::to_string(lpBound) + "," + std::to_string(rootDualBound) + "," +
    std::to_string(dualBound) + "," + std::to_string(primalBound) + "," +
    std::to_string(heuristicTime) + "," + std::to_string(rootDualBoundTime) + "," +
    std::to_string(terminationTime);
}

