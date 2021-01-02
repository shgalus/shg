/**
 * \file src/laplace.cc
 * Laplace distribution.
 * \date Created on 6 January 2013.
 */

#include <shg/laplace.h>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <shg/except.h>
#include <shg/utils.h>

namespace SHG {

using SHG::sqr;
using std::abs;
using std::exp;
using std::invalid_argument;
using std::log;
using std::size_t;
using std::sqrt;
using std::string;

Laplace_distribution::Laplace_distribution(double mu, double lambda)
     : mu(mu), lambda(lambda) {
     if (lambda <= 0.0)
          throw invalid_argument(
               "non-positive lambda in Laplace_distribution");
}

double Laplace_distribution::f(double x) {
     return 0.5 * exp(-abs(x - mu) / lambda) / lambda;
}

double Laplace_distribution::cdf(double x) {
     const double t = (x - mu) / lambda;
     return (t > 0.0) ? 1.0 - 0.5 * exp(-t) : 0.5 * exp(t);
}

double Laplace_distribution::invcdf(double p) {
     if (p <= 0.0 || p >= 1.0)
          throw invalid_argument(
               "bad p in Laplace_distribution::invcdf");
     return ((p *= 2.0) > 1.0) ? mu - lambda * log(2.0 - p)
                               : mu + lambda * log(p);
}

double median(const SHG::Vecdouble& x) {
     const int n = x.size();
     if (n < 1)
          throw invalid_argument("invalid dimension in median");
     const std::div_t r = std::div(n, 2);
     if (r.rem)
          return x[r.quot];
     return 0.5 * (x[r.quot - 1] + x[r.quot]);
}

double weighted_median(const SHG::Vecdouble& x,
                       const SHG::Vecdouble& w) {
     const size_t n = x.size();
     if (n < 1 || w.size() != n)
          throw invalid_argument(__func__);
     double u = 0.0;
     for (size_t i = 0; i < n; i++) {
          if (w[i] < 0.0)
               throw invalid_argument(__func__);
          if (i && (x[i] < x[i - 1]))
               throw invalid_argument(__func__);
          u -= w[i];
     }
     if (u >= 0.0)  // all weights equal to 0
          throw invalid_argument(__func__);
     int k = 0;
     do {
          u += 2.0 * w[k++];
     } while (u < 0.0);
     if (u > 0.0)
          return x[k - 1];
     int l = k;
     do {
          u += 2.0 * w[l++];
     } while (u <= 0.0);
     return 0.5 * (x[k - 1] + x[l - 1]);
}

Unilapmixmod::Degenerate_distribution::Degenerate_distribution()
     : Exception("degenerate distribution in m-step") {}

Unilapmixmod::Unilapmixmod(const SHG::Vecdouble& x, int K)
     : n(x.size()),
       K(K),
       x(x),
       pi(K),
       mu(K),
       lambda(K),
       psi(n, K),
       loglik(),
       x_sorted(n),
       order(n) {
     SHG_ASSERT(K >= 1);
     SHG_ASSERT(n >= 1);
     std::vector<double> y(n);
     for (int i = 0; i < n; i++)
          y[i] = x[i];
     auto v = indirect_sort(y);
     for (int i = 0; i < n; i++)
          order[i] = v[i];
     for (int i = 0; i < n; i++)
          x_sorted(i) = x(order(i));
}

double Unilapmixmod::estep() {
     const double oldloglik = loglik;
     double s;
     int i, k;
     loglik = 0.0;
     for (i = 0; i < n; i++) {
          const double xi = x(i);
          s = 0.0;
          for (k = 0; k < K; k++)
               s += (psi(i, k) = pi(k) * Laplace_distribution(
                                              mu(k), lambda(k))
                                              .f(xi));
          SHG_ASSERT(s > 0.0);
          for (k = 0; k < K; k++)
               psi(i, k) /= s;
          loglik += log(s);
     }
     return loglik - oldloglik;
}

void Unilapmixmod::mstep() {
     int i, k;
     double ak, bk, muk;
     SHG::Vecdouble w(n), w_sorted(n);
     for (k = 0; k < K; k++) {
          bk = 0.0;
          for (i = 0; i < n; i++)
               bk += (w(i) = psi(i, k));
          pi(k) = bk / n;
          for (i = 0; i < n; i++)
               w_sorted(i) = w(order(i));
          mu(k) = muk = weighted_median(x_sorted, w_sorted);
          ak = 0.0;
          for (i = 0; i < n; i++)
               ak += w(i) * abs(x(i) - muk);
          SHG_ASSERT(bk > 0.0);
          if ((lambda(k) = ak / bk) <= 0.0)
               throw Degenerate_distribution();
     }
}

Laplace_mixture::Error::Error()
     : Exception("Laplace mixture error") {}

Laplace_mixture::Laplace_mixture(const SHG::Vecdouble& w,
                                 const SHG::Vecdouble& mu,
                                 const SHG::Vecdouble& lambda)
     : n_(w.size()),
       w(w),
       mu(mu),
       lambda(lambda),
       moments_calculated(false),
       mean_(),
       sdev_(),
       skew_(),
       curt_() {
     if (n_ < 1 || mu.size() != n_ || lambda.size() != n_)
          throw Error();
     double s = 0.0;
     for (size_t i = 0; i < n_; i++) {
          if (lambda[i] <= 0.0 || w[i] < 0.0 || w[i] > 1.0)
               throw Error();
          s += w[i];
     }
     if (abs(s - 1.0) > 1e-8)
          throw Error();
}

double Laplace_mixture::p(double x) const {
     double s = 0.0;
     for (size_t i = 0; i < n_; i++)
          s += w(i) * Laplace_distribution(mu(i), lambda(i)).f(x);
     return s;
}

double Laplace_mixture::cdf(double x) const {
     double s = 0.0;
     for (size_t i = 0; i < n_; i++)
          s += w(i) * Laplace_distribution(mu(i), lambda(i)).cdf(x);
     return s;
}

double Laplace_mixture::invcdf(double p) const {
     if (p <= 0.0 || p >= 1.0)
          throw Error();
     double a = -1e3, b = 1e3, x = 0.0;
     if (p <= cdf(a) || p >= cdf(b))
          throw Error();

     // Bisection.
     while (b - a > 1e-10)
          if (cdf(x = 0.5 * (a + b)) < p)
               a = x;
          else
               b = x;
     return x;
}

double Laplace_mixture::mean() {
     moments();
     return mean_;
}
double Laplace_mixture::sdev() {
     moments();
     return sdev_;
}
double Laplace_mixture::skew() {
     moments();
     return skew_;
}
double Laplace_mixture::curt() {
     moments();
     return curt_;
}

void Laplace_mixture::generate(SHG::RNG& rng, int n,
                               SHG::Vecdouble& x) {
     x.resize(n);
     int i, k;
     for (i = 0; i < n; i++) {
          // Generate component.
          k = rng.finite(w);
          SHG_ASSERT(0 <= k && k < n);
          // Generate observation from this component.
          x(i) = rng.laplace(mu(k), lambda(k));
     }
}

void Laplace_mixture::write(std::ostream& f) const {
     using SHG::write;
     write(w, f);
     write(mu, f);
     write(lambda, f);
}

Laplace_mixture Laplace_mixture::read(std::istream& f) {
     SHG::Vecdouble w, mu, lambda;
     using SHG::read;
     read(w, f);
     read(mu, f);
     read(lambda, f);
     return Laplace_mixture(w, mu, lambda);
}

void Laplace_mixture::moments() {
     if (moments_calculated)
          return;
     double m1 = 0.0, m2 = 0.0, m3 = 0.0, m4 = 0.0;

     // Calculate the first four raw moments of the mixture.
     for (size_t i = 0; i < n_; i++) {
          const double wi = w[i];
          const double mu1 = mu[i];
          const double mu2 = sqr(mu1);
          const double lambda2 = sqr(lambda[i]);
          m1 += wi * mu1;
          m2 += wi * (mu2 + 2.0 * lambda2);
          m3 += wi * mu1 * (mu2 + 6.0 * lambda2);
          m4 += wi *
                (mu2 * (mu2 + 12.0 * lambda2) + 24.0 * sqr(lambda2));
     }
     mean_ = m1;
     const double sm = sqr(m1);
     const double var = m2 - sm;
     if (var <= 0.0)
          throw Error();
     sdev_ = sqrt(var);
     skew_ = (m3 - 3.0 * m1 * m2 + 2.0 * m1 * sm) / sdev_ / var;
     curt_ = (m4 - 4.0 * m1 * m3 + 6.0 * sm * m2 - 3.0 * sqr(sm)) /
                  var / var -
             3.0;
     moments_calculated = true;
}

}  // namespace SHG
