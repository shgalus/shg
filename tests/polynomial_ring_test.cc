#include <shg/polynomial_ring.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(polynomial_ring_test)

template <typename T>
std::string to_string(T const& x) {
     std::ostringstream oss;
     oss << x;
     if (!oss)
          throw std::invalid_argument(__func__);
     return oss.str();
}

BOOST_AUTO_TEST_CASE(basic_test) {
     using SHG::ALGEBRA::Polynomial_ring;
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Field_Q;
     using SHG::ALGEBRA::Element;

     Field_Q Q;
     Polynomial_ring ring(&Q);
     BOOST_CHECK(ring.field() == &Q);
     BOOST_CHECK(ring.dim() == 1);
     Polynomial p(ring.field(), ring.dim());
     p += {Q.element(1), Monomial{3}};
     p += Q.element(-1);
     BOOST_CHECK(to_string(p) == "1 2 1 1 3 -1 1 0");
     Element x = ring.element(p);
     BOOST_CHECK(ring.value(x) == p);
     Element y{x};
     BOOST_CHECK(y == x);
     Element z = pow(x, 5);
     BOOST_CHECK(to_string(z) ==
                 "1 6 1 1 15 -5 1 12 10 1 9 -10 1 6 5 1 3 -1 1 0");
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
