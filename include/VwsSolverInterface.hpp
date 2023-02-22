/**
 * @file VpcWarmStart.hpp
 * @author Sean Kelley
 * @date 2023-01-31
 */
// standard library modules
#include <random> // mt19937
#include <vector>

// shared modules
#include "CbcModel.hpp" // CbcModel

// project modules
#include "PartialBBDisjunction.hpp" // DisjunctiveTerm, Disjunction, getSolverForTerm


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
  std::vector< std::vector< std::vector< std::vector<double> > > > cutCertificate;

  /** vector of disjunctions used to create VPCs from previous MIPs */
  std::vector<PartialBBDisjunction*> disjunction;

  /** number of solutions to save each solve */
  int maxSavedSolutions;

  /** All previously encountered solutions
   * indexed by [problem][solution][variable] */
  std::vector< std::vector< std::vector<double>>> solutions;

  /** names of variables in each instance
   * indexed by [problem][column index] */
  std::vector< std::vector< std::string > > variableNames;

  /** names of constraints in each instance
   * indexed by [problem][row index] */
  std::vector< std::vector< std::string > > constraintNames;

  /** Default constructor */
  VwsSolverInterface(int maxSavedSolutions=100);

  /** Solve given model. With probability p, solve PRLPs to create VPCs. With
   * probability (1-p), create VPCs from previous disjunctions and Farkas multipliers. */
  void solve(CbcModel& model);

  // note: should be able to create VPCs with GMICs in the case the RHS does not change

}; /* VpcWarmStart */

