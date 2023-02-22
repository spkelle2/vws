/**
 * @file VwsSolverInterface.cpp
 * @author Sean Kelley
 * @date 2023-01-31
 */

// standard library
#include <iostream> // cout
#include <vector> // vector

// project modules
#include "VwsUtility.hpp"
#include "VwsSolverInterface.hpp"


/** constructor */
VwsSolverInterface::VwsSolverInterface(int maxSavedSolutions):
  maxSavedSolutions(maxSavedSolutions) {
} /* constructor */


/** Solve a MIP with VPCs added */
void VwsSolverInterface::solve(CbcModel& model){

  // todo: enforce that we have a MIP with same number of variables and constraints

  // set model parameters
  model.setMaximumSavedSolutions(maxSavedSolutions);

  // save variable and constraint names
  variableNames.push_back(getVariableNames(model));
  constraintNames.push_back(getConstraintNames(model));

  // run initial solve if it hasn't happened yet bc branchAndBound assumes it's done
  if (model.status() < 0) {
    model.initialSolve();
  }

  // solve the MIP and record related stats
  model.branchAndBound(3);

  // save the solutions in memory
  std::vector< std::vector <double>> problemSolutions;
  for (int j = 0; j < model.numberSavedSolutions(); j++) {
    std::vector<double> solution(model.savedSolution(j),
                                 model.savedSolution(j) + model.getNumCols());
    problemSolutions.push_back(solution);
  }
  solutions.push_back(problemSolutions);

} /* solve */

