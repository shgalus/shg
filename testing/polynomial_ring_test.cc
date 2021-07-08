#include <shg/polynomial_ring.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(polynomial_ring_test)

using namespace SHG::ALGEBRA;
using std::invalid_argument;

template <typename T>
std::string to_string(T const& x) {
     std::ostringstream oss;
     oss << x;
     if (!oss)
          throw invalid_argument(__func__);
     return oss.str();
}

BOOST_AUTO_TEST_CASE(basic_test) {
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

}  // namespace SHG::Testing
