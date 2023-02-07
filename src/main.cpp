/**
 * @file main.cpp
 * @author Sean Kelley
 * @date 2023-02-01
 */

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <chrono> // for timing
#include <limits> // numeric_limits
#include <memory> // for smart pointers

// For option handling
#include <getopt.h> // getopt, getopt_long
#define no_argument 0
#define required_argument 1
#define optional_argument 2

// COIN-OR
#include <OsiSolverInterface.hpp>

// VPC
#include "SolverHelper.hpp"
// #include "analysis.hpp"  // for printHeader when reactivated

// Project files
#include "Parameters.hpp"
using namespace StrengtheningParameters;
#include "VpcWarmStart.hpp"
#include "utility.hpp"


const std::vector<std::string> OverallTimeStatsName {
    "INIT_SOLVE_TIME",
    "CUT_TIME",
    "BB_TIME",
    "APPLY_TIME",
    "TOTAL_TIME"
};

// Main file variables
Parameters params;
OsiSolverInterface* solver;
std::string dir = "", filename = "", instname = "", in_file_ext = "";

// Catch abort signal if it ever gets sent
/**
 * Catch a signal. You can output debugging info.
 * If you return from this function, and it was called
 * because abort() was called, your program will exit or crash anyway
 * (with a dialog box on Windows).
 */
#include <csignal>
void signal_handler_with_error_msg(int signal_number) {
  error_msg(errorstring, "Abort or seg fault message received. Signal number: %d.\n", signal_number);
  writeErrorToLog(errorstring, params.logfile);
  exit(1);
} /* signal_handler_with_error_msg */

int startUp(int argc, char** argv);
int processArgs(int argc, char** argv);
void initializeSolver(OsiSolverInterface* &solver);



/****************** MAIN FUNCTION **********************/
int main(int argc, char** argv) {
  // Do this early in your program's initialization
  std::signal(SIGABRT, signal_handler_with_error_msg);
  std::signal(SIGSEGV, signal_handler_with_error_msg);

  // Print welcome message, set up logfile
  int status = startUp(argc, argv);
  if (status) { return status; }

  // Set up solver and get initial solution
  initializeSolver(solver);
} /* main */

/**
 * Call this early to print welcome message, etc.
 */
int startUp(int argc, char** argv) {
  int status = 0;

  status = processArgs(argc, argv);
  if (status) {
    return status;
  }

  // Get instance file
  if (params.get(stringParam::FILENAME).empty()) {
    error_msg(errorstring,
        "No instance file provided. Use -f /path/to/instance.mps to specify the instance.\n");
    exit(1);
  }
  printf("Instance file: %s\n", params.get(stringParam::FILENAME).c_str());
  
  if (parseFilename(dir, instname, in_file_ext, params.get(stringParam::FILENAME), params.logfile) != 0) {
    error_msg(errorstring,
        "Unable to parse filename: %s. Found: dir=\"%s\", instname=\"%s\",ext=\"%s\".\n",
        params.get(stringParam::FILENAME).c_str(), dir.c_str(),
        instname.c_str(), in_file_ext.c_str());
    exit(1);
  }
  filename = dir + "/" + instname;

  // Prepare logfile
  const std::string logname = params.get(stringParam::LOGFILE);
  bool logexists = false; 
  if (!logname.empty()) {
    logexists = fexists(logname.c_str());
    params.logfile = fopen(logname.c_str(), "a");
  }

  // todo: params need to be updated to vws params with an analysis.cpp for this project
  //  if (params.logfile != NULL) {
  //    if (!logexists) {
  //      printHeader(params, OverallTimeStatsName);
  //    }
  //    fprintf(params.logfile, "%s,", instname.c_str());
  //    printParams(params, params.logfile, 2); // only values
  //    fflush(params.logfile);
  //  }

  return status;
} /* startUp */

