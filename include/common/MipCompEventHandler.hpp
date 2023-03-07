/**
 * @file MipCompEventHandler.hpp
 * @author Sean Kelley
 * @date 2023-03-02
 */

// coin-or modules
#include "CbcEventHandler.hpp"
#include <CbcModel.hpp>

// project modules
#include "RunData.hpp"

/** Class to trap events and capture statistics */
class MipCompEventHandler : public CbcEventHandler {
public:
  RunData data;

  /** Event handler */
  virtual CbcAction event(CbcEvent whichEvent);

  /** Default constructor */
  MipCompEventHandler();

  /** constructor from CbcModel */
  MipCompEventHandler(CbcModel *model);

  /** Destructor */
  virtual ~MipCompEventHandler();

  /** Copy Constructor */
  MipCompEventHandler(const MipCompEventHandler& rhs);

  /** Assignment */
  MipCompEventHandler& operator=(const MipCompEventHandler& rhs);

  /** Clone */
  virtual CbcEventHandler* clone() const;

protected:
  /** Copy our stuff */
  void initialize(const MipCompEventHandler* const rhs);
}; /* VwsEventHandler */