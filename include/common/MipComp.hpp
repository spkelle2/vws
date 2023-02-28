/**
 * @file MipComp.hpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

// standard library
#include <filesystem> // path
#include <string>
#include <vector>

// coin-or modules
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface

// project modules
#include "VwsSolverInterface.hpp"

// namespaces
namespace fs = std::__fs::filesystem;

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

  /** the solver used for the series of mipModels */
  VwsSolverInterface seriesSolver;

  /** vector of MIP instances to solve */
  std::vector< OsiClpSolverInterface > instanceSolvers;

  /** names of each solved MIP instance */
  std::vector< std::string > instanceNames;

  /** the dual bound after solving the root LP relaxation for each instance */
  std::vector< double > lpBounds;

  /** the dual bound after running cut generation on the root node */
  std::vector< double > rootDualBounds;

  /** the best found dual bound for each instance */
  std::vector< double > dualBounds;

  /** the best found primal bound for each instance */
  std::vector< double > primalBounds;

  /** time to solve the root LP relaxation for each instance */
  std::vector< double > lpBoundTimes;

  /** time from calling model.branchAndBound() to completion of cut generation on root node */
  std::vector< double > rootDualBoundTimes;

  /** duration of model.branchAndBound() calls */
  std::vector< double > terminationTimes;

  /** Where best solutions will be saved */
  fs::path solutionDirectory;

  /** Where csvData (e.g. vector attributes) will be saved */
  fs::path csvPath;

  /** Constructor. Initializes attributes based on provided file. */
  MipComp(const char * filePath, const char * solutionDirectoryChars,
          const char * csvPathChars);

  /** Solve series of MIP models provided at construction. */
  void solveSeries();

  /** writes the data collected from a test run to a csv */
  void writeCsvData(fs::path csvPath);

}; /* VpcWarmStart */
