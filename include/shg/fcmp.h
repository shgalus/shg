/**
 * \file include/shg/fcmp.h
 * Floating point comparisons.
 */

#ifndef SHG_FCMP_H
#define SHG_FCMP_H

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <type_traits>

namespace SHG {

/**
 * Absolutely compares two floating-point numbers.
 *
 * \returns -1 if \f$u < v - \epsilon\f$
 *
 * \returns 0 if \f$|u - v| \leqslant \epsilon\f$
 *
 * \returns 1 if \f$v > u + \epsilon\f$
 *
 * \throws std::invalid_argument if \c eps is not a number from \f$[0,
 * \infty)\f$ or if the function \c std::isfinite returns false for \c
 * u or \c v
 *
 * \see fprcmp()
 */
template <typename T>
int facmp(T u, T v, T eps) {
     static_assert(std::is_floating_point<T>::value,
                   "floating-point type required");
     if (!(eps >= 0.0 && std::isfinite(u) && std::isfinite(v)))
          throw std::invalid_argument("invalid argument in facmp");
     const T d = v - u;
     return d > eps ? -1 : (-d > eps ? 1 : 0);
}

// clang-format off
/**
 * Relatively compares two floating-point numbers.
 *
 * \returns -1 if \f$u < v - \epsilon \max(|u|, |v|) \quad (u \text {
 * is definitely less than } v)\f$
 *
 * \returns 0 if \f$|u - v| \leqslant \epsilon \max(|u|, |v|) \quad (u
 * \text { is approximately equal to } v)\f$
 *
 * \returns 1 if \f$u > v + \epsilon \max(|u|, |v|) \quad (u \text {
 * is definitely greater than } v)\f$
 *
 * \throws std::invalid_argument if \c eps is not a number from [0, 1)
 * or if the function \c std::isfinite returns false for \c u or \c v
 *
 * \see facmp()
 *
 * \see \cite knuth-2002b, section 4.2.2, p. 250--252
 *
 * \see [What Every Computer Scientist Should Know About
 * Floating-Point Arithmetic]
 * (https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html)
 *
 * \see [Floating-point Comparison]
 * (https://www.boost.org/doc/libs/1_72_0/libs/math/doc/html/math_toolkit/float_comparison.html)
 *
 * \see [Theory behind floating point comparisons]
 * (https://www.boost.org/doc/libs/1_72_0/libs/test/doc/html/boost_test/testing_tools/extended_comparison/floating_point/
floating_points_comparison_theory.html")
 *
 */
// clang-format on
template <typename T>
int frcmp(T u, T v, T eps) {
     static_assert(std::is_floating_point<T>::value,
                   "floating-point type required");
     if (!(eps >= 0.0 && eps < 1.0) ||
         !(std::isfinite(u) && std::isfinite(v)))
          throw std::invalid_argument("invalid argument in frcmp");
     const T d = v - u;
     const T max = eps * std::max(std::abs(u), std::abs(v));
     return d > max ? -1 : (-d > max ? 1 : 0);
}

/**
 * Approximately absolutely equal. \returns `facmp(u, v, eps) == 0`
 */
template <typename T>
inline bool faeq(T u, T v, T eps) {
     return facmp(u, v, eps) == 0;
}

/**
 * Approximately absolutely not equal. \returns `facmp(u, v, eps) !=
 * 0`
 */
template <typename T>
inline bool fane(T u, T v, T eps) {
     return facmp(u, v, eps) != 0;
}

/**
 * Approximately absolutely less than. \returns `facmp(u, v, eps) < 0`
 */
template <typename T>
inline bool falt(T u, T v, T eps) {
     return facmp(u, v, eps) < 0;
}

/**
 * Approximately absolutely less than or equal. \returns `facmp(u, v,
 * eps) <= 0`
 */
template <typename T>
inline bool fale(T u, T v, T eps) {
     return facmp(u, v, eps) <= 0;
}

/**
 * Approximately absolutely greater than. \returns `facmp(u, v, eps) >
 * 0`
 */
template <typename T>
inline bool fagt(T u, T v, T eps) {
     return facmp(u, v, eps) > 0;
}

/**
 * Approximately absolutely greater than or equal. \returns `facmp(u,
 * v, eps) >= 0`
 */
template <typename T>
inline bool fage(T u, T v, T eps) {
     return facmp(u, v, eps) >= 0;
}

/**
 * Approximately relatively equal. \returns `frcmp(u, v, eps) == 0`
 */
template <typename T>
inline bool freq(T u, T v, T eps) {
     return frcmp(u, v, eps) == 0;
}

/**
 * Approximately relatively not equal. \returns `frcmp(u, v, eps) !=
 * 0`
 */
template <typename T>
inline bool frne(T u, T v, T eps) {
     return frcmp(u, v, eps) != 0;
}

/**
 * Approximately relatively less than. \returns `frcmp(u, v, eps) < 0`
 */
template <typename T>
inline bool frlt(T u, T v, T eps) {
     return frcmp(u, v, eps) < 0;
}

/**
 * Approximately relatively less than or equal. \returns `frcmp(u, v,
 * eps) <= 0`
 */
template <typename T>
inline bool frle(T u, T v, T eps) {
     return frcmp(u, v, eps) <= 0;
}

/**
 * Approximately relatively greater than. \returns `frcmp(u, v, eps) >
 * 0`
 */
template <typename T>
inline bool frgt(T u, T v, T eps) {
     return frcmp(u, v, eps) > 0;
}

/**
 * Approximately relatively greater than or equal. \returns `frcmp(u,
 * v, eps) >= 0`
 */
template <typename T>
inline bool frge(T u, T v, T eps) {
     return frcmp(u, v, eps) >= 0;
}

}  // namespace SHG

#endif
