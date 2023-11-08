#include <shg/normmix.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(normmix_test)

using SHG::Normal_mixture;
using SHG::Vecdouble;
using SHG::faeq;

BOOST_AUTO_TEST_CASE(normmix_1_test) {
     Vecdouble w(3), m(3), s(3);
     w[0] = 0.25;
     w[1] = 0.50;
     w[2] = 0.25;
     m[0] = -1.0;
     m[1] = 0.00;
     m[2] = 1.00;
     s[0] = 0.50;
     s[1] = 0.25;
     s[2] = 0.50;
     Normal_mixture nm(w, m, s);

     // Test only cdf and invcdf for p in [0.0001, 0.9999] by 0.0001.
     for (int i = 1; i <= 9999; i++) {
          double const p = i * 0.0001;
          double const x = nm.invcdf(p);
          double const q = nm.cdf(x);
          BOOST_CHECK(faeq(p, q, 1e-10));
     }

     double const mean = 0.0;
     double const var = 0.65625;
     double const sdev = sqrt(var);
     double const skew = 0.0;
     double const m4 = (2.0 + 11.0 / 16.0) * 0.25 * 2 +
                       (3.0 / 4.0 / 4.0 / 4.0 / 4.0) * 0.5;
     double const curt = m4 / var / var - 3.0;
     BOOST_CHECK(faeq(nm.mean(), mean, 1e-10));
     BOOST_CHECK(faeq(nm.sdev(), sdev, 1e-10));
     BOOST_CHECK(faeq(nm.skew(), skew, 1e-10));
     BOOST_CHECK(faeq(nm.curt(), curt, 1e-10));
}

BOOST_AUTO_TEST_CASE(normmix_2_test) {
     Vecdouble w(3), m(3), s(3);
     w[0] = 0.20;
     w[1] = 0.30;
     w[2] = 0.50;
     m[0] = 0.00;
     m[1] = 0.50;
     m[2] = 1.00;
     s[0] = 0.20;
     s[1] = 0.30;
     s[2] = 0.50;
     Normal_mixture nm(w, m, s);

     // Test only cdf and invcdf for p in [0.0001, 0.9999] by 0.0001.
     for (int i = 1; i <= 9999; i++) {
          double const p = i * 0.0001;
          double const x = nm.invcdf(p);
          double const q = nm.cdf(x);
          BOOST_CHECK(faeq(p, q, 1e-10));
     }

     double const mean = 0.65;
     double const var = 0.3125;
     double const sdev = sqrt(var);
     double const skew = 0.069 / var / sdev;
     double const m4 = 0.26115625;
     double const curt = m4 / var / var - 3.0;
     BOOST_CHECK(faeq(nm.mean(), mean, 1e-10));
     BOOST_CHECK(faeq(nm.sdev(), sdev, 1e-10));
     BOOST_CHECK(faeq(nm.skew(), skew, 1e-10));
     BOOST_CHECK(faeq(nm.curt(), curt, 1e-10));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
