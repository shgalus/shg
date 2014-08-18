/* drbnwtsn.h: Durbin-Watson statistic */

/**
 * \file include/shg/drbnwtsn.h
 * Durbin-Watson statistic.
 * \date Created on 12 July 2010.
 */

#ifndef SHG_DRBNWTSN_H
#define SHG_DRBNWTSN_H

#include <iostream>

namespace SHG {

/** \addtogroup specfunc */
/** \{ */

/**
 * Cumulative distribution function of the Durbin-Watson statistic.
 *
 * \f$n \geq 1\f$ denotes the number of observations, \f$k \geq 1\f$
 * is the number of regressors excluding the intercept, \f$x\f$ is the
 * point at which the function should be evaluated,
 * \f$\mathit{lower}\f$ should be \f$\mathit{true}\f$ for \f$d_L\f$
 * and \f$\mathit{false}\f$ for \f$d_U\f$, \f$\mathit{eps} > 0\f$ is
 * the accuracy for calculating the result (see equation (4) on page 2
 * in <a href="drbnwtsn.pdf">Cumulative distribution function of the
 * Durbin-Watson statistic.</a>), \f$\mathit{iter} \geq 1\f$ is the
 * maximum number of iterations in the Simpson method. In the
 * \f$i\f$-th iteration, \f$1 \leq i \leq \mathit{iter}\f$, \f$2^i\f$
 * new values of the integrand are calculated. The total number of
 * evaluations of integrand is \f$2^{\mathit{iter} + 1} + 1\f$. There
 * must be \f$n - k - 1 > 1\f$.
 *
 * \return the value of the cumulative distribution function
 *
 * \exception std::invalid_argument if \f$n \leq 0\f$, \f$k \leq 0\f$,
 * \f$n - k - 1 \leq 1\f$, \f$\mathit{eps} \leq 0\f$ or
 * \f$\mathit{iter} < 1\f$
 *
 * \exception std::range_error if the number of iterations would be
 * greater than \f$\mathit{iter}\f$
 *
 * See <a href="drbnwtsn.pdf">Cumulative distribution function of the
 * Durbin-Watson statistic.</a>
 *
 * \sa ppdw, swtbl
 */
double dwcdf(int n, int k, double x, bool lower, double eps, int iter);

/**
 * The percentage points of the Durbin-Watson statistic.
 *
 * \f$n \geq 1\f$ denotes the number of observations, \f$k \geq 1\f$ is
 * the number of regressors excluding the intercept, \f$0 < p < 1\f$ is
 * the probability for which percentage point shoul be evaluated,
 * \f$\mathit{eps} > 0\f$ is the accuracy of calculating. There must be
 * \f$n - k - 1 > 1\f$.
 *
 * On return, \f$x_L\f$ and \f$x_U\f$ contain values such that
 * \f$\Pr(d_L < x_L) = p\f$ and \f$\Pr(d_U < x_U) = p\f$.
 *
 * The method of bisection is used twice to solve equations \f$\Pr(d_L <
 * x_L) = p\f$ and \f$\Pr(d_U < x_U) = p\f$.
 *
 * \exception std::invalid_argument if \f$n \leq 0\f$, \f$k \leq 0\f$,
 * \f$n - k - 1 \leq 1\f$, \f$p \leq 0\f$ or \f$ p \geq 1\f$
 *
 * \exception std::range_error if the number of iterations in dwcdfwould
 * be greater than 15
 *
 * \sa dwcdf, swtbl
 */
void ppdw(int n, int k, double p, double* xl, double* xu, double eps);

/**
 * Prints tables of the Durbin-Watson statistic.
 *
 * The function prints to the stream \f$f\f$ tables of the
 * Durbin-Watson statistic like those in \cite savin-white-1977.
 *
 * Calls ppdw(int, int, double, double*, double*, double). For \f$n =
 * 6, ..., 200\f$, \f$k = 1, ..., 5\f$, the fifteen differences
 * between the generated tables and the original tables of Savin and
 * White are listed in the subroutine ppdtst in testing/drbnwtsn.cc.
 */
void swtbl(std::ostream& f = std::cout);

/** \} */

}       // namespace SHG

#endif
