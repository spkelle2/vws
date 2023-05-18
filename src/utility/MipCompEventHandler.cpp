/**
 * @file MipCompEventHandler.cpp
 * @author Sean Kelley
 * @date 2023-02-27
 */

// coin-or modules
#include <CbcModel.hpp>
#include "CbcEventHandler.hpp"
#include "OsiSolverInterface.hpp"

// project modules
#include "MipCompEventHandler.hpp"
#include "RunData.hpp"
#include "VwsEventHandler.hpp"


/** Event handler */
CbcEventHandler::CbcAction MipCompEventHandler::event(CbcEvent whichEvent) {

  // let the VwsEventHandler handle the action
  CbcAction action = VwsEventHandler::event(whichEvent);

  // overwrite the heuristic time with each primal heuristic pass to get time at the last one
  if ((model_->specialOptions() & 2048) == 0 &&
      (whichEvent == CbcEventHandler::afterHeuristic ||
       whichEvent == CbcEventHandler::heuristicPass)) {
    data.heuristicTime = model_->getCurrentSeconds();
    data.heuristicPrimalBound = model_->getObjValue();
  }

  // get root node info
  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::afterRootCuts) {
    double direction = model_->solver()->getObjSense();
    data.lpBound = model_->getContinuousObjective() * direction;
    data.rootDualBoundPreVpc = model_->solver()->getObjValue() * direction;

    // determine what root dual bound would be if we were able to add VPCs
    if (cuts) {
      OsiSolverInterface *solver = model_->solver()->clone();
      solver->initialSolve();
      solver->applyCuts(*cuts);
      solver->resolve();
      data.rootDualBound = solver->getObjValue() * direction;
    } else {
      data.rootDualBound = data.rootDualBoundPreVpc;
    }
    data.rootDualBoundTime = model_->getCurrentSeconds();
  }

  // pause when we hit a terminating condition
  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::endSearch) {
    double direction = model_->solver()->getObjSense();
    data.dualBound = model_->getBestPossibleObjValue() * direction;
    data.primalBound = model_->getObjValue() * direction;
    data.terminationTime = model_->getCurrentSeconds();
  }

  return action;
}

/** constructor (default) */
MipCompEventHandler::MipCompEventHandler() : VwsEventHandler(){
  initialize(NULL);
}

/** constructor from CbcModel */
MipCompEventHandler::MipCompEventHandler(CbcModel *model) : VwsEventHandler(model){
  initialize(NULL);
}

/** destructor */
MipCompEventHandler::~MipCompEventHandler() {
  VwsEventHandler::~VwsEventHandler();
}

/** copy constructor */
MipCompEventHandler::MipCompEventHandler(const MipCompEventHandler& rhs) : VwsEventHandler(rhs) {
  initialize(&rhs);
}

/** assignment operator */
MipCompEventHandler& MipCompEventHandler::operator=(const MipCompEventHandler& rhs) {
  if (this != &rhs) {
    VwsEventHandler::operator=(rhs);
  }
  initialize(&rhs);
  return *this;
}

/** clone */
CbcEventHandler* MipCompEventHandler::clone() const {
  return new MipCompEventHandler(*this);
}

/** Copy our stuff */
void MipCompEventHandler::initialize(const MipCompEventHandler* const rhs) {
  // VwsEventHandler::initialize called either explicitly or through inheritance
  // in each function calling self's initialize, so no need to call it here
  if (rhs) {
    this->data = rhs->data;
  } else {
    RunData data = RunData();
  }
}
