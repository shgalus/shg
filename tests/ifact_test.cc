#include <shg/ifact.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <shg/utils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(ifact_test)

/**
 * Returns true if and only if \c n is a prime number.
 *
 * \implementation Each integer number can be expressed as \f$6k +
 * i\f$ for certain integer \f$k\f$ and \f$i \in \{-1, 0, 1, 2, 3, 4\}
 * \f$. \f$6k + 0, 6k + 2, 6k + 3, 6k + 4\f$ are not prime and \f$6k -
 * 1, 6k + 1\f$ may be prime. The algorith checks if \f$n\f$ is
 * divisible by the numbers of the form \f$6k - 1, 6k + 1\f$.
 */
bool test_is_prime(int n) {
     if (n <= 3)
          return n > 1;
     if (n % 2 == 0 || n % 3 == 0)
          return false;
     for (int i = 5; i * i <= n; i += 6)
          if (n % i == 0 || n % (i + 2) == 0)
               return false;
     return true;
}

BOOST_DATA_TEST_CASE(is_prime_test, bdata::xrange(100), xr) {
     using SHG::is_prime;

     int const n = xr;
     BOOST_CHECK(is_prime(n) == test_is_prime(n));
}

BOOST_AUTO_TEST_CASE(is_prime_cpp_int_test) {
     using SHG::is_prime;
     using boost::multiprecision::cpp_int;
     using boost::multiprecision::pow;

     cpp_int const two = 2;
     // Two Mersenne numbers. See \cite marzantowicz-zarzycki-2012,
     // page 52.
     cpp_int const M_23 = pow(two, 23) - 1;
     cpp_int const M_31 = pow(two, 31) - 1;
     BOOST_CHECK(!is_prime(M_23));
     BOOST_CHECK(is_prime(M_31));
}

BOOST_AUTO_TEST_CASE(integer_factorization_test) {
     using SHG::Integer_factorization;
     using SHG::ipower;
     Integer_factorization<unsigned> f;
     BOOST_CHECK(f.repr().size() == 0);
     f.factorize(0);
     BOOST_CHECK(f.repr().size() == 0);
     f.factorize(1);
     BOOST_CHECK(f.repr().size() == 0);
     for (unsigned n = 2; n <= 100; n++) {
          f.factorize(n);
          auto const r = f.repr();
          unsigned n1 = 1;
          for (unsigned i = 0; i < r.size(); i++) {
               if (i > 0)
                    BOOST_CHECK(r[i].p > r[i - 1].p);
               BOOST_CHECK(r[i].n > 0);
               n1 *= ipower(r[i].p, r[i].n);
          }
          BOOST_CHECK(n1 == n);
     }
}

BOOST_AUTO_TEST_CASE(integer_factorization_cpp_int_test) {
     using SHG::Integer_factorization;
     using boost::multiprecision::cpp_int;
     using boost::multiprecision::pow;
     cpp_int const t[]{2, 3, 5, 7, 7883, 7901, 7907, 7919};

     Integer_factorization<cpp_int> f;
     cpp_int n{1};
     for (unsigned i = 0; i < std::size(t); i++)
          n *= pow(t[i], i + 1);
     f.factorize(n);
     auto const r{f.repr()};
     BOOST_REQUIRE(r.size() == std::size(t));
     for (std::size_t i = 0; i < std::size(t); i++)
          BOOST_CHECK(r[i].p == t[i] && r[i].n == i + 1);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
