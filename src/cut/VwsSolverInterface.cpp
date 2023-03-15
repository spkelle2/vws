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
#include <OsiCuts.hpp> // OsiCuts

// vpc modules
#include "CglVPC.hpp" // CglVPC
#include "SolverInterface.hpp" // SolverInterface
#include "VPCParameters.hpp" // VPCParameters

// project modules
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp" // getVariableNames, getConstraintNames, putBackSolutions


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
CbcModel VwsSolverInterface::solve(OsiClpSolverInterface& instanceSolver,
                                   std::string vpcGenerator, bool usePreprocessing,
                                   CbcEventHandler* eventHandler){

  // todo: enforce that we have a MIP with same number of variables and constraints
  // todo: enforce that we have a MIP with expected constraint format
  // todo: create a parameters file for vws that can set up parameters for VPC and CBC

  verify(vpcGenerator == "PRLP" || vpcGenerator == "Farkas" || vpcGenerator == "None",
         "VwsSolverInterface::solve: vpcGenerator must be either PRLP, Farkas, or None");

  double vpcGenTime = 0.0;

  // instanceSolver passed by reference since names aren't carried over when CbcModel constructor copies it
  variableNames.push_back(getVariableNames(instanceSolver));
  constraintNames.push_back(getConstraintNames(instanceSolver));

  // create cuts - either by PRLP or by previous farkas multipliers
  std::shared_ptr<OsiCuts> disjCuts;
  if (vpcGenerator == "PRLP") {
    disjCuts = createDisjunctiveCutsFromPRLP(instanceSolver, vpcGenTime);
  } else if (vpcGenerator == "Farkas") {
    disjCuts = createDisjunctiveCutsFromFarkasMultipliers(instanceSolver);
  }

  // instantiate and solve the model
  // we need instanceSolver to instantiate the preprocessor so solution info is
  // returned to it in postprocessing. Therefore, we have to instantiate model
  // within a subroutine, but still need to access it here, necessitating the pointer
  std::shared_ptr<CbcModel> model;
  if (usePreprocessing) {
    model = preprocessedBranchAndCut(instanceSolver, disjCuts.get(), eventHandler, vpcGenTime);
  } else {
    model = unprocessedBranchAndCut(instanceSolver, disjCuts.get(), eventHandler, vpcGenTime);
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
    OsiClpSolverInterface& solver, OsiCuts* cuts, CbcEventHandler* eventHandler,
    double vpcGenTime){

  // instantiate the model
  if (cuts) {
    solver.applyCuts(*cuts);
  }
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
    OsiClpSolverInterface& instanceSolver, OsiCuts* cuts, CbcEventHandler* eventHandler,
    double vpcGenTime){

  if (cuts) {
    instanceSolver.applyCuts(*cuts);
  }
  std::shared_ptr<CbcModel> model = std::make_shared<CbcModel>(instanceSolver);

  // preprocess the problem
  CglPreProcess process;
  OsiClpSolverInterface * preprocessedSolver =
      dynamic_cast<OsiClpSolverInterface*>(process.preProcess(instanceSolver, false, 5));
  verify(preprocessedSolver, "Pre-processing says infeasible");

  // run the default branch and cut scheme on the preprocessed problem
  std::shared_ptr<CbcModel> preprocessedModel =
      unprocessedBranchAndCut(*preprocessedSolver, NULL, eventHandler, vpcGenTime);

  // move the solutions, bound info, and event handler to the original model/solver interface
  model->setMaximumSavedSolutions(maxExtraSavedSolutions);
  putBackSolutions(preprocessedModel.get(), model.get(), &process);
  process.postProcess(*preprocessedModel->solver());
  model->moveInfo(*preprocessedModel);
  model->passInEventHandler(preprocessedModel->getEventHandler());

  return model;
} /* preprocessedBranchAndCut */

// todo: place const wherever I'm copying objects and don't need to

/** Creates cuts from a PRLP relaxation of the disjunctive terms found from
 *  partially solving the given problem encoded in the solver interface.
 *  Simplified from Strengthening's CglAdvCut::generateCuts */
std::shared_ptr<OsiCuts> VwsSolverInterface::createDisjunctiveCutsFromPRLP(
    OsiClpSolverInterface si, double& vpcGenTime) {

  // todo set time limit and get time stats
  si.initialSolve();
  verify(si.isProvenOptimal(), "Solver must be optimal to create disjunctive cuts");
  std::shared_ptr<OsiCuts> disjCuts = std::make_shared<OsiCuts>();

  //==================================================//
  // Set up VPC generation
  VPCParametersNamespace::VPCParameters vpc_params;
  vpc_params.set(VPCParametersNamespace::DISJ_TERMS, disjunctiveTerms);
  vpc_params.set(VPCParametersNamespace::CUTLIMIT, si.getFractionalIndices().size());
  vpc_params.set(VPCParametersNamespace::TIMELIMIT, vpcGenTimeRatio * maxRunTime);
  vpc_params.set(VPCParametersNamespace::PARTIAL_BB_TIMELIMIT, vpcGenTimeRatio * maxRunTime);
  vpc_params.set(VPCParametersNamespace::USE_ALL_ONES, 1);
  vpc_params.set(VPCParametersNamespace::USE_ITER_BILINEAR, 1);
  vpc_params.set(VPCParametersNamespace::USE_DISJ_LB, 1);
  vpc_params.set(VPCParametersNamespace::USE_TIGHT_POINTS, 0);
  vpc_params.set(VPCParametersNamespace::USE_TIGHT_RAYS, 0);
  vpc_params.set(VPCParametersNamespace::USE_UNIT_VECTORS, 0);

  std::shared_ptr<CglVPC> gen = std::make_shared<CglVPC>(vpc_params);
  // todo: check that the RHS's created here match what we would get from VwsUtilility::getCertificate
  gen->generateCuts(si, *disjCuts);
  Disjunction* disj = gen->disj();
  vpcGenTime = gen->timer.get_total_time("TOTAL_TIME");

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

  // todo: recalc RHS from Farkas multipliers
  std::shared_ptr<OsiCuts> disjCuts;

  for (int probIdx=0; probIdx < cutCertificates.size(); probIdx++){
    for (int cutIdx=0; cutIdx < cutCertificates[probIdx].size(); cutIdx++){
      Disjunction* disj = vpcGenerators[probIdx]->disj();
      std::vector< std::vector<double> > a;
      a.resize(cutCertificates[probIdx][cutIdx].size());
      for (int termIdx=0; termIdx < cutCertificates[probIdx][cutIdx].size(); termIdx++){
        OsiSolverInterface* termSolver;
        disj->getSolverForTerm(termSolver, termIdx, &si, false, .001, NULL);
        getCutFromCertificate(a[termIdx], cutCertificates[probIdx][cutIdx][termIdx], termSolver);
      }
    }
  }

  return disjCuts;

  // confirm we have a^T x >= b
  // take a max with those from the other disjunctive terms
  // take a min for rhs's - maybe need to add a double to getCutFromCertificate to get RHS
  // use eigen

}