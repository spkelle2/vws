/**
 * @file MipCompEventHandler.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

// coin-or modules
#include <CbcModel.hpp>
#include "CbcEventHandler.hpp"

// project modules
#include "MipCompEventHandler.hpp"

MipCompEventHandler::MipCompEventHandler()
{
}

MipCompEventHandler::MipCompEventHandler(const MipCompEventHandler &rhs)
  : rootDualBoundTime(rhs.rootDualBoundTime),
    rootDualBound(rhs.rootDualBound),
    CbcEventHandler(rhs){

}

MipCompEventHandler::MipCompEventHandler(CbcModel *model)
    : CbcEventHandler(model)
{
}

MipCompEventHandler::~MipCompEventHandler()
{
}

MipCompEventHandler &MipCompEventHandler::operator=(const MipCompEventHandler &rhs)
{
  if (this != &rhs) {
    rootDualBoundTime = rhs.rootDualBoundTime;
    rootDualBound = rhs.rootDualBound;
    CbcEventHandler::operator=(rhs);
  }
  return *this;
}

CbcEventHandler *MipCompEventHandler::clone() const
{
  return new MipCompEventHandler(*this);
}

CbcEventHandler::CbcAction MipCompEventHandler::event(CbcEvent whichEvent)
{
  // pause each time the tree accurately reflects solver status
  if ((model_->specialOptions() & 2048) == 0 && CbcEventHandler::treeStatus) {
    // pause once the first node is on the tree, i.e. root node is processed
    if (model_->getNodeCount2() == 1) {
      rootDualBoundTime = model_->getCurrentSeconds();
      rootDualBound = model_->getBestPossibleObjValue();
    }
  }
  return noAction;
}