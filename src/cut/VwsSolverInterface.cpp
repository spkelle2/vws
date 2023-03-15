/**
 * @file VwsSolverInterface.cpp
 * @author Sean Kelley
 * @date 2023-01-31
 */

// standard library
#include <iostream> // cout
#include <memory> // shared_ptr
#include <string> // to_string
#include <vector> // vector

// coin-or modules
#include "CbcEventHandler.hpp" // CbcEventHandler
#include "CbcModel.hpp"
#include "CbcSolver.hpp" // cbcMain0
#include "CbcSolverHeuristics.hpp" // doHeuristics
#include "CbcStrategy.hpp" // CbcStrategy
#include "CglPreProcess.hpp" // CglPreProcess
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface

// project modules
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp" // getVariableNames, getConstraintNames, putBackSolutions


/** constructor */
VwsSolverInterface::VwsSolverInterface(int maxExtraSavedSolutions, int maxRunTime, int disjunctiveTerms):
  maxExtraSavedSolutions(maxExtraSavedSolutions),
  maxRunTime(maxRunTime),
  disjunctiveTerms(disjunctiveTerms)
  {
} /* constructor */


/** Solve a MIP with VPCs added. Adds the provided eventHandler to the solve,
 * preprocessing the input model if requested */
CbcModel VwsSolverInterface::solve(OsiClpSolverInterface& instanceSolver,
                                   bool usePreprocessing, CbcEventHandler* eventHandler){

  // todo: enforce that we have a MIP with same number of variables and constraints
  // todo: create a parameters file for vws that can set up parameters for VPC and CBC

  // instanceSolver passed by reference since names aren't carried over when CbcModel constructor copies it
  variableNames.push_back(getVariableNames(instanceSolver));
  constraintNames.push_back(getConstraintNames(instanceSolver));

  // instantiate and solve the model
  // we need instanceSolver to instantiate the preprocessor so solution info is
  // returned to it in postprocessing. Therefore, we have to instantiate model
  // within a subroutine, but still need to access it here, necessitating the pointer
  std::shared_ptr<CbcModel> model;
  if (usePreprocessing) {
    model = preprocessedBranchAndCut(instanceSolver, eventHandler);
  } else {
    model = unprocessedBranchAndCut(instanceSolver, eventHandler);
  }

  // save the solutions in memory
  std::vector< std::vector <double>> problemSolutions;
  for (int j = 0; j < model->numberSavedSolutions(); j++) {
    std::vector<double> solution(model->savedSolution(j),
                                 model->savedSolution(j) + model->getNumCols());
    problemSolutions.push_back(solution);
  }
  solutions.push_back(problemSolutions);

  // return the model for any further analysis by user
  return *model;
} /* solve */

/** Uses a default Branch and Cut scheme to solve a MIP without additional preprocessing.
 *  Note: we give a new name to the solver here since it could already be preprocessed
 *  and we don't want to clash names with the unprocessed instanceSolver from the
 *  original formulation. Adds the provided eventHandler to the solve. */
std::shared_ptr<CbcModel> VwsSolverInterface::unprocessedBranchAndCut(
    OsiClpSolverInterface& solver, CbcEventHandler* eventHandler){

  // instantiate the model
  std::shared_ptr<CbcModel> model = std::make_shared<CbcModel>(solver);
  model->passInEventHandler(eventHandler);

  // turn on heuristics
  CbcSolverUsefulData cbcData;
  CbcMain0(*model, cbcData);
  cbcData[CbcParam::DIVEOPT]->setVal(2);
  cbcData[CbcParam::FPUMPITS]->setVal(30);
  // turn down heuristics timeliness
//  cbcData[CbcParam::FPUMPTUNE]->setVal(1005043);
//  cbcData[CbcParam::DIVINGC]->setVal("on");
//  cbcData[CbcParam::RINS]->setVal("on");
  doHeuristics(model.get(), 1, cbcData, cbcData.noPrinting(), 1005043);

  // turn on strong branching and cut generation
  CbcStrategyDefault strategy(false, 5, 0);
  model->setStrategy(strategy);

  // set number of solutions to save and time limit
  model->setMaximumSavedSolutions(maxExtraSavedSolutions);
  model->setDblParam(CbcModel::CbcMaximumSeconds, maxRunTime);

#ifndef TRACE
  std::cout << "using elapsed time" << std::endl;
  model->setUseElapsedTime(true);
#endif

  // run the solver
  model->branchAndBound();

  // update the solver interface with the solution info
  solver = *dynamic_cast<OsiClpSolverInterface*>(model->solver());

  return model;
} /* unprocessedBranchAndCut */

/** Preprocesses a MIP, uses a default Branch and Cut scheme to solve it,
 * and returns the preprocessed solution information back to the original problem
 * space. Adds the provided eventHandler to the solve. */
std::shared_ptr<CbcModel> VwsSolverInterface::preprocessedBranchAndCut(
    OsiClpSolverInterface& instanceSolver, CbcEventHandler* eventHandler){

  std::shared_ptr<CbcModel> model = std::make_shared<CbcModel>(instanceSolver);

  // preprocess the problem
  CglPreProcess process;
  OsiClpSolverInterface * preprocessedSolver =
      dynamic_cast<OsiClpSolverInterface*>(process.preProcess(instanceSolver, false, 5));
  verify(preprocessedSolver, "Pre-processing says infeasible");

  // run the default branch and cut scheme on the preprocessed problem
  std::shared_ptr<CbcModel> preprocessedModel =
      unprocessedBranchAndCut(*preprocessedSolver, eventHandler);

  // move the solutions, bound info, and event handler to the original model/solver interface
  model->setMaximumSavedSolutions(maxExtraSavedSolutions);
  putBackSolutions(preprocessedModel.get(), model.get(), &process);
  process.postProcess(*preprocessedModel->solver());
  model->moveInfo(*preprocessedModel);
  model->passInEventHandler(preprocessedModel->getEventHandler());

  return model;
} /* preprocessedBranchAndCut */