/**
 * \file include/shg/brent.h
 * Brent method.
 * \date Created on 28 December 2013.
 */

#ifndef SHG_BRENT_H
#define SHG_BRENT_H

#include <cmath>
#include <limits>
#include <stdexcept>

namespace SHG {

/**
 * \defgroup numerical_analysis Numerical analysis
 *
 * Numerical analysis.
 *
 * \{
 */

/**
 * Brent method for finding zeros.
 *
 * \param func function or function object
 * \param [in] x1 begin of interval
 * \param [in] x2 end of interval
 * \param [in] tol tolerance
 *
 * \return Root of the function.
 *
 * \exception std::invalid_argument if func(x1) and func(x2) have the
 * same sign
 *
 * \exception std::runtime_error if the maximum number of iterations
 * is exceeded
 *
 * \implementation
 * Adapted from \cite press-teukolsky-vetterling-flannery-2007, p.
 * 454-456.
 */
template <class T>
double brent(T& func, const double x1, const double x2,
             const double tol) {
     using std::abs;
     using std::invalid_argument;
     using std::runtime_error;
     auto sign = [](const double& a, const double& b) {
          return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);
     };
     const int maxit = 100;
     constexpr double eps = std::numeric_limits<double>::epsilon();
     // Dummy initialization of e for compiler.
     double a = x1, b = x2, c = x2, d = 0.0, e = 0.0, fa = func(a),
            fb = func(b), fc, p, q, r, s, tol1, xm;
     if ((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0))
          throw invalid_argument("root must be bracketed in brent");
     fc = fb;
     for (int it = 0; it < maxit; it++) {
          if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0)) {
               c = a;
               fc = fa;
               e = d = b - a;
          }
          if (abs(fc) < abs(fb)) {
               a = b;
               b = c;
               c = a;
               fa = fb;
               fb = fc;
               fc = fa;
          }
          tol1 = 2.0 * eps * abs(b) + 0.5 * tol;
          xm = 0.5 * (c - b);
          if (abs(xm) <= tol1 || fb == 0.0)
               return b;
          if (abs(e) >= tol1 && abs(fa) > abs(fb)) {
               s = fb / fa;
               if (a == c) {
                    p = 2.0 * xm * s;
                    q = 1.0 - s;
               } else {
                    q = fa / fc;
                    r = fb / fc;
                    p = s * (2.0 * xm * q * (q - r) -
                             (b - a) * (r - 1.0));
                    q = (q - 1.0) * (r - 1.0) * (s - 1.0);
               }
               if (p > 0.0)
                    q = -q;
               p = abs(p);
               const double min1 = 3.0 * xm * q - abs(tol1 * q);
               const double min2 = abs(e * q);
               if (2.0 * p < (min1 < min2 ? min1 : min2)) {
                    e = d;
                    d = p / q;
               } else {
                    d = xm;
                    e = d;
               }
          } else {
               d = xm;
               e = d;
          }
          a = b;
          fa = fb;
          if (abs(d) > tol1)
               b += d;
          else
               b += sign(tol1, xm);
          fb = func(b);
     }
     throw runtime_error(
          "maximum number of iterations exceeded in brent");
}

/** \} */ /* end of group numerical_analysis */

}  // namespace SHG

#endif
