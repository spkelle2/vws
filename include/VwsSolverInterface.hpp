/**
 * @file VpcWarmStart.hpp
 * @author Sean Kelley
 * @date 2023-01-31
 */
// standard library modules
#include <memory> // shared_ptr
#include <vector>

// shared modules
#include "CbcModel.hpp" // CbcModel

// project modules
#include "PartialBBDisjunction.hpp" // Disjunction


/**
 * @brief Warm-starts MIPs with VPCs
 *
 * This class facilitates solving a series of MIPs. Some MIPs in the series will solve
 * a set of PRLPs to generate VPCs and will record the disjunction and Farkas multipliers
 * associated with each VPC. The other MIPs in the series will apply the previous
 * dijsunctions and Farkas multipliers to generate VPCs.
 */
class VwsSolverInterface {
public:
  /** vector of Farkas multipliers used to create VPCs from previous MIPs.
   * Indexed [mip][cut][disjunctive term][Farkas multiplier] */
  std::vector< std::vector< std::vector< std::vector<double> > > > cutCertificates;

  /** vector of disjunctions used to create VPCs from previous MIPs */
  std::vector<PartialBBDisjunction*> disjunctions;

  /** number of solutions to save each solve */
  int maxExtraSavedSolutions;

  /** number of seconds to give each solve */
  int maxRunTime;

  /** number of terms to include in each disjunction */
  int disjunctiveTerms;

  /** All previously encountered solutions
   * indexed by [problem][solution][variable] */
  std::vector< std::vector< std::vector<double>>> solutions;

  /** All previously encountered dual bounds
   * indexed by [problem] */
  std::vector<double> dualBounds;

  /** names of variables in each instance
   * indexed by [problem][column index] */
  std::vector< std::vector< std::string > > variableNames;

  /** names of constraints in each instance
   * indexed by [problem][row index] */
  std::vector< std::vector< std::string > > constraintNames;

  /** Default constructor */
  VwsSolverInterface(int maxExtraSavedSolutions=100, int maxRunTime=1000000, int disjunctiveTerms=64);

  /** Solve given model. With probability p, solve PRLPs to create VPCs. With
   * probability (1-p), create VPCs from previous disjunctions and Farkas multipliers. */
  void solve(OsiClpSolverInterface& instanceSolver, bool usePreprocessing=true);

  // note: should be able to create VPCs with GMICs in the case the RHS does not change

protected:

  /** Solve the MIP encoded in instanceSolver without presolving */
  std::shared_ptr<CbcModel> unprocessedBranchAndCut(OsiClpSolverInterface& solver);

  /** Solve the MIP encoded in instanceSolver with presolving */
  std::shared_ptr<CbcModel> preprocessedBranchAndCut(OsiClpSolverInterface& instanceSolver);

}; /* VpcWarmStart */

