/* runs.cc: testing runs */

/**
 * \file testing/runs.cc
 * Testing runs.
 * \date Created on 28 September 2013.
 */

#include <cmath>        // abs
#include <algorithm>    // min
#include <utility>      // swap
#include "shg/runs.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::abs;
using std::min;
using std::size_t;

namespace {

double newton(int n, int k) {
     SHG_ASSERT(n >= 0 && k >= 0);
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
          return 2.0 * newton(n1 - 1, l - 1) *
               newton(n2 - 1, l - 1) / newton(n1 + n2, n1);
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

void test_count_runs_double(const SHG::Vecdouble& x,
                            size_t n1,
                            size_t n2,
                            size_t k) {
     size_t n1l, n2l, kl;
     SHG::count_runs<double>(x, 2, n1l, n2l, kl);
     SHG_ASSERT(n1l == n1 && n2l == n2 && kl == k);
}

void test_count_runs_int(const SHG::Vecint& x,
                         size_t n1,
                         size_t n2,
                         size_t k) {
     size_t n1l, n2l, kl;
     SHG::count_runs<int>(x, 2, n1l, n2l, kl);
     SHG_ASSERT(n1l == n1 && n2l == n2 && kl == k);
}

void first_test() {
     double p1, p2;
     size_t n1, n2, k;

     n1 = n2 = k = 0;
     runs(n1, n2, k, true, p1, p2);
     SHG_ASSERT(abs(p1 - 1.0) < 1e-15);
     SHG_ASSERT(abs(p2 - 1.0) < 1e-15);
     for (k = 1; k <= 10; k++) {
          runs(n1, n2, k, true, p1, p2);
          SHG_ASSERT(abs(p1 - 1.0) < 1e-15);
          SHG_ASSERT(abs(p2 - 0.0) < 1e-15);
     }

     for (n2 = 1; n2 <= 20; n2++) {
          k = 0;
          runs(n1, n2, k, true, p1, p2);
          SHG_ASSERT(abs(p1 - 0.0) < 1e-15);
          SHG_ASSERT(abs(p2 - 1.0) < 1e-15);
          k = 1;
          runs(n1, n2, k, true, p1, p2);
          SHG_ASSERT(abs(p1 - 1.0) < 1e-15);
          SHG_ASSERT(abs(p2 - 1.0) < 1e-15);
          for (k = 2; k <= 10; k++) {
               runs(n1, n2, k, true, p1, p2);
               SHG_ASSERT(abs(p1 - 1.0) < 1e-15);
               SHG_ASSERT(abs(p2 - 0.0) < 1e-15);
          }
     }
     n2 = 0;
     for (n1 = 1; n1 <= 20; n1++) {
          k = 0;
          runs(n1, n2, k, true, p1, p2);
          SHG_ASSERT(abs(p1 - 0.0) < 1e-15);
          SHG_ASSERT(abs(p2 - 1.0) < 1e-15);
          k = 1;
          runs(n1, n2, k, true, p1, p2);
          SHG_ASSERT(abs(p1 - 1.0) < 1e-15);
          SHG_ASSERT(abs(p2 - 1.0) < 1e-15);
          for (k = 2; k <= 10; k++) {
               runs(n1, n2, k, true, p1, p2);
               SHG_ASSERT(abs(p1 - 1.0) < 1e-15);
               SHG_ASSERT(abs(p2 - 0.0) < 1e-15);
          }
     }
     n1 = 5;
     n2 = 6;
     for (k = 0; k < 2; k++) {
          runs(n1, n2, k, true, p1, p2);
          SHG_ASSERT(abs(p1 - 0.0) < 1e-15);
          SHG_ASSERT(abs(p2 - 1.0) < 1e-15);
     }
     size_t k1 = (n1 != n2 ? 2 * min(n1, n2) + 1 : n1 + n2) + 1;
     for (k = k1; k <= k1 + 10; k++) {
          runs(n1, n2, k, true, p1, p2);
          SHG_ASSERT(abs(p1 - 1.0) < 1e-15);
          SHG_ASSERT(abs(p2 - 0.0) < 1e-15);
     }

     // Sum up all non-vanishing probabilities P(K = k) and check if
     // their sum equals 1.
     for (n1 = 1; n1 <= 30; n1++)
          for (n2 = 1; n2 <= 30; n2++) {
               const int kmax = n1 != n2 ?
                    2 * min(n1, n2) + 1 : n1 + n2;
               double s = 0.0;
               for (int k = 2; k <= kmax; k++) {
                    runs(n1, n2, k, true, p1, p2);
                    const double p = p1 + p2 - 1.0;
                    SHG_ASSERT(p >= 0.0);
                    SHG_ASSERT(abs(p - pdf(n1, n2, k)) < 1e-15);
                    SHG_ASSERT(abs(p1 - cdf(n1, n2, k)) < 1e-14);
                    s += p;
               }
               SHG_ASSERT(abs(s - 1.0) < 2e-15);
          }
}

void second_test() {
     // Example from Wolf and Wolfowitz.
     double pe1, pe2, pa1, pa2;
     runs(50, 50, 34, true, pe1, pe2);
     runs(50, 50, 34, false, pa1, pa2);
     SHG_ASSERT(abs(pe1 - 0.000411735) < 5e-10);
     SHG_ASSERT(abs(pa1 - 0.000316224) < 5e-10);

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
     SHG_ASSERT(maxd > 0.012620 && maxd < 0.012621);
     try {
          runs(600, 600, 300, true, pe1, pe2);
          SHG_ASSERT(false);
     } catch (const std::range_error&) {}
}

void test_count_runs() {
     test_count_runs_double({}, 0, 0, 0);
     test_count_runs_double({1}, 1, 0, 1);
     test_count_runs_double({3}, 0, 1, 1);
     test_count_runs_double({1, 1}, 2, 0, 1);
     test_count_runs_double({1, 3}, 1, 1, 2);
     test_count_runs_double({3, 1}, 1, 1, 2);
     test_count_runs_double({3, 3}, 0, 2, 1);
     test_count_runs_double({1, 1, 1}, 3, 0, 1);
     test_count_runs_double({1, 1, 3}, 2, 1, 2);
     test_count_runs_double({1, 3, 1}, 2, 1, 3);
     test_count_runs_double({3, 1, 1}, 2, 1, 2);
     test_count_runs_double({1, 3, 3}, 1, 2, 2);
     test_count_runs_double({3, 1, 3}, 1, 2, 3);
     test_count_runs_double({3, 3, 1}, 1, 2, 2);
     test_count_runs_double({3, 3, 3}, 0, 3, 1);
     test_count_runs_double({1, 1, 1, 2, 2, 1, 2}, 4, 3, 4);

     test_count_runs_int({}, 0, 0, 0);
     test_count_runs_int({1}, 1, 0, 1);
     test_count_runs_int({3}, 0, 1, 1);
     test_count_runs_int({1, 1}, 2, 0, 1);
     test_count_runs_int({1, 3}, 1, 1, 2);
     test_count_runs_int({3, 1}, 1, 1, 2);
     test_count_runs_int({3, 3}, 0, 2, 1);
     test_count_runs_int({1, 1, 1}, 3, 0, 1);
     test_count_runs_int({1, 1, 3}, 2, 1, 2);
     test_count_runs_int({1, 3, 1}, 2, 1, 3);
     test_count_runs_int({3, 1, 1}, 2, 1, 2);
     test_count_runs_int({1, 3, 3}, 1, 2, 2);
     test_count_runs_int({3, 1, 3}, 1, 2, 3);
     test_count_runs_int({3, 3, 1}, 1, 2, 2);
     test_count_runs_int({3, 3, 3}, 0, 3, 1);
     test_count_runs_int({1, 1, 1, 2, 2, 1, 2}, 4, 3, 4);
}

}       // anonymous namespace

void test_runs() {
     first_test();
     second_test();
     test_count_runs();
}

}       // namespace Testing
}       // namespace SHG
