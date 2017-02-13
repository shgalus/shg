/* smc.cc: testing semi-Markov chain */

/**
 * \file testing/smc.cc
 * Testing semi-Markov chain.
 * \date Created on 12 December 2011.
 */

#include <cmath>
#include <iostream>
#include <memory>
#include "shg/utils.h"
#include "shg/mzt.h"
#include "shg/smc.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::unique_ptr;
using SHG::Vecint;
using SHG::Vecdouble;
using SHG::SMC;

namespace {

int generate(SHG::MZT& g, SMC::STD& d) {
     int s = 0;
     for (int i = 0; i < 1000; i++) {
          const int x = d.generate(g);
          SHG_ASSERT(x > 0);
          s += x;
     }
     return s;
}

void test_STD() {
     SHG::MZT g;
     unique_ptr<SMC::STD> a;
     int s = 0;

     Vecint x(3);
     Vecdouble p(3);
     x[0] = 1; x[1] = 2; x[2] = 3;
     p[0] = 0.5; p[1] = 1.0 / 3.0; p[2] = 1.0 / 6.0;
     a.reset(new SMC::Finite(x, p));
     s += generate(g, *a);

     a.reset(new SMC::Geometric(0.5));
     s += generate(g, *a);

     a.reset(new SMC::Negative_binomial(0.5, 0.5));
     s += generate(g, *a);

     a.reset(new SMC::Logarithmic(0.5));
     s += generate(g, *a);

     a.reset(new SMC::Poisson(0.5));
     s += generate(g, *a);
     SHG_ASSERT(s == 8134);
}

// Results for example 1 (cut = true).

const struct {int x, s, j;} ex1xsj[4] = {
     {1, 0, 2},
     {4, 1, 3},
     {3, 5, 4},
     {1, 8, 0}
};

const int ex1z[9] = {2, 3, 3, 3, 3, 4, 4, 4, 0};

// Results for example 2 (cut = false).

const struct {int x, s, j;} ex2xsj[4] = {
     {1, 0, 2},
     {4, 1, 3},
     {3, 5, 4},
     {2, 8, 0}
};

const int ex2z[10] = {2, 3, 3, 3, 3, 4, 4, 4, 0, 0};

void test_SMC() {
     const int nstates = 5;
     SMC smc(5);
     for (int i = 0; i < nstates; i++) {
          smc.alpha_[i] = 1.0 / nstates;
          for (int j = 0; j < nstates; j++)
               smc.P_(i, j) = 1.0 / (nstates - 1);
          smc.P_(i, i) = 0.0;
     }
     Vecint x(4);
     Vecdouble p(4);
     x[0] = 1; p[0] = 0.4;
     x[1] = 2; p[1] = 0.3;
     x[2] = 3; p[2] = 0.2;
     x[3] = 4; p[3] = 0.1;
     smc.std_ = new SMC::Finite(x, p);

     // Generate with cut = true.
     smc.g_ = new SHG::MZT;
     SHG_ASSERT(smc.generate(9, true) == 0);
     SHG_ASSERT(smc.X.size() == 4 &&
                smc.S.size() == 4 &&
                smc.J.size() == 4);
     SHG_ASSERT(smc.Z.size() == 9);
     for (int i = 0; i < 4; i++) {
          SHG_ASSERT(smc.X[i] == ex1xsj[i].x);
          SHG_ASSERT(smc.S[i] == ex1xsj[i].s);
          SHG_ASSERT(smc.J[i] == ex1xsj[i].j);
     }
     for (int i = 0; i < 9; i++)
          SHG_ASSERT(smc.Z[i] == ex1z[i]);

     // Restart the same generator.
     delete smc.g_;
     smc.g_ = 0;

     // Generate with cut = false.
     smc.g_ = new SHG::MZT;
     SHG_ASSERT(smc.generate(9, false) == 0);
     SHG_ASSERT(smc.X.size() == 4 &&
                smc.S.size() == 4 &&
                smc.J.size() == 4);
     SHG_ASSERT(smc.Z.size() == 10);
     for (int i = 0; i < 4; i++) {
          SHG_ASSERT(smc.X[i] == ex2xsj[i].x);
          SHG_ASSERT(smc.S[i] == ex2xsj[i].s);
          SHG_ASSERT(smc.J[i] == ex2xsj[i].j);
     }
     for (int i = 0; i < 10; i++)
          SHG_ASSERT(smc.Z[i] == ex2z[i]);
}

void test_unideggaumix() {
     SHG::MZT g;
     SHG::Unideggaumix u(5000, 5);
     u.pi(0) = 0.1; u.mu(0) = -0.08; u.sigma(0) = 0.02;
     u.pi(1) = 0.2; u.mu(1) = -0.01; u.sigma(1) = 0.005;
     u.pi(2) = 0.3; u.mu(2) =  0.01; u.sigma(2) = 0.005;
     u.pi(3) = 0.2; u.mu(3) =  0.08; u.sigma(3) = 0.02;
     u.pi(4) = 0.2; u.x0 = 0; u.fx0 = 1e10;
     // Generate sample.
     int k = 0;
     for (int i = 0; i < 4; i++) {
          const int n = iceil(u.n * u.pi(i));
          const double mu = u.mu(i);
          const double sigma = u.sigma(i);
          for (int j = 0; j < n; j++)
               u.x(k++) = mu + sigma * g.normal();
     }
     while (k < u.n)
          u.x(k++) = u.x0;
     u.eps = 1e-5;
     u.maxit = 1000;
     u.estimate();
     header("test_unideggaumix()");
     logfile << "status = " << u.get_status() << '\n';
     logfile << std::scientific;
     if (u.get_status() == 0) {
          logfile << "N iters: " << u.iter << '\n';
          double s = 0.0;
          for (int k = 0; k < u.K; k++) {
               s += u.pi(k);
               logfile << u.pi(k);
               if (k < u.K1)
                    logfile << "   " << u.mu(k) << "   " << u.sigma(k);
               logfile << '\n';
          }
          logfile << s << '\n';
     }
     footer("test_unideggaumix()");
}

}       // anonymous namespace

void test_smc() {
     test_STD();
     test_SMC();
     test_unideggaumix();
}

}       // namespace Testing
}       // namespace SHG
