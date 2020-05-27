#include "shg/mzt.h"
#include <cmath>
#include <ios>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "shg/mzt.h"

namespace SHG::BTesting {

BOOST_AUTO_TEST_SUITE(mzt_test)

namespace {

// clang-format off
constexpr int wz_t[35] {
     6,   3,  11,   3,   0,   4,   0,
    13,   8,  15,  11,  11,  14,   0,
     6,  15,   0,   2,   3,  11,   0,
     5,  14,   2,  14,   4,   8,   0,
     7,  15,   7,  10,  12,   2,   0
};
// clang-format on

}  // anonymous namespace

// Test from Wieczorkowski, Zielinski, page 40.
BOOST_AUTO_TEST_CASE(wiecz_ziel_test) {
     using std::floor;
     using std::fmod;
     using std::pow;
     MZT g;
     int k = 0;
     for (int i = 1; i <= 20005; i++) {
          const double x = g();
          if (i > 20000) {
               for (int j = 1; j <= 7; j++)
                    BOOST_CHECK(fmod(floor(pow(16.0, j) * x), 16.0) ==
                                wz_t[k++]);
          }
     }
}

BOOST_AUTO_TEST_CASE(write_read_test) {
     using std::ios_base;
     using std::stringstream;

     const ios_base::openmode mode =
          ios_base::out | ios_base::in | ios_base::binary;
     MZT g;
     stringstream ss(mode);
     g.write(ss);
     BOOST_CHECK(!ss.fail());
     MZT g1(1, 2, 3, 4);
     g1.read(ss);
     BOOST_CHECK(!ss.fail());
     for (int i = 0; i < 100000; i++)
          BOOST_CHECK(g.uni(100) == g1.uni(100));
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
}

BOOST_AUTO_TEST_CASE(logarithmic_test) {
     MZT g;
     unsigned long s = 0;
     for (int i = 0; i < 100; i++)
          s += g.logarithmic(0.5);
     BOOST_CHECK(s == 131);
}

namespace {

constexpr double g_p[9]{0.0001, 0.001, 0.01,  0.1,   0.5,
                        0.9,    0.99,  0.999, 0.9999};

}  // anonymous namespace

BOOST_AUTO_TEST_CASE(geometric_test) {
     MZT g;

     unsigned int s = 0;
     for (std::size_t i = 0; i < std::size(g_p); i++) {
          const double pi = g_p[i];
          for (int j = 0; j < 1000; j++)
               s += g.geometric(pi);
     }
     BOOST_CHECK(s == 11278386u);

     double d = 0.0;
     for (size_t i = 0; i < 1000; i++)
          BOOST_CHECK_NO_THROW(d += g.geometric(1e-8));
     BOOST_CHECK(std::abs(d - 100262203685.0) < 1e-20);
}

BOOST_AUTO_TEST_CASE(gamma_test) {
     MZT g;
     for (int i = 0; i < 1000; i++) {
          const double gam = g.gamma(1000, 22);
          BOOST_CHECK(std::isfinite(gam));
     }
}

BOOST_AUTO_TEST_CASE(beta_test) {
     MZT g;
     for (int i = 0; i < 1000; i++) {
          const double beta = g.beta(1e10, 1e10);
          BOOST_CHECK(std::isfinite(beta));
     }
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
}

namespace {

constexpr double mu[]{0.01, 0.5, 1.0, 5.0, 10.0, 50.0, 100.0, 1000.0};

}  // anonymous namespace

BOOST_AUTO_TEST_CASE(poisson_test) {
     MZT g;
     unsigned int s = 0;
     for (std::size_t i = 0; i < std::size(mu); i++) {
          const double mui = mu[i];
          for (int j = 0; j < 1000; j++)
               s += g.poisson(mui);
     }
     BOOST_CHECK(s == 1166264u);
}

namespace {

constexpr double nb_t[]{0.001, 0.01, 0.1,   0.5,
                        1.0,   10.0, 100.0, 1000.0};
constexpr double nb_p[]{0.0001, 0.001, 0.01,  0.1,   0.5,
                        0.9,    0.99,  0.999, 0.9999};

}  // anonymous namespace

BOOST_AUTO_TEST_CASE(negative_binomial_test) {
     MZT g;
     unsigned int s = 0;
     for (std::size_t i = 0; i < std::size(nb_t); i++) {
          const double ti = nb_t[i];
          for (std::size_t j = 0; j < std::size(nb_p); j++) {
               const double pj = nb_p[j];
               for (int k = 0; k < 1000; k++)
                    s += g.negative_binomial(ti, pj);
          }
     }
     BOOST_CHECK(s == 3760982173u);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting
