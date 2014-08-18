/* specfunc.cc: testing special functions */

/**
 * \file testing/specfunc.cc
 * Testing special functions.
 * \date Created on 10 July 2012.
 */

#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include "shg/mconsts.h"
#include "shg/utils.h"
#include "shg/specfunc.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::abs;
using std::atof;
using std::ifstream;
using std::string;
using SHG::skipcomment;

namespace {

double newy(ifstream& f) {
     string s;
     skipcomment(f, s);
     SHG_ASSERT(f);
     return atof(s.c_str());
}

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
     } catch (std::invalid_argument) {}
}

}       // anonymous namespace

void test_specfunc() {
     ifstream f(datapath("specfunc.txt").c_str());
     SHG_ASSERT(f);
     double x, y, z;

     // loggamma
     for (int i = 0; i <= 100; i++) {
          y = newy(f);
          z = SHG::loggamma(1.0 + i * 0.01);
          SHG_ASSERT(abs(y - z) < 2e-11);
     }
     for (int i = 0; i < 400; i++) {
          y = newy(f);
          z = SHG::loggamma((i + 1) * 0.25);
          SHG_ASSERT(abs(y - z) < 2e-11);
     }

     // alnorm and normal_integral
     for (int i = 0; i <= 400; i++) {
          x = -10.0 + i * 0.05;
          y = newy(f);
          z = SHG::alnorm(x);
          SHG_ASSERT(abs(y - z) < 5e-12);
          z = SHG::normal_integral(x);
          SHG_ASSERT(abs(y - z) < 5e-12);
          y = SHG::alnorm(x, true);
          z = SHG::normal_integral(x, true);
          SHG_ASSERT(abs(y - z) < 5e-12);
     }

     // ppnd7
     for (int i = 0; i < 199; i++) {
          y = newy(f);
          z = SHG::ppnd7(0.005 * (i + 1));
          SHG_ASSERT(abs(y - z) < 3e-7);
     }

     // gammad
     for (int ix = 0; ix <= 100; ix++) {
          const double x = ix * 0.1;
          for (int p = 1; p <= 100; p++) {
               y = newy(f);
               z = SHG::gammad(x, p * 0.1);
               SHG_ASSERT(abs(y - z) < 1e-8);
          }
     }

     // probst
     for (int df = 1; df <= 18; df++)
          for (int i = 0; i <= 80; i++) {
               y = newy(f);
               z = SHG::probst(i * 0.1, df);
               SHG_ASSERT(abs(y - z) < 1e-14);
          }

     // betain
     for (int i = 1; i <= 9; i++) {
          const double x = i * 0.1;
          for (int p = 1; p <= 10; p++)
               for (int q = 1; q <= 10; q ++) {
                    y = newy(f);
                    z = SHG::betain(x, p, q);
                    SHG_ASSERT(abs(y - z) < 1e-10);
               }
     }

     // cdffdist
     for (int m = 1; m <= 10; m++)
          for (int n = 1; n <= 10; n++)
               for (int x = 1; x <= 10; x++) {
                    y = newy(f);
                    z = SHG::cdffdist(m, n, x);
                    SHG_ASSERT(abs(y - z) < 3e-8);
               }
     SHG_ASSERT(f);
     // Make sure the file was exhausted.
     string s;
     skipcomment(f, s);
     SHG_ASSERT(f.eof() && !f.bad());
     test_digamma();
}

}       // namespace Testing
}       // namespace SHG
