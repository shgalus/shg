#include <shg/mzt.h>
#include <sstream>
#include <shg/except.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(mzt_test)

using SHG::MZT;
using SHG::Vecint;
using SHG::Vecdouble;
using SHG::faeq;
using SHG::Invalid_argument;
using std::nextafter;

// Test from Wieczorkowski, Zielinski, page 40.
BOOST_AUTO_TEST_CASE(wiecz_ziel_test) {
     using std::floor;
     using std::fmod;
     using std::pow;
     // clang-format off
     Vecint const wz{
          6,   3,  11,   3,   0,   4,   0,
         13,   8,  15,  11,  11,  14,   0,
          6,  15,   0,   2,   3,  11,   0,
          5,  14,   2,  14,   4,   8,   0,
          7,  15,   7,  10,  12,   2,   0
     };
     // clang-format on
     MZT g;
     int k = 0;
     for (int i = 1; i <= 20005; i++) {
          double const x = g();
          if (i > 20000) {
               for (int j = 1; j <= 7; j++)
                    BOOST_CHECK(fmod(floor(pow(16.0, j) * x), 16.0) ==
                                wz[k++]);
          }
     }
}

BOOST_AUTO_TEST_CASE(write_read_test) {
     using std::stringstream;

     MZT g;
     stringstream ss(bininpout);
     g.write(ss);
     BOOST_CHECK(!ss.fail());
     MZT g1(1, 2, 3, 4);
     g1.read(ss);
     BOOST_CHECK(!ss.fail());
     for (int i = 0; i < 100000; i++)
          BOOST_CHECK(g.uni(100) == g1.uni(100));
}

BOOST_AUTO_TEST_CASE(uni_test) {
     MZT g;
     Vecint const res1{1, 9, 8, 4, 4, 5, 9, 2, 5, 6};
     Vecint const res2{9, 16, 7, 3, 4, 4, 15, 13, 13, 13};
     for (int i = 0; i < 10; i++)
          BOOST_CHECK(g.uni(10) == res1[i]);
     for (int i = 0; i < 10; i++)
          BOOST_CHECK(g.uni(3, 17) == res2[i]);
     BOOST_CHECK_THROW(g.uni(0), Invalid_argument);
     BOOST_CHECK_THROW(g.uni(-1), Invalid_argument);
     BOOST_CHECK_THROW(g.uni(1, 1), Invalid_argument);
     BOOST_CHECK_THROW(g.uni(1, 0), Invalid_argument);
}

BOOST_AUTO_TEST_CASE(random_sample_test) {
     MZT g;
     Vecint x;
     for (int k = 0; k < 20; k++)
          for (int N = 1; N <= 5; N++)
               for (int n = 1; n <= N; n++) {
                    g.random_sample(n, N, x);
                    for (size_t i = 0; i < x.size(); i++)
                         BOOST_CHECK(x[i] >= 0 && x[i] < N);
                    for (size_t i = 1; i < x.size(); i++)
                         BOOST_CHECK(x[i - 1] < x[i]);
               }
     BOOST_CHECK_THROW(g.random_sample(0, 9, x), Invalid_argument);
     BOOST_CHECK_THROW(g.random_sample(-1, 9, x), Invalid_argument);
     BOOST_CHECK_THROW(g.random_sample(10, 9, x), Invalid_argument);
}

BOOST_AUTO_TEST_CASE(logarithmic_test) {
     MZT g;
     unsigned long s = 0;
     for (int i = 0; i < 100; i++)
          s += g.logarithmic(0.5);
     BOOST_CHECK(s == 131);
     double const a = nextafter(0.0, -1.0);
     double const b = nextafter(1.0, 2.0);
     BOOST_CHECK_THROW(g.logarithmic(a), Invalid_argument);
     BOOST_CHECK_THROW(g.logarithmic(0.0), Invalid_argument);
     BOOST_CHECK_THROW(g.logarithmic(1.0), Invalid_argument);
     BOOST_CHECK_THROW(g.logarithmic(b), Invalid_argument);
}

