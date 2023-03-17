/**
 * @file RunData.hpp
 * @author Sean Kelley
 * @date 2023-03-03
 */

#ifndef RUNDATA_H_
#define RUNDATA_H_


// standard modules
#include <ghc/filesystem.hpp> // path
#include <string> // string

// namespaces
namespace fs = ghc::filesystem;

/** A data class to track specifics on a given solved MIP instance */
struct RunData {
public:
  /** the dual bound after solving the root LP relaxation for each instance */
  double lpBound;

  /** the dual bound after running cut generation on the root node */
  double rootDualBound;

  /** the best found dual bound for each instance */
  double dualBound;

  /** the objective value of the best solution found after primal heuristics */
  double heuristicPrimalBound;

  /** the best found primal bound for each instance */
  double primalBound;

  /** time to finish primal heuristics */
  double heuristicTime;

  /** time from calling model.branchAndBound() to completion of cut generation on root node */
  double rootDualBoundTime;

  /** duration of model.branchAndBound() call */
  double terminationTime;

  /** max run time given to model.branchAndBound() call */
  double maxTerminationTime;

  /** duration of set up, model.branchAndBound(), and tear down */
  double completionTime;

  /** max duration of set up, model.branchAndBound(), and tear down */
  double maxCompletionTime;

  /** whether or not preprocessing was enabled */
  bool usePreprocessing;

  /** Which generator was used for VPCs */
  std::string vpcGenerator;

  /** Get a comma-separated string of the names of RunData's attributes */
  std::string getHeader();

  /** Get a comma-separated string of the values of RunData's attributes */
  std::string getValues();

  /** writes this struct's attributes to the given csv file */
  void writeData(fs::path filePath);
};

#endif
