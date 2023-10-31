/**
 * @file VwsEventHandler.cpp
 * @author Sean Kelley
 * @date 2023-05-01
 */

#include <memory> // shared_ptr

// coin-or modules
#include "CbcEventHandler.hpp"
#include "CbcModel.hpp"
#include "OsiSolverInterface.hpp"

// vpc modules
#include "utility.hpp"

// project modules
#include "VwsEventHandler.hpp"

/** Event handler */
CbcEventHandler::CbcAction VwsEventHandler::event(CbcEvent whichEvent) {

  // get the bound from LP relaxation with and without vpcs
  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::startUp){

    double direction = model_->solver()->getObjSense();
    OsiSolverInterface *solver = model_->solver()->clone();

    // get the LP bound
    solver->initialSolve();
    data.lpBound = solver->getObjValue() * direction;

    // apply the cuts
    if (cuts){
      solver->applyCuts(*cuts);
    }
    solver->resolve();

    // get the LP bound after vpcs
    data.lpBoundPostVpc = solver->getObjValue() * direction;
  }

  // overwrite heuristic time and bound with each primal heuristic pass to values at last one
  if ((model_->specialOptions() & 2048) == 0 && (whichEvent == CbcEventHandler::afterHeuristic ||
       whichEvent == CbcEventHandler::heuristicPass) && !finished_primal_heuristics) {
    timer.end_timer("heuristicTime");
    data.heuristicTime = timer.get_time("heuristicTime");
    timer.start_timer("heuristicTime");
    data.heuristicPrimalBound = model_->getObjValue();
  }

  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::afterRootCuts) {

    // get the dual bound before cuts and after root node cuts
    double direction = model_->solver()->getObjSense();
    data.rootDualBound = model_->solver()->getObjValue() * direction;

    // stop the root node timer
    timer.end_timer("rootDualBoundTime");
    data.rootDualBoundTime = timer.get_time("rootDualBoundTime");
    finished_primal_heuristics = true;
  }

  // record solution times
  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::solution) {

      // record time to first solution
      if (data.firstSolutionTime <= 1e-16){
        timer.end_timer("firstSolutionTime");
        data.firstSolutionTime = timer.get_time("firstSolutionTime");
      }
      // record time to best solution
      if (model_->solver()->getObjValue() < model_->getObjValue()){
        timer.end_timer("bestSolutionTime");
        data.bestSolutionTime = timer.get_time("bestSolutionTime");
        timer.start_timer("bestSolutionTime");
      }
  }

  // pause when we hit a terminating condition
  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::endSearch) {
    double direction = model_->solver()->getObjSense();
    data.dualBound = model_->getBestPossibleObjValue() * direction;
    data.primalBound = model_->getObjValue() * direction;
    data.nodes = model_->getNodeCount();
    data.iterations = model_->getIterationCount();
    timer.end_timer("terminationTime");
    data.terminationTime = timer.get_time("terminationTime");
  }

  return noAction;
}

/** constructor (default) */
VwsEventHandler::VwsEventHandler() : CbcEventHandler(){
  initialize(NULL);
}

/** constructor from CbcModel */
VwsEventHandler::VwsEventHandler(CbcModel *model) : CbcEventHandler(model){
  initialize(NULL);
}

/** destructor */
VwsEventHandler::~VwsEventHandler() {
}

/** copy constructor */
VwsEventHandler::VwsEventHandler(const VwsEventHandler& rhs) : CbcEventHandler(rhs) {
  initialize(&rhs);
}

/** assignment operator */
VwsEventHandler& VwsEventHandler::operator=(const VwsEventHandler& rhs) {
  if (this != &rhs) {
    CbcEventHandler::operator=(rhs);
  }
  initialize(&rhs);
  return *this;
}

/** clone */
CbcEventHandler* VwsEventHandler::clone() const {
  return new VwsEventHandler(*this);
}

/** Copy our stuff */
void VwsEventHandler::initialize(const VwsEventHandler* const rhs) {
  if (rhs) {
    this->cuts = rhs->cuts;
    this->data = rhs->data;
    this->timer = rhs->timer;
    this->finished_primal_heuristics = rhs->finished_primal_heuristics;
  } else {
    RunData data = RunData();
    TimeStats timer = TimeStats();
    finished_primal_heuristics = false;
  }
}
