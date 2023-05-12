/**
 * @file TestVwsSolverInterface.cpp
 * @author Sean Kelley
 * @date 2023-02-14
 */

#define CATCH_CONFIG_MAIN

// standard library
#include <cstdlib> // abs

// unit test library
#include "catch.hpp"

// coin-or modules
#include "CbcEventHandler.hpp" // CbcEventHandler
#include "OsiClpSolverInterface.hpp" // OsiClpSolverInterface
#include <OsiCuts.hpp> // OsiCuts

// vpc modules
#include "Disjunction.hpp" // DisjExitReason
#include "StrongBranchingDisjunction.hpp" // StrongBranchingDisjunction
#include "VPCParameters.hpp" // VPCParameters

// project unit test modules
#include "MipCompEventHandler.hpp"
#include "VwsEventHandler.hpp"
#include "VwsSolverInterface.hpp"
#include "VwsUtility.hpp"


class TestHandler : public VwsEventHandler {
public:
  virtual CbcAction event(CbcEvent whichEvent);
  TestHandler();
  TestHandler(CbcModel *model);
  virtual ~TestHandler();
  TestHandler(const TestHandler &rhs);
  TestHandler &operator=(const TestHandler &rhs);
  virtual CbcEventHandler *clone() const;
};

TestHandler::TestHandler()
  : VwsEventHandler()
{
}

TestHandler::TestHandler(const TestHandler &rhs)
  : VwsEventHandler(rhs)
{
}

TestHandler::TestHandler(CbcModel *model)
  : VwsEventHandler(model)
{
}

TestHandler::~TestHandler()
{
  VwsEventHandler::~VwsEventHandler();
}

TestHandler &TestHandler::operator=(const TestHandler &rhs)
{
  if (this != &rhs) {
    VwsEventHandler::operator=(rhs);
  }
  return *this;
}

CbcEventHandler *TestHandler::clone() const
{
  return new TestHandler(*this);
}

CbcEventHandler::CbcAction TestHandler::event(CbcEvent whichEvent)
{
  VwsEventHandler::event(whichEvent);

  // Once we've done root cuts, kill the solve
  if ((model_->specialOptions() & 2048) == 0 && whichEvent == CbcEventHandler::treeStatus) {
    return stop;
  }
  return noAction;
}


// --------------------------- constructor tests -------------------------------
TEST_CASE("Test default constructor", "[VwsSolverInterface::VwsSolverInterface]") {

  // Ensure we set up as expected
  VwsSolverInterface seriesSolver(5, 50, 16);
  REQUIRE(seriesSolver.maxExtraSavedSolutions == 5);
  REQUIRE(seriesSolver.maxRunTime == 50);
  REQUIRE(seriesSolver.disjunctiveTerms == 16);
  REQUIRE(seriesSolver.vpcGenTimeRatio == 0.1);
}


// ------------------------------ solve tests ----------------------------------

void checkNames(VwsSolverInterface &seriesSolver, int probIdx){
  // check that row and column names are correct
  REQUIRE(seriesSolver.variableNames.size() == probIdx + 1);
  REQUIRE(seriesSolver.variableNames[probIdx].size() == 27);
  for (int i = 0; i < seriesSolver.variableNames[probIdx].size(); i++){
    std::string name = i < 9 ? "C10" + std::to_string(i + 1) : "C1" + std::to_string(i + 1);
    REQUIRE(seriesSolver.variableNames[probIdx][i] == name);
  }
  REQUIRE(seriesSolver.constraintNames.size() == probIdx + 1);
  REQUIRE(seriesSolver.constraintNames[probIdx].size() == 20);
  for (int i = 0; i < seriesSolver.constraintNames[probIdx].size(); i++){
    std::string name = i < 9 ? "R10" + std::to_string(i + 1) : "R1" + std::to_string(i + 1);
    REQUIRE(seriesSolver.constraintNames[probIdx][i] == name);
  }
}

