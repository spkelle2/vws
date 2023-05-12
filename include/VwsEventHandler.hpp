/**
 * @file VwsEventHandler.hpp
 * @author Sean Kelley
 * @date 2023-05-01
 */

#pragma once

// standard library
#include <memory> // shared_ptr

// coin-or modules
#include "CbcEventHandler.hpp"
#include "OsiCuts.hpp"

/** Class to trap events and capture statistics */
class VwsEventHandler : public CbcEventHandler {
public:
  OsiCuts * cuts;

  bool stopNow;

  /** Event handler */
  virtual CbcAction event(CbcEvent whichEvent);

  /** Default constructor */
  VwsEventHandler();

  /** constructor from CbcModel */
  VwsEventHandler(CbcModel *model);

  /** Destructor */
  virtual ~VwsEventHandler();

  /** Copy Constructor */
  VwsEventHandler(const VwsEventHandler& rhs);

  /** Assignment */
  VwsEventHandler& operator=(const VwsEventHandler& rhs);

  /** Clone */
  virtual CbcEventHandler* clone() const;

protected:
  /** Copy our stuff */
  void initialize(const VwsEventHandler* const rhs);
}; /* VwsEventHandler */