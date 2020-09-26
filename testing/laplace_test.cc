#include "shg/laplace.h"
#include "shg/mzt.h"
#include "shg/utils.h"
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(laplace_test)

namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(distribution_test,
                     bdata::xrange(-100, 101) *
                          bdata::xrange(1, 101) *
                          bdata::xrange(1, 10),
                     xr1, xr2, xr3) {
     const double mu = xr1 / 10.0;
     const double lambda = xr2 / 10.0;
     const double p = xr3 / 10.0;
     Laplace_distribution d(mu, lambda);
     BOOST_CHECK(faeq(p, d.cdf(d.invcdf(p)), 2e-15));
}

BOOST_DATA_TEST_CASE(median_test, bdata::xrange(99), xr1) {
     const int n = xr1 + 1;
     Vecdouble x(n);
     for (int i = 0; i < n; i++)
          x[i] = i;
     const double m =
          (n % 2 == 1) ? x[n / 2] : 0.5 * (x[n / 2 - 1] + x[n / 2]);
     BOOST_CHECK(faeq(median(x), m, 1e-15));
}

BOOST_AUTO_TEST_CASE(weighted_median_test) {
     Vecdouble w, x;

     x.resize(1);
     w.resize(1);
     x[0] = 0;
     w[0] = 1;
     BOOST_CHECK(faeq(weighted_median(x, w), x[0], 1e-15));

     x.resize(2);
     w.resize(2);
     x[0] = 0;
     x[1] = 1;
     w[0] = 1;
     w[1] = 0;
     BOOST_CHECK(faeq(weighted_median(x, w), x[0], 1e-15));
     w[0] = 0;
     w[1] = 1;
     BOOST_CHECK(faeq(weighted_median(x, w), x[1], 1e-15));

     x.resize(3);
     w.resize(3);
     x[0] = 0;
     x[1] = 1;
     x[2] = 3;
     w[0] = 1;
     w[1] = 0;
     w[2] = 0;
     BOOST_CHECK(faeq(weighted_median(x, w), x[0], 1e-15));
     w[0] = 0;
     w[1] = 1;
     w[2] = 0;
     BOOST_CHECK(faeq(weighted_median(x, w), x[1], 1e-15));
     w[0] = 0;
     w[1] = 0;
     w[2] = 1;
     BOOST_CHECK(faeq(weighted_median(x, w), x[2], 1e-15));
     w[0] = 1;
     w[1] = 1;
     w[2] = 0;
     BOOST_CHECK(
          faeq(weighted_median(x, w), 0.5 * (x[0] + x[1]), 1e-15));
     w[0] = 0;
     w[1] = 1;
     w[2] = 1;
     BOOST_CHECK(
          faeq(weighted_median(x, w), 0.5 * (x[1] + x[2]), 1e-15));
     w[0] = 1;
     w[1] = 0;
     w[2] = 1;
     BOOST_CHECK(
          faeq(weighted_median(x, w), 0.5 * (x[0] + x[2]), 1e-15));

     x.resize(4);
     w.resize(4);
     x[0] = 0;
     x[1] = 1;
     x[2] = 3;
     x[3] = 6;
     w[0] = 1;
     w[1] = 0;
     w[2] = 0;
     w[3] = 0;
     BOOST_CHECK(faeq(weighted_median(x, w), x[0], 1e-15));
     w[0] = 0;
     w[1] = 1;
     w[2] = 0;
     w[3] = 0;
     BOOST_CHECK(faeq(weighted_median(x, w), x[1], 1e-15));
     w[0] = 0;
     w[1] = 0;
     w[2] = 1;
     w[3] = 0;
     BOOST_CHECK(faeq(weighted_median(x, w), x[2], 1e-15));
     w[0] = 0;
     w[1] = 0;
     w[2] = 0;
     w[3] = 1;
     BOOST_CHECK(faeq(weighted_median(x, w), x[3], 1e-15));
     w[0] = 1;
     w[1] = 1;
     w[2] = 0;
     w[3] = 0;
     BOOST_CHECK(
          faeq(weighted_median(x, w), 0.5 * (x[0] + x[1]), 1e-15));
     w[0] = 1;
     w[1] = 0;
     w[2] = 1;
     w[3] = 0;
     BOOST_CHECK(
          faeq(weighted_median(x, w), 0.5 * (x[0] + x[2]), 1e-15));
     w[0] = 1;
     w[1] = 0;
     w[2] = 0;
     w[3] = 1;
     BOOST_CHECK(
          faeq(weighted_median(x, w), 0.5 * (x[0] + x[3]), 1e-15));
     w[0] = 0;
     w[1] = 1;
     w[2] = 1;
     w[3] = 0;
     BOOST_CHECK(
          faeq(weighted_median(x, w), 0.5 * (x[1] + x[2]), 1e-15));
     w[0] = 0;
     w[1] = 1;
     w[2] = 0;
     w[3] = 1;
     BOOST_CHECK(
          faeq(weighted_median(x, w), 0.5 * (x[1] + x[3]), 1e-15));
     w[0] = 0;
     w[1] = 0;
     w[2] = 1;
     w[3] = 1;
     BOOST_CHECK(
          faeq(weighted_median(x, w), 0.5 * (x[2] + x[3]), 1e-15));
     w[0] = 1;
     w[1] = 1;
     w[2] = 1;
     w[3] = 0;
     BOOST_CHECK(faeq(weighted_median(x, w), x[1], 1e-15));
     w[0] = 1;
     w[1] = 1;
     w[2] = 0;
     w[3] = 1;
     BOOST_CHECK(faeq(weighted_median(x, w), x[1], 1e-15));
     w[0] = 1;
     w[1] = 0;
     w[2] = 1;
     w[3] = 1;
     BOOST_CHECK(faeq(weighted_median(x, w), x[2], 1e-15));
     w[0] = 0;
     w[1] = 1;
     w[2] = 1;
     w[3] = 1;
     BOOST_CHECK(faeq(weighted_median(x, w), x[2], 1e-15));
     w[0] = 1;
     w[1] = 1;
     w[2] = 1;
     w[3] = 1;
     BOOST_CHECK(
          faeq(weighted_median(x, w), 0.5 * (x[1] + x[2]), 1e-15));

     w = 0;
     BOOST_CHECK_THROW(weighted_median(x, w), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(mixture_test) {
     Vecdouble w(3), mu(3), lambda(3);

     w(0) = 1.0 / 2.0;
     mu(0) = -1.0;
     lambda(0) = 1.0;
     w(1) = 1.0 / 3.0;
     mu(1) = 0.0;
     lambda(1) = 2.0;
     w(2) = 1.0 / 6.0;
     mu(2) = 2.0;
     lambda(2) = 1.0;

     // Moments about zero of this mixture:
     const double m1 = -1.0 / 6.0;
     // const double m2 = 31.0 / 6.0;
     // const double m3 = -1.0 / 6.0;
     // const double m4 = 967.0 / 6.0;

     // Central moments of this mixture:
     const double nu2 = 185.0 / 36.0;  // variance
     const double nu3 = 520.0 / 216.0;
     const double nu4 = 209841.0 / 1296.0;

     const double sdev = std::sqrt(nu2);
     const double skew = nu3 / nu2 / sdev;
     const double curt = nu4 / sqr(nu2) - 3.0;

     Laplace_mixture m(w, mu, lambda);

     BOOST_CHECK(faeq(m1, m.mean(), 1e-14));
     BOOST_CHECK(faeq(sdev, m.sdev(), 1e-14));
     BOOST_CHECK(faeq(skew, m.skew(), 1e-14));
     BOOST_CHECK(faeq(curt, m.curt(), 1e-14));

     // Test generate().
     Vecdouble x;
     MZT mzt;
     m.generate(mzt, 4000, x);

     // Test EM algorithm.
     Unilapmixmod u(x, 3);
     u.pi = w;
     u.mu = mu;
     u.lambda = lambda;
     u.estep();
     u.mstep();
     int iter = 0;
     for (;;) {
          iter++;
          if (std::abs(u.estep()) < 1e-7)
               break;
          u.mstep();
     }
     BOOST_CHECK(iter == 347);

     // Results should be:
     Vecdouble w0(3), mu0(3), lambda0(3);
     w0(0) = 0.46334203;
     mu0(0) = -1.07436524;
     lambda0(0) = 0.92922847;
     w0(1) = 0.38795240;
     mu0(1) = 0.24140911;
     lambda0(1) = 2.01801607;
     w0(2) = 0.14870557;
     mu0(2) = 2.00744725;
     lambda0(2) = 1.00035131;

     for (int i = 0; i < 3; i++) {
          BOOST_CHECK(faeq(u.pi(i), w0(i), 1e-8));
          BOOST_CHECK(faeq(u.mu(i), mu0(i), 1e-8));
          BOOST_CHECK(faeq(u.lambda(i), lambda0(i), 1e-8));
     }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
