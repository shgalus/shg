/* numalg.h: numerical algorithms */

/**
 * \file include/shg/numalg.h
 * Numerical algorithms.
 * Created on  2 April 2017.
 */

#ifndef SHG_NUMALG_H
#define SHG_NUMALG_H

#include <complex>
#include "shg/vector.h"
#include "shg/matrix.h"

namespace SHG {

/**
 * Solves system of linear equations.
 */
int solve_linear(Matdouble& a, Vecdouble& x);

/**
 * Solves the equation \f$a_0 + a_1x + \ldots + a_{n - 1}x^{n - 1} =
 * 0\f$, \f$n \geq 1\f$.
 */
int solve_polynomial(const Vecdouble& a,
                     Vector<std::complex<double>>& x);

}       // namespace SHG

#endif