void initializeSolver(OsiSolverInterface* &solver) {
  // Generate cuts
  solver = new OsiClpSolverInterface;
  setLPSolverParameters(solver, params.get(VERBOSITY));

  int status = 0;
  if (in_file_ext.compare("lp") == 0) {
#ifdef TRACE
    printf("\n## Reading LP file. ##\n");
#endif
    status = solver->readLp(params.get(stringParam::FILENAME).c_str());
  } else {
    if (in_file_ext.compare("mps") == 0) {
#ifdef TRACE
      printf("\n## Reading MPS file. ##\n");
#endif
      status = solver->readMps(params.get(stringParam::FILENAME).c_str());
    } else {
      try {
#ifdef TRACE
        printf("\n## Reading MPS file. ##\n");
#endif
        status = solver->readMps(params.get(stringParam::FILENAME).c_str());
      } catch (std::exception& e) {
        error_msg(errorstring, "Unrecognized extension: %s.\n",
            in_file_ext.c_str());
        writeErrorToLog(errorstring, params.logfile);
        exit(1);
      }
    }
  } // read file
  if (status < 0) {
    error_msg(errorstring, "Unable to read in file %s.\n",
        params.get(stringParam::FILENAME).c_str());
    writeErrorToLog(errorstring, params.logfile);
    exit(1);
  }

  // Make sure we are doing a minimization problem; this is just to make later
  // comparisons simpler (i.e., a higher LP obj after adding the cut is better).
  if (solver->getObjSense() < 1e-3) {
    printf(
        "\n## Detected maximization problem. Negating objective function to make it minimization. ##\n");
    solver->setObjSense(1.0);
    const double* obj = solver->getObjCoefficients();
    for (int col = 0; col < solver->getNumCols(); col++) {
      solver->setObjCoeff(col, -1. * obj[col]);
    }
    double objOffset = getObjOffset(solver);
    if (objOffset != 0.) {
      solver->setDblParam(OsiDblParam::OsiObjOffset, -1. * objOffset);
    }
  }
} /* initializeSolver */

/**
 * See params.hpp for descriptions of the parameters
 */
