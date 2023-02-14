/**
 * @file MipComp.hpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

// standard library
#include <string>
#include <vector>

// coin-or modules
#include "CbcModel.hpp"

// project modules
#include "VwsSolverInterface.hpp"

/**
 * @brief Object to manage runs for the MIP Computational Contest
 *
 * This tracks the series of instances to solve for the contest, solves them,
 * handles input/output, etc.
 */
class MipComp {
public:

  // todo: test that ranged constraints work as intended
  // todo: create VPCs with GMICs in the case only the objective changes (should be fine for variable or constraint additions)

  /** max run time on each MIP instance */
  int timeout;

  /** vector of file locations for each MIP to solve */
  std::vector< CbcModel > mipModels;

  /** the solver used for the series of mipModels */
  VwsSolverInterface solver;

  /** Constructor. Initializes attributes from provided file. */
  MipComp(char * filePath);

  /** Solve series of MIP models provided at construction. */
  void solveSeries();

}; /* VpcWarmStart */
