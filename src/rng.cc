/**
 * \file src/rng.cc
 * Random number generator.
 * \date Created on 11 December 2011.
 */

#include <shg/rng.h>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <shg/except.h>
#include <shg/mconsts.h>

namespace SHG {

using std::exp;
using std::floor;
using std::log;
using std::log1p;
using std::numeric_limits;
using std::overflow_error;
using std::size_t;
using std::sqrt;
using std::tan;

double RNG::unipos() {
     double x;
     do {
          x = operator()();
     } while (x < numeric_limits<double>::min());
     return x;
}

double RNG::uniopen() {
     double x;
     for (;;) {
          x = operator()();
          if (x > 0.0 && x < 1.0)
               return x;
     }
}

int RNG::uni(int b) {
     SHG_VALIDATE(b > 0);
     return static_cast<int>(b * operator()());
}

int RNG::uni(int a, int b) {
     SHG_VALIDATE(a < b);
     return static_cast<int>(a + (b - a) * operator()());
}

double RNG::exponential() {
     return -log(unipos());
}

void RNG::simplex_surface(Vecdouble& x) {
     SHG_VALIDATE(x.size() > 0);
     size_t i;
     double s = 0.0;
     for (i = 0; i < x.size(); i++)
          s += x[i] = exponential();
     for (i = 0; i < x.size(); i++)
          x[i] /= s;
}

int RNG::finite(Vecdouble const& p) {
     SHG_VALIDATE(p.size() > 0);
     size_t const n = p.size() - 1;
     double const u = operator()();
     double s = 0.0;
     size_t i;
     for (i = 0; i < n; i++)
          if (u < (s += p[i]))
               break;
     return i;
}

double RNG::normal() {
     double x, u, x1;
     do {
          x = -log(unipos());
          u = -2.0 * log(unipos());
          x1 = x - 1.0;
     } while (x1 * x1 > u);
     return operator()() < 0.5 ? -x : x;
}

void RNG::random_sample(int n, int N, Vecint& x) {
     SHG_VALIDATE(n > 0 && n <= N);
     x.resize(n);
     for (int t = 0, m = 0; m < n; t++)
          if ((N - t) * operator()() < static_cast<double>(n - m))
               x[m++] = t;
}

unsigned long RNG::logarithmic(double p) {
     SHG_VALIDATE(p > 0 && p < 1.0);
     double const r = log(1.0 - p);
     double const v = unipos();
     if (v >= p)
          return 1;
     double const u = unipos();
     double const q = 1.0 - exp(r * u);
     if (v <= q * q) {
          double const x = 1.0 + log(v) / log(q);
          // See \cite ansi-cpp-2012, 4.9.2.
          if (x >= static_cast<double>(
                        numeric_limits<unsigned long>::max()))
               throw overflow_error("overflow in RNG::logarithmic");
          return static_cast<unsigned long>(x);
     }
     if (v <= q)
          return 2;
     return 1;
}

unsigned int RNG::geometric(double p) {
     SHG_VALIDATE(p > 0.0 && p <= 1.0);
     if (p < 1.0) {
          double const r = log(unipos()) / log1p(-p) + 1;
          if (r > numeric_limits<unsigned int>::max())
               throw overflow_error(__func__);
          return static_cast<unsigned long>(r);
     } else {
          return 1;
     }
}

double RNG::gamma(double const a, double const b) {
     SHG_VALIDATE(a > 0.0 && b > 0.0);
     if (a >= numeric_limits<unsigned int>::max()) {
          return b *
                 (gamma_large(floor(a)) + gamma_frac(a - floor(a)));
     } else {
          unsigned int const na = static_cast<int>(a);
          if (a == na)
               return b * gamma_int(na);
          else if (na == 0)
               return b * gamma_frac(a);
          else
               return b * (gamma_int(na) + gamma_frac(a - na));
     }
}

double RNG::beta(double a, double b) {
     SHG_VALIDATE(a > 0.0 && b > 0.0);
     double const x1 = gamma(a);
     double const x2 = gamma(b);
     return x1 / (x1 + x2);
}

unsigned int RNG::binomial(double p, unsigned int n) {
     SHG_VALIDATE(p >= 0.0 && p <= 1.0);
     int a, b, k = 0;
     double x;

     while (n > 10) {
          a = 1 + (n / 2);
          b = 1 + n - a;
          x = beta(a, b);
          if (x >= p) {
               n = a - 1;
               p /= x;
          } else {
               k += a;
               n = b - 1;
               p = (p - x) / (1 - x);
          }
     }
     for (unsigned int i = 0; i < n; i++)
          if (operator()() < p)
               k++;
     return k;
}

unsigned int RNG::poisson(double mu) {
     SHG_VALIDATE(mu > 0.0);
     unsigned int m, k = 0;
     double x;

     while (mu > 10.0) {
          m = static_cast<int>(0.875 * mu);
          x = gamma_int(m);
          if (x < mu) {
               k += m;
               mu -= x;
          } else {
               return k + binomial(mu / x, m - 1);
          }
     }
     double const emu = exp(-mu);
     double prod = 1.0;
     do {
          prod *= operator()();
          k++;
     } while (prod > emu);
     return k - 1;
}

unsigned int RNG::negative_binomial(double t, double p) {
     SHG_VALIDATE(t > 0.0 && p > 0.0 && p < 1.0);
     double const mu = gamma(t) * (1.0 - p) / p;

     /*
      * If the statement below were return (mu > 0.0) ? poisson(mu) :
      * 0; there could be an assertion error in poisson(). Probably
      * optimizing compiler does not store calculated mu in memory and
      * compares mu as denormal number with 0. Under GCC it is enough
      * to write if (mu < numeric_limits<double>::min()) cout << mu to
      * see a lot of positive numbers less than min().
      *
      * Another problem is if it is justified to return 0 mu == 0. If
      * mu == 0, however, the Poisson distribution can be considered
      * as a degenerated distribution (probability of zero occurences
      * of impossible event).
      */

     return mu < numeric_limits<double>::min() ? 0 : poisson(mu);
}

double RNG::laplace(double mu, double lambda) {
     SHG_VALIDATE(lambda > 0.0);
     double const u = 2.0 * uniopen();
     if (u < 1.0)
          return mu + lambda * log(u);
     else
          return mu - lambda * log(2.0 - u);
}

RNG::~RNG() {}

double RNG::gamma_int(unsigned int const a) {
     if (a >= 12)
          return gamma_large(a);
     double s = 0.0;
     for (unsigned int i = 0; i < a; i++)
          s -= log(unipos());
     /*
      * Sum of a independent exponentially distributed random numbers.
      * To avoid underflow, we do not use logarithmic method.
      */
     return s;
}

double RNG::gamma_large(double const a) {
     using Constants::pi;
     double const sqa = sqrt(2.0 * a - 1.0);
     double x, y, v;
     do {
          do {
               y = tan(pi<double> * operator()());
               x = sqa * y + a - 1.0;
          } while (x <= 0.0);
          v = operator()();
     } while (v > (1.0 + y * y) *
                       exp((a - 1.0) * log(x / (a - 1.0)) - sqa * y));
     return x;
}

double RNG::gamma_frac(double const a) {
     using Constants::e;
     if (a == 0.0)
          return 0.0;
     double const p = e<double> / (a + e<double>);
     double q, x, u, v;
     do {
          u = operator()();
          v = unipos();
          if (u < p) {
               x = exp((1.0 / a) * log(v));
               q = exp(-x);
          } else {
               x = 1.0 - log(v);
               q = exp((a - 1.0) * log(x));
          }
     } while (operator()() >= q);
     return x;
}

}  // namespace SHG