int processArgs(int argc, char** argv) {
  // Handle inputs
  // struct option declared in getopt.h
  // name: name of the long option
  // has_arg: 0,1,2 for none, required, or optional
  // *flag: how results are returned; if NULL, getopt_long() returns val (e.g., can be the equivalent short option character), and o/w getopt_long() returns 0, and flag points to a var which is set to val if the option is found, but left unchanged if the option is not found
  // val: value to return, or to load into the variable pointed to by flag
  const char* const short_opts = "b:B:c:d:f:g:hi:l:m:o:r:s:t:v:";
  const struct option long_opts[] =
  {
      {"bb_runs",               required_argument, 0, 'b'},
      {"bb_mode",               required_argument, 0, 'b'*'2'},
      {"bb_strategy",           required_argument, 0, 'B'},
      {"cutlimit",              required_argument, 0, 'c'},
      {"disj_terms",            required_argument, 0, 'd'},
      {"file",                  required_argument, 0, 'f'},
      {"gomory",                required_argument, 0, 'g'},
      {"help",                  no_argument,       0, 'h'},
      {"ip_obj",                required_argument, 0, 'i'},
      {"logfile",               required_argument, 0, 'l'},
      {"mode",                  required_argument, 0, 'm'},
      {"optfile",               required_argument, 0, 'o'},
      {"rounds",                required_argument, 0, 'r'},
      {"solfile",               required_argument, 0, 's'*'1'},
      {"strengthen",            required_argument, 0, 's'},
      {"temp",                  required_argument, 0, 't'*'1'},
      {"timelimit",             required_argument, 0, 't'},
      {"verbosity",             required_argument, 0, 'v'},
      {nullptr, no_argument, nullptr, 0}
  };

  int inp;
  int status = 0;
  while ((inp = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
    switch (inp) {
      case 'b': {
                  int val;
                  intParam param = intParam::BB_RUNS;
                  if (!parseInt(optarg, val)) {
                    error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                    exit(1);
                  }
                  params.set(param, val);
                  break;
                }
      case 'b'*'2': {
                  int val;
                  intParam param = intParam::BB_MODE;
                  if (!parseInt(optarg, val)) {
                    error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                    exit(1);
                  }
                  params.set(param, val);
                  break;
                }
      case 'B': {
                  int val;
                  intParam param = intParam::BB_STRATEGY;
                  if (!parseInt(optarg, val)) {
                    error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                    exit(1);
                  }
                  params.set(param, val);
                  break;
                }
      case 'c': {
                  int val;
                  intParam param = intParam::CUTLIMIT;
                  if (!parseInt(optarg, val)) {
                    error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                    exit(1);
                  }
                  params.set(param, val);
                  break;
                }
      case 'd': {
                  int val;
                  intParam param = intParam::DISJ_TERMS;
                  if (!parseInt(optarg, val)) {
                    error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                    exit(1);
                  }
                  params.set(param, val);
                  break;
                }
      case 'f': {
                  params.set(stringParam::FILENAME, optarg);
                  break;
                }
      case 'g': {
                  int val;
                  intParam param = intParam::GOMORY;
                  if (!parseInt(optarg, val)) {
                    error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                    exit(1);
                  }
                  params.set(param, val);
                  break;
                }
      case 'i': {
                 double val;
                 doubleParam param = doubleParam::IP_OBJ;
                 if (!parseDouble(optarg, val)) {
                   error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                   exit(1);
                 }
                 params.set(param, val);
                 break;
               }
      case 'l': {
                  params.set(stringParam::LOGFILE, optarg);
                  break;
                }
      case 'm': {
                 int val;
                 intParam param = intParam::MODE;
                 if (!parseInt(optarg, val)) {
                   error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                   exit(1);
                 }
                 params.set(param, val);
                 break;
               }
      case 'o': {
                  params.set(stringParam::OPTFILE, optarg);
                  break;
                }
      case 'r': {
                   int val;
                   intParam param = intParam::ROUNDS;
                   if (!parseInt(optarg, val)) {
                     error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                     exit(1);
                   }
                   params.set(param, val);
                   break;
                 }
      case 's': {
                   int val;
                   intParam param = intParam::STRENGTHEN;
                   if (!parseInt(optarg, val)) {
                     error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                     exit(1);
                   }
                   params.set(param, val);
                   break;
                 }
      case 's'*'1': {
                  params.set(stringParam::SOLFILE, optarg);
                  break;
                }
      case 't'*'1': {
                      int val;
                      intParam param = intParam::TEMP;
                      if (!parseInt(optarg, val)) {
                        error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                        exit(1);
                      }
                      params.set(param, val);
                      break;
                    }
      case 't': {
                  double val;
                  doubleParam param = doubleParam::TIMELIMIT;
                  if (!parseDouble(optarg, val)) {
                    error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                    exit(1);
                  }
                  params.set(param, val);
                  break;
                }
      case 'v': {
                   int val;
                   intParam param = intParam::VERBOSITY;
                   if (!parseInt(optarg, val)) {
                     error_msg(errorstring, "Error reading %s. Given value: %s.\n", params.name(param).c_str(), optarg);
                     exit(1);
                   }
                   params.set(param, val);
                   break;
                 }
      case 'h':
      case '?':
      default: {
                 // print help
                std::string helpstring;
                helpstring += "\n## DESCRIPTION ##\n";
                helpstring += "Code for generating custom cuts.\n";
                helpstring += "\n## OPTIONS ##\n";
                helpstring += "-h, --help\n\tPrint this help message.\n";
                helpstring += "--temp\n\tSet temporary options (e.g., value of 1 = do preprocessing on instance).\n";
                helpstring += "\n# Input/output #\n";
                helpstring += "-f file, --file=file\n\tFilename.\n";
                helpstring += "-i val, --ip_obj=val\n\tValue of integer optimum for this instance (takes precedence over -o/--optfile).\n";
                helpstring += "-l logfile, --logfile=logfile\n\tWhere to print log messages.\n";
                helpstring += "-o optfile, --optfile=optfile\n\tWhere to find integer optimum value information (a csv file formatted as \"instance_name,value\" on each row).\n";
                helpstring += "--solfile=solfile\n\tWhere to find integer optimum solution information (e.g., a mst/sol file produced by Gurobi/CPLEX/etc).\n";
                helpstring += "-v level, --verbosity=level\n\tVerbosity level (0: print little, 1: let solver output be visible).\n";
                helpstring += "\n# General cut options #\n";
                helpstring += "-c num cuts, --cutlimit=num cuts\n\tMaximum number of cuts to generate (0+ = as given, -k = k * # fractional variables at root).\n";
                helpstring += "-d num terms, --disj_terms=num terms\n\tMaximum number of disjunctive terms or disjunctions to generate (depending on mode).\n";
                helpstring += "-g +/- 0-3, --gomory=+/- 0-3\n\t0: do not use Gomory cuts, 1: generate Gomory cuts via CglGMI, 2: generate Gomory cuts via gmic.cpp, 3: try closed-form strengthening (<0: only gen, >0: also apply to LP).\n";
                helpstring += "-m mode, --mode=mode\n\tDescription needs to be entered.\n";
                helpstring += "-r num rounds, --rounds=num rounds\n\tNumber of rounds of cuts to apply.\n";
                helpstring += "-s 0/1/2, --strengthen=0/1/2\n\tWhether to strengthen cuts.\n";
                helpstring += "-t num seconds, --timelimit=num seconds\n\tTotal number of seconds allotted for cut generation.\n";
                helpstring += "\n# Branch-and-bound options #\n";
                helpstring += "-b 0+ --bb_runs=0+\n\tNumber of branch-and-bound repeats.\n";
                helpstring += "-B strategy --bb_strategy=strategy\n\tBranch-and-bound strategy (see BBHelper.hpp).\n";
                helpstring += "--bb_mode={0,1,10,11,100,...,111}\n\tWhich branch-and-bound experiments to run (ones = no cuts, tens = mycuts, hundreds = gmics).\n";
                helpstring += "## END OF HELP ##\n";
                std::cout << helpstring << std::endl;
                status = 1;
               }
    } // switch statement for input
  } // process args
  //for (int i = 0; i < argc; i++) {
  //  std::cout << argv[i] << " ";
  //}
  //std::cout << std::endl;
  return status;
} /* processArgs */

