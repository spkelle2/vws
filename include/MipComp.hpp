/**
 * @file MipComp.hpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

// standard library
#include <filesystem.hpp> // path
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

  // todo: create VPCs with GMICs in the case only the objective changes (should be fine for variable or constraint additions)

  /** name of the way to generate vpcs */
  std::string vpcGenerator;

  /** the solver used for the series of mipModels */
  VwsSolverInterface seriesSolver;

  /** vector of MIP instances to solve */
  std::vector< OsiClpSolverInterface > instanceSolvers;

  /** names of each solved MIP instance */
  std::vector< std::string > instanceNames;

  /** indices of each solve MIP instance */
  std::vector< int > instanceIndices;

  /** the corresponding optimal objective value for each instance in instanceSolvers */
  std::vector< double > primalBounds;

  /** Where csvData (e.g. vector attributes) will be saved */
  fs::path csvPath;

  /** vector of RunData objects to store data from each run */
  std::vector<RunData> runData;

  /** which solver to use for experiments */
  std::string mipSolver;

  /** index of the random seed */
  int seedIndex;

  /** whether to tighten disjunctions by dropping prunable terms */
  bool tighten_disjunction;

  /** whether to tighten parametric disjunctive cuts for coefficient matrix perturbations */
  bool tighten_matrix_perturbation;

  /** whether to tighten parametric disjunctive cuts for infeasible to feasible terms */
  bool tighten_infeasible_to_feasible_term;

  /** whether to tighten parametric disjunctive cuts for feasible to infeasible bases */
  bool tighten_feasible_to_infeasible_basis;

  /** Constructor. Initializes attributes based on provided file. */
  MipComp(std::string inputFolderStr, std::string csvPathStr, double maxRunTime,
          std::string vpcGenerator, int terms, std::string mipSolver,
          bool providePrimalBound, int seedIndex, bool tighten_disjunction=false,
          bool tighten_matrix_perturbation=false, bool tighten_infeasible_to_feasible_term=false,
          bool tighten_feasible_to_infeasible_basis=false);

  /** Solve series of MIP models provided at construction. */
  void solveSeries();

}; /* VpcWarmStart */
