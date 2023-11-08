#include <shg/binom.h>
#include <boost/math/special_functions/binomial.hpp>
#include <shg/utils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(binom_test)

using SHG::binom;
using SHG::narrow_cast;

/**
 * Returns \f$\binom{n}{k}\f$.
 */
template <typename T>
T boost_binom(T n, T k) {
     using boost::math::binomial_coefficient;
     return narrow_cast<T, double>(
          binomial_coefficient<double>(n, k));
}

BOOST_DATA_TEST_CASE(binom_basic_test,
                     bdata::xrange(11) * bdata::xrange(11), xr1,
                     xr2) {
     {
          unsigned const n = xr1;
          unsigned const k = xr2;

          if (k > n)
               BOOST_CHECK_THROW(binom(n, k), std::invalid_argument);
          else
               BOOST_CHECK(binom(n, k) == boost_binom(n, k));
     }
     {
          int const n = xr1;
          int const k = xr2;

          if (k > n)
               BOOST_CHECK_THROW(binom(n, k), std::invalid_argument);
          else
               BOOST_CHECK(binom(n, k) == boost_binom(n, k));
     }
     {
          int const n = xr1 - 5;
          int const k = xr2 - 5;

          if (n < 0 || k < 0 || k > n)
               BOOST_CHECK_THROW(binom(n, k), std::invalid_argument);
          else
               BOOST_CHECK(binom(n, k) == boost_binom(n, k));
     }
}

BOOST_DATA_TEST_CASE(binom_overflow_test, bdata::xrange(11), xr) {
     {
          unsigned const n = std::numeric_limits<unsigned>::max();
          unsigned const k = xr + 2;

          BOOST_CHECK_THROW(binom(n, k), std::overflow_error);
     }
     {
          int const n = std::numeric_limits<int>::max();
          int const k = xr + 2;

          BOOST_CHECK_THROW(binom(n, k), std::overflow_error);
     }
}

BOOST_AUTO_TEST_CASE(multiprecision_test) {
     namespace mp = boost::multiprecision;
     mp::cpp_int const n = 100;
     mp::cpp_int k;
     mp::cpp_int s = 0;

     for (k = 0; k <= n; k++)
          s += binom(n, k);
     BOOST_CHECK(s == mp::pow(mp::cpp_int(2), 100));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