void checkSolutions(VwsSolverInterface& seriesSolver, const OsiClpSolverInterface& instanceSolver,
                    const CbcModel& model, int probIdx){

  for (int i = 0; i < seriesSolver.solutions[probIdx].size(); i++){
    // the solution should be feasible
    REQUIRE(isFeasible(instanceSolver, seriesSolver.solutions[probIdx][i]));
    // the solution should match that found by the CBC model
    for (int j = 0; j < seriesSolver.solutions[probIdx][i].size(); j++){
      REQUIRE(std::abs(seriesSolver.solutions[probIdx][i][j] - model.savedSolution(i)[j]) < .0001);
    }
  }
}

TEST_CASE("Test a single solve", "[VwsSolverInterface::solve]") {

  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  VwsSolverInterface seriesSolver;
  CbcModel model = seriesSolver.solve(instanceSolver);

  // check that row and column names are correct
  checkNames(seriesSolver, 0);

  // check that we have an optimal solution
  REQUIRE(model.getObjValue() == 34);

  // we only solve one problem
  REQUIRE(seriesSolver.solutions.size() == 1);

  // the problem should have one solution with heuristics turned on now
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model, 0);
}

TEST_CASE( "Solve a second time", "[VwsSolverInterface::solve]" ){

  // solve a model with the VwsSolverInterface
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver);

  // check on rerun that we get acceptable duplication
  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  CbcModel model = seriesSolver.solve(instanceSolver);

  // check that row and column names are correct
  checkNames(seriesSolver, 1);

  // check that we have an optimal solution
  REQUIRE(model.getObjValue() == 34);

  // we've solved two problems now
  REQUIRE(seriesSolver.solutions.size() == 2);

  // the problem should have one solution
  REQUIRE(seriesSolver.solutions[1].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model, 1);
}

TEST_CASE( "Solve without preprocessing", "[VwsSolverInterface::solve]" ) {
  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  VwsSolverInterface seriesSolver;
  CbcModel model = seriesSolver.solve(instanceSolver, "None", false);

  // check that row and column names are correct
  checkNames(seriesSolver, 0);

  // check that we have the same optimal solution
  REQUIRE(model.getObjValue() == 34);

  // we only solve one problem
  REQUIRE(seriesSolver.solutions.size() == 1);

  // the problem should have one solution now that heuristics work
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model, 0);
}

// make sure the above wasn't a fluke because preprocessing had no effect on small problem
TEST_CASE( "Check larger instance preprocessed", "[VwsSolverInterface::solve][long]" ) {
  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver =
      extractSolverInterfaceFromGunzip("../src/test/datasets/vary_rhs/series_2/rhs_s2_i01.mps.gz");
  VwsSolverInterface seriesSolver(10, 120, 16, .8);
  CbcModel model = seriesSolver.solve(instanceSolver);

  // check that we have valid bounds
  REQUIRE((-17180 < model.getObjValue() && model.getObjValue() < -17150));

  // the problem should have two solutions since we didn't specify a max
  REQUIRE(seriesSolver.solutions[0].size() >= 1);
  checkSolutions(seriesSolver, instanceSolver, model, 0);
}

TEST_CASE( "Check larger instance unprocessed", "[VwsSolverInterface::solve][long]" ) {
  // solve a model with the VwsSolverInterface and ensure we get an optimal solution
  OsiClpSolverInterface instanceSolver =
      extractSolverInterfaceFromGunzip("../src/test/datasets/vary_rhs/series_2/rhs_s2_i01.mps.gz");
  VwsSolverInterface seriesSolver(10, 120, 4, .8);
  seriesSolver.maxExtraSavedSolutions = 0;
  CbcModel model = seriesSolver.solve(instanceSolver, "None", false);

  // check that we have valid bounds
  REQUIRE((-17180 < model.getObjValue() && model.getObjValue() < -17150));

  // the problem should have one solution since we don't save extras
  REQUIRE(seriesSolver.solutions[0].size() >= 1);
  checkSolutions(seriesSolver, instanceSolver, model, 0);
}

