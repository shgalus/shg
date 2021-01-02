/**
 * \file include/shg/specfunc.h
 * Special functions.
 * \date Created on 10 July 2012.
 */

#ifndef SHG_SPECFUNC_H
#define SHG_SPECFUNC_H

#include <cmath>
#include <shg/mconsts.h>

namespace SHG {

/**
 * \defgroup specfunc Special functions
 * Some special functions.
 * \{
 */

/**
 * The normal integral.
 *
 * Returns \f$F(x)\f$ (upper == false) or \f$1 - F(x)\f$ (upper ==
 * true), where \f$F(x)\f$ is the cumulative distribution function of
 * the standard normal distribution.
 */
template <class T>
T normal_integral(T x, bool upper = false) {
     static_assert(std::is_floating_point<T>::value,
                   "T must be a floating point type");

     return upper ? static_cast<T>(0.5) *
                         std::erfc(SHG::Constants::isqrt2<T> * x)
                  : static_cast<T>(0.5) +
                         static_cast<T>(0.5) *
                              std::erf(SHG::Constants::isqrt2<T> * x);
}

/**
 * The percentage points of the normal distribution.
 *
 * Returns \f$x\f$ such that \f$F(x) = p\f$, where \f$F\f$(x) is the
 * distribution function of the standardized normal distribution. The
 * accuracy is about seven decimal digits for \f$\min(p, 1 - p) >
 * 10^{-316}\f$.
 *
 * \exception std::invalid_argument if \f$p \leq 0\f$ or \f$p \geq
 * 1\f$
 *
 * Source: \cite wichura-1988.
 */
double ppnd7(double p);

/**
 * Incomplete gamma integral.
 *
 * Returns \f$\frac{1}{\Gamma(p)} \int_0^x t^{p - 1} e^{-t} dt\f$
 * for \f$p > 0\f$ and \f$x \geq 0\f$.
 *
 * \exception std::invalid_argument if \f$x < 0\f$ or \f$p \leq 0\f$
 *
 * \note If a random variable \f$X\f$ has a gamma distribution with
 * probability density function \f[ f(x; b, p) = \left\{ \begin{array}
 * {ll} \frac{b^p}{\Gamma(p)} x^{p - 1} e^{-bx} & \mbox{for $x > 0$}
 * \\ 0 & \mbox{for $x \leq 0$} \end{array} \right.\f] where \f$b\f$
 * and \f$p\f$ are positive parameters, then \f$P(X < x) =
 * \mathit{gammad}(bx, p)\f$. If \f$X\f$ has a chi-squared
 * distribution with \f$n\f$ degrees of freedom (\f$p = n / 2\f$, \f$b
 * = 1 / 2\f$), then \f$P(X < x) = \mathit{gammad}(x / 2, n / 2)\f$.
 *
 * Source: \cite shea-1988.
 */
double gammad(double x, double p);

/**
 * The cumulative distribution function of Student's \f$t\f$
 * distribution.
 *
 * For given \f$t \in \mathbf{R}\f$ and \f$\mathit{df} \geq 1\f$, the
 * function returns \f$\Pr(X \leq t)\f$, where \f$X\f$ is a random
 * variable with Student's central \f$t\f$ distribution with
 * \f$\mathit{df}\f$ degrees of freedom. The accuracy should be near
 * to the number of significant digits on a computer.
 *
 * \exception std::invalid_argument if \f$\mathit{df} < 1\f$
 *
 * The function is a C++ translation of the original Fortran code of
 * the Algorithm AS 3 by B. E. Cooper \cite griffiths-hill-1986, p.
 * 38-39.
 */
double probst(double t, int df);

/**
 * The incomplete beta integral.
 *
 * For given \f$0 \leq x \leq 1\f$, \f$p > 0\f$, \f$q > 0\f$ and
 * \f$\ln B(p, q)\f$, the function computes the incomplete beta
 * function \f[\frac{1}{B(p, q)} \int_0^x t^{p - 1} (1 - t)^{q - 1}
 * dt,\f] where \f[B(p, q) = \frac{\Gamma(p) \Gamma(q)}{\Gamma(p +
 * q)}\f] is the beta function \cite fisz-1969, p. 167. The accuracy
 * of the result should not be less than \f$10^{-8}\f$.
 *
 * \exception std::invalid_argument if \f$p \leq 0\f$, \f$q \leq 0\f$,
 * \f$x < 0\f$ or \f$x > 1\f$
 *
 * Source: K. L. Majumder and G. P. Bhattacharjee, Algorithm AS 63,
 * in: \cite griffiths-hill-1986, p. 117-120.
 *
 * \sa betain(double x, double p, double q)
 */
double betain(double x, double p, double q, double beta);

/**
 * The incomplete beta integral.
 *
 * For given \f$0 \leq x \leq 1\f$, \f$p > 0\f$ and \f$q > 0\f$, the
 * function computes the incomplete beta function \f[\frac{1}{B(p, q)}
 * \int_0^x t^{p - 1} (1 - t)^{q - 1} dt,\f] where \f[B(p, q) =
 * \frac{\Gamma(p) \Gamma(q)}{\Gamma(p + q)}\f] is the beta function
 * \cite fisz-1969, p. 167. The accuracy of the result should not be
 * less than \f$10^{-8}\f$.
 *
 * \exception std::invalid_argument if \f$p \leq 0\f$, \f$q \leq 0\f$,
 * \f$x < 0\f$ or \f$x > 1\f$
 *
 * \sa betain(double x, double p, double q, double beta)
 */
double betain(double x, double p, double q);

/**
 * The cumulative distribution function of the \f$F\f$ distribution.
 *
 * The Fisher-Snedecor \f$F\f$ distribution with \f$m > 0\f$ degrees
 * of freedom in numerator and \f$n > 0\f$ degrees of freedom in
 * denominator is a distribution of a random variable \f$\frac{Y/ m}{Z
 * / n}\f$, where \f$Y\f$ and \f$Z\f$ are independent random variables
 * with \f$\chi^2(m)\f$ and \f$\chi^2(n)\f$ distributions,
 * respectively. The cumulative distribution function of this
 * distribution is \cite bartoszewicz-1989, p. 43: \f[ F(x; m, n) =
 * \frac{m^{m / 2}n^{n / 2} \Gamma((m + n) / 2)} {\Gamma(m / 2)
 * \Gamma(n / 2)} \int_0^x \frac{t^{m / 2 - 1}}{(mt + n)^{(m + n) /
 * 2}} dt \f] for \f$x > 0\f$ and \f$F(x; m, n) = 0\f$ for \f$x \leq
 * 0\f$.
 *
 * For given \f$m \geq 1\f$, \f$n \geq 1\f$ and \f$x \in
 * \mathbf{R}\f$, the function returns \f$F(x; m, n)\f$.
 *
 * \exception std::invalid_argument if \f$m < 1\f$ or \f$n < 1\f$
 */
double cdffdist(int m, int n, double x);

/**
 * Digamma function.
 *
 * Calculates logarithmic derivative of the gamma function: \f[\psi(x)
 * = \frac{d}{dx} \ln \Gamma(x) = \frac{\Gamma'(x)}{\Gamma(x)}.\f]
 *
 * \exception std::invalid_argument if \f$x \leq 0\f$
 *
 * Source: \cite bernardo-1976.
 *
 * \note The Fortran code for digamma function could be found at <a
 * href = "http://lib.stat.cmu.edu/apstat/103">StatLib - Applied
 * Statistics algorithms (algorithm 103)</a> at the time of writing.
 * See also <a href =
 * "http://www.uv.es/~bernardo/1976AppStatist.pdf">Algorithm AS 103.
 * Psi (Digamma) Function.</a>
 *
 * \note There are more possibilities to implement this function:
 * - algorithm 610 at <a href =
 *   "http://www.netlib.org/toms/index.html">ACM Collected
 *   Algorithms</a>
 * - functions \c psi.f, \c dpsi.f and \c cpsi.f at <a href =
 *   "http://www.netlib.org/fn/">fn library</a>
 * - functions \c psi and \c psitst at <a href =
 *   "http://www.netlib.org/specfun/">specfun library</a>
 * - Boost implementation of Digamma
 */
float digamma(float x);

/** \} */  // end of group specfunc

}  // namespace SHG

#endif
