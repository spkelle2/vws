/**
 * @file VwsSolverInterface.cpp
 * @author Sean Kelley
 * @date 2023-01-31
 */

// standard library
#include <iostream> // cout
#include <string> // to_string
#include <vector> // vector

// coin-or modules
#include "CbcModel.hpp"
#include "CbcSolver.hpp" // cbcMain0
#include "CbcSolverHeuristics.hpp" // doHeuristics
#include "CbcStrategy.hpp" // CbcStrategy
#include "CglPreProcess.hpp" // CglPreProcess
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface

// project modules
#include "VwsEventHandler.hpp"
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp" // getVariableNames, getConstraintNames, putBackSolutions


/** constructor */
VwsSolverInterface::VwsSolverInterface(int maxSavedSolutions, int maxRunTime, int disjunctiveTerms):
  maxSavedSolutions(maxSavedSolutions),
  maxRunTime(maxRunTime),
  disjunctiveTerms(disjunctiveTerms)
  {
} /* constructor */


/** Solve a MIP with VPCs added */
void VwsSolverInterface::solve(OsiClpSolverInterface& instanceSolver){

  // todo: make preprocessing optional
  // todo: enforce that we have a MIP with same number of variables and constraints
  // todo: create a parameters file for vws that can set up parameters for VPC and CBC

  // save variable and constraint names
  variableNames.push_back(getVariableNames(instanceSolver));
  constraintNames.push_back(getConstraintNames(instanceSolver));

  // preprocess the problem
  CbcModel * model = new CbcModel(instanceSolver);
  CglPreProcess process;
  OsiSolverInterface * preprocessedSolver = process.preProcess(instanceSolver, false, 5);
  verify(preprocessedSolver, "Pre-processing says infeasible");
  CbcModel * preprocessedModel = new CbcModel(*preprocessedSolver);

  // turn on heuristics
  CbcSolverUsefulData cbcData;
  CbcMain0(*preprocessedModel, cbcData);
  doHeuristics(preprocessedModel, 2, cbcData, cbcData.noPrinting(), 1005043);

  // turn on strong branching and cut generation
  CbcStrategyDefault strategy(false, 5, 0);
  preprocessedModel->setStrategy(strategy);

  // set number of solutions to save and time limit
  preprocessedModel->setMaximumSavedSolutions(maxSavedSolutions);
  preprocessedModel->setDblParam(CbcModel::CbcMaximumSeconds, maxRunTime);

  // run the solver
  preprocessedModel->branchAndBound();

  // move the solutions and bound info to the original model and solver interface
  putBackSolutions(preprocessedModel, model, &process);
  process.postProcess(*preprocessedModel->solver());
  model->moveInfo(*preprocessedModel);
  // model.initialSolve();

  // save the solutions in memory
  std::vector< std::vector <double>> problemSolutions;
  for (int j = 0; j < model->numberSavedSolutions(); j++) {
    std::vector<double> solution(model->savedSolution(j),
                                 model->savedSolution(j) + model->getNumCols());
    problemSolutions.push_back(solution);
  }
  solutions.push_back(problemSolutions);
  dualBounds.push_back(model->getBestPossibleObjValue());

  delete preprocessedModel;
  delete model;
} /* solve */

