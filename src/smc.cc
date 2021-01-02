/**
 * \file src/smc.cc
 * Semi-Markov chain.
 * \date Created on 12 December 2011.
 */

#include <shg/smc.h>
#include <cmath>
#include <limits>
#include <string>
#include <shg/mconsts.h>
#include <shg/mstat.h>

namespace SHG {

using std::abs;
using std::exp;
using std::log;
using std::numeric_limits;
using std::size_t;
using std::sqrt;
using std::string;

SMC::STD::~STD() {}

SMC::Finite::Finite(const Vecint& x, const Vecdouble& p)
     : x(x), p(p) {
     SHG_ASSERT(x.size() == p.size());
     double sum = 0.0;
     for (size_t i = 0; i < x.size(); i++) {
          SHG_ASSERT(x[i] > 0);
          SHG_ASSERT(p[i] > 0.0);
          sum += p[i];
     }
     SHG_ASSERT(abs(sum - 1.0) < 1e-4);
}

int SMC::Finite::generate(RNG& g) const {
     const int i = g.finite(p);
     SHG_ASSERT(static_cast<size_t>(i) < x.size());
     return x[i];
}

SMC::Geometric::Geometric(double p) : p(p) {
     SHG_ASSERT(0.0 < p && p <= 1.0);
}

int SMC::Geometric::generate(RNG& g) const {
     const unsigned int x = g.geometric(p);
     SHG_ASSERT(x > 0 && x <= static_cast<unsigned int>(
                                   numeric_limits<int>::max()));
     return x;
}

SMC::Negative_binomial::Negative_binomial(double t, double p)
     : t(t), p(p) {
     SHG_ASSERT(t > 0.0 && p > 0.0 && p < 1.0);
}

int SMC::Negative_binomial::generate(RNG& g) const {
     const unsigned int x = g.negative_binomial(t, p);
     SHG_ASSERT(
          x < static_cast<unsigned int>(numeric_limits<int>::max()));
     return x + 1;
}

SMC::Logarithmic::Logarithmic(double p) : p(p) {
     SHG_ASSERT(p > 0.0 && p < 1.0);
}

int SMC::Logarithmic::generate(RNG& g) const {
     const unsigned long x = g.logarithmic(p);
     SHG_ASSERT(x > 0 && x <= static_cast<unsigned long>(
                                   numeric_limits<int>::max()));
     return x;
}

SMC::Poisson::Poisson(double mu) : mu(mu) {
     SHG_ASSERT(mu > 0.0);
}

int SMC::Poisson::generate(RNG& g) const {
     const unsigned int x = g.poisson(mu);
     SHG_ASSERT(
          x < static_cast<unsigned int>(numeric_limits<int>::max()));
     return x + 1;
}

SMC::SMC(std::size_t s)
     : s_(s),
       alpha_(s),
       P_(s, s),
       std_(0),
       g_(0),
       X(),
       S(),
       J(),
       Z() {
     SHG_ASSERT(s > 0);
}

SMC::~SMC() {
     delete std_;
     delete g_;
}

int SMC::generate(int T, bool cut) {
     int status = check_data();
     if (status)
          return status;
     if (T < 1)
          return 6;

     // Generate successive sojourn times while their sum is less than
     // T. Use Z to keep them temporarily. The number of generated
     // sojourn times is kept in n and their sum in k.
     Z.resize(T);
     int j, k = 0;
     size_t n = 0;
     while (k < T)
          k += (Z[n++] = std_->generate(*g_));
     if (k > T) {
          if (cut)
               Z[n - 1] -= k - T;
          else
               T = k;
     }
     X.resize(n);
     S.resize(n);
     J.resize(n);
     for (size_t i = 0; i < n; i++)
          X[i] = Z[i];
     Z.resize(T);
     Vecdouble q(s_);
     S[0] = 0;
     J[0] = k = g_->finite(alpha_);
     int t = 0;
     for (j = 0; j < X[0]; j++)
          Z[t++] = k;
     for (size_t i = 1; i < n; i++) {
          S[i] = S[i - 1] + X[i - 1];
          // generate J[i];
          for (size_t j = 0; j < s_; j++)
               q[j] = P_[k][j];
          J[i] = k = g_->finite(q);
          for (int j = 0; j < X[i]; j++) {
               SHG_ASSERT(t < T);
               Z[t++] = k;
          }
     }

     // Check assertions.
     SHG_ASSERT(S[0] == 0);
     for (size_t i = 0; i < n; i++) {
          if (i > 0) {
               SHG_ASSERT(J[i] != J[i - 1]);
               SHG_ASSERT(S[i] == S[i - 1] + X[i - 1]);
          }
          for (t = S[i]; t < S[i] + X[i]; t++)
               SHG_ASSERT(Z[t] == J[i]);
     }
     return 0;
}

// Returns: 0 - ok, 1 - invalid dimension of alpha_ or P_, 2 -
// incorrect value of alpha_, 3 - incorrect value of P_, 4 - std_ not
// assigned, 5 - g_ not assigned.
int SMC::check_data() {
     const double eps = 1e-12;
     if (alpha_.size() != s_ || P_.nrows() != s_ || P_.ncols() != s_)
          return 1;
     double sa = 0.0, sp;
     size_t i, j;
     for (i = 0; i < s_; i++) {
          if (alpha_[i] < 0.0 || alpha_[i] > 1.0)
               return 2;
          sa += alpha_[i];
          sp = 0.0;
          for (j = 0; j < s_; j++) {
               const double p = P_(i, j);
               if (p < 0.0 || p > 1.0)
                    return 3;
               sp += p;
          }
          if (abs(P_(i, i)) >= eps)
               return 3;
          if (abs(sp - 1.0) >= eps)
               return 3;
     }
     if (abs(sa - 1.0) >= eps)
          return 2;
     if (std_ == 0)
          return 4;
     if (g_ == 0)
          return 5;
     return 0;
}

Unideggaumix::Unideggaumix(int n, int K)
     : n(n),
       K(K),
       K1(K - 1),
       x(n),
       pi(K),
       mu(K1),
       sigma(K1),
       x0(),
       fx0(),
       eps(),
       maxit(),
       psi(n, K),
       loglik(),
       iter(),
       status(1) {}

void Unideggaumix::estimate() {
     double s = 0.0;
     status = 2;
     for (int i = 0; i < K; i++)
          if (pi(i) <= 0.0)
               return;
          else
               s += pi(i);
     for (int i = 0; i < K; i++)
          pi(i) /= s;
     status = 3;
     for (int i = 0; i < K1; i++)
          if (sigma(i) <= 0.0)
               return;
     status = 4;
     if (eps < 0.0)
          return;
     status = 5;
     if (maxit < 1)
          return;
     iter = 0;
     Vector<bool> eq(n);
     for (int i = 0; i < n; i++)
          eq(i) = abs(x(i) - x0) < 1e-12;
     status = 7;
     if (fx0 < 1.0)
          return;
     // oldloglik is initialized only to shut up compiler warnings
     double oldloglik = 0.0, invsigma, psiik;
     for (;;) {
          // e-step, calculate loglik
          loglik = 0.0;
          for (int i = 0; i < n; i++) {
               s = 0.0;
               for (int k = 0; k < K1; k++) {
                    invsigma = 1.0 / sigma(k);
                    psiik =
                         SHG::Constants::isqrt2pi<double> * invsigma *
                         exp(-0.5 * sqr((x(i) - mu(k)) * invsigma));
                    s += (psi(i, k) = pi(k) * psiik);
               }
               if (eq(i))
                    s += (psi(i, K1) = pi(K1) * fx0);
               else
                    psi(i, K1) = 0.0;
               SHG_ASSERT(s > 0.0);
               for (int k = 0; k < K; k++)
                    psi(i, k) /= s;
               loglik += log(s);
          }
          s = pi(K1);
          for (int k = 0; k < K1; k++)
               s += pi(k);
          if (++iter > 1) {
               if (abs(loglik - oldloglik) < eps) {
                    status = 0;
                    return;
               }
               oldloglik = loglik;
          }
          status = 6;  // maxiter exceeded
          if (iter >= maxit)
               return;
          // m-step
          for (int k = 0; k < K1; k++) {
               double s = 0.0, s1 = 0.0, s2 = 0.0;
               const double muk = mu(k);
               for (int i = 0; i < n; i++) {
                    const double psiik = psi(i, k);
                    const double xi = x(i);
                    s += psiik;
                    s1 += psiik * xi;
                    s2 += psiik * sqr(xi - muk);
               }
               pi(k) = s / n;
               SHG_ASSERT(s > 0.0);
               mu(k) = s1 / s;
               sigma(k) = sqrt(s2 / s);
               SHG_ASSERT(sigma(k) > 0.0);
          }
          // k == K1
          s = 0.0;
          for (int i = 0; i < n; i++)
               s += psi(i, K1);
          pi(K1) = s / n;
     }
}

}  // namespace SHG
