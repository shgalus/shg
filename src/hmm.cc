/**
 * \file src/hmm.cc
 * Hidden Markov models.
 * \date Created on 1 May 2011.
 */

#include <shg/hmm.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>
#include <shg/except.h>
#include <shg/mconsts.h>
#include <shg/specfunc.h>
#include <shg/utils.h>

namespace SHG {

using std::abs;
using std::atan;
using std::exp;
using std::invalid_argument;
using std::log;
using std::numeric_limits;
using std::size_t;
using std::sqrt;
using std::vector;

Normal_hmm::Normal_hmm(Matdouble const& P, Vecdouble const& p,
                       Vecdouble const& mu, Vecdouble const& sigma,
                       Vecdouble const& y)
     : T_(y.size()),
       s_(p.size()),
       P(P),
       p(p),
       mu(mu),
       sigma(sigma),
       y(y),
       logL(),
       gamma(T_, s_),
       logprob(),
       X(T_),
       alpha(T_, s_),
       beta(T_, s_),
       c(T_),
       d(T_),
       aux(T_) {
     if (P.nrows() != s_ || P.ncols() != s_ || mu.size() != s_ ||
         sigma.size() != s_ || T_ < 2 || s_ < 1)
          throw invalid_argument("invalid dimensions in Normal_hmm");
     double const eps = 0.01;
     double u;
     size_t i, j;
     for (i = 0, u = 0.0; i < s_; i++)
          u += p(i);
     if (abs(u - 1.0) > eps)
          throw invalid_argument(
               "initial state probabilities "
               "not normalized in Normal_hmm");
     for (i = 0; i < s_; i++) {
          for (j = 0, u = 0.0; j < s_; j++)
               u += P(i, j);
          if (abs(u - 1.0) > eps)
               throw invalid_argument(
                    "transition matrix "
                    "not normalized in Normal_hmm");
     }
     for (i = 0; i < s_; i++)
          if (sigma(i) <= 0.0)
               throw invalid_argument(
                    "standard deviation "
                    "not positive in Normal_hmm");
}

int Normal_hmm::forwardbackward() {
     size_t i, j, t, t1;
     double sum, u;

     u = 0.0;
     for (j = 0; j < s_; j++)
          u += alpha(0, j) = p(j) * phi(0, j);
     if (u <= 0.0)
          return 1;
     SHG_ASSERT(u > 0.0);
     c(0) = u = 1.0 / u;
     for (j = 0; j < s_; j++)
          alpha(0, j) *= u;
     logL = -log(u);
     for (t = 1; t < T_; t++) {
          t1 = t - 1;
          u = 0.0;
          for (j = 0; j < s_; j++) {
               sum = 0.0;
               for (i = 0; i < s_; i++)
                    sum += alpha(t1, i) * P(i, j);
               u += alpha(t, j) = sum * phi(t, j);
          }
          // This may happen that in an iteration phi(t, j) is
          // extremaly small but positive (like 1e-320). Thus alpha(t,
          // j) may be set to an extremely big number which produces
          // overflow in the next iteration.
          if (u < 1e-70)
               return 2;
          SHG_ASSERT(u > 0.0);
          c(t) = u = 1.0 / u;
          logL -= log(u);
          for (j = 0; j < s_; j++)
               alpha(t, j) *= u;
     }

     d(T_ - 1) = u = 1.0 / s_;
     for (i = 0; i < s_; i++)
          beta(T_ - 1, i) = u;
     for (t1 = T_ - 1; t1 > 0; t1--) {
          t = t1 - 1;
          u = 0.0;
          for (i = 0; i < s_; i++) {
               sum = 0.0;
               for (j = 0; j < s_; j++)
                    sum += P(i, j) * phi(t1, j) * beta(t1, j);
               u += beta(t, i) = sum;
          }
          if (u <= 0.0)
               return 3;
          SHG_ASSERT(u > 0.0);
          d(t) = u = 1.0 / u;
          for (i = 0; i < s_; i++)
               beta(t, i) *= u;
     }

     // Calculate gamma(t, i) and aux(t).
     for (t = 0; t < T_; t++) {
          sum = 0.0;
          for (i = 0; i < s_; i++) {
               u = alpha(t, i) * beta(t, i);
               sum += u;
               gamma(t, i) = u;
          }
          if (sum <= 0.0)
               return 4;
          SHG_ASSERT(sum > 0.0);
          for (i = 0; i < s_; i++)
               gamma(t, i) /= sum;
          aux(t) = sum;
     }
     return 0;
}

int Normal_hmm::baumwelch() {
     size_t const T1 = T_ - 1;
     Vecdouble sg(s_), sg1(s_);
     double u, pij, sg1i, mui;
     size_t i, j, t;

     for (i = 0; i < s_; i++) {
          u = 0.0;
          for (t = 0; t < T1; t++)
               u += gamma(t, i);
          sg1(i) = u;
          if (u <= 0.0)
               return 5;
          SHG_ASSERT(u > 0.0);
          sg(i) = u + gamma(T1, i);
          SHG_ASSERT(sg(i) > 0.0);
     }
     for (i = 0; i < s_; i++)
          p(i) = gamma(0, i);
     for (i = 0; i < s_; i++) {
          sg1i = sg1(i);
          for (j = 0; j < s_; j++) {
               pij = P(i, j);
               u = 0.0;
               for (t = 0; t < T1; t++)
                    u += d(t) * alpha(t, i) * beta(t + 1, j) * pij *
                         phi(t + 1, j) / aux(t);
               P(i, j) = u / sg1i;
               SHG_ASSERT(P(i, j) >= 0.0 && P(i, j) - 1.0 < 1e-8);
          }
     }
     for (i = 0; i < s_; i++) {
          u = 0.0;
          for (t = 0; t < T_; t++)
               u += gamma(t, i) * y(t);
          mu(i) = u / sg(i);
     }
     for (i = 0; i < s_; i++) {
          u = 0.0;
          mui = mu(i);
          for (t = 0; t < T_; t++)
               u += gamma(t, i) * sqr(y(t) - mui);
          SHG_ASSERT(u >= 0);
          sigma(i) = u = sqrt(u / sg(i));
          if (u <= 0.0)
               return 6;
          SHG_ASSERT(u > 0.0);
     }
     return 0;
}

void Normal_hmm::viterbi() {
     static double const mind = -numeric_limits<double>::max();
     size_t const T1 = T_ - 1;
     Matdouble delta(T_, s_);
     Matint psi(T_, s_);
     Matdouble logp(s_, s_);
     size_t i, j, t, t1, imax;
     double d, dmax;

     for (i = 0; i < s_; i++)
          for (j = 0; j < s_; j++)
               logp(i, j) = P(i, j) > 0.0 ? log(P(i, j)) : mind;
     for (i = 0; i < s_; i++) {
          delta(0, i) =
               p(i) > 0.0 ? log(p(i)) + log(phi(0, i)) : mind;
          psi(0, i) = 0;  // unneeded
     }
     for (t = 1; t < T_; t++) {
          t1 = t - 1;
          for (j = 0; j < s_; j++) {
               imax = 0;
               dmax = delta(t1, 0) > mind && logp(0, j) > mind
                           ? delta(t1, 0) + logp(0, j)
                           : mind;
               for (i = 1; i < s_; i++) {
                    d = delta(t1, i) > mind && logp(i, j) > mind
                             ? delta(t1, i) + logp(i, j)
                             : mind;
                    if (d > dmax) {
                         dmax = d;
                         imax = i;
                    }
               }
               delta(t, j) = dmax + log(phi(t, j));
               psi(t, j) = imax;
          }
     }
     imax = 0;
     logprob = delta(T1, 0);
     for (i = 1; i < s_; i++)
          if (delta(T1, i) > logprob) {
               logprob = delta(T1, i);
               imax = i;
          }
     X(T1) = imax;
     for (t = T1; t > 0; t--)
          X(t - 1) = psi(t, X(t));
}

void Normal_hmm::sort() {
     vector<double> w(s_);
     size_t i, j;
     for (i = 0; i < s_; i++)
          w[i] = mu(i);
     vector<size_t> v = indirect_sort(w);
     Vecdouble e(s_);

     for (i = 0; i < s_; i++)
          e(i) = p(v[i]);
     p = e;
     for (i = 0; i < s_; i++)
          e(i) = mu(v[i]);
     mu = e;
     for (i = 0; i < s_; i++)
          e(i) = sigma(v[i]);
     sigma = e;
     // exchange columns
     for (i = 0; i < s_; i++) {
          for (j = 0; j < s_; j++)
               e(j) = P(i, v[j]);
          for (j = 0; j < s_; j++)
               P(i, j) = e(j);
     }
     // exchange rows
     for (j = 0; j < s_; j++) {
          for (i = 0; i < s_; i++)
               e(i) = P(v[i], j);
          for (i = 0; i < s_; i++)
               P(i, j) = e(i);
     }
}

// phi(y[t], mu[j], sigma[j])
double Normal_hmm::phi(size_t t, size_t j) {
     SHG_ASSERT(sigma(j) > 0.0);
     double const s = 1.0 / sigma[j];
     double const x = (y[t] - mu[j]) * s;
     return Constants::isqrt2pi<double> * s * exp(-0.5 * x * x);
}

void gen_nhmm(Matdouble const& P, Vecdouble const& p,
              Vecdouble const& mu, Vecdouble const& sigma,
              std::size_t const T, Vecdouble& y, Vecint& X, MZT& g) {
     size_t const s = P.nrows();
     SHG_ASSERT(s == P.ncols() && s == p.size() && s == mu.size() &&
                s == sigma.size());
     SHG_ASSERT(T > 0);

     Vecdouble q(s);
     y.resize(T);
     X.resize(T);
     int i;
     X[0] = i = g.finite(p);
     y[0] = mu[i] + g.normal() * sigma[i];
     for (size_t t = 1; t < T; t++) {
          // Catastrophe.
          for (size_t j = 0; j < s; j++)
               q[j] = P[i][j];
          X[t] = i = g.finite(q);
          y[t] = mu[i] + g.normal() * sigma[i];
     }
}

void corr_hmm(Vecdouble const& delta, Matdouble const& gamma,
              Vecdouble const& G, double const E, double const v,
              int const K, Vecdouble& r) {
     SHG_ASSERT(K > 0);
     SHG_ASSERT(v > 0.0);
     int const m = delta.size();
     double const E2 = E * E;
     r.resize(K + 1);
     r(0) = 1.0;
     Matdouble gammak =
          diagonal_matrix<double>(static_cast<size_t>(m));
     double s, s1;
     int i, j, k;
     for (k = 1; k <= K; k++) {
          // Calculate E(g(X_t) * g(X_{t + k})).
          right_multiply_and_assign(gammak, gamma);
          s = 0.0;
          for (i = 0; i < m; i++) {
               s1 = 0.0;
               for (j = 0; j < m; j++)
                    s1 += G(j) * gammak(i, j);
               s += delta(i) * G(i) * s1;
          }
          r(k) = (s - E2) / v;
     }
}

namespace {

void corr_nhmm_id(Vecdouble const& delta, Matdouble const& gamma,
                  Vecdouble const& mu, Vecdouble const& sigma,
                  int const K, Vecdouble& r) {
     double E = 0.0;  // E(X_t)
     double v = 0.0;  // var(X_t)

     for (size_t i = 0; i < delta.size(); i++) {
          E += delta(i) * mu(i);
          v += delta(i) * (sqr(mu(i)) + sqr(sigma(i)));
     }
     v -= sqr(E);
     // E(g(X_t) | C_t = i) = mu(i), so we have G = mu.
     corr_hmm(delta, gamma, mu, E, v, K, r);
}

void corr_nhmm_abs(Vecdouble const& delta, Matdouble const& gamma,
                   Vecdouble const& mu, Vecdouble const& sigma,
                   int const K, Vecdouble& r) {
     static double const  // sqrt(2/pi)
          C = 2.0 * SHG::Constants::isqrt2pi<double>;
     int const m = delta.size();
     double E = 0.0;  // E(X_t)
     double v = 0.0;  // var(X_t)
     Vecdouble G(m);  // E(g(X_t) | C_t = i)
     double mi, si, z;

     for (int i = 0; i < m; i++) {
          mi = mu(i);
          si = sigma(i);
          SHG_ASSERT(si > 0.0);
          z = mi / si;
          G(i) = C * si * exp(-0.5 * sqr(z)) +
                 mi * (1.0 - 2.0 * normal_integral(-z));
          E += delta(i) * G(i);
          v += delta(i) * (sqr(mi) + sqr(si));
     }
     v -= sqr(E);
     corr_hmm(delta, gamma, G, E, v, K, r);
}

void corr_nhmm_sqr(Vecdouble const& delta, Matdouble const& gamma,
                   Vecdouble const& mu, Vecdouble const& sigma,
                   int const K, Vecdouble& r) {
     int const m = delta.size();
     double E = 0.0;  // E(X_t)
     double v = 0.0;  // var(X_t)
     Vecdouble G(m);  // E(g(X_t) | C_t = i)
     double m2, s2, z;

     for (int i = 0; i < m; i++) {
          m2 = sqr(mu(i));
          s2 = sqr(sigma(i));
          G(i) = m2 + s2;
          E += delta(i) * G(i);
          z = m2 * (m2 + 6.0 * s2) + 3.0 * sqr(s2);
          v += delta(i) * z;
     }
     v -= sqr(E);
     corr_hmm(delta, gamma, G, E, v, K, r);
}

}  // anonymous namespace

void corr_nhmm(Vecdouble const& delta, Matdouble const& gamma,
               Vecdouble const& mu, Vecdouble const& sigma,
               int const function_g, int const K, Vecdouble& r) {
     switch (function_g) {
     case 0:
          corr_nhmm_id(delta, gamma, mu, sigma, K, r);
          break;
     case 1:
          corr_nhmm_abs(delta, gamma, mu, sigma, K, r);
          break;
     case 2:
          corr_nhmm_sqr(delta, gamma, mu, sigma, K, r);
          break;
     default:
          SHG_ASSERT(0);
          break;
     }
}

}  // namespace SHG
