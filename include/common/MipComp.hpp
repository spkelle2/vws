/**
 * @file MipComp.hpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

// standard library
#include <ghc/filesystem.hpp> // path
#include <string>
#include <vector>

// coin-or modules
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface

// project modules
#include "RunData.hpp"
#include "VwsSolverInterface.hpp"

// namespaces
namespace fs = ghc::filesystem;

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
  double timeout;

  /** number of disjunctive terms to use in experiments */
  int terms;

  /** name of the way to generate vpcs */
  std::string vpcGenerator;

  /** the solver used for the series of mipModels */
  VwsSolverInterface seriesSolver;

  /** vector of MIP instances to solve */
  std::vector< OsiClpSolverInterface > instanceSolvers;

  /** names of each solved MIP instance */
  std::vector< std::string > instanceNames;

  /** Where csvData (e.g. vector attributes) will be saved */
  fs::path csvPath;

  /** Where bound data  will be saved */
  fs::path boundPath;

  /** vector of RunData objects to store data from each run */
  std::vector<RunData> runData;

  /** Constructor. Initializes attributes based on provided file. */
  MipComp(std::string filePathStr, std::string csvPathStr, double maxRunTime,
          std::string vpcGenerator, int terms);

  /** Solve series of MIP models provided at construction. */
  void solveSeries();

}; /* VpcWarmStart */
