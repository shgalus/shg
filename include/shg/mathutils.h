/**
 * \file include/shg/mathutils.h
 * Mathematical utilities.
 */

#ifndef SHG_MATHUTILS_H
#define SHG_MATHUTILS_H

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/math/special_functions/fibonacci.hpp>
#include <shg/matrix.h>
#include <shg/utils.h>
#include <shg/ifact.h>

namespace SHG {

/**
 * Returns n-th Fibonacci number.
 *
 * \f$f_0 = 0\f$, \f$f_1 = 1\f$, \f$f_{n + 2} = f_{n + 1} + f_{n}\f$,
 * \f$n \geq 0\f$.
 *
 * \throws std::invalid_argument if \f$n < 0\f$
 */
template <typename T>
T fibonacci(long long n);

/**
 * Returns \c true if \e n is a Fibonacci number.
 *
 * \implementation See
 * https://en.wikipedia.org/wiki/Fibonacci_sequence#Identification
 * (15 November 2024)
 */
template <typename T>
bool is_fibonacci(T const& n);

/**
 * Returns \c true if \e n is square-free, ie. if \e n is not
 * divisible by the square of any integer other than one.
 */
template <typename T>
bool is_square_free(T const& n);

/**
 * Status for Lin_Dioph2.
 */
enum class Status {
     success,
     no_solution,
};

/**
 * Linear Diophantine equation with two unknowns.
 *
 * Solves \f$a_1x_1 + a_2x_2 = b\f$, where \f$a_1\f$, \f$a_2\f$,
 * \f$b\f$, are integer numbers.
 *
 * Both Lin_Dioph2(T const&, T const&, T const&) and solve(T const&, T
 * const&, T const&):
 *
 * - throw \f$\verb+std::invalid_argument+\f$ if \f$a_1 = a_2 = 0\f$,
 *
 * - return \f$\var{status} = \var{no\_solution}\f$ if \f$\gcd(a_1,
 * a_2) \nmid b\f$ and set \f$d = \gcd(a_1, a_2)\f$,
 *
 * - return \f$\var{status} = \var{success}\f$ if \f$\gcd(a_1, a_2)
 * \mid b\f$ and set \f$d = \gcd(a_1, a_2)\f$ and \f$x_1^0, x_2^0,
 * c_1, c_2 \in \integern\f$ such that \f[x_1 = x_1^0 + c_1t \\ x_2 =
 * x_2^0 + c_2t\f] are solutions for all \f$t \in \integern\f$.
 */
template <typename T>
class Lin_Dioph2 {
     static_assert(std::numeric_limits<T>::is_integer);
     static_assert(std::numeric_limits<T>::is_signed);

public:
     Lin_Dioph2() = default;
     Lin_Dioph2(T const& a1, T const& a2, T const& b);
     void solve(T const& a1, T const& a2, T const& b);
     Status status{};
     T d{};
     T x1{};
     T x2{};
     T c1{};
     T c2{};
};

/**
 * System of linear Diophantine equations.
 *
 * Solves \f$Ax = b\f$, where \f$A\f$ is an \f$m \times n\f$ matrix,
 * \f$x\f$ is an \f$n \times 1\f$ vector, \f$b\f$ is an \f$m \times
 * 1\f$ vector, \f$m \geq 1\f$, \f$n \geq 1\f$.
 */
template <typename T>
class Lin_Dioph {
     static_assert(std::numeric_limits<T>::is_integer);
     static_assert(std::numeric_limits<T>::is_signed);

public:
     using Vec = SHG::Vector<T>;
     using Mat = SHG::Matrix<T>;
     bool solve(Mat const& A, Vec const& b);
     Vec x0{};
     Mat C{};

private:
     using Index = std::size_t;
     /**
      * Transforms matrix to triangular form.
      * See \cite blankinship-1966a.
      */
     static Index intrank(Mat& mat, Index m, Index n, Index e);
     /**
      * Seeks the smallest positive integer \f$d\f$ for which an
      * integer solution of \f$Ax = bd\f$ exists. If no solution
      * exists, returns false, otherwise values of \f$d\f$ and the
      * solution vector \f$x\f$ are returned. If more than one
      * solution exists then auxiliary solutions are returned in the
      * matrix \f$Y\f$. The additional solutions are obtained by
      * adding any linear combination of the first \f$k\f$ rows of
      * \f$Y\f$ to the solution \f$x\f$. Note that a diophantine
      * solution exists if and only if \f$d\f$ is returned as 1 and
      * solveinteger is returned as true.
      *
      * See \cite blankinship-1966b.
      */
     static bool solveinteger(Index m, Index n, Mat const& A,
                              Vec const& b, T& d, Vec& x, Mat& Y,
                              Index& k);
};

}  // namespace SHG

#include <shg/mathutils-inl.h>

#endif
