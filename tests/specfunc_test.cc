#include <shg/specfunc.h>
#include <boost/math/distributions/fisher_f.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/students_t.hpp>
#include <boost/math/special_functions/beta.hpp>
#include <boost/math/special_functions/digamma.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <shg/mconsts.h>
#include <shg/utils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(specfunc_test)

using SHG::narrow_cast;
using SHG::faeq;

BOOST_AUTO_TEST_CASE(normal_integral_at_zero_test) {
     using boost::math::normal_distribution;
     double const y = SHG::normal_integral(0.0);
     BOOST_CHECK(faeq(y, 0.5, 1e-12));
}

BOOST_DATA_TEST_CASE(normal_integral_test, bdata::xrange(61), xr1) {
     using boost::math::normal_distribution;
     double const x = -3.0 + xr1 * 0.1;
     double const y = SHG::normal_integral(x);
     normal_distribution normal;
     double const z = cdf(normal, x);
     BOOST_CHECK(faeq(y, z, 1e-12));
}

BOOST_DATA_TEST_CASE(ppnd_test, bdata::xrange(199), xr1) {
     using boost::math::normal_distribution;
     int const i = xr1;
     double const x = 0.005 * (i + 1);
     double const y = SHG::ppnd7(x);
     normal_distribution normal;
     double const z = quantile(normal, x);
     BOOST_CHECK(faeq(y, z, 3e-7));
}

BOOST_DATA_TEST_CASE(gammad_test,
                     bdata::xrange(101) * bdata::xrange(100), xr1,
                     xr2) {
     using boost::math::gamma_p;
     double const x = xr1 * 0.1;
     double const p = (xr2 + 1) * 0.1;
     double const y = SHG::gammad(x, p);
     double const z = gamma_p(p, x);
     BOOST_CHECK(faeq(y, z, 1e-8));
}

BOOST_DATA_TEST_CASE(probst_test,
                     bdata::xrange(18) * bdata::xrange(81), xr1,
                     xr2) {
     using boost::math::students_t_distribution;
     int const df = xr1 + 1;
     double const x = xr2 * 0.1;
     double const y = SHG::probst(x, df);
     students_t_distribution t(narrow_cast<double, int>(df));
     double const z = cdf(t, x);
     BOOST_CHECK(faeq(y, z, 1e-15));
}

BOOST_DATA_TEST_CASE(betain_test,
                     bdata::xrange(9) * bdata::xrange(10) *
                          bdata::xrange(10),
                     xr1, xr2, xr3) {
     using boost::math::ibeta;
     double const x = xr1 * 0.1;
     double const p = xr2 + 1;
     double const q = xr3 + 1;
     double const y = SHG::betain(x, p, q);
     double const z = ibeta(p, q, x);
     BOOST_CHECK(faeq(y, z, 1e-10));
}

BOOST_DATA_TEST_CASE(cdffdist_test,
                     bdata::xrange(10) * bdata::xrange(10) *
                          bdata::xrange(10),
                     xr1, xr2, xr3) {
     using boost::math::fisher_f_distribution;
     int const m = xr1 + 1;
     int const n = xr2 + 1;
     double const x = xr3 + 1;

     double const y = SHG::cdffdist(m, n, x);
     fisher_f_distribution f(narrow_cast<double, int>(m),
                             narrow_cast<double, int>(n));
     double const z = cdf(f, x);
     BOOST_CHECK(faeq(y, z, 3e-8));
}

BOOST_AUTO_TEST_CASE(digamma_at_zero_test) {
     BOOST_CHECK_THROW(SHG::digamma(0.0), std::invalid_argument);
}

// digamma(0.5) = -2 ln(2) - gamma
BOOST_AUTO_TEST_CASE(digamma_at_half_test) {
     using SHG::Constants::gamma;
     BOOST_CHECK(faeq<double>(SHG::digamma(0.5),
                              -2.0 * std::log(2.0) - gamma<double>,
                              9e-7));
}

// digamma(n) = -gamma + \sum_{k = 1}^{n - 1} (1 / k) for n >= 2
BOOST_DATA_TEST_CASE(digamma_at_int_test, bdata::xrange(19), xr1) {
     using SHG::Constants::gamma;
     int const n = xr1 + 2;
     double s = 0.0;
     for (int k = n - 1; k > 0; k--)
          s += 1.0 / k;
     s -= gamma<double>;
     BOOST_CHECK(faeq<double>(SHG::digamma(n), s, 3e-7));
}

BOOST_DATA_TEST_CASE(digamma_test, bdata::xrange(100), xr1) {
     float const x = (xr1 + 1) * 0.1;
     float const y = SHG::digamma(x);
     float const z = boost::math::digamma(x);
     BOOST_CHECK(faeq(y, z, 1e-6f));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
