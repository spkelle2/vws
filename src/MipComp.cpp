/**
 * @file MipComp.cpp
 * @author Sean Kelley
 * @date 2023-02-12
 */

// standard library
#include <cstdio> // cout, endl
#include <ctime> // time_t, localtime
#include <fstream> // ofstream
#include <iomanip> // put_time
#include <memory> // shared_ptr
#include <regex> // regex
#include <string> // string
#include <cstdlib> // for srand() and rand()
#include <ctime>   // for time()

// coin-or modules
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface

// vpc modules
#include "VPCParameters.hpp" // VPCParameters
using namespace VPCParametersNamespace;

// project modules
#include "MipComp.hpp" // MipComp
#include "VwsUtility.hpp" // verify, extractModelFromGunzip, writeSolution

// namespaces
namespace fs = ghc::filesystem;

/** Reads the data in file path into MipComp instance. Captures run time limit of
 * each instance and creates a CbcModel instance for each mps file. Assumes
 * filePath is a .test file and is located in same directory structure as MIPcc repo. */
MipComp::MipComp(std::string inputFolderStr, std::string csvPathStr, double maxRunTime,
                 std::string vpcGenerator, int terms, std::string mipSolver,
                 bool providePrimalBound, int seedIndex) :
  csvPath(csvPathStr), vpcGenerator(vpcGenerator), mipSolver(mipSolver), seedIndex(seedIndex) {

  // containers for sorting input files and bounds
  std::vector<fs::path> inputFiles;
  std::vector<fs::path> primalBoundFiles;

  // validate file paths
  fs::path inputFolder(inputFolderStr);
  verify(fs::is_directory(inputFolder), "The path " + inputFolder.string() + " must exist and be a folder.", false);
  verify(!fs::exists(csvPath), "The file " + csvPath.string() + " should not exist.", false);

  // get each .mps file in the input folder
  for (const auto& entry : fs::directory_iterator(inputFolder.string())) {
    if (fs::is_regular_file(entry) && entry.path().extension() == ".mps") {
      inputFiles.push_back(entry.path());
      if (providePrimalBound){
        fs::path pb_file = entry.path();
        pb_file.replace_extension(".pb");
        verify(fs::exists(pb_file), "The optimal objective for " + entry.path().string() +
                                    " should be supplied in " + pb_file.string(), false);
        primalBoundFiles.push_back(pb_file);
      }
    }
  }

  // Sort the file names alphabetically
  std::sort(inputFiles.begin(), inputFiles.end());
  std::sort(primalBoundFiles.begin(), primalBoundFiles.end());

  // read in each instance in alphabetical order
  for (const auto& inputPath : inputFiles) {
    OsiClpSolverInterface instanceSolver;
    instanceSolver.readMps(inputPath.c_str(), true, false);
    instanceSolvers.push_back(instanceSolver);
    std::string instance_name = inputPath.stem().replace_extension("").string();
    instanceNames.push_back(instance_name);

    // capture index since alphabetical order doesn't match instance number for long series
    std::regex number_at_end_pattern(R"((\d+)$)");
    std::smatch match;
    if (std::regex_search(instance_name, match, number_at_end_pattern)) {
      instanceIndices.push_back(std::stoi(match[1].str()));
    } else {
      verify(false, "No number found at the end of the string.");
    }
  }

  // read in each optimal objective in alphabetical order
  for (const auto& primalBoundPath : primalBoundFiles) {
    // save the string in primalBoundPath to a float
    std::ifstream primalBoundFile(primalBoundPath);
    std::string line;
    std::getline(primalBoundFile, line);
    primalBounds.push_back(std::stod(line));
  }

  // Seed the random number generator with the current time and generate a random number
  srand(time(0));
  int randomNum = rand() % 100 + 1;
  
  // set parameters
  VPCParameters params;
  params.set(RANDOM_SEED, randomNum); // how many active leaves in the disjunction
  params.set(DISJ_TERMS, terms); // how many active leaves in the disjunction
  params.set(TIMELIMIT, maxRunTime); // max time for vpc generation
  params.set(PARTIAL_BB_TIMELIMIT, maxRunTime); // max time for creating partial bb tree
  params.set(PARTIAL_BB_KEEP_PRUNED_NODES, 1); // get the entirety of the partial bb tree
  params.set(MODE, 0); // create a partial bb tree to get the disjunction
  params.set(VPCParametersNamespace::BB_TIMELIMIT, maxRunTime); // max time for branch and cut
  params.set(intParam::VERBOSITY, 1); // light output
  params.set(stringParam::TMPFOLDER, "."); // save tmp files in current directory
  if (providePrimalBound) {
    // set parameters to use provided bound and skip heuristics
    params.set(BB_STRATEGY, get_bb_option_value({
        BB_Strategy_Options::user_cuts, // to allow VPCs and data collection
        BB_Strategy_Options::heuristics_off,
        BB_Strategy_Options::use_best_bound
    }));
  } else {
    // set parameters to use heuristics and ignore bound files
    params.set(BB_STRATEGY, get_bb_option_value({
        BB_Strategy_Options::user_cuts, // to allow VPCs and data collection
        BB_Strategy_Options::heuristics_on
    }));
  }

  // set the solver interface
  seriesSolver = VwsSolverInterface(params, mipSolver);
} /* Constructor */

