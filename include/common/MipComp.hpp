/**
 * @file MipComp.hpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

// standard library
#include <string>
#include <vector>

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

  /** max run time on each MIP instance */
  int timeout;

  /** vector of file locations for each MIP to solve */
  std::vector< std::string > mpsFiles;

  /** the solver used for each mip in the series */
  VwsSolverInterface solver;

  /** Constructor */
  MipComp(char * filePath);

  /** reads the list of MIP problems to solve for the MIPComp */
  std::vector<std::string> parseTestFile(char * filePath);

  /** Solve series of MIP models saved in input file locations. */
  void solveSeries();

  // note: should be able to create VPCs with GMICs in the case the RHS does not change

}; /* VpcWarmStart */
