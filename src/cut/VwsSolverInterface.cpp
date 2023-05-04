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
#include "CbcModel.hpp"
#include "CbcSolver.hpp" // cbcMain0
#include "CbcSolverHeuristics.hpp" // doHeuristics
#include "CbcStrategy.hpp" // CbcStrategy
#include "CglPreProcess.hpp" // CglPreProcess
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface
#include <OsiCuts.hpp> // OsiCuts

// Eigen library
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>

// vpc modules
#include "CglVPC.hpp" // CglVPC
#include "CompleteDisjunction.hpp" // CompleteDisjunction
#include "SolverInterface.hpp" // SolverInterface
#include "VPCParameters.hpp" // VPCParameters

// project modules
#include "VwsEventHandler.hpp" // VwsEventHandler
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp" // getVariableNames, getConstraintNames, putBackSolutions, findNonZero


/** constructor */
VwsSolverInterface::VwsSolverInterface(int maxExtraSavedSolutions, double maxRunTime,
                                       int disjunctiveTerms, double vpcGenTimeRatio):
  maxExtraSavedSolutions(maxExtraSavedSolutions),
  maxRunTime(maxRunTime),
  disjunctiveTerms(disjunctiveTerms),
  vpcGenTimeRatio(vpcGenTimeRatio)
  {
} /* constructor */


/** Solve a MIP with VPCs added. Adds the provided eventHandler to the solve,
 * preprocessing the input model if requested */
