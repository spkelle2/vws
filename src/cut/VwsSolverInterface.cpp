/**
 * @file VwsSolverInterface.cpp
 * @author Sean Kelley
 * @date 2023-01-31
 */

#include <chrono> // now, timeSinceEpoch
#include <iostream> // cout
#include <random> // seed_seq, mt19937, uniform_real_distribution

// #include "BBHelper.hpp" // doBranchAndBoundWithUserCutsCbc
#include "VwsSolverInterface.hpp"


/** Default constructor */
VwsSolverInterface::VwsSolverInterface() {
  // seed the random number generator
  const int givenSeed = 1;  // params.get(intParam::RANDOM_SEED);
  const auto randomSeed = givenSeed; // > 0 ? givenSeed : std::chrono::system_clock::now().time_since_epoch().count();
  std::seed_seq currentSeed{randomSeed};
  randomNumberGenerator.seed(currentSeed);
} /* default constructor */


/** Solve a MIP with VPCs added */
void VwsSolverInterface::solve(CbcModel& model, double p){

  // todo: check any assumptions on the model
  assert(0 <= p && p <= 1);  // probability should be between 0 and 1 inclusive

  std::uniform_real_distribution<> zeroOneUniformDistribution(0.0, 1.0);

  if (zeroOneUniformDistribution(randomNumberGenerator) < p || !disjunction.size()) {
    model.initialSolve();
  } else {
    model.initialSolve();
  }
} /* solve */
