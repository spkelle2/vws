/**
 * @file VwsEventHandler.cpp
 * @author Sean Kelley
 * @date 2023-02-27
 */

// coin-or modules
#include <CbcModel.hpp>
#include "CbcEventHandler.hpp"

// project modules
#include "VwsEventHandler.hpp"


/** Event handler */
CbcEventHandler::CbcAction VwsEventHandler::event(CbcEvent whichEvent) {
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
    this->num_vars = rhs->num_vars;
  } else {
    this->num_vars = -1;
  }
}
