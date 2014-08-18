/* mstat.cc: testing mathematical statistics */

/**
 * \file testing/mstat.cc
 * Testing mathematical statistics.
 * \date Created on 4 April 2010.
 */

#include <cmath>
#include <cstdlib>
#include <limits>
#include "shg/mzt.h"
#include "shg/mstat.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::size_t;
using std::abs;

namespace {

// Cumulative distribution function of the Kolmogorov-Smirnov
// distribution as a sum of the series 1 - 2 \sum{j = 1}^{\infty}
// (-1)^{j - 1} exp(-2j^2z^2).
double F(const double x) {
     if (x <= 0.0)
          return 0.0;
     double s = 0.0;
     bool neg = false;
     for (int j = 1;; j++) {
          const  double y = j * x;
          const  double z = std::exp(-2.0 * y * y);
          if (z < std::numeric_limits<double>::min())
               break;
          if (neg) {
               s -= z;
               neg = false;
          } else {
               s += z;
               neg = true;
          }
     }
     s = 1.0 - 2.0 * s;
     if (s < 0.0)
          return 0.0;
     return s;
}

void test_ksdist() {
     for (int i = 0; i <= 500; i++) {
          const double x = 0.01 * i;
          const double y = F(x);
          const double z = SHG::ksdist(x);
          const double d = abs(y - z);
          SHG_ASSERT(y >= 0.0);
          SHG_ASSERT(y <= 1.0);
          SHG_ASSERT(z >= 0.0);
          SHG_ASSERT(z <= 1.0);
          SHG_ASSERT(d < 1e-15);
     }
}

void test_mstat1() {
     const SHG::Vecdouble xd {0.57, 0.71, 0.85, 0.99, 1.13, 1.27, 1.41,
               1.55, 1.69, 1.83, 1.97, 2.11, 2.28};
     const SHG::Vector<size_t> nd {1, 2, 9, 25, 37, 53, 56, 53, 25,
               19, 16, 3, 1};
     SHG_ASSERT(xd.size() == nd.size());
     SHG::Vecdouble x(sum(nd));
     for (size_t i = 0, n = 0; i < nd.size(); i++)
          for (size_t j = 0; j < nd[i]; j++)
               x(n++) = xd[i];
     const double p = SHG::chi2normtest(x, 10);
     SHG_ASSERT(abs(p) < 1e-12);
}

void test_mstat2() {
     const double mu = 5;
     const double sigma = sqrt(2.0);
     SHG::Vecdouble x(2000);
     SHG::MZT g;

     // Generate sample from N(mu, sigma^2).
     for (size_t i = 0; i < x.size(); i++)
          x(i) = mu + sigma * g.normal();
     const double p = SHG::chi2normtest(x, 10);
     SHG_ASSERT(abs(p - 0.889589) < 5e-7);
}

void deq(double x, double y) {
     SHG_ASSERT(abs(x - y) < 1e-15);
}

void test_sample() {
     const std::vector<double> v {1, 2, 2, 3, 3, 3, 4, 4, 4, 4};
     const SHG::Sample s(v);
     for (int i = 0; i <= 500; i++) {
          const double x = i / 100.0;
          const double F = s.cdf(x), F1 = s.lcdf(x);
          if (x < 1.0)
               deq(F, 0.0);
          else if (x < 2.0)
               deq(F, 0.1);
          else if (x < 3.0)
               deq(F, 0.3);
          else if (x < 4.0)
               deq(F, 0.6);
          else
               deq(F, 1.0);
          if (x > 4.0)
               deq(F1, 1.0);
          else if (x > 3.0)
               deq(F1, 0.6);
          else if (x > 2.0)
               deq(F1, 0.3);
          else if (x > 1.0)
               deq(F1, 0.1);
          else
               deq(F1, 0.0);
     }
     for (int i = 1; i < 1000; i++) {
          const double p = i / 1000.0, q = s.quantile(p);
          // For i = 100, there is an error here with -Ofast.
          // s.quantile(0.1) should return 1 but returns 2.
          if (p > 0.6)
               deq(q, 4.0);
          else if (p > 0.3)
               deq(q, 3.0);
          else if (p > 0.1)
               deq(q, 2.0);
          else
               deq(q, 1.0);
     }
}

/*
 * The results tested here are:
 *
 * 1.08037435341377e-02   9.73707791984890e-01
 * 6.52407729020162e-02   8.07194225353378e-08
 *
 * The results get with Numerical Recipes with (14.3.18) are:
 *
 * 1.08037435377046e-02   9.73012808686660e-01
 * 6.52407728998911e-02   7.35239129875459e-08
 *
 */
void test_ks() {
     SHG::MZT g;
     SHG::Vecdouble x(2000);
     double d, prob;

     for (size_t i = 0; i < x.size(); i++)
          x[i] = 3 + 0.5 * g.normal();
     SHG::ksnormtest(x, d, prob);
     SHG_ASSERT(abs(d - 1.08037435341377e-02) < 1e-6);
     SHG_ASSERT(abs(prob - 9.73707791984890e-01) < 1e-6);

     for (size_t i = 0; i < x.size(); i++)
          x[i] = g();
     SHG::ksnormtest(x, d, prob);
     SHG_ASSERT(abs(d - 6.52407729020162e-02) < 1e-6);
     SHG_ASSERT(abs(prob - 8.07194225353378e-08) < 1e-14);
}

void test_run_length_distribution() {
     using std::vector;
     using SHG::run_length_distribution;
     vector<int> x {0, 0, 1, 0, 2, 2, 1, 0, 1, 2, 2, 1, 0, 0, 0, 0,
               1, 1, 2};
     vector<vector<int>> v = run_length_distribution(x, 3);
     const vector<vector<int>>
          r {{2, 1, 1, 4}, {1, 1, 1, 1, 2}, {2, 2, 1}};
     SHG_ASSERT(v.size() == r.size());
     for (size_t i = 0; i < v.size(); i++) {
          SHG_ASSERT(v[i].size() == r[i].size());
          for (size_t j = 0; j < v[i].size(); j++)
               SHG_ASSERT(v[i][j] == r[i][j]);
     }
     try {
          x[1] = 3;
          v = run_length_distribution(x, 3);
          SHG_ASSERT(0);
     } catch (std::invalid_argument) {}
     x.resize(0);
     v = run_length_distribution(x, 3);
     SHG_ASSERT(v.size() == 3);
     for (size_t i = 0; i < v.size(); i++)
          SHG_ASSERT(v[i].size() == 0);
}

void test_mle_lsd() {
     using SHG::mle_lsd;
     SHG_ASSERT(abs(mle_lsd(1.5) - 0.5335892440) < 1e-8);
     SHG_ASSERT(abs(mle_lsd(11.0) - 0.9762779470) < 1e-8);
     SHG_ASSERT(abs(mle_lsd(501.0) - 0.9997606870) < 3e-8);
     try {
          const double x = mle_lsd(0.9999);
          SHG_ASSERT(0 && x == x);
     } catch (SHG::Invalid_argument) {}
}

void test_cdf_lsd() {
     using std::isfinite;
     using SHG::cdf_lsd;
     for (int k = 0; k < 100; k++)
          SHG_ASSERT(isfinite(cdf_lsd(k, 0.00001)));
     for (int k = 0; k < 100; k++)
          SHG_ASSERT(isfinite(cdf_lsd(k, 0.5)));
     for (int k = 0; k < 100; k++)
          SHG_ASSERT(isfinite(cdf_lsd(k, 0.99999)));
}

void test_mle_negative_binomial() {
     const double t0 = 1.0;
     const double p0 = 0.5;
     SHG::MZT g;
     SHG::Vecint x(200);
     for (size_t i = 0; i < x.size(); i++)
          x(i) = g.negative_binomial(t0, p0);
     double t, p;
     SHG::mle_negative_binomial(x, t, p);
     SHG_ASSERT(abs(t - 1.05285074569) < 1e-9);
     SHG_ASSERT(abs(p - 0.50427490943) < 1e-9);
}

// Calculate cdf of negative binomial distribution summing probability
// function.
double cdf_nb(double x, double t, double p) {
     const double q = 1.0 - p;
     SHG_ASSERT(t > 0.0 && p > 0.0 && q > 0.0);
     if (x < 0.0)
          return 0.0;
     const int n = SHG::ifloor(x);
     double b = 1.0, s = 1.0, z = 1.0;
     for (int i = 1; i <= n; i++) {
          b *= (t + i - 1) / i;
          z *= q;
          s += b * z;
     }
     return std::pow(p, t) * s;
}

void test_cdf_negative_binomial() {
     using SHG::cdf_negative_binomial;
     double t, p;
     auto test = [&t, &p](const double eps) {
          for (int i = -5; i <= 500; i++) {
               const double F0 = cdf_negative_binomial(i, t, p);
               const double F1 = cdf_nb(i, t, p);
               SHG_ASSERT(abs(F0 - F1) < eps);
          }
     };
     t = 1.000; p = 0.500; test(1e-9);
     t = 0.001; p = 0.999; test(1e-9);
     t = 0.001; p = 0.001; test(1e-7);
}

void test_assessment_of_discrete_distribution() {
     using SHG::Assessment_of_discrete_distribution;
     const int n = 200;
     SHG::MZT g;
     SHG::Vecint x(n);
     // Sample from geometric distribution.
     {
          for (size_t i = 0; i < x.size(); i++)
               SHG_ASSERT((x(i) = g.geometric(0.5)) > 0);
          Assessment_of_discrete_distribution a(x);
          a.run();
          SHG_ASSERT(abs(a.geometric()   - 1.0000000) < 5e-7);
          SHG_ASSERT(abs(a.poisson()     - 0.0000000) < 5e-7);
          SHG_ASSERT(abs(a.logarithmic() - 0.5236627) < 5e-7);
          SHG_ASSERT(abs(a.negbin()      - 0.0012100) < 5e-7);
     }
     // Sample from Poisson distribution.
     {
          for (size_t i = 0; i < x.size(); i++)
               SHG_ASSERT((x(i) = g.poisson(1.0)) >= 0);
          Assessment_of_discrete_distribution a(x);
          a.run();
          SHG_ASSERT(abs(a.geometric()   - 0.0000000) < 5e-7);
          SHG_ASSERT(abs(a.poisson()     - 0.6940636) < 5e-7);
          SHG_ASSERT(abs(a.logarithmic() - 0.0000000) < 5e-7);
          SHG_ASSERT(abs(a.negbin()      - 1.0000000) < 5e-7);
     }
     // Sample from logarithmic series distribution.
     {
          for (size_t i = 0; i < x.size(); i++)
               SHG_ASSERT((x(i) = g.logarithmic(0.5)) > 0);
          Assessment_of_discrete_distribution a(x);
          a.run();
          SHG_ASSERT(abs(a.geometric()   - 0.1649365) < 5e-7);
          SHG_ASSERT(abs(a.poisson()     - 0.0000000) < 5e-7);
          SHG_ASSERT(abs(a.logarithmic() - 0.8575215) < 5e-7);
          SHG_ASSERT(abs(a.negbin()      - 0.0000000) < 5e-7);
     }
     // Sample from negative binomial distribution.
     {
          for (size_t i = 0; i < x.size(); i++)
               SHG_ASSERT((x(i) = g.negative_binomial(1.0, 0.5)) >= 0);
          Assessment_of_discrete_distribution a(x);
          a.run();
          SHG_ASSERT(abs(a.geometric()   - 0.0000000) < 5e-7);
          SHG_ASSERT(abs(a.poisson()     - 0.0000379) < 5e-7);
          SHG_ASSERT(abs(a.logarithmic() - 0.0000000) < 5e-7);
          SHG_ASSERT(abs(a.negbin()      - 0.9999419) < 5e-7);
     }
}

}       // anonymous namespace

void test_mstat() {
     test_ksdist();
     test_mstat1();
     test_mstat2();
     test_sample();
     test_ks();
     test_run_length_distribution();
     test_mle_lsd();
     test_cdf_lsd();
     test_mle_negative_binomial();
     test_cdf_negative_binomial();
     test_assessment_of_discrete_distribution();
}

}       // namespace Testing
}       // namespace SHG
