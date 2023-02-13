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

  /** random number generator used to sample from 0-1 distribution to determine
   * if VPCs are created from solving PRLPs or from applying previous disjunctions
   * and Farkas multipliers */
  std::mt19937 randomNumberGenerator;

  /** Default constructor */
  VwsSolverInterface();

  /** Solve given model. With probability p, solve PRLPs to create VPCs. With
   * probability (1-p), create VPCs from previous disjunctions and Farkas multipliers. */
  void solve(CbcModel& model, double p);

  // note: should be able to create VPCs with GMICs in the case the RHS does not change

}; /* VpcWarmStart */

