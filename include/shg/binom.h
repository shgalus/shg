/**
 * \file include/shg/binom.h
 * Binomial coefficients.
 */

#ifndef SHG_BINOM_H
#define SHG_BINOM_H

#include <limits>
#include <stdexcept>
#include <type_traits>
#include <boost/multiprecision/cpp_int.hpp>

namespace SHG {

/**
 * \addtogroup miscellaneous_utilities
 *
 * \{
 */

/**
 * Calculates binomial coefficient \f$\binom{n}{k}\f$.
 *
 * \implementation
 *
 * Uses the formula \f$\binom{n}{k} = \prod_{i = 1}^k \frac{n + 1 -
 * i}{i}\f$.
 */
template <typename T>
T binom(T n, T k) {
     static_assert(std::is_integral<T>::value,
                   "T must be an integer type.");
     if constexpr (std::is_signed<T>::value) {
          if (n < 0 || k < 0)
               throw std::invalid_argument(__func__);
     }
     if (k > n)
          throw std::invalid_argument(__func__);
     if (k > n - k)
          k = n - k;
     constexpr T M = std::numeric_limits<T>::max();
     T c = 1;
     for (T i = 1; i <= k; i++, n--) {
          /*
           * If c / i > M / n, overflow will occur. If c / i == M / n,
           * overflow may or may not occur.
           *
           * For example, when calculating \binom{5}{3} with M = 9,
           * for i = 2 we have c / i = 5 / 2 = 2 and M / n = 9 / 4 =
           * 2, but the resulting c = 10 is greater than M.
           *
           * I do not investigate this further and for safety I leave
           * the weak inequality.
           */
          if (c / i >= M / n)
               throw std::overflow_error(__func__);
          c = c / i * n + c % i * n / i;
     }
     return c;
}

template <>
boost::multiprecision::cpp_int binom(
     boost::multiprecision::cpp_int n,
     boost::multiprecision::cpp_int k);

/** \} */

}  // namespace SHG

#endif
