/**
 * @file MipCompEventHandler.cpp
 * @author Sean Kelley
 * @date 2023-02-27
 */

// coin-or modules
#include <CbcModel.hpp>
#include "CbcEventHandler.hpp"

// project modules
#include "MipCompEventHandler.hpp"
#include "RunData.hpp"


/** Event handler */
CbcEventHandler::CbcAction MipCompEventHandler::event(CbcEvent whichEvent) {

  // overwrite the heuristic time with each primal heuristic pass to get time at the last one
  if ((model_->specialOptions() & 2048) != 0 && whichEvent == CbcEventHandler::heuristicPass) {
    data.heuristicTime = model_->getCurrentSeconds();
  }

  // pause each time the tree accurately reflects solver status
  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::afterRootCuts) {
    double direction = model_->solver()->getObjSense();
    data.lpBound = model_->getContinuousObjective() * direction;
    data.rootDualBound = model_->solver()->getObjValue() * direction;
    data.rootDualBoundTime = model_->getCurrentSeconds();
  }

  // pause when we hit a terminating condition
  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::endSearch) {
    double direction = model_->solver()->getObjSense();
    data.dualBound = model_->getBestPossibleObjValue() * direction;
    data.primalBound = model_->getObjValue() * direction;
    data.terminationTime = model_->getCurrentSeconds();
  }

  return noAction;
}

/** constructor (default) */
MipCompEventHandler::MipCompEventHandler() : CbcEventHandler(){
  initialize(NULL);
}

/** constructor from CbcModel */
MipCompEventHandler::MipCompEventHandler(CbcModel *model) : CbcEventHandler(model){
  initialize(NULL);
}

/** destructor */
MipCompEventHandler::~MipCompEventHandler() {
}

/** copy constructor */
MipCompEventHandler::MipCompEventHandler(const MipCompEventHandler& rhs) : CbcEventHandler(rhs) {
  initialize(&rhs);
}

/** assignment operator */
MipCompEventHandler& MipCompEventHandler::operator=(const MipCompEventHandler& rhs) {
  if (this != &rhs) {
    CbcEventHandler::operator=(rhs);
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
  if (rhs) {
    this->data = rhs->data;
  } else {
    RunData data = RunData();
  }
}
