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
 * \defgroup floating_point_comparisons Floating point comparisons
 *
 * Floating point comparisons
 *
 * \{
 */

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
int facmp(T u, T v, T eps);

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
int frcmp(T u, T v, T eps);

/**
 * Approximately absolutely equal. \returns `facmp(u, v, eps) == 0`
 */
template <typename T>
bool faeq(T u, T v, T eps);

/**
 * Approximately absolutely not equal. \returns `facmp(u, v, eps) !=
 * 0`
 */
template <typename T>
bool fane(T u, T v, T eps);

/**
 * Approximately absolutely less than. \returns `facmp(u, v, eps) < 0`
 */
template <typename T>
bool falt(T u, T v, T eps);

/**
 * Approximately absolutely less than or equal. \returns `facmp(u, v,
 * eps) <= 0`
 */
template <typename T>
bool fale(T u, T v, T eps);

/**
 * Approximately absolutely greater than. \returns `facmp(u, v, eps) >
 * 0`
 */
template <typename T>
bool fagt(T u, T v, T eps);

/**
 * Approximately absolutely greater than or equal. \returns `facmp(u,
 * v, eps) >= 0`
 */
template <typename T>
bool fage(T u, T v, T eps);

/**
 * Approximately relatively equal. \returns `frcmp(u, v, eps) == 0`
 */
template <typename T>
bool freq(T u, T v, T eps);

/**
 * Approximately relatively not equal. \returns `frcmp(u, v, eps) !=
 * 0`
 */
template <typename T>
bool frne(T u, T v, T eps);

/**
 * Approximately relatively less than. \returns `frcmp(u, v, eps) < 0`
 */
template <typename T>
bool frlt(T u, T v, T eps);

/**
 * Approximately relatively less than or equal. \returns `frcmp(u, v,
 * eps) <= 0`
 */
template <typename T>
bool frle(T u, T v, T eps);

/**
 * Approximately relatively greater than. \returns `frcmp(u, v, eps) >
 * 0`
 */
template <typename T>
bool frgt(T u, T v, T eps);

/**
 * Approximately relatively greater than or equal. \returns `frcmp(u,
 * v, eps) >= 0`
 */
template <typename T>
bool frge(T u, T v, T eps);

/** \} */ /* end of group floating_point_comparisons */

}  // namespace SHG

#include <shg/fcmp-inl.h>

#endif
