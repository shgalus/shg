#include "shg/normmix.h"
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(normmix_test)

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
          const double p = i * 0.0001;
          const double x = nm.invcdf(p);
          const double q = nm.cdf(x);
          BOOST_CHECK(faeq(p, q, 1e-10));
     }

     const double mean = 0.0;
     const double var = 0.65625;
     const double sdev = sqrt(var);
     const double skew = 0.0;
     const double m4 = (2.0 + 11.0 / 16.0) * 0.25 * 2 +
                       (3.0 / 4.0 / 4.0 / 4.0 / 4.0) * 0.5;
     const double curt = m4 / var / var - 3.0;
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
          const double p = i * 0.0001;
          const double x = nm.invcdf(p);
          const double q = nm.cdf(x);
          BOOST_CHECK(faeq(p, q, 1e-10));
     }

     const double mean = 0.65;
     const double var = 0.3125;
     const double sdev = sqrt(var);
     const double skew = 0.069 / var / sdev;
     const double m4 = 0.26115625;
     const double curt = m4 / var / var - 3.0;
     BOOST_CHECK(faeq(nm.mean(), mean, 1e-10));
     BOOST_CHECK(faeq(nm.sdev(), sdev, 1e-10));
     BOOST_CHECK(faeq(nm.skew(), skew, 1e-10));
     BOOST_CHECK(faeq(nm.curt(), curt, 1e-10));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
