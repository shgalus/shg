#include "shg/runs.h"
#include <algorithm>
#include <cmath>
#include <utility>
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

namespace SHG::BTesting {

BOOST_AUTO_TEST_SUITE(runs_test)

namespace {

// Newton symbol.
double newton(int n, int k) {
     BOOST_CHECK(n >= 0 && k >= 0);
     if (n < k)
          return 0.0;
     int nk = n - k;
     if (k > nk)
          std::swap(k, nk);
     double c = 1.0;
     for (int i = 1; i <= k; i++)
          c *= static_cast<double>(nk + i) / static_cast<double>(i);
     return c;
}

// Returns P(K = k | N1 = n1, N2 = n2).
double pdf(int n1, int n2, int k) {
     if (k % 2 == 0) {
          const int l = k / 2;
          return 2.0 * newton(n1 - 1, l - 1) * newton(n2 - 1, l - 1) /
                 newton(n1 + n2, n1);
     } else {
          const int l = (k - 1) / 2;
          const double s1 = newton(n1 - 1, l - 1) * newton(n2 - 1, l);
          const double s2 = newton(n1 - 1, l) * newton(n2 - 1, l - 1);
          const double s3 = newton(n1 + n2, n1);
          return (s1 + s2) / s3;
     }
}

// Returns P(K <= k | N1 = n1, N2 = n2).
double cdf(int n1, int n2, int k) {
     double p = 0.0;
     for (int i = 2; i <= k; i++)
          p += pdf(n1, n2, i);
     return p;
}

}  // anonymous namespace

BOOST_AUTO_TEST_CASE(basic_test) {
     using std::abs;
     using std::min;
     using std::size_t;
     double p1, p2;
     size_t n1, n2, k;

     n1 = n2 = k = 0;
     runs(n1, n2, k, true, p1, p2);
     BOOST_CHECK(abs(p1 - 1.0) < 1e-15);
     BOOST_CHECK(abs(p2 - 1.0) < 1e-15);
     for (k = 1; k <= 10; k++) {
          runs(n1, n2, k, true, p1, p2);
          BOOST_CHECK(abs(p1 - 1.0) < 1e-15);
          BOOST_CHECK(abs(p2 - 0.0) < 1e-15);
     }

     for (n2 = 1; n2 <= 20; n2++) {
          k = 0;
          runs(n1, n2, k, true, p1, p2);
          BOOST_CHECK(abs(p1 - 0.0) < 1e-15);
          BOOST_CHECK(abs(p2 - 1.0) < 1e-15);
          k = 1;
          runs(n1, n2, k, true, p1, p2);
          BOOST_CHECK(abs(p1 - 1.0) < 1e-15);
          BOOST_CHECK(abs(p2 - 1.0) < 1e-15);
          for (k = 2; k <= 10; k++) {
               runs(n1, n2, k, true, p1, p2);
               BOOST_CHECK(abs(p1 - 1.0) < 1e-15);
               BOOST_CHECK(abs(p2 - 0.0) < 1e-15);
          }
     }
     n2 = 0;
     for (n1 = 1; n1 <= 20; n1++) {
          k = 0;
          runs(n1, n2, k, true, p1, p2);
          BOOST_CHECK(abs(p1 - 0.0) < 1e-15);
          BOOST_CHECK(abs(p2 - 1.0) < 1e-15);
          k = 1;
          runs(n1, n2, k, true, p1, p2);
          BOOST_CHECK(abs(p1 - 1.0) < 1e-15);
          BOOST_CHECK(abs(p2 - 1.0) < 1e-15);
          for (k = 2; k <= 10; k++) {
               runs(n1, n2, k, true, p1, p2);
               BOOST_CHECK(abs(p1 - 1.0) < 1e-15);
               BOOST_CHECK(abs(p2 - 0.0) < 1e-15);
          }
     }
     n1 = 5;
     n2 = 6;
     for (k = 0; k < 2; k++) {
          runs(n1, n2, k, true, p1, p2);
          BOOST_CHECK(abs(p1 - 0.0) < 1e-15);
          BOOST_CHECK(abs(p2 - 1.0) < 1e-15);
     }
     size_t k1 = (n1 != n2 ? 2 * min(n1, n2) + 1 : n1 + n2) + 1;
     for (k = k1; k <= k1 + 10; k++) {
          runs(n1, n2, k, true, p1, p2);
          BOOST_CHECK(abs(p1 - 1.0) < 1e-15);
          BOOST_CHECK(abs(p2 - 0.0) < 1e-15);
     }

     // Sum up all non-vanishing probabilities P(K = k) and check if
     // their sum equals 1.
     for (n1 = 1; n1 <= 30; n1++)
          for (n2 = 1; n2 <= 30; n2++) {
               const int kmax =
                    n1 != n2 ? 2 * min(n1, n2) + 1 : n1 + n2;
               double s = 0.0;
               for (int k = 2; k <= kmax; k++) {
                    runs(n1, n2, k, true, p1, p2);
                    const double p = p1 + p2 - 1.0;
                    BOOST_CHECK(p >= 0.0);
                    BOOST_CHECK(abs(p - pdf(n1, n2, k)) < 1e-15);
                    BOOST_CHECK(abs(p1 - cdf(n1, n2, k)) < 1e-14);
                    s += p;
               }
               BOOST_CHECK(abs(s - 1.0) < 2e-15);
          }
}

BOOST_AUTO_TEST_CASE(wolf_wolfowitz_example_test) {
     using std::abs;
     double pe1, pe2, pa1, pa2;
     runs(50, 50, 34, true, pe1, pe2);
     runs(50, 50, 34, false, pa1, pa2);
     BOOST_CHECK(abs(pe1 - 0.000411735) < 5e-10);
     BOOST_CHECK(abs(pa1 - 0.000316224) < 5e-10);

     int n1 = 500, n2 = 500, k;
     double d, maxd = -1.0;
     for (k = n1 / 2; k <= 3 * (n1 / 2); k++) {
          runs(n1, n2, k, true, pe1, pe2);
          runs(n1, n2, k, false, pa1, pa2);
          d = abs(pe1 - pa1);
          if (d > maxd)
               maxd = d;
          d = abs(pe2 - pa2);
          if (d > maxd)
               maxd = d;
     }
     BOOST_CHECK(maxd > 0.012620 && maxd < 0.012621);

     BOOST_CHECK_THROW(runs(600, 600, 300, true, pe1, pe2),
                       std::range_error);
}

using test_types = boost::mpl::list<int, double>;

BOOST_AUTO_TEST_CASE_TEMPLATE(count_runs_test, T, test_types) {
     std::size_t n1, n2, k;
     count_runs<T>({}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 0 && n2 == 0 && k == 0);
     count_runs<T>({1}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 1 && n2 == 0 && k == 1);
     count_runs<T>({3}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 0 && n2 == 1 && k == 1);
     count_runs<T>({1, 1}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 2 && n2 == 0 && k == 1);
     count_runs<T>({1, 3}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 1 && n2 == 1 && k == 2);
     count_runs<T>({3, 1}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 1 && n2 == 1 && k == 2);
     count_runs<T>({3, 3}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 0 && n2 == 2 && k == 1);
     count_runs<T>({1, 1, 1}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 3 && n2 == 0 && k == 1);
     count_runs<T>({1, 1, 3}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 2 && n2 == 1 && k == 2);
     count_runs<T>({1, 3, 1}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 2 && n2 == 1 && k == 3);
     count_runs<T>({3, 1, 1}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 2 && n2 == 1 && k == 2);
     count_runs<T>({1, 3, 3}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 1 && n2 == 2 && k == 2);
     count_runs<T>({3, 1, 3}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 1 && n2 == 2 && k == 3);
     count_runs<T>({3, 3, 1}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 1 && n2 == 2 && k == 2);
     count_runs<T>({3, 3, 3}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 0 && n2 == 3 && k == 1);
     count_runs<T>({1, 1, 1, 2, 2, 1, 2}, 2, n1, n2, k);
     BOOST_CHECK(n1 == 4 && n2 == 3 && k == 4);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting
