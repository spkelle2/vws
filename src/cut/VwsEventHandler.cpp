/**
 * @file VwsEventHandler.cpp
 * @author Sean Kelley
 * @date 2023-05-01
 */

#include <memory> // shared_ptr

// coin-or modules
#include "CbcEventHandler.hpp"
#include "CbcModel.hpp"

// project modules
#include "CglStoredVpc.hpp"
#include "VwsEventHandler.hpp"


/** Event handler */
CbcEventHandler::CbcAction VwsEventHandler::event(CbcEvent whichEvent) {

  // make cuts available after the root node
  // todo: ensure propagation throughout the tree - see john's comment on github
  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::afterRootCuts) {
    if (cuts) {
      std::shared_ptr<CglStoredVpc> store = std::make_shared<CglStoredVpc>();
      for (int i = 0; i<cuts->sizeRowCuts(); i++) {
        store->addCut(cuts->rowCut(i));
      }
      model_->addCutGenerator(store.get(), 1, "StoredVPCs");
    }
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
  }
}