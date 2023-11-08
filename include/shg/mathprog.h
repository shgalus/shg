/**
 * \file include/shg/mathprog.h
 * Mathematical programming.
 * \date Created on 16 July 2014.
 */

#ifndef SHG_MATHPROG_H
#define SHG_MATHPROG_H

#include <shg/except.h>
#include <shg/matrix.h>

namespace SHG {

/**
 * \defgroup mathematical_programming Mathematical programming
 *
 * Mathematical programming.
 *
 * \{
 */

/**
 * Solves the linear programming problem by revised simplex method.
 * Solves the problem in standard form: minimize \f$c^T x\f$ subject
 * to \f$Ax = b\f$, \f$x \geq 0\f$, where \f$b \geq 0\f$. \f$m \geq
 * 1\f$ is the number of constraints, \f$n \geq 1\f$ is the number of
 * variables, \f$A\f$ is an \f$m \times n\f$ matrix, \f$b\f$ is an
 * \f$m \times 1\f$ vector, \f$c\f$ is an \f$n \times 1\f$ vector,
 * \f$x\f$ is an \f$n \times 1\f$ vector. The parameter \a eps defines
 * the tolerance: for a real number a, if abs(a) < eps, then a is
 * treated as 0.
 *
 * \retval 0 ok, solution found
 * \retval 1 invalid input
 * \retval 2 no feasible solution
 * \retval 3 unbounded solution
 *
 * If the returned value is 0, then \a x(j), j = 0, ..., n - 1,
 * contains values of \a x and \a f is the value of objective function
 * in \a x. If the return value is not 0, the values of f and x are
 * not changed by this function.
 *
 * \date Written on 8 January 2012.
 *
 * \implementation Translated from \cite syslo-deo-kowalik-1999, p.
 * 23-25.
 */
int revsimplex(Matdouble const& A, Vecdouble const& b,
               Vecdouble const& c, Vecdouble& x, double& f,
               double eps = 1e-12);

/**
 * Simplex method. A simple wrapper for revsimplex().
 * \date Written on 8 January 2012.
 */
class Simplex {
public:
     /**
      * %Exception class thrown by the constructor.
      */
     struct Error : public virtual Exception {
          Error();
     };
     /**
      * Kinds of signs in constraints.
      */
     enum Equality {
          eq,  ///< equality
          le,  ///< less than or equal
          ge   ///< greater than or equal
     };
     /**
      * Directions of optimization.
      */
     enum Direction {
          min,  ///< minimization
          max   ///< maximization
     };
     typedef Vector<Equality> Vecequality;
     /**
      * Constructor. Calls revsimplex().
      *
      * \param [in] m the number of constraints
      * \param [in] n the number of variables
      * \param [in] A left-hand side coefficients
      * \param [in] b right-hand side coefficients
      * \param [in] c coefficients of objective function
      * \param [in] e signs in constraints
      * \param [in] d direction of optimization
      * \param [in] eps eps for revsimplex()
      *
      * \exception SHG::Simplex::Error if invalid argument is given
      */
     Simplex(std::size_t m, std::size_t n, Matdouble const& A,
             Vecdouble const& b, Vecdouble const& c,
             Vecequality const& e, Direction d, double eps = 1e-12);
     int status;   ///< return value from revsimplex()
     double f;     ///< value of objective function
     Vecdouble x;  ///< point with optimum
};

/**
 * Wolfe method for quadratic programming problem.
 *
 * Minimizes \f$px + x^TCx\f$ subject to \f$Ax = b\f$, \f$x \geq 0\f$.
 * \f$p\f$ and \f$x\f$ are \f$n \times 1\f$ vectors, \f$b\f$ is an
 * \f$m \times 1\f$ vector, \f$A\f$ is an \f$m \times n\f$ matrix,
 * \f$C\f$ is an \f$n \times n\f$ matrix. The matrix \f$C\f$ must be
 * symmetric (only upper right triangle is used) and should be
 * positive definite. The number of linear equality constraints
 * \f$m\f$ and the number of variables \f$n\f$ must be greater than 0.
 *
 * \retval 0 ok, solution found
 * \retval 1 invalid arguments
 * \retval 2 not enough memory
 * \retval 3 the maximum number of simplex iteration steps exceeded
 * \retval 4 no feasible solution in initiation phase
 * \retval 5 no feasible solution in recursion phase
 * \retval 6 unbounded solution in initiation phase
 * \retval 7 unbounded solution in recursion phase
 *
 * If the returned value is 0, \a x contains the solution, \a f
 * contains the minimum value, otherwise they are udefined.
 *
 * \note The parameter \a C should be a vector of size n x (n + 1) / 2
 * which contains the upper right triangle of the matrix \f$C\f$
 * written by rows.
 *
 * \date Written on 10 May 2009.
 *
 * \internal
 * Implementation.
 * ===============
 *
 * Problem.
 * --------
 *
 * Let A be an m x n matrix, b >= 0 an m x 1 vector, p an 1 x n
 * vector, C an n x n symmetric, positive semidefinite matrix, and x
 * an n x 1 vector. The quadratic programming problem is:
 *
 *     minimize px + x^(T)Cx
 *     subject to Ax = b,
 *     x >= 0.
 *
 * The Wolfe method is based on the theorem that x >= 0 solves the
 * quadratic programming problem if Ax = b and there exist n x 1
 * vector v >= 0 and m x 1 vector u such that
 *
 *     v^(T)x = 0,
 *     2Cx - v + A^(T)u + p^(T) = 0.
 *
 * Convergence of the "short form" of the method, used here, requires
 * that either p = 0 or C is positive definite.
 *
 * Algorithm.
 * ----------
 *
 * Let z1, z2 be n x 1 vectors and w be an m x 1 vector.
 *
 * 1. Initiation. Use the simplex method to minimize w(1) + ... + w(m)
 * to zero subject to
 *
 *     Ax + w = b,
 *     2Cx - v + A^(T)u + z1 - z2 = -p^(T),
 *     x, v, z1, z2, w >= 0,
 * keeping u and v zero. Discard w and the unused components of z1 and
 * z2. Let the remaining n components be denoted by z, and their
 * coefficients by E. We have a solution of the system
 *
 *     Ax = b,
 *     2Cx - v + A^(T)u + Ez = -p^(T),
 *     x, v, z >= 0.
 *
 * 2. Recursion. Take u = u1 - u2, u1, u2 >= 0. Use the simplex method
 * to minimize z(1) + z(2) + ... + z(n) to zero subject to
 *
 *     Ax = b,
 *     2Cx - v + A^(T)u1 - A^(T)u2 + Ez = -p^(T),
 *     x, v, u1, u2, z >= 0,
 * under the side condition: for k = 1, ..., n: if x(k) is in the
 * basis, do not admit v(k); if v(k) is in the basis, do not admit
 * x(k).
 *
 * 3. Termination. If p = 0 or C is positive definite, the objective
 * function z(1) + ... + z(n) will vanish in at most 3n over n (Newton
 * symbol) iterations, yielding z = 0. The x-part of the terminal
 * basic solution is a solution of the quadratic programming problem.
 *
 * Implementation.
 * ---------------
 *
 * The initial simplex table for the initiation phase is
 *
 * |Basis| x | v | u | z | w |RHS |
 * |:---:|:-:|:-:|:-:|:-:|:-:|:--:|
 * |  w  | A | 0 | 0 | 0 | I | b  |
 * |  z  | 2C| -I|A^T| I | 0 |-p^T|
 * |     |   | 0 | 0 | 0 | 0 |    |
 *
 * The n variables z are choosen to have -p^T >= 0. The same table may
 * be used in the recursion phase. The numberes of basis variables
 * will be kept in a special vector, so there is no need to keep their
 * coefficients in the table. The table in both initiation and
 * recursion will be
 *
 * |Basis| x | v | u |RHS |
 * |:---:|:-:|:-:|:-:|:-: |
 * |  w  | A | 0 | 0 | b  |
 * |  z  | 2C| -I|A^T|-p^T|
 * |     |   | 0 | 0 |    |
 *
 * In the initiation phase u and v will not enter the basis and in the
 * recursion phase w will not enter the basis and the side condition
 * will be used. Numbers of variables will be as follows:
 *
 *     x(1) ... x(n) - 0 ... n - 1
 *     v(1) ... v(n) - n ... 2n - 1
 *     u(1) ... u(m) - 2n ... 2n + m - 1
 *     z(1) ... z(n) - 2n + m ... 3n + m - 1
 *     w(1) ... w(m) - 3n + m ... 3n + 2m - 1
 *
 * The simplex table will be kept in the matrix
 *
 *     t(m + n + 1, 2n + m + 1),
 *
 * the m + n-th row and 2n + m-th column being added for coefficients.
 * The numbers of basis and non-basis variables will be kept in the
 * integer vectors B(m + n) and N(2n + m).
 *
 * Both phases will be performed within the same loop. The flow is
 * arranged to affect whole rows of the table t in order to make
 * eventual version using scratch file easier to write.
 *
 * References.
 * -----------
 *
 * \cite wolfe-1959, \cite gass-1980, p. 276-282,
 * \cite grabowski-1980, p. 245-251,
 * \cite bronsztejn-siemiendiajew-musiol-muhlig-2004, p. 943-945
 */
int wolfe(Vecdouble const& p, Vecdouble const& C, Matdouble const& A,
          Vecdouble const& b, Vecdouble& x, double& f);

/** \} */ /* end of group mathematical_programming */

}  // namespace SHG

#endif
