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

  /** vector of timers associated with each solve */
  std::vector<TimeStats> timers;

  /** number of solutions to save each solve */
  int maxExtraSavedSolutions;

  /** number of seconds to give each solve */
  double maxRunTime;

  /** max proportion of maxRunTime to allow VPC generation from PRLPs */
  double vpcGenTimeRatio;

  /** number of terms to include in each disjunction */
  int disjunctiveTerms;

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
  VwsSolverInterface(int maxExtraSavedSolutions=100, double maxRunTime=1000000,
                     int disjunctiveTerms=16, double vpcGenTimeRatio=0.1);

  /** Solve a MIP with VPCs added. */
  CbcModel solve(const OsiClpSolverInterface& instanceSolver, const std::string vpcGenerator,
                 VwsEventHandler& eventHandler);

  /** Solve a MIP with VPCs added and not care about the eventHandler */
  CbcModel solve(const OsiClpSolverInterface& instanceSolver, const std::string vpcGenerator);

  /** Creates cuts from a PRLP relaxation of the disjunctive terms found from
   *  partially solving the given problem. Simplified from Strengthening's
   *  CglAdvCut::generateCuts */
  std::shared_ptr<OsiCuts> createVpcsFromNewDisjunctionPRLP(OsiClpSolverInterface* si,
                                                            VwsEventHandler& eventHandler);

  /** Parameterize each previously created disjunctive cut with its disjunction
   *  and Farkas multipliers applied to the given solver. Borrowed from Strengthening's
   *  main.cpp */
  std::shared_ptr<OsiCuts> createVpcsFromFarkasMultipliers(OsiClpSolverInterface* si,
                                                           VwsEventHandler& eventHandler);

  /// @brief create VPCs by solving each PRLP resulting from applying each disjunction in <disjunctions> to <si>
  std::shared_ptr<OsiCuts> createVpcsFromOldDisjunctionPRLP(OsiClpSolverInterface* si,
                                                            VwsEventHandler& eventHandler);

}; /* VwsSolverInterface */

