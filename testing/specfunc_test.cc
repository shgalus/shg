#include <shg/specfunc.h>
#include <boost/math/distributions/fisher_f.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/students_t.hpp>
#include <boost/math/special_functions/beta.hpp>
#include <boost/math/special_functions/digamma.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <shg/mconsts.h>
#include <shg/utils.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(specfunc_test)

BOOST_AUTO_TEST_CASE(normal_integral_at_zero_test) {
     using boost::math::normal_distribution;
     const double y = normal_integral(0.0);
     BOOST_CHECK(faeq(y, 0.5, 1e-12));
}

namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(normal_integral_test, bdata::xrange(61), xr1) {
     using boost::math::normal_distribution;
     const double x = -3.0 + xr1 * 0.1;
     const double y = normal_integral(x);
     normal_distribution normal;
     const double z = cdf(normal, x);
     BOOST_CHECK(faeq(y, z, 1e-12));
}

BOOST_DATA_TEST_CASE(ppnd_test, bdata::xrange(199), xr1) {
     using boost::math::normal_distribution;
     const int i = xr1;
     const double x = 0.005 * (i + 1);
     const double y = ppnd7(x);
     normal_distribution normal;
     const double z = quantile(normal, x);
     BOOST_CHECK(faeq(y, z, 3e-7));
}

BOOST_DATA_TEST_CASE(gammad_test,
                     bdata::xrange(101) * bdata::xrange(100), xr1,
                     xr2) {
     using boost::math::gamma_p;
     const double x = xr1 * 0.1;
     const double p = (xr2 + 1) * 0.1;
     const double y = gammad(x, p);
     const double z = gamma_p(p, x);
     BOOST_CHECK(faeq(y, z, 1e-8));
}

BOOST_DATA_TEST_CASE(probst_test,
                     bdata::xrange(18) * bdata::xrange(81), xr1,
                     xr2) {
     using boost::math::students_t_distribution;
     const int df = xr1 + 1;
     const double x = xr2 * 0.1;
     const double y = probst(x, df);
     students_t_distribution t(narrow_cast<double, int>(df));
     const double z = cdf(t, x);
     BOOST_CHECK(faeq(y, z, 1e-15));
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
     BOOST_CHECK(faeq(y, z, 1e-10));
}

BOOST_DATA_TEST_CASE(cdffdist_test,
                     bdata::xrange(10) * bdata::xrange(10) *
                          bdata::xrange(10),
                     xr1, xr2, xr3) {
     using boost::math::fisher_f_distribution;
     const int m = xr1 + 1;
     const int n = xr2 + 1;
     const double x = xr3 + 1;

     const double y = cdffdist(m, n, x);
     fisher_f_distribution f(narrow_cast<double, int>(m),
                             narrow_cast<double, int>(n));
     const double z = cdf(f, x);
     BOOST_CHECK(faeq(y, z, 3e-8));
}

BOOST_AUTO_TEST_CASE(digamma_at_zero_test) {
     BOOST_CHECK_THROW(digamma(0.0), std::invalid_argument);
}

// digamma(0.5) = -2 ln(2) - gamma
BOOST_AUTO_TEST_CASE(digamma_at_half_test) {
     using Constants::gamma;
     BOOST_CHECK(faeq<double>(
          digamma(0.5), -2.0 * std::log(2.0) - gamma<double>, 9e-7));
}

// digamma(n) = -gamma + \sum_{k = 1}^{n - 1} (1 / k) for n >= 2
BOOST_DATA_TEST_CASE(digamma_at_int_test, bdata::xrange(19), xr1) {
     using Constants::gamma;
     const int n = xr1 + 2;
     double s = 0.0;
     for (int k = n - 1; k > 0; k--)
          s += 1.0 / k;
     s -= gamma<double>;
     BOOST_CHECK(faeq<double>(digamma(n), s, 3e-7));
}

BOOST_DATA_TEST_CASE(digamma_test, bdata::xrange(100), xr1) {
     const float x = (xr1 + 1) * 0.1;
     const float y = digamma(x);
     const float z = boost::math::digamma(x);
     BOOST_CHECK(faeq(y, z, 1e-6f));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
