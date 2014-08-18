/* normmix.cc: mixtures of normal densities */

/**
 * \file src/normmix.cc
 * Mixtures of normal densities.
 * \date Created on 9 October 2010.
 */

#include <cmath>
#include <string>
#include "shg/mstat.h"          // normal_pdf
#include "shg/specfunc.h"
#include "shg/utils.h"
#include "shg/normmix.h"

namespace SHG {

using std::size_t;
using std::abs;
using std::sqrt;
using std::string;

Normal_mixture::Error::Error()
     : Exception(string("normal mixture error")) {}

Normal_mixture::Normal_mixture(const Vecdouble& w,
                               const Vecdouble& mu,
                               const Vecdouble& sigma)
     : n(w.size()), w(w), mu(mu), sigma(sigma),
       moments_calculated(false), mean_(), sdev_(),
       skew_(), curt_() {
     if (n < 1 || mu.size() != n || sigma.size() != n)
          throw Error();
     double s = 0.0;
     for (size_t i = 0; i < n; i++) {
          if (sigma[i] <= 0.0 || w[i] < 0.0 || w[i] > 1.0)
               throw Error();
          s += w[i];
     }
     if (abs(s - 1.0) > 1e-8)
          throw Error();
}

double Normal_mixture::p(double x) const {
     double s = 0.0;
     for (size_t i = 0; i < n; i++)
          s += w[i] * normal_pdf(x, mu[i], sigma[i]);
     return s;
}

double Normal_mixture::cdf(double x) const {
     double s = 0.0;
     for (size_t i = 0; i < n; i++)
          s += w[i] * normal_integral((x - mu[i]) / sigma[i]);
     return s;
}

double Normal_mixture::invcdf(double p) const {
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

double Normal_mixture::mean() {moments(); return mean_;}
double Normal_mixture::sdev() {moments(); return sdev_;}
double Normal_mixture::skew() {moments(); return skew_;}
double Normal_mixture::curt() {moments(); return curt_;}

void Normal_mixture::write(std::ostream& f) const {
     using SHG::write;
     write(w, f);
     write(mu, f);
     write(sigma, f);
}

Normal_mixture Normal_mixture::read(std::istream& f) {
     SHG::Vecdouble w, mu, sigma;
     using SHG::read;
     read(w, f);
     read(mu, f);
     read(sigma, f);
     return Normal_mixture(w, mu, sigma);
}

void Normal_mixture::moments() {
     if (moments_calculated)
          return;
     double m1 = 0.0, m2 = 0.0, m3 = 0.0, m4 = 0.0;

     // Calculate the first four raw moments of the mixture.
     for (size_t i = 0; i < n; i++) {
          const double wi = w[i];
          const double mu1 = mu[i];
          const double mu2 = sqr(mu1);
          const double s2 = sqr(sigma[i]);
          m1 += wi * mu1;
          m2 += wi * (mu2 + s2);
          m3 += wi * (mu2 + 3.0 * s2) * mu1;
          m4 += wi * (sqr(mu2) + 6.0 * mu2 * s2 + 3.0 * sqr(s2));
     }
     mean_ = m1;
     const double sm = sqr(m1);
     const double var = m2 - sm;
     if (var <= 0.0)
          throw Error();
     sdev_ = sqrt(var);
     skew_ = (m3 - 3.0 * m1 * m2 + 2.0 * m1 * sm) / sdev_ / var;
     curt_ = (m4 - 4.0 * m1 * m3 + 6.0 * sm * m2 - 3.0 * sqr(sm)) /
          var / var - 3.0;
     moments_calculated = true;
}

}       // namespace SHG
