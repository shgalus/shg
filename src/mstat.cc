/* mstat.cc: mathematical statistics */

/**
 * \file src/mstat.cc
 * Mathematical statistics.
 * \date Created on 16 June 2009.
 */

#include <cmath>
#include <algorithm>
#include "shg/brent.h"
#include "shg/except.h"         // SHG_ASSERT
#include "shg/mconsts.h"
#include "shg/specfunc.h"
#include "shg/utils.h"          // iceil, ifloor, sqr
#include "shg/mstat.h"

namespace SHG {

using std::abs;
using std::exp;
using std::sqrt;
using std::pow;
using std::size_t;
using std::vector;
using std::invalid_argument;
using std::runtime_error;
using std::range_error;
using std::isfinite;

double ksdist(double x) {
     using SHG::Constants::sqrt2pi;
     using SHG::Constants::sqrpi8;
     if (x < 0.18)
          return 0.0;
     if (x < 1.18) {
          const double y = exp(-sqrpi8<double>() / sqr(x));
          return sqrt2pi<double>() / x *
               (y + pow(y, 9) + pow(y, 25) + pow(y, 49));
     } else {
          const double y = exp(-2.0 * sqr(x));
          return 1.0 - 2.0 * (y - pow(y, 4) + pow(y, 9));
     }
}

double mean(const Vecdouble& x) {
     double s = 0.0;
     const int n = x.size();
     for (int i = 0; i < n; i++)
          s += (x[i] - s) / (i + 1);
     return s;
}

void mean_var(const Vecdouble& x, double& mean, double& var) {
     const int n = x.size();
     if (n < 1)
          throw invalid_argument(__func__);
     mean = x[0];
     var = 0.0;
     double d;
     for (int i = 1; i < n; i++) {
          d = x[i] - mean;
          mean += d / (i + 1);
          var += (x[i] - mean) * d;
     }
     var /= n;
     if (var < 0.0)
          throw runtime_error(__func__);
}

double stddev(const double* x, size_t n) {
     if (n == 0)
          throw invalid_argument(__func__);
     double m = x[0], s = 0.0, d;
     for (size_t i = 1; i < n; i++) {
          d = x[i] - m;
          m += d / (i + 1);
          s += (x[i] - m) * d;
     }
     s /= n;
     s = sqrt(s);
     if (!isfinite(s))
          throw range_error(__func__);
     return s;
}

void mean_var1(const Vecdouble& x, double& mean, double& var) {
     const int n = x.size();
     if (n < 2)
          throw invalid_argument(__func__);
     mean = x[0];
     var = 0.0;
     double d;
     for (int i = 1; i < n; i++) {
          d = x[i] - mean;
          mean += d / (i + 1);
          var += (x[i] - mean) * d;
     }
     var /= (n - 1);
     if (var < 0.0)
          throw runtime_error(__func__);
}

void acf(const Vecdouble& x, int K, Vecdouble& r) {
     acf(x, mean(x), K, r);
}

void acf(const Vecdouble& x, const double mean, const int K,
         Vecdouble& r) {
     const int n = x.size();
     if (n < 1 || K < 0)
          throw invalid_argument(__func__);
     int k, i, nk;
     double s, c = 0.0;
     for (i = 0; i < n; i++)
          c += sqr((x(i) - mean));
     if (c <= 1e-13)
          throw runtime_error("second moment equals to 0 in acf");
     r.resize(K + 1);
     r(0) = 1.0;
     for (k = 1; k <= K; k++) {
          s = 0.0;
          nk = n - k;
          for (i = 0; i < nk; i++)
               s += (x(i) - mean) * (x(i + k) - mean);
          r(k) = s / c;
     }
}

double chi2normtest(const Vecdouble& x, const int r) {
     const int n = x.size();
     if (n < 2 || r < 4)
          throw invalid_argument(__func__);
     const int r1 = r - 1;
     const double np = static_cast<double>(n) / static_cast<double>(r);
     double m, s, xi;
     int i, j;

     /* Calculate mean and standard deviation. */
     mean_var(x, m, s);
     s = sqrt(s);

     if (s <= 1e-13)
          throw runtime_error("sigma equals to 0 in chi2normtest");
     Vecint c(r, 0);
     Vecdouble a(r1);

     /* Calculate beginnings of equiprobable intervals. */
     for (i = 0; i < r1; i++)
          a(i) = m + s * ppnd7((i + 1) / static_cast<double>(r));

     /* Calculate interval frequency. */
     for (i = 0; i < n; i++) {
          xi = x[i];
          for (j = 0; j < r1; j++)
               if (xi < a(j))
                    break;
          c(j)++;
     }

     /* Calculate chi2. */
     double chi2 = 0.0;
     for (i = 0; i < r; i++)
          chi2 += sqr(c(i) - np);

     return 1.0 - gammad(0.5 * chi2 / np, 0.5 * (r - 3));
}

double chi2stdnormtest(const Vecdouble& x, int r) {
     const int n = x.size();
     if (n < 2 || r < 4)
          throw invalid_argument(__func__);
     const int r1 = r - 1;
     const double np = static_cast<double>(n) / static_cast<double>(r);
     double xi;
     int i, j;

     Vecint c(r, 0);
     Vecdouble a(r1);

     /* Calculate beginnings of equiprobable intervals. */
     for (i = 0; i < r1; i++)
          a(i) = ppnd7((i + 1) / static_cast<double>(r));

     /* Calculate interval frequency. */
     for (i = 0; i < n; i++) {
          xi = x[i];
          for (j = 0; j < r1; j++)
               if (xi < a(j))
                    break;
          c(j)++;
     }

     /* Calculate chi2. */
     double chi2 = 0.0;
     for (i = 0; i < r; i++)
          chi2 += sqr(c(i) - np);

     return 1.0 - gammad(0.5 * chi2 / np, 0.5 * (r - 1));
}

void ksnormtest(const Vecdouble& x, double& d, double& prob) {
     Vecdouble v(x);
     double m, s;
     mean_var(v, m, s);
     s = sqrt(s);
     if (s <= 1e-13)
          throw runtime_error("sigma equals to 0 in ksnormtest");
     for (size_t i = 0; i < v.size(); i++)
          v[i] = (v[i] - m) / s;
     auto cdf = [](double x) {return normal_integral(x);};
     ksone(cdf, v, d, prob);
}

Sample::Sample(const std::vector<double>& v) :
     data(srt(v)), n(data.size()), en(n) {
     if (n == 0)
          throw invalid_argument(__func__);
}

double Sample::cdf(double x) const {
     const vector<double>::const_iterator b = data.begin();
     return (upper_bound(b, data.end(), x) - b) / en;
}

double Sample::lcdf(double x) const {
     const vector<double>::const_iterator b = data.begin();
     return (lower_bound(b, data.end(), x) - b) / en;
}

double Sample::quantile(double p) const {
     SHG_VALIDATE(0 < p && p < 1);
     return data[iceil(n * p) - 1];
}

double Sample::range() const {
     return max() - min();
}

double Sample::iqr() const {
     return quantile(0.75) - quantile(0.25);
}

double Sample::min() const {
     return data[0];
}

double Sample::max() const {
     return data[n - 1];
}

Sample::Histdata::Histdata(double min, double max)
     : min(min), max(max), h(0.0), maxheight(0.0), f(0) {}

Sample::Histdata Sample::histogram() const {
     double h = 2.0 * pow(n, -1.0 / 3.0) * iqr();
     /*
      * It may happen that iqr() == 0. This means that cdf has a jump
      * in a certain point x0 which is of the length at least 1/2. In
      * this case at least half of the sample has a constant value. We
      * are trying with the full range.
      */
     if (h < eps)
          h = 2.0 * pow(n, -1.0 / 3.0) * range();
     if (h < eps)
          return Histdata(0.0, 0.0);
     return histogram(iceil(range() / h));
}

Sample::Histdata Sample::histogram(int k) const {
     if (k <= 0)
          return Histdata(0.0, 0.0);
     Histdata hd(min(), max());
     hd.h = (hd.max - hd.min) / k;
     if (hd.h >= eps) {
          hd.f.resize(k);
          double z = 0.0, z1, d;
          for (int j = 0, j1 = 1; j < k; j++, j1++) {
               z1 = j1 < k
                    ? lcdf(hd.min + hd.h * j1)
                    : 1.0;
               z1 /= hd.h;
               d = hd.f[j] = z1 - z;
               if (d > hd.maxheight)
                    hd.maxheight = d;
               z = z1;
          }
     }
     return hd;
}

const double Sample::eps = 1e-12;

vector<double> Sample::srt(const vector<double>& v) {
     vector<double> x = v;
     sort(x.begin(), x.end());
     return x;
}

std::vector<std::vector<int>> run_length_distribution(
     const std::vector<int>& x, int m) {
     vector<vector<int>> v(m);
     vector<int>::size_type k = 0;
     int l;
     while (k < x.size()) {
          const auto a = x[k];
          l = 1;
          while (++k < x.size() && x[k] == a)
               ++l;
          if (a < 0 || a >= m)
               throw invalid_argument("element out of range in"
                                      " run_length_distribution");
          v[a].push_back(l);
     }
     return v;
}

double mle_lsd(const double mean) {
     using std::log;
     using std::numeric_limits;
     SHG_VALIDATE(mean > 1.0);
     double a = numeric_limits<double>::epsilon();
     double b = 1.0 - a;
     double p = 0.0;            // shut up compiler warnings
     auto fprim = [mean](const double p) -> double {
          const double z = 1.0 - p;
          return mean / p + 1.0 / log(z) / z;
     };
     if (fprim(a) < 0.0 || fprim(b) > 0.0)
          throw runtime_error("sign error in mle_lsd");
     // Bisection.
     while (b - a > 1e-8)
          if (fprim(p = 0.5 * (a + b)) > 0.0)
               a = p;
          else
               b = p;
     return p;
}

double cdf_lsd(const double x, const double p) {
     using std::log1p;
     SHG_VALIDATE(0.0 < p && p < 1.0);
     if (x < 1.0)
          return 0.0;
     const double l1mp = log1p(-p);
     if (!isfinite(l1mp))
          throw range_error("range error in cdf_lsd");
     const double a = -1.0 / l1mp;
     if (!isfinite(a))
          throw range_error("range error in cdf_lsd");
     const int n = ifloor(x);
     double s = 0.0, z = p;
     for (int k = 1; k <= n; k++) {
          s += a / k * z;
          z *= p;
     }
     return s;
}

void mle_negative_binomial(const SHG::Vecint& x, double& t, double& p) {
     const int n = x.size();
     if (n < 1)
          throw invalid_argument("n < 1 in mle_negative_binomial");
     for (int i = 0; i < n; i++)
          if (x(i) < 0)
               throw invalid_argument(
                    "x(i) < 0 in mle_negative_binomial");
     double mean, var;
     {
          Vecdouble z(n);
          for (int i = 0; i < n; i++)
               z(i) = x(i);
          mean_var(z, mean, var);
     }
     if (mean < 1e-13 || var < 1e-13)
          throw runtime_error(
               "degenerate sample in mle_negative_binomial");
     // Find estimators of p and t by the method of moments.
     p = mean / var;
     if (p <= 0.0 || p >= 1.0)
          throw runtime_error(
               "invalid estimator of p in mle_negative_binomial");
     t = mean * p / (1.0 - p);
     auto f = [x, n, mean](const double t) -> double {
          double s = 0.0;
          for (int i = 0; i < n; i++) {
               const double xi = x(i);
               for (int j = 0; j < xi; j++)
                    s += 1.0 / (t + j);
          }
          s += n * std::log(t / (t + mean));
          return s;
     };
     t = brent(f, 0.5 * t, 2.0 * t, 1e-5);
     if (t <= 0.0)
          throw runtime_error(
               "invalid estimator of t in mle_negative_binomial");
     p = t / (t + mean);
}

double cdf_negative_binomial(double x, double t, double p) {
     using std::lgamma;
     if (t <= 0.0 || p <= 0.0 || p >= 1.0)
          throw invalid_argument(
               "invalid argument in cdf_negative_binomial");
     if (x < 0.0)
          return 0.0;
     const int n = ifloor(x);
     const double logbeta =
          lgamma(t) + lgamma(n + 1.0) - lgamma(t + n + 1.0);
     return betain(p, t, n + 1, logbeta);
}

Assessment_of_discrete_distribution::
Assessment_of_discrete_distribution(const SHG::Vecint& x)
     : n(x.size()), x(x), mean_(), var_(),
       geometric_(-1.0), poisson_(-1.0),
       logarithmic_(-1.0), negbin_(-1.0) {
     if (n < 2)
          throw invalid_argument("n < 2");
     SHG::Vecdouble z(n);
     for (int i = 0; i < n; i++) {
          if (x(i) < 0)
               throw invalid_argument("negative values in sample");
          z(i) = x(i);
     }
     mean_var(z, mean_, var_);
     if (abs(var_) < 1e-13)
          throw invalid_argument("degenerate distribution");
}

void Assessment_of_discrete_distribution::run() {
     // geometric
     {
          const double p = 1.0 / mean_;         // ML estimator of p
          if (0.0 < p && p < 1.0) {
               const double q = 1.0 - p;
               auto cdf = [q](const double x) -> double {
                    if (x < 1.0)
                         return 0.0;
                    return 1.0 - std::pow(q, ifloor(x));
               };
               double d;
               ksonedc(cdf, x, d, geometric_);
          } else {
               geometric_ = 0.0;
          }
     }
     // poisson
     {
          const double
               lambda = 1.0 / mean_;    // ML estimator of lambda
          SHG_ASSERT(lambda > 0.0);
          auto cdf = [lambda](const double x) -> double {
               return x < 0.0 ?
               0.0 : 1.0 - gammad(lambda, ifloor(x) + 1);
          };
          double d;
          ksonedc(cdf, x, d, poisson_);
     }
     // logarithmic
     {
          if (mean_ > 1.0) {
               const double p = mle_lsd(mean_);
               auto cdf = [p](const double x) -> double {
                    return cdf_lsd(x, p);
               };
               double d;
               ksonedc(cdf, x, d, logarithmic_);
          } else {
               logarithmic_ = 0.0;
          }
     }
     // negative binomial
     {
          double t, p;
          double d;
          try {
               mle_negative_binomial(x, t, p);
               auto cdf = [t, p](const double x) -> double {
                    return cdf_negative_binomial(x, t, p);
               };
               ksonedc(cdf, x, d, negbin_);
          } catch (const runtime_error&) {
               // we assume that if p or t cannot be easy estimated,
               // the distribution is far from negative binomial
               negbin_ = 0;
          }
     }
}

Unigaumixmod::Degenerate_distribution::Degenerate_distribution()
     : Exception("degenerate distribution in m-step") {}

Unigaumixmod::Unigaumixmod(const Vecdouble& x, int K)
     : n(x.size()), K(K), x(x), pi(K), mu(K), sigma(K), psi(n, K),
       loglik(0.0) {
     SHG_ASSERT(K >= 1);
     SHG_ASSERT(n >= 1);
}

double Unigaumixmod::estep() {
     const double oldloglik = loglik;
     double s;
     int i, k;
     loglik = 0.0;
     for (i = 0; i < n; i++) {
          s = 0.0;
          for (k = 0; k < K; k++)
               s += (psi(i, k) = pi(k) *
                     normal_pdf(x(i), mu(k), sigma(k)));
          SHG_ASSERT(s > 0.0);
          for (k = 0; k < K; k++)
               psi(i, k) /= s;
          loglik += log(s);
     }
     return loglik - oldloglik;
}

void Unigaumixmod::mstep() {
     double s, s1, s2, muk, psiik, xi;
     int i, k;
     for (k = 0; k < K; k++) {
          s = s1 = s2 = 0.0;
          muk = mu(k);
          for (i = 0; i < n; i++) {
               psiik = psi(i, k);
               xi = x(i);
               s += psiik;
               s1 += psiik * xi;
               s2 += psiik * sqr(xi - muk);
          }
          pi(k) = s / n;
          SHG_ASSERT(s > 0.0);
          mu(k) = s1 / s;
          if ((sigma(k) = sqrt(s2 / s)) <= 0.0)
               throw Degenerate_distribution();
     }
}

}       // namespace SHG