// checks that we get optimal solution when adding vpcs from PRLP and Farkas Multipliers
// well currently just checks that root node is correct, but we'll get back to this
TEST_CASE( "Check solve with VPCs added", "[VwsSolverInterface::solve]" ) {

  // solve a model with the VwsSolverInterface
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  VwsSolverInterface seriesSolver(10, 60, 64);
  MipCompEventHandler * eventHandler = new MipCompEventHandler();
  CbcModel model = seriesSolver.solve(instanceSolver, "PRLP", false, eventHandler);
  eventHandler = dynamic_cast<MipCompEventHandler*>(model.getEventHandler());

  // we should have one vpc generator and set of farkas multipliers
  REQUIRE(seriesSolver.disjunctions.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);

  // we should have found 6 cuts (for the number of fractional variables in the root LP relaxation)
  REQUIRE(seriesSolver.cutCertificates[0].size() == 6);

  // each cut should have 64 disjunctive terms
  for (int cutIdx=0; cutIdx<seriesSolver.cutCertificates[0].size(); cutIdx++) {
    REQUIRE(seriesSolver.cutCertificates[0][cutIdx].size() == 64);
    // each disjunctive term should have more than 47 coefficients (27 variables, 20 constraints, n branching decisions)
    for (int termIdx=0; termIdx<seriesSolver.cutCertificates[0][cutIdx].size(); termIdx++) {
      REQUIRE(47 < seriesSolver.cutCertificates[0][cutIdx][termIdx].size());
    }
  }

  // check the primal solutions
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model, 0);

  // check the bounds - root LP relaxation should be much improved from VPCs
  REQUIRE(model.getObjValue() == 34);
  REQUIRE((25 < eventHandler->data.rootDualBoundPreVpc &&
           eventHandler->data.rootDualBoundPreVpc < 26));
  REQUIRE((29 < eventHandler->data.rootDualBound && eventHandler->data.rootDualBound < 30));

  // but the underlying model unchanged
  REQUIRE((20 < model.getContinuousObjective() && model.getContinuousObjective() < 21));
  REQUIRE(model.solver()->getNumRows() == 20);

  // check vpcs when parameterized
  eventHandler = new MipCompEventHandler();
  CbcModel model2 = seriesSolver.solve(instanceSolver, "Farkas", false, eventHandler);
  eventHandler = dynamic_cast<MipCompEventHandler*>(model2.getEventHandler());

  // Farkas does not create an additional VPC generator or set of multipliers
  REQUIRE(seriesSolver.disjunctions.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);

  // The underlying model should not have changed
  REQUIRE(model2.solver()->getNumRows() == 20);
  REQUIRE((20 < model2.getContinuousObjective() && model2.getContinuousObjective() < 21));

  // check the primal solutions
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model2, 1);

  // check the bounds - root LP relaxation should be much improved from VPCs
  REQUIRE(model2.getObjValue() == 34);
  REQUIRE((25 < eventHandler->data.rootDualBoundPreVpc &&
           eventHandler->data.rootDualBoundPreVpc < 26));
  REQUIRE((29 < eventHandler->data.rootDualBound && eventHandler->data.rootDualBound < 30));

}

