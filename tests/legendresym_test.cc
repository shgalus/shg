#include <shg/legendresym.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <shg/utils.h>  // SHG::divide
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(legendresym_test)

BOOST_AUTO_TEST_CASE(basic_test) {
     using SHG::legendre;
     using SHG::divide;
     int q, r;

     BOOST_CHECK_THROW(legendre(0, 2), std::invalid_argument);

     for (int k = -20; k < 20; k++) {
          int const ap = legendre(k, 3);
          divide(k, 3, q, r);
          switch (r % 3) {
          case 0:
               BOOST_CHECK(ap == 0);
               break;
          case 1:
               BOOST_CHECK(ap == 1);
               break;
          case 2:
               BOOST_CHECK(ap == -1);
               break;
          default:
               BOOST_CHECK(false);
               break;
          }
     }

     for (int k = -20; k < 20; k++) {
          int const ap = legendre(k, 5);
          divide(k, 5, q, r);
          switch (r % 5) {
          case 0:
               BOOST_CHECK(ap == 0);
               break;
          case 1:
          case 4:
               BOOST_CHECK(ap == 1);
               break;
          case 2:
          case 3:
               BOOST_CHECK(ap == -1);
               break;
          default:
               BOOST_CHECK(false);
               break;
          }
     }

     for (int k = -20; k < 20; k++) {
          int const ap = legendre(k, 7);
          divide(k, 7, q, r);
          switch (r % 7) {
          case 0:
               BOOST_CHECK(ap == 0);
               break;
          case 1:
          case 2:
          case 4:
               BOOST_CHECK(ap == 1);
               break;
          case 3:
          case 5:
          case 6:
               BOOST_CHECK(ap == -1);
               break;
          default:
               BOOST_CHECK(false);
               break;
          }
     }
}

BOOST_AUTO_TEST_CASE(multiplication_test) {
     using SHG::legendre;
     int const tab[] = {3, 5, 7, 11, 13, 17};
     int const N = 20;

     for (std::size_t i = 0; i < std::size(tab); i++) {
          int const p = tab[i];
          for (int a = -N; a <= N; a++)
               for (int b = -N; b <= N; b++)
                    BOOST_CHECK(legendre(a * b, p) ==
                                legendre(a, p) * legendre(b, p));
     }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