/** Solves each instance in the series and prints each's run metadata to stdout */
void MipComp::solveSeries() {

  // get the expected count of variables and constraints for the series
  int numCols = instanceSolvers[0].getNumCols();
  int numRows = instanceSolvers[0].getNumRows();

  // solve each instance in the series
  for (int i = 0; i < instanceSolvers.size(); i++) {

    std::time_t startTime = std::time(nullptr);
    OsiClpSolverInterface instanceSolver = instanceSolvers[i];

    // print instance name and start time
    std::cout << "[INSTANCE] " << instanceNames[i] << ".mps" << std::endl;
    std::cout << "[GENERATOR] " << vpcGenerator << std::endl;
    std::cout << "[START] " << std::put_time(std::localtime(&startTime), "%FT%T") << std::endl;

    // first iteration should always use New if it makes VPCs - otherwise use requested generator
    std::string genType = i < 1 && vpcGenerator != "None" ? "New" : vpcGenerator;

    // solve the instance with the given generator
    double primalBound = primalBounds.size() > 0 ? primalBounds[i] :
        std::numeric_limits<double>::max();
    RunData data;
    try {
      // this isn't a hard requirement, but unless you're really doing a good job with
      // bookkeeping, you want to keep the number of variables and constraints constant
      verify(numCols == instanceSolver.getNumCols() && numRows == instanceSolver.getNumRows(),
             "All instances must have the same number of variables and constraints.");
      data = seriesSolver.solve(instanceSolver, genType, primalBound);
    } catch (const std::runtime_error& e) {

      // skip this experiment if we get an error other than dot product difference
      std::string errorMessage = e.what();
      if (errorMessage != "Point: Calculated dot product with obj differs from solver's") {

        // print the error message, unless we know it printed already
        if (errorMessage != "VPC tried to exit with error code 1") {
          std::cerr << errorMessage << std::endl;
        }
        // breaking if we expected VPCs in the first iteration and did not get them
        if (vpcGenerator != "None" and i == 0 and data.numCuts == 0){
          std::cerr << "No vpcs were made from a new disjunction in first iteration. Stopping series." << std::endl;
          break;
        } else {
          // otherwise this is still a critical error, but just for this iteration, so move on
          continue;
        }
      }
    }

    // stop the series if we sought vpcs on the first iteration but didn't get any.
    // Need to generate cuts so we have a disjunction/multipliers to reuse and an ideal bound to compare
    data.instanceIndex = instanceIndices[i];
    data.seedIndex = seedIndex;
    if (vpcGenerator != "None" and i == 0 and data.numCuts == 0){
      std::cerr << "No vpcs were made from a new disjunction in first iteration. Stopping series." << std::endl;
      break;
    }

    // if generating vpcs didn't work, just skip recording this instance
    // since bound comparisons will be off or incomplete
    if (data.numCuts or vpcGenerator == "None"){
      // save the data collected by the event handler
      data.writeData(csvPath);
      runData.push_back(data);
    }

    // print end time
    std::time_t endTime = std::time(nullptr);
    std::cout << "[END] " << std::put_time(std::localtime(&endTime), "%FT%T") << std::endl;
  }
} /* solveSeries */
