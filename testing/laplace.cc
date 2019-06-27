/* laplace.cc: testing Laplace distribution */

/**
 * \file testing/laplace.cc
 * Testing Laplace distribution.
 * \date Created on 6 January 2013.
 */

#include <cmath>
#include "shg/utils.h"
#include "shg/mzt.h"
#include "shg/laplace.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

using std::abs;
using std::sqrt;
using SHG::Vecdouble;
using SHG::Laplace_distribution;
using SHG::median;
using SHG::weighted_median;
using SHG::Unilapmixmod;
using SHG::Laplace_mixture;
using SHG::sqr;

void cmp(double x, double y) {
     SHG_ASSERT(abs(x - y) < 1e-15);
}

void test_laplace_distribution() {
     for (int imu = -100; imu <= 100; imu++) {
          const double mu = imu / 10.0;
          for (int ilambda = 1; ilambda <= 100; ilambda++) {
               const double lambda = ilambda / 10.0;
               Laplace_distribution d(mu, lambda);
               for (int ip = 1; ip <= 9; ip++) {
                    const double p = ip / 10.0;
                    SHG_ASSERT(abs(p - d.cdf(d.invcdf(p))) < 2e-15);
               }
          }
     }
}

void test_median() {
     for (int n = 1; n < 100; n++) {
          Vecdouble x(n);
          for (int i = 0; i < n; i++)
               x[i] = i;
          const double m = (n % 2 == 1) ? x[n / 2] :
               0.5 * (x[n / 2 - 1] + x[n / 2]);
          cmp(median(x), m);
     }
}

void test_weighted_median() {
     Vecdouble w, x;

     x.resize(1); w.resize(1);
     x[0] = 0;
     w[0] = 1;
     cmp(weighted_median(x, w), x[0]);

     x.resize(2); w.resize(2);
     x[0] = 0; x[1] = 1;
     w[0] = 1; w[1] = 0;
     cmp(weighted_median(x, w), x[0]);
     w[0] = 0; w[1] = 1;
     cmp(weighted_median(x, w), x[1]);

     x.resize(3); w.resize(3);
     x[0] = 0; x[1] = 1; x[2] = 3;
     w[0] = 1; w[1] = 0; w[2] = 0;
     cmp(weighted_median(x, w), x[0]);
     w[0] = 0; w[1] = 1; w[2] = 0;
     cmp(weighted_median(x, w), x[1]);
     w[0] = 0; w[1] = 0; w[2] = 1;
     cmp(weighted_median(x, w), x[2]);
     w[0] = 1; w[1] = 1; w[2] = 0;
     cmp(weighted_median(x, w), 0.5 * (x[0] + x[1]));
     w[0] = 0; w[1] = 1; w[2] = 1;
     cmp(weighted_median(x, w), 0.5 * (x[1] + x[2]));
     w[0] = 1; w[1] = 0; w[2] = 1;
     cmp(weighted_median(x, w), 0.5 * (x[0] + x[2]));

     x.resize(4); w.resize(4);
     x[0] = 0; x[1] = 1; x[2] = 3; x[3] = 6;
     w[0] = 1; w[1] = 0; w[2] = 0; w[3] = 0;
     cmp(weighted_median(x, w), x[0]);
     w[0] = 0; w[1] = 1; w[2] = 0; w[3] = 0;
     cmp(weighted_median(x, w), x[1]);
     w[0] = 0; w[1] = 0; w[2] = 1; w[3] = 0;
     cmp(weighted_median(x, w), x[2]);
     w[0] = 0; w[1] = 0; w[2] = 0; w[3] = 1;
     cmp(weighted_median(x, w), x[3]);
     w[0] = 1; w[1] = 1; w[2] = 0; w[3] = 0;
     cmp(weighted_median(x, w), 0.5 * (x[0] + x[1]));
     w[0] = 1; w[1] = 0; w[2] = 1; w[3] = 0;
     cmp(weighted_median(x, w), 0.5 * (x[0] + x[2]));
     w[0] = 1; w[1] = 0; w[2] = 0; w[3] = 1;
     cmp(weighted_median(x, w), 0.5 * (x[0] + x[3]));
     w[0] = 0; w[1] = 1; w[2] = 1; w[3] = 0;
     cmp(weighted_median(x, w), 0.5 * (x[1] + x[2]));
     w[0] = 0; w[1] = 1; w[2] = 0; w[3] = 1;
     cmp(weighted_median(x, w), 0.5 * (x[1] + x[3]));
     w[0] = 0; w[1] = 0; w[2] = 1; w[3] = 1;
     cmp(weighted_median(x, w), 0.5 * (x[2] + x[3]));
     w[0] = 1; w[1] = 1; w[2] = 1; w[3] = 0;
     cmp(weighted_median(x, w), x[1]);
     w[0] = 1; w[1] = 1; w[2] = 0; w[3] = 1;
     cmp(weighted_median(x, w), x[1]);
     w[0] = 1; w[1] = 0; w[2] = 1; w[3] = 1;
     cmp(weighted_median(x, w), x[2]);
     w[0] = 0; w[1] = 1; w[2] = 1; w[3] = 1;
     cmp(weighted_median(x, w), x[2]);
     w[0] = 1; w[1] = 1; w[2] = 1; w[3] = 1;
     cmp(weighted_median(x, w), 0.5 * (x[1] + x[2]));

     w = 0;
     try {
          weighted_median(x, w);
          SHG_ASSERT(0);
     } catch (const std::invalid_argument&) {}
}