TEST_CASE( "Check solve longer with VPCs added", "[VwsSolverInterface::solve][long]" ) {

  // solve a model with the VwsSolverInterface
  OsiClpSolverInterface instanceSolver =
      extractSolverInterfaceFromGunzip("../src/test/datasets/vary_rhs/series_2/rhs_s2_i01.mps.gz");
  VwsSolverInterface seriesSolver(10, 180, 4, .8);

  // generate vpcs with the PRLP
  MipCompEventHandler * eventHandler = new MipCompEventHandler();
  CbcModel model = seriesSolver.solve(instanceSolver, "PRLP", false, eventHandler);
  eventHandler = dynamic_cast<MipCompEventHandler*>(model.getEventHandler());

  // we should have one vpc generator and set of farkas multipliers
  REQUIRE(seriesSolver.disjunctions.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);

  // we should have added cuts
  REQUIRE(seriesSolver.cutCertificates[0].size() > 0);

  // each cut should have 4 disjunctive terms
  for (int cutIdx=0; cutIdx<seriesSolver.cutCertificates[0].size(); cutIdx++) {
    REQUIRE(seriesSolver.cutCertificates[0][cutIdx].size() == 4);
    // each disjunctive term should have 2252 coefficients (1000 variables, 1250 constraints, 2 branching decisions)
    for (int termIdx=0; termIdx<seriesSolver.cutCertificates[0][cutIdx].size(); termIdx++) {
      REQUIRE(seriesSolver.cutCertificates[0][cutIdx][termIdx].size() ==
              2252 + seriesSolver.disjunctions[0]->common_changed_bound.size());
    }
  }

  // check the primal solutions
  REQUIRE(seriesSolver.solutions[0].size() == 1);
  checkSolutions(seriesSolver, instanceSolver, model, 0);

  // check the bounds are valid
  REQUIRE((-17170 < model.getObjValue() && model.getObjValue() < -17150));
  REQUIRE(model.getContinuousObjective() < eventHandler->data.rootDualBoundPreVpc);
  REQUIRE(eventHandler->data.rootDualBoundPreVpc < eventHandler->data.rootDualBound);
  REQUIRE(eventHandler->data.rootDualBound < model.getObjValue());


  // generate vpcs with Farkas multipliers
  eventHandler = new MipCompEventHandler();
  CbcModel model2 = seriesSolver.solve(instanceSolver, "Farkas", false, eventHandler);
  eventHandler = dynamic_cast<MipCompEventHandler*>(model2.getEventHandler());

  // we should still have one vpc generator and set of farkas multipliers
  REQUIRE(seriesSolver.disjunctions.size() == 1);
  REQUIRE(seriesSolver.cutCertificates.size() == 1);

  // check the primal solutions
  REQUIRE(seriesSolver.solutions[1].size() >= 1);
  checkSolutions(seriesSolver, instanceSolver, model2, 1);

  // check the bounds are valid (implies cuts were added)
  REQUIRE((-17170 < model2.getObjValue() && model2.getObjValue() < -17150));
  REQUIRE(model2.getContinuousObjective() < eventHandler->data.rootDualBoundPreVpc);
  REQUIRE(eventHandler->data.rootDualBoundPreVpc < eventHandler->data.rootDualBound);
  REQUIRE(eventHandler->data.rootDualBound < model2.getObjValue());
}

// --------------------- create disjunctive cut tests --------------------------

TEST_CASE( "Check VPCs created successfully from PRLP",
           "[VwsSolverInterface::createDisjunctiveCutsFromPRLP]" ) {

  // solve a model with the VwsSolverInterface to get the solution
  OsiClpSolverInterface instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  VwsSolverInterface seriesSolver;
  seriesSolver.solve(instanceSolver);

  // check that the vpcs we get are valid
  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/bm23/series_1/bm23_i01.mps.gz");
  std::shared_ptr<OsiCuts> disjCuts = seriesSolver.createDisjunctiveCutsFromPRLP(instanceSolver);

  // we should have added some cuts
  REQUIRE(disjCuts->sizeRowCuts() > 0);
  for (int i=0; i<disjCuts->sizeRowCuts(); i++){

    // each cut should be of form a^T x >= b
    REQUIRE(disjCuts->rowCutPtr(i)->lb() > -COIN_DBL_MAX);
    REQUIRE(disjCuts->rowCutPtr(i)->ub() == COIN_DBL_MAX);

    // each cut should be valid for the optimal solution
    REQUIRE(isFeasible(disjCuts->rowCut(i), seriesSolver.solutions[0][0]));
  }
}

