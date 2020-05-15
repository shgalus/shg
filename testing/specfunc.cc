/* specfunc.cc: testing special functions */

/**
 * \file testing/specfunc.cc
 * Testing special functions.
 * \date Created on 10 July 2012.
 */

#include <cmath>
#include "shg/mconsts.h"
#include "shg/specfunc.h"
#include "ppnd7.h"
#include "gammad.h"
#include "probst.h"
#include "betain.h"
#include "cdffdist.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::abs;

namespace {

void test_digamma() {
     using SHG::Constants::gamma;
     using SHG::digamma;
     using std::log;
     const double g = SHG::Constants::gamma<double>();
     // digamma(1) = -gamma
     SHG_ASSERT(abs(digamma(1) + gamma<double>()) < 5e-7);
     // digamma(0.5) = -2 ln(2) - gamma
     SHG_ASSERT(abs(digamma(0.5) + 2.0 * log(2.0) + gamma<double>())
                < 9e-7);
     // digamma(n) = -gamma + \sum_{k = 1}^{n - 1} (1/k) for n >= 2.
     for (int n = 2; n <= 20; n++) {
          double s = 0.0;
          for (int k = n - 1; k > 0; k--)
               s += 1.0 / k;
          s -= g;
          SHG_ASSERT(abs(digamma(n) - s) < 3e-7);
     }
     try {
          digamma(0.0);
          SHG_ASSERT(false);
     } catch (const std::invalid_argument&) {}
}

void test_alnorm_and_normal_integral() {
     for (int i = 0; i <= 400; i++) {
          const double x = -10.0 + i * 0.05;
          const double y1 = SHG::alnorm(x);
          const double z1 = SHG::normal_integral(x);
          SHG_ASSERT(abs(y1 - z1) < 5e-12);
          const double y2 = SHG::alnorm(x, true);
          const double z2 = SHG::normal_integral(x, true);
          SHG_ASSERT(abs(y2 - z2) < 5e-12);
     }
}

void test_ppnd7() {
     for (int i = 0; i < 199; i++) {
          const double x = 0.005 * (i + 1);
          const double y = SHG::ppnd7(x);
          const double z = tab_ppnd7[i];
          SHG_ASSERT(abs(y - z) < 3e-7);
     }
}

void test_gammad() {
     int k = 0;
     for (int ix = 0; ix <= 100; ix++) {
          const double x = ix * 0.1;
          for (int p = 1; p <= 100; p++) {
               const double y = SHG::gammad(x, p * 0.1);
               const double z = tab_gammad[k++];
               SHG_ASSERT(abs(y - z) < 1e-8);
          }
     }
     SHG_ASSERT(k == 10100);
}

void test_probst() {
     int k = 0;
     for (int df = 1; df <= 18; df++)
          for (int i = 0; i <= 80; i++) {
               const double x = i * 0.1;
               const double y = SHG::probst(x, df);
               const double z = tab_probst[k++];
               SHG_ASSERT(abs(y - z) < 1e-14);
          }
     SHG_ASSERT(k == 1458);
}

void test_betain() {
     int k = 0;
     for (int i = 1; i <= 9; i++) {
          const double x = i * 0.1;
          for (int p = 1; p <= 10; p++)
               for (int q = 1; q <= 10; q ++) {
                    const double y = SHG::betain(x, p, q);
                    const double z = tab_betain[k++];
                    SHG_ASSERT(abs(y - z) < 1e-10);
               }
     }
     SHG_ASSERT(k == 900);
}

void test_cdffdist() {
     int k = 0;
     for (int m = 1; m <= 10; m++)
          for (int n = 1; n <= 10; n++)
               for (int x = 1; x <= 10; x++) {
                    const double y = SHG::cdffdist(m, n, x);
                    const double z = tab_cdffdist[k++];
                    SHG_ASSERT(abs(y - z) < 3e-8);
               }
     SHG_ASSERT(k == 1000);
}

}       // anonymous namespace

void test_special_functions() {
     test_digamma();
     test_alnorm_and_normal_integral();
     test_ppnd7();
     test_gammad();
     test_probst();
     test_betain();
     test_cdffdist();
}

}       // namespace Testing
}       // namespace SHG