void test_laplace_mixture() {
     Vecdouble w(3), mu(3), lambda(3);
     w(0) = 1.0 / 2.0; mu(0) = -1.0; lambda(0) = 1.0;
     w(1) = 1.0 / 3.0; mu(1) =  0.0; lambda(1) = 2.0;
     w(2) = 1.0 / 6.0; mu(2) =  2.0; lambda(2) = 1.0;

     // Moments about zero of this mixture:
     const double m1 = -1.0 / 6.0;
     // const double m2 = 31.0 / 6.0;
     // const double m3 = -1.0 / 6.0;
     // const double m4 = 967.0 / 6.0;
     // Central moments of this mixture:
     const double nu2 = 185.0 / 36.0;   // variance
     const double nu3 = 520.0 / 216.0;
     const double nu4 = 209841.0 / 1296.0;

     const double sdev = sqrt(nu2);
     const double skew = nu3 / nu2 / sdev;
     const double curt = nu4 / sqr(nu2) - 3.0;

     Laplace_mixture m(w, mu, lambda);
     const double accu = 1e-14;
     SHG_ASSERT(abs(m1 - m.mean()) < accu);
     SHG_ASSERT(abs(sdev - m.sdev()) < accu);
     SHG_ASSERT(abs(skew - m.skew()) < accu);
     SHG_ASSERT(abs(curt - m.curt()) < accu);

     // Test generate().
     Vecdouble x;
     SHG::MZT mzt;
     m.generate(mzt, 4000, x);

     // Test EM algorithm.
     Unilapmixmod u(x, 3);
     u.pi = w;
     u.mu = mu;
     u.lambda = lambda;
     u.estep();
     u.mstep();
     int iter = 0;
     for (;;) {
          iter++;
          if (abs(u.estep()) < 1e-7)
               break;
          u.mstep();
     }
     // Results should be:
     Vecdouble w0(3), mu0(3), lambda0(3);
     w0(0) = 0.46334203;
     mu0(0) = -1.07436524;
     lambda0(0) = 0.92922847;
     w0(1) = 0.38795240;
     mu0(1) = 0.24140911;
     lambda0(1) = 2.01801607;
     w0(2) = 0.14870557;
     mu0(2) = 2.00744725;
     lambda0(2) = 1.00035131;
     const double accu2 = 1e-8;
     SHG_ASSERT(iter == 347);
     for (int i = 0; i < 3; i++) {
          SHG_ASSERT(abs(u.pi(i) - w0(i)) < accu2);
          SHG_ASSERT(abs(u.mu(i) - mu0(i)) < accu2);
          SHG_ASSERT(abs(u.lambda(i) - lambda0(i)) < accu2);
     }
}

}       // anonymous namespace

void test_laplace() {
     test_laplace_distribution();
     test_median();
     test_weighted_median();
     test_laplace_mixture();
}

}       // namespace Testing
}       // namespace SHG
