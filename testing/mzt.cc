/* mzt.cc: testing MZT random number generator */

/**
 * \file testing/mzt.cc
 * Testing MZT random number generator.
 * \date Created on 4 April 2010.
 */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ios>
#include <sstream>
#include "shg/vector.h"
#include "shg/mzt.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::numeric_limits;
using std::ios_base;
using std::stringstream;
using std::size_t;
using std::overflow_error;
using SHG::MZT;
using SHG::Vecint;

namespace {

// Test from Wieczorkowski, Zielinski, page 40.
void wiecz_ziel() {
     static const SHG::Vecint t {
          6,   3,  11,   3,   0,   4,   0,
         13,   8,  15,  11,  11,  14,   0,
          6,  15,   0,   2,   3,  11,   0,
          5,  14,   2,  14,   4,   8,   0,
          7,  15,   7,  10,  12,   2,   0
     };

     MZT g;
     int k = 0;
     for (int i = 1; i <= 20005; i++) {
          const double x = g();
          if (i > 20000) {
               for (int j = 1; j <= 7; j++)
                    SHG_ASSERT(fmod(floor(pow(16.0, j) * x), 16.0) ==
                               t[k++]);
          }
     }
}

void write_read() {
     const ios_base::openmode mode = ios_base::out | ios_base::in |
          ios_base::binary;

     MZT g(12, 34, 56, 78);
     stringstream ss(mode);
     g.write(ss);
     SHG_ASSERT(!ss.fail());
     MZT g1(1, 2, 3, 4);
     g1.read(ss);
     SHG_ASSERT(!ss.fail());
     for (int i = 0; i < 100000; i++)
          SHG_ASSERT(g.uni(100) == g1.uni(100));
}

void random_sample() {
     MZT g;
     Vecint x;
     for (int k = 0; k < 20; k++)
          for (int N = 1; N <= 5; N++)
               for (int n = 1; n <= N; n++) {
                    g.random_sample(n, N, x);
                    for (size_t i = 0; i < x.size(); i++)
                         SHG_ASSERT(x[i] >= 0 && x[i] < N);
                    for (size_t i = 1; i < x.size(); i++)
                         SHG_ASSERT(x[i - 1] < x[i]);
               }
}

void logarithmic() {
     MZT g;
     unsigned long s = 0;
     for (int i = 0; i < 100; i++)
          s += g.logarithmic(0.5);
     SHG_ASSERT(s == 131);
}

void geometric() {
     static const SHG::Vecdouble p {
          0.0001, 0.001, 0.01, 0.1, 0.5, 0.9, 0.99, 0.999, 0.9999
     };
     MZT g;

     unsigned int s = 0;
     for (size_t i = 0; i < p.size(); i++) {
          const double pi = p[i];
          for (int j = 0; j < 1000; j++)
               s += g.geometric(pi);
     }
     SHG_ASSERT(s == 11278386u);

     double d = 0.0;
     for (size_t i = 0; i < 1000; i++) {
          try {
               d += g.geometric(1e-8);
          } catch (const std::overflow_error&) {
               SHG_ASSERT(false);
          }
     }
     SHG_ASSERT(std::abs(d - 100262203685.0) < 1e-20);
}

void gamma() {
     MZT g;
     for (int i = 0; i < 1000; i++) {
          const double gam = g.gamma(1000, 22);
          SHG_ASSERT(std::isfinite(gam));
     }
}

void beta() {
     MZT g;
     for (int i = 0; i < 1000; i++) {
          g.beta(1e10, 1e10);
     }
}

void binomial() {
     MZT g;
     double p;
     unsigned int n;

     p = 0.0;
     for (n = 0; n < 1000; n++)
          SHG_ASSERT(g.binomial(p, n) == 0);
     p = 1.0;
     for (n = 0; n < 1000; n++)
          SHG_ASSERT(g.binomial(p, n) == n);
     p = 0.5;
     for (n = 0; n < 10000; n++)
          SHG_ASSERT(g.binomial(p, n) <= n);
}

void poisson() {
     const SHG::Vecdouble mu {
          0.01, 0.5, 1.0, 5.0, 10.0, 50.0, 100.0, 1000.0
     };
     MZT g;
     unsigned int s = 0;
     for (size_t i = 0; i < mu.size(); i++) {
          const double mui = mu[i];
          for (int j = 0; j < 1000; j++)
               s += g.poisson(mui);
     }
     SHG_ASSERT(s == 1166264u);
}

void negative_binomial() {
     const SHG::Vecdouble t {
          0.001, 0.01, 0.1, 0.5, 1.0, 10.0, 100.0, 1000.0
     };
     const SHG::Vecdouble p {
          0.0001, 0.001, 0.01, 0.1, 0.5, 0.9, 0.99, 0.999, 0.9999
     };

     MZT g;
     unsigned int s = 0;
     for (size_t i = 0; i < t.size(); i++) {
          const double ti = t[i];
          for (size_t j = 0; j < p.size(); j++) {
               const double pj = p[j];
               for (int k = 0; k < 1000; k++)
                    s += g.negative_binomial(ti, pj);
          }
     }
     SHG_ASSERT(s == 3760982173u);
}

}       // anonymous namespace

void test_mzt() {
     wiecz_ziel();
     write_read();
     random_sample();
     logarithmic();
     geometric();
     gamma();
     beta();
     binomial();
     poisson();
     negative_binomial();
}

}       // namespace Testing
}       // namespace SHG
