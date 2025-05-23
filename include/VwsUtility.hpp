/**
 * @file VwsUtility.hpp
 * @author Sean Kelley
 * @date 2023-02-13
 */

// standard library
#include <filesystem.hpp> // path
#include <string> // string

// coin-or modules
#include "CbcModel.hpp"

// Eigen library
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>

// vpc modules
#include "CglVPC.hpp"

// namespaces
using namespace Eigen;
namespace fs = ghc::filesystem;

/** Unzips the .gz file at instancePath and reads it into an OsiClpSolverInterface */
OsiClpSolverInterface extractSolverInterfaceFromGunzip(fs::path instancePath);

/** get the variable names from a CbcModel */
std::vector<std::string> getVariableNames(const OsiSolverInterface& instanceSolver);

/** get the constraint names from a CbcModel */
std::vector<std::string> getConstraintNames(const OsiSolverInterface& instanceSolver);

/** writes solution in <variableValue> to <solutionPath>. Removes previous
 * solution recorded at <solutionPath>. */
void writeSolution(std::vector<double> variableValues, std::vector<std::string> variableNames,
                   fs::path solutionPath);

/** transform solutions from the presolved model to the original instance */
void putBackSolutions(CbcModel *presolvedModel, CbcModel *model, CglPreProcess *preProcess);

/** Get the Farkas multipliers for the disjunctive cuts generated by the VPC generator.
   *  The multipliers are returned as a vector of vectors of vectors of doubles.
   *  The first index is the cut index, the second index is the disjunctive term index,
   *  and the third index is the index of the variable in the disjunctive term.
   *  Borrowed from Strengthening's main.cpp */
std::vector< std::vector < std::vector<double> > > getFarkasMultipliers(
    const OsiSolverInterface& solver, const Disjunction& disj, const OsiCuts& currCuts);

/** Find the Farkas certificate for a cut given the solver for its disjunctive term */
void getCertificate(
    /// [out] Farkas multipliers (vector of length m + m_t + n)
    std::vector<double>& v,
    /// [in] number of nonzero cut coefficients
    const int num_elem,
    /// [in] indices of nonzero cut coefficients
    const int* const ind,
    /// [in] nonzero cut coefficients
    const double* const coeff,
    /// [in] rhs of cut
    const double cut_rhs,
    // [in] LP solver corresponding to disjunctive term
    OsiSolverInterface* const solver);

/** create the basis matrix for generating the cut certificate */
void createEigenMatrix(
    /// [out] sparse matrix, in row-major form
    Eigen::SparseMatrix<double,Eigen::RowMajor>& M,
    /// [in] COIN-OR solver
    const OsiSolverInterface* const solver,
    /// [in] set of rows that we want to consider
    const std::vector<int>& rows,
    /// [in] set of variable bounds we want to explicitly use
    const std::vector<int>& cols);

/** Find the solution x to Ax = b */
void solveLinearSystem(
    Eigen::VectorXd& x,
    const Eigen::SparseMatrix<double>& A,
    const Eigen::VectorXd& b);

void prepareRow(
    /// [out] Eigen::Triplet<double> that we are updating
    std::vector<Eigen::Triplet<double> >& tripletList,
    /// [in] sparse matrix from COIN-OR, in row-major form
    const CoinPackedMatrix* const mat,
    /// [in] row we are inserting
    const int row,
    /// [in] row in which to insert this value
    const int tmp_row);

void insertRow(
    /// [out] sparse matrix, in row-major form
    Eigen::SparseMatrix<double,Eigen::RowMajor>& M,
    /// [in] sparse matrix from COIN-OR, in row-major form
    const CoinPackedMatrix* const mat,
    /// [in] row we are inserting
    const int row,
    /// [in] row in which to insert this entry
    const int tmp_row);

/** Find alpha and beta such that alpha^T x >= beta for all x feasible for solver
 *  where alpha and beta are derived by taking a linear combination of solver's
 *  constraints with coefficients prescribed by the Farkas multipliers v.
 *  Note: First m+m_t rows of v correspond to A;D^t; the next n are bounds on the variables */
void getCutFromCertificate(
    /// [out] calculated cut coefficients
    std::vector<double>& alpha,
    /// [out] calculated cut bound
    double& beta,
    /// [in] Farkas multipliers
    const std::vector<double>& v,
    /// [in] LP solver corresponding to disjunctive term
    const OsiSolverInterface* const solver);

/** find the smallest value in each column given a vector of row vectors */
std::vector<double> elementWiseMax(std::vector< std::vector<double> > a);

/** find the smallest value in a vector */
double min(std::vector<double> a);

/** check if cut is valid for given solution */
bool isFeasible(
    /// [in] problem
    const OsiRowCut& cut,
    /// [in] solution
    const std::vector<double>& sol);

/** find the nonzero indices and elemnets of a vector */
void findNonZero(
    /// [in] vector
    const std::vector<double>& vec,
    /// [out] indices of nonzero elements
    std::vector<int>& indices,
    /// [out] nonzero elements
    std::vector<double>& elements);