TEST_CASE( "Check VPCs created successfully from Farkas",
           "[VwsSolverInterface::createDisjunctiveCutsFromFarkasMultipliers][long]" ) {

  // solve a model with the VwsSolverInterface
  OsiClpSolverInterface instanceSolver =
      extractSolverInterfaceFromGunzip("../src/test/datasets/vary_rhs/series_2/rhs_s2_i01.mps.gz");
  VwsSolverInterface seriesSolver(10, 120, 4, .8);
  TestHandler handler;
  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);

  // solve a second instance to get a solution for it
  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_rhs/series_2/rhs_s2_i02.mps.gz");
  CbcModel model = seriesSolver.solve(instanceSolver);

  // check that the vpcs we get are valid
  std::shared_ptr<OsiCuts> disjCuts = seriesSolver.createDisjunctiveCutsFromFarkasMultipliers(instanceSolver);
  // we should have added a lot of cuts
  REQUIRE(disjCuts->sizeRowCuts() > 0);
  for (int i=0; i<disjCuts->sizeRowCuts(); i++){

    // each cut should be of form a^T x >= b
    REQUIRE(disjCuts->rowCutPtr(i)->lb() > -COIN_DBL_MAX);
    REQUIRE(disjCuts->rowCutPtr(i)->ub() == COIN_DBL_MAX);

    // each cut should be valid for the optimal solution
    REQUIRE(isFeasible(disjCuts->rowCut(i), seriesSolver.solutions[1][0]));
  }

}

TEST_CASE( "Create VPCs from Farkas multipliers on problems with large changes in feasible region (edge case)",
           "[VwsSolverInterface::createDisjunctiveCutsFromFarkasMultipliers][long]" ) {

  // set up reusable stuff
  VwsSolverInterface seriesSolver(10, 600, 4, .8);
  TestHandler handler;
  OsiClpSolverInterface instanceSolver;

  // create cuts for first 10 instances
  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i01.mps.gz");
  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);
//  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i02.mps.gz");
//  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);
//  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i03.mps.gz");
//  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);
//  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i04.mps.gz");
//  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);
//  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i05.mps.gz");
//  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);
//  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i06.mps.gz");
//  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);
//  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i07.mps.gz");
//  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);
//  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i08.mps.gz");
//  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);
//  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i09.mps.gz");
//  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);
//  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i10.mps.gz");
//  seriesSolver.solve(instanceSolver, "PRLP", false, &handler);
//
//  // get solutions for the weird instance
//  instanceSolver = extractSolverInterfaceFromGunzip("../src/test/datasets/vary_bounds/series_1/bnd_s1_i38.mps.gz");
//  seriesSolver.solve(instanceSolver, "None", true, &handler);
//
//  // check that the vpcs we get are valid
//  std::shared_ptr<OsiCuts> disjCuts = seriesSolver.createDisjunctiveCutsFromFarkasMultipliers(instanceSolver);
//  // this will fail bc I'm using partial tree - todo: complete tree
//  for (int i=0; i<disjCuts->sizeRowCuts(); i++){
//
//    // each cut should be of form a^T x >= b
//    REQUIRE(disjCuts->rowCutPtr(i)->lb() > -COIN_DBL_MAX);
//    REQUIRE(disjCuts->rowCutPtr(i)->ub() == COIN_DBL_MAX);
//    REQUIRE(isFeasible(disjCuts->rowCut(i), seriesSolver.solutions[10][0]));
//
//    // each cut should be valid for the optimal solution
////    if (!isFeasible(disjCuts->rowCut(i), seriesSolver.solutions[10][0])){
////      std::cout << i << std::endl;
////    }
//  }
  std::cout << "hello" << std::endl;
}