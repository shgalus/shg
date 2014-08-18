/* hmm.cc: testing hidden Markov models */

/**
 * \file testing/hmm.cc
 * Testing hidden Markov models.
 * \date Created on 1 May 2011.
 */

#include <cmath>
#include <cstddef>
#include "shg/except.h"
#include "shg/hmm.h"
#include "shg/mstat.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

void test_nhmm() {
     using std::size_t;
     using std::abs;
     using SHG::Vecdouble;
     using SHG::Matdouble;

     const int T = 5000;
     const int s = 3;

     // Correct results.
     const double reslogL = 1.046005701735e+04;
     const Matdouble resP(s, s, {
               7.858939e-01, 1.094508e-01, 1.046553e-01,
               6.822764e-02, 4.536568e-01, 4.781155e-01,
               8.339405e-02, 3.252405e-01, 5.913655e-01
           });
     const Vecdouble resp {3.189684e-39, 1.000000e+00, 0.000000e+00};
     const Vecdouble resmu {-4.996472e-02, 2.464085e-03,
               5.017120e-02};
     const Vecdouble ressigma {2.052194e-02, 3.020659e-02,
               9.825068e-03};
     const int resnequal = 4474;
     const SHG::Vecint resX {
          1, 2, 2, 1, 2, 1, 1, 1, 2, 2,
          2, 2, 2, 0, 0, 0, 0, 0, 2, 2,
          1, 2, 0, 0, 0, 2, 2, 2, 2, 1,
          2, 1, 2, 2, 1, 2, 2, 2, 2, 1,
          1, 2, 2, 0, 0, 2, 2, 2, 2, 2,
          2, 2, 0, 0, 1, 2, 2, 2, 2, 1,
          2, 2, 2, 2, 2, 2, 1, 2, 2, 2,
          1, 1, 2, 1, 2, 2, 1, 2, 2, 2,
          2, 2, 1, 2, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 1, 1, 2, 1, 2, 2, 2,
          2, 1, 1, 2, 2, 1, 1, 2, 2, 2,
          2, 2, 1, 2, 1, 1, 1, 1, 2, 2,
          2, 2, 1, 2, 2, 1, 2, 2, 1, 1,
          2, 1, 2, 1, 2, 0, 1, 1, 2, 2,
          2, 2, 2, 1, 1, 2, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 1, 2, 2, 2, 2,
          2, 2, 0, 1, 1, 2, 2, 1, 1, 2,
          1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 1, 1, 2, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 1, 1, 2, 0, 0, 1,
          1, 1, 1, 2, 0, 0, 0, 0, 2, 2,
          1, 1, 1, 2, 2, 0, 0, 0, 0, 1,
          1, 1, 2, 2, 2, 2, 2, 2, 2, 1,
          2, 1, 1, 2, 2, 2, 1, 2, 2, 0,
          0, 0, 0, 0, 0, 0, 2, 1, 2, 2,
          2, 2, 2, 2, 2, 2, 1, 1, 2, 1,
          1, 1, 2, 2, 1, 1, 1, 1, 2, 1,
          2, 2, 2, 1, 2, 2, 2, 1, 2, 2,
          2, 1, 2, 2, 2, 2, 1, 1, 1, 2
     };

     const double eps = 1e-7;
     SHG::MZT g;
     Matdouble P(s, s, {
               0.80, 0.10, 0.10,
               0.05, 0.50, 0.45,
               0.10, 0.30, 0.60
           });
     Vecdouble p {0.30, 0.50, 0.20}, mu {-0.05, 0.00, 0.05},
          sigma {0.02, 0.03, 0.01}, y;
     Vecint X;
     double mean, stdd, logL;
     int i, j, t, n, status;

     // Generate Markov chain.
     SHG::gen_nhmm(P, p, mu, sigma, T, y, X, g);

     // Initialize P, p, mu, sigma.
     for (i = 0; i < s; i++) {
          g.simplex_surface(p);
          for (j = 0; j < s; j++)
               P(i, j) = p(j);
     }
     g.simplex_surface(p);
     mean_var(y, mean, stdd);
     stdd = sqrt(stdd);
     mu(0) = mean - stdd;
     mu(1) = mean;
     mu(2) = mean + stdd;
     sigma = stdd;

     SHG::Normal_hmm h(P, p, mu, sigma, y);

     status = h.forwardbackward();
     SHG_ASSERT(status == 0);
     logL = h.logL;
     for (;;) {
          const double prevlogL = logL;
          status = h.baumwelch();
          SHG_ASSERT(status == 0);
          status = h.forwardbackward();
          SHG_ASSERT(status == 0);
          logL = h.logL;
          if (logL - prevlogL < 1e-12)
               break;
     }
     h.sort();
     h.viterbi();
     SHG_ASSERT(abs(logL - reslogL) < eps);
     for (i = 0; i < s; i++) {
          SHG_ASSERT(abs(h.p(i) - resp[i]) < eps);
          for (j = 0; j < s; j++)
               SHG_ASSERT(abs(h.P(i, j) - resP(i, j)) < eps);
          SHG_ASSERT(abs(h.mu(i) - resmu[i]) < eps);
          SHG_ASSERT(abs(h.sigma(i) - ressigma[i]) < eps);
     }
     n = 0;
     for (t = 0; t < T; t++)
          if (X[t] == h.X(t))
               n++;
     SHG_ASSERT(n == resnequal);

     // Check only resX.size() first values.
     SHG_ASSERT(resX.size() < h.X.size());
     for (size_t i = 0; i < resX.size(); i++)
          SHG_ASSERT(h.X(i) == resX(i));
}

}       // anonymous namespace

void test_hmm() {
     test_nhmm();
}

}       // namespace Testing
}       // namespace SHG
