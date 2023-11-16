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

// vpc modules
#include "PartialBBDisjunction.hpp" // PartialBBDisjunction
#include "TimeStats.hpp" // Timer

// project modules
#include "VwsEventHandler.hpp" // CbcEventHandler

struct BBInfo {
  double obj = 0.; // objective value of best IP-feasible solution
  double bound = 0.; // best dual bound found (best objective of any leaf node)
  long iters = 0; // # iters to solve the instance
  long nodes = 0; // # nodes to solve the instance
  long root_passes = 0; // # passes of cuts at the root node
  double first_cut_pass = 0.; // bound after one round of cuts at the root
  double last_cut_pass = 0.; // bound after last round of cuts at the root
  long root_iters = 0; // # iters spent at root node
  double root_time = 0.; // time spent at the root node
  double last_sol_time = 0.; // time that best IP-feasible solution was found
  double time = 0.; // total time to solve the instance
};

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

  /** vector of cut generators used to create VPCs from PRLPs */
  std::vector< std::shared_ptr<PartialBBDisjunction> > disjunctions;

  /** parameters used to control VPC functions */
  VPCParametersNamespace::VPCParameters params;

  /** name of the MILP solver to use - either CBC or Gurobi */
  std::string mipSolver;

  /** Default constructor */
  VwsSolverInterface();

  /** constructor */
  VwsSolverInterface(VPCParametersNamespace::VPCParameters params, std::string mipSolver);

  /** Solve a MIP with VPCs added */
  RunData solve(const OsiClpSolverInterface& instanceSolver, const std::string vpcGenerator);

  /** Creates cuts from a PRLP relaxation of the disjunctive terms found from
   *  partially solving the given problem. Simplified from Strengthening's
   *  CglAdvCut::generateCuts */
  std::shared_ptr<OsiCuts> createVpcsFromNewDisjunctionPRLP(OsiClpSolverInterface* si,
                                                            RunData& data);

  /** Parameterize each previously created disjunctive cut with its disjunction
   *  and Farkas multipliers applied to the given solver. Borrowed from Strengthening's
   *  main.cpp */
  std::shared_ptr<OsiCuts> createVpcsFromFarkasMultipliers(OsiClpSolverInterface* si,
                                                           RunData& data);

  /// @brief create VPCs by solving each PRLP resulting from applying each disjunction in <disjunctions> to <si>
  std::shared_ptr<OsiCuts> createVpcsFromOldDisjunctionPRLP(OsiClpSolverInterface* si,
                                                            RunData& data);

}; /* VwsSolverInterface */