BOOST_AUTO_TEST_CASE(geometric_test) {
     MZT g;
     Vecdouble const p{0.0001, 0.001, 0.01,  0.1,   0.5,
                       0.9,    0.99,  0.999, 0.9999};
     unsigned int s = 0;
     for (std::size_t i = 0; i < p.size(); i++) {
          double const pi = p[i];
          for (int j = 0; j < 1000; j++)
               s += g.geometric(pi);
     }
     BOOST_CHECK(s == 11278386u);

     double d = 0.0;
     for (size_t i = 0; i < 1000; i++)
          BOOST_CHECK_NO_THROW(d += g.geometric(1e-8));
     BOOST_CHECK(faeq(d, 100262203685.0, 1e-20));

     double const b = nextafter(1.0, 2.0);
     BOOST_CHECK_THROW(g.geometric(0.0), Invalid_argument);
     BOOST_CHECK_NO_THROW(g.geometric(1.0));
     BOOST_CHECK_THROW(g.geometric(b), Invalid_argument);
}

BOOST_AUTO_TEST_CASE(gamma_test) {
     MZT g;
     for (int i = 0; i < 1000; i++) {
          double const gam = g.gamma(1000, 22);
          BOOST_CHECK(std::isfinite(gam));
     }
     BOOST_CHECK_THROW(g.gamma(0.0, 1.0), Invalid_argument);
     BOOST_CHECK_THROW(g.gamma(1.0, 0.0), Invalid_argument);
}

BOOST_AUTO_TEST_CASE(beta_test) {
     MZT g;
     for (int i = 0; i < 1000; i++) {
          double const beta = g.beta(1e10, 1e10);
          BOOST_CHECK(std::isfinite(beta));
     }
     BOOST_CHECK_THROW(g.beta(0.0, 1.0), Invalid_argument);
     BOOST_CHECK_THROW(g.beta(1.0, 0.0), Invalid_argument);
}

BOOST_AUTO_TEST_CASE(binomial_test) {
     MZT g;
     double p;
     unsigned int n;

     p = 0.0;
     for (n = 0; n < 1000; n++)
          BOOST_CHECK(g.binomial(p, n) == 0);
     p = 1.0;
     for (n = 0; n < 1000; n++)
          BOOST_CHECK(g.binomial(p, n) == n);
     p = 0.5;
     for (n = 0; n < 10000; n++)
          BOOST_CHECK(g.binomial(p, n) <= n);

     double const a = nextafter(0.0, -1.0);
     double const b = nextafter(1.0, 2.0);
     BOOST_CHECK_THROW(g.binomial(a, 0), Invalid_argument);
     BOOST_CHECK_THROW(g.binomial(b, 0), Invalid_argument);
}

BOOST_AUTO_TEST_CASE(poisson_test) {
     Vecdouble const mu{0.01, 0.5,  1.0,   5.0,
                        10.0, 50.0, 100.0, 1000.0};
     MZT g;
     unsigned int s = 0;
     for (std::size_t i = 0; i < mu.size(); i++) {
          double const mui = mu[i];
          for (int j = 0; j < 1000; j++)
               s += g.poisson(mui);
     }
     BOOST_CHECK(s == 1166264u);
     BOOST_CHECK_THROW(g.poisson(0.0), Invalid_argument);
}

BOOST_AUTO_TEST_CASE(negative_binomial_test) {
     Vecdouble const t{0.001, 0.01, 0.1,   0.5,
                       1.0,   10.0, 100.0, 1000.0};
     Vecdouble const p{0.0001, 0.001, 0.01,  0.1,   0.5,
                       0.9,    0.99,  0.999, 0.9999};

     MZT g;
     unsigned int s = 0;
     for (std::size_t i = 0; i < t.size(); i++) {
          double const ti = t[i];
          for (std::size_t j = 0; j < p.size(); j++) {
               double const pj = p[j];
               for (int k = 0; k < 1000; k++)
                    s += g.negative_binomial(ti, pj);
          }
     }
     BOOST_CHECK(s == 3760982173u);
     BOOST_CHECK_THROW(g.negative_binomial(0.0, 0.5),
                       Invalid_argument);
     BOOST_CHECK_THROW(g.negative_binomial(1.0, 0.0),
                       Invalid_argument);
     BOOST_CHECK_THROW(g.negative_binomial(1.0, 1.0),
                       Invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