CbcModel VwsSolverInterface::solve(const OsiClpSolverInterface& instanceSolver,
                                   std::string vpcGenerator, bool usePreprocessing,
                                   VwsEventHandler* eventHandler){

  // todo: enforce that we have a MIP with same number of variables and constraints
  // todo: enforce that we have a MIP with expected constraint format
  // todo: create a parameters file for vws that can set up parameters for VPC and CBC

  verify(vpcGenerator == "PRLP" || vpcGenerator == "Farkas" || vpcGenerator == "None",
         "VwsSolverInterface::solve: vpcGenerator must be either PRLP, Farkas, or None");
  // currently, we don't know how to weave cuts through the preprocessor, so we
  // lack a way to create VPCs and add them after root cut generation
  verify(!((vpcGenerator == "PRLP" || vpcGenerator == "Farkas") && usePreprocessing),
         "VwsSolverInterface::solve: if creating VPCs, the preprocessor must be disabled");

  // if we don't have an event handler, create a default one
  VwsEventHandler h;
  if (!eventHandler){
    eventHandler = &h;
  }

  // instanceSolver passed by reference since names aren't carried over when CbcModel constructor copies it
  variableNames.push_back(getVariableNames(instanceSolver));
  constraintNames.push_back(getConstraintNames(instanceSolver));

  // create cuts - either by PRLP or by previous farkas multipliers
  // todo: make a timer class in scope and use it here
  std::time_t startTime = std::time(nullptr);
  std::shared_ptr<OsiCuts> disjCuts;
  if (vpcGenerator == "PRLP") {
    disjCuts = createDisjunctiveCutsFromPRLP(instanceSolver);
  } else if (vpcGenerator == "Farkas") {
    disjCuts = createDisjunctiveCutsFromFarkasMultipliers(instanceSolver);
  }
  eventHandler->cuts = disjCuts.get();
  std::time_t endTime = std::time(nullptr);
  double vpcGenTime = std::difftime(endTime, startTime);

  // instantiate and solve the model
  // we need instanceSolver to instantiate the preprocessor so solution info is
  // returned to it in postprocessing. Therefore, we have to instantiate model
  // within a subroutine, but still need to access it here, necessitating the pointer
  std::shared_ptr<CbcModel> model;
  if (usePreprocessing) {
    model = preprocessedBranchAndCut(instanceSolver, eventHandler, vpcGenTime);
  } else {
    model = unprocessedBranchAndCut(instanceSolver, eventHandler, vpcGenTime);
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
    OsiClpSolverInterface solver, VwsEventHandler* eventHandler, double vpcGenTime){

  std::shared_ptr<CbcModel> model = std::make_shared<CbcModel>(solver);
  model->passInEventHandler(eventHandler);

  // turn on heuristics
  CbcSolverUsefulData cbcData;
  CbcMain0(*model, cbcData);
  cbcData[CbcParam::DIVEOPT]->setVal(2);
  cbcData[CbcParam::FPUMPITS]->setVal(30);
  cbcData[CbcParam::FPUMPTUNE]->setVal(1005043);
  cbcData[CbcParam::DIVINGC]->setVal("on");
  cbcData[CbcParam::RINS]->setVal("on");
  doHeuristics(model.get(), 1, cbcData, cbcData.noPrinting(), 1005043);

  // turn on strong branching and cut generation
  CbcStrategyDefault strategy(false, 5, 0);
  model->setStrategy(strategy);

  // set number of solutions to save and time limit
  model->setMaximumSavedSolutions(maxExtraSavedSolutions);
  model->setDblParam(CbcModel::CbcMaximumSeconds, maxRunTime - vpcGenTime);

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
    OsiClpSolverInterface instanceSolver, VwsEventHandler* eventHandler, double vpcGenTime){

  std::shared_ptr<CbcModel> model = std::make_shared<CbcModel>(instanceSolver);

  // preprocess the problem
  CglPreProcess process;
  OsiClpSolverInterface * preprocessedSolver =
      dynamic_cast<OsiClpSolverInterface*>(process.preProcess(instanceSolver, false, 5));
  verify(preprocessedSolver, "Pre-processing says infeasible");

  // run the default branch and cut scheme on the preprocessed problem
  std::shared_ptr<CbcModel> preprocessedModel =
      unprocessedBranchAndCut(*preprocessedSolver, eventHandler, vpcGenTime);

  // move the solutions, bound info, and event handler to the original model/solver interface
  model->setMaximumSavedSolutions(maxExtraSavedSolutions);
  putBackSolutions(preprocessedModel.get(), model.get(), &process);
  process.postProcess(*preprocessedModel->solver());
  model->moveInfo(*preprocessedModel);
  model->passInEventHandler(preprocessedModel->getEventHandler());

  return model;
} /* preprocessedBranchAndCut */

/** Creates cuts from a PRLP relaxation of the disjunctive terms found from
 *  partially solving the given problem encoded in the solver interface.
 *  Simplified from Strengthening's CglAdvCut::generateCuts */
std::shared_ptr<OsiCuts> VwsSolverInterface::createDisjunctiveCutsFromPRLP(
    OsiClpSolverInterface si) {

  si.initialSolve();
  verify(si.isProvenOptimal(), "Solver must be optimal to create disjunctive cuts");
  std::shared_ptr<OsiCuts> disjCuts = std::make_shared<OsiCuts>();

  //==================================================//
  // Set up VPC generation
  VPCParametersNamespace::VPCParameters vpc_params;
  vpc_params.set(VPCParametersNamespace::DISJ_TERMS, disjunctiveTerms);
//  vpc_params.set(VPCParametersNamespace::CUTLIMIT, si.getFractionalIndices().size());
//  vpc_params.set(VPCParametersNamespace::TIMELIMIT, vpcGenTimeRatio * maxRunTime);
//  vpc_params.set(VPCParametersNamespace::PARTIAL_BB_TIMELIMIT, vpcGenTimeRatio * maxRunTime);
//  vpc_params.set(VPCParametersNamespace::USE_ALL_ONES, 1);
//  vpc_params.set(VPCParametersNamespace::USE_ITER_BILINEAR, 1);
//  vpc_params.set(VPCParametersNamespace::USE_DISJ_LB, 1);
//  vpc_params.set(VPCParametersNamespace::USE_TIGHT_POINTS, 0);
//  vpc_params.set(VPCParametersNamespace::USE_TIGHT_RAYS, 0);
//  vpc_params.set(VPCParametersNamespace::USE_UNIT_VECTORS, 0);
  vpc_params.set(VPCParametersNamespace::MODE, 0); // 0 BB, 1 splits, 4 strong branching

  std::shared_ptr<CglVPC> gen = std::make_shared<CglVPC>(vpc_params);
  gen->generateCuts(si, *disjCuts);
  Disjunction* disj = gen->disj();

  // if we have cuts, save the cut generator and the Farkas multipliers
  if (disj && disj->terms.size() > 0 && disjCuts->sizeCuts() > 0 && disj->integer_sol.size() == 0){
    vpcGenerators.push_back(gen);
    cutCertificates.push_back(getFarkasMultipliers(si, *gen, *disjCuts));
    return disjCuts;
  }
  else {
    std::shared_ptr<OsiCuts> emptyCuts;
    return emptyCuts;
  }
}

/** Parameterize each previously created disjunctive cut with its disjunction
 *  and Farkas multipliers applied to the given solver. Borrowed from Strengthening's
 *  main.cpp */
std::shared_ptr<OsiCuts> VwsSolverInterface::createDisjunctiveCutsFromFarkasMultipliers(
    OsiClpSolverInterface si) {

  verify(cutCertificates.size() > 0, "No certificates to create disjunctive cuts");

  std::shared_ptr<OsiCuts> disjCuts = std::make_shared<OsiCuts>();
  bool feasibleTermSolver;
  bool ambiguousTermSolver;

  // set up vectors of disjunctive cuts
  // [problemIdx][cutIdx][termIdx][variableIdx]
  std::vector< std::vector< std::vector< std::vector<double> > > > a;
  // [problemIdx][cutIdx][termIdx]
  std::vector< std::vector< std::vector<double> > > b;
  a.resize(cutCertificates.size());
  b.resize(cutCertificates.size());
  for (int probIdx=0; probIdx < cutCertificates.size(); probIdx++){
    a[probIdx].resize(cutCertificates[probIdx].size());
    b[probIdx].resize(cutCertificates[probIdx].size());
    Disjunction* disj = vpcGenerators[probIdx]->disj();
    for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++) {
      a[probIdx][cutIdx].resize(disj->num_terms);
      b[probIdx][cutIdx].resize(disj->num_terms);
    }

    // calculate a cut for each disjunctive term
    ambiguousTermSolver = false;
    feasibleTermSolver = false;
    for (int termIdx=0; termIdx < disj->num_terms; termIdx++) {
      OsiSolverInterface* termSolver;
      disj->getSolverForTerm(termSolver, termIdx, &si, false, .001, NULL, false, false);
      // if we don't know if we're optimal or primal infeasible,
      // we can't safely use these cuts, so skip to next problem
      if (!termSolver->isProvenOptimal() && !termSolver->isProvenPrimalInfeasible()){
        ambiguousTermSolver = true;
        break;
      }
      for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++){
        a[probIdx][cutIdx][termIdx].resize(si.getNumCols());
        if (termSolver->isProvenOptimal()) {
          feasibleTermSolver = true;
          getCutFromCertificate(a[probIdx][cutIdx][termIdx], b[probIdx][cutIdx][termIdx],
                                cutCertificates[probIdx][cutIdx][termIdx], termSolver);
        } else {
          // primal infeasible - no cuts will violate this term,
          // so just make its cut to where it won't be chosen
          std::fill(a[probIdx][cutIdx][termIdx].begin(), a[probIdx][cutIdx][termIdx].end(), -1e50);
          b[probIdx][cutIdx][termIdx] = 1e50;
        }
      }
    }

    // if we are unsure the status of a solver or all terms were infeasible, move on
    if (ambiguousTermSolver || !feasibleTermSolver) {
      continue;
    }

    // create a valid disjunctive cut given all the valid term cuts
    for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++){
      std::vector<double> alpha = elementWiseMax(a[probIdx][cutIdx]);
      double beta = min(b[probIdx][cutIdx]);
      std::vector<int> indices;
      std::vector<double> elements;
      findNonZero(alpha, indices, elements);
      OsiRowCut cut;
      cut.setRow(indices.size(), indices.data(), elements.data());
      cut.setLb(beta);
      disjCuts->insert(cut);
    }
  }

  return disjCuts;
}