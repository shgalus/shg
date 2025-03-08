/**
 * \file include/shg/safeint.h
 * Safe integer arithmetic operations.
 */

#ifndef SHG_SAFEINT_H
#define SHG_SAFEINT_H

#include <limits>
#include <type_traits>

namespace SHG {

/**
 * \defgroup safe_int_arith Safe integer arithmetic operations.
 *
 * Safe integer arithmetic operations. Functions to be used in simple
 * cases when the machinery of Boost Safe numerics library is
 * undesirable.
 *
 * \todo Doxygen does not handle properly functions with the same
 * name but different requirements.
 *
 * \{
 */

template <typename T>
concept Integer = std::is_integral<T>::value;
template <typename T>
concept Signed_integer = Integer<T> && std::is_signed<T>::value;
template <typename T>
concept Unsigned_integer = Integer<T> && !Signed_integer<T>;

/**
 * Returns true iff sum is allowed in type T.
 */
template <typename T>
constexpr bool is_add_safe(T const& x, T const& y)
     requires(Unsigned_integer<T>);

/**
 * Returns true iff difference is allowed in type T.
 */
template <typename T>
constexpr bool is_sub_safe(T const& x, T const& y)
     requires(Unsigned_integer<T>);

/**
 * Returns true iff product is allowed in type T.
 */
template <typename T>
constexpr bool is_mul_safe(T const& x, T const& y)
     requires(Unsigned_integer<T>);

/**
 * Returns true iff quotient is allowed in type T.
 */
template <typename T>
constexpr bool is_div_safe(T const& x, T const& y)
     requires(Unsigned_integer<T>);

/**
 * Returns true iff sum is allowed in type T.
 */
template <typename T>
constexpr bool is_add_safe(T const& x, T const& y)
     requires(Signed_integer<T>);

/**
 * Returns true iff difference is allowed in type T.
 */
template <typename T>
constexpr bool is_sub_safe(T const& x, T const& y)
     requires(Signed_integer<T>);

/**
 * Returns true iff product is allowed in type T.
 */
template <typename T>
constexpr bool is_mul_safe(T const& x, T const& y)
     requires(Signed_integer<T>);

/**
 * Returns true iff quotient is allowed in type T.
 */
template <typename T>
constexpr bool is_div_safe(T const& x, T const& y)
     requires(Signed_integer<T>);

/** \} */ /* end of group safe_int_arith */

}  // namespace SHG

#include <shg/safeint-inl.h>

#endif
