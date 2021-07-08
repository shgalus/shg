/**
 * \file src/binom.cc
 * Binomial coefficients.
 */

#include <shg/binom.h>

namespace SHG {

template <>
boost::multiprecision::cpp_int binom(
     boost::multiprecision::cpp_int n,
     boost::multiprecision::cpp_int k) {
     if (n < 0 || k < 0 || k > n)
          throw std::invalid_argument(__func__);
     if (k > n - k)
          k = n - k;
     boost::multiprecision::cpp_int c = 1;
     for (boost::multiprecision::cpp_int i = 1; i <= k; i++, n--)
          c = c * n / i;
     return c;
}

}  // namespace SHG
