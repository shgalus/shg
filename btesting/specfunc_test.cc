#include "shg/specfunc.h"
#include <boost/math/special_functions/beta.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

namespace SHG::BTesting {

BOOST_AUTO_TEST_SUITE(specfunc_test)

namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(gammad_test,
                     bdata::xrange(101) * bdata::xrange(100), xr1,
                     xr2) {
     using boost::math::gamma_p;
     const double x = xr1 * 0.1;
     const double p = (xr2 + 1) * 0.1;
     const double y = gammad(x, p);
     const double z = gamma_p(p, x);
     BOOST_CHECK(std::abs(y - z) < 1e-8);
}

BOOST_DATA_TEST_CASE(betain_test,
                     bdata::xrange(9) * bdata::xrange(10) *
                          bdata::xrange(10),
                     xr1, xr2, xr3) {
     using boost::math::ibeta;
     const double x = xr1 * 0.1;
     const double p = xr2 + 1;
     const double q = xr3 + 1;
     const double y = betain(x, p, q);
     const double z = ibeta(p, q, x);
     BOOST_CHECK(std::abs(y - z) < 1e-10);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting
