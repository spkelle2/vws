/**
 * @file VwsEventHandler.cpp
 * @author Sean Kelley
 * @date 2023-05-01
 */

// coin-or modules
#include "CbcEventHandler.hpp"
#include "CbcModel.hpp"

// project modules
#include "VwsEventHandler.hpp"


/** Event handler */
CbcEventHandler::CbcAction VwsEventHandler::event(CbcEvent whichEvent) {

  // add desired cuts to the root node
  // todo: ensure propagation throughout the tree - see john's comment on github
  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::afterRootCuts) {
    if (cuts) {
      model_->solver()->applyCuts(*cuts);
      model_->solver()->resolve();
      model_->setMoreSpecialOptions2(model_->moreSpecialOptions2() | 524288);
      stopNow = true;
    }
  }

  return stopNow ? stop : noAction;
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
  }
}