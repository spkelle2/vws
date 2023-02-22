/**
 * @file MipCompEventHandler.hpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

// coin-or modules
#include <CbcModel.hpp>
#include "CbcEventHandler.hpp"

/** Standard definition for event handlers in CBC */
class MipCompEventHandler : public CbcEventHandler {
public:
  virtual CbcAction event(CbcEvent whichEvent);
  MipCompEventHandler();
  MipCompEventHandler(CbcModel *model);
  virtual ~MipCompEventHandler();
  MipCompEventHandler(const MipCompEventHandler &rhs);
  MipCompEventHandler &operator=(const MipCompEventHandler &rhs);
  virtual CbcEventHandler *clone() const;

  /** time from calling model.branchAndBound() to completion of cut generation on root node */
  double rootDualBoundTime;

  /** Dual bound at completion of cut generation on root node */
  double rootDualBound;
};
