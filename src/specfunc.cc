/* specfunc.cc: special functions */

/**
 * \file src/specfunc.cc
 * Special functions.
 * \date Created on 10 July 2012.
 */

#include <cmath>
#include <limits>
#include <stdexcept>
#include "shg/mconsts.h"
#include "shg/specfunc.h"

namespace SHG {

double ppnd7(const double p) {
     using std::abs;
     using std::sqrt;
     using std::log;

     static const double a0 = 3.3871327179e+0;
     static const double a1 = 5.0434271938e+1;
     static const double a2 = 1.5929113202e+2;
     static const double a3 = 5.9109374720e+1;
     static const double b1 = 1.7895169469e+1;
     static const double b2 = 7.8757757664e+1;
     static const double b3 = 6.7187563600e+1;

     static const double c0 = 1.4234372777e+0;
     static const double c1 = 2.7568153900e+0;
     static const double c2 = 1.3067284816e+0;
     static const double c3 = 1.7023821103e-1;
     static const double d1 = 7.3700164250e-1;
     static const double d2 = 1.2021132975e-1;

     static const double e0 = 6.6579051150e+0;
     static const double e1 = 3.0812263860e+0;
     static const double e2 = 4.2868294337e-1;
     static const double e3 = 1.7337203997e-2;
     static const double f1 = 2.4197894225e-1;
     static const double f2 = 1.2258202635e-2;

     const double q = p - 0.5;
     double r, z;

     if (abs(q) <= 0.425) {
          r = 0.180625 - q * q;
          return q * (((a3 * r + a2) * r + a1) * r + a0) /
               (((b3 * r + b2) * r + b1) * r + 1.0);
     }
     r = q < 0.0 ? p : 1.0 - p;
     if (r <= 0.0)
          throw std::invalid_argument(__func__);
     if ((r = sqrt(-log(r))) > 5.0) {
          r -= 5.0;
          z = (((e3 * r + e2) * r + e1) * r + e0) /
               ((f2 * r + f1) * r + 1.0);
     } else {
          r -= 1.6;
          z = (((c3 * r + c2) * r + c1) * r + c0) /
               ((d2 * r + d1) * r + 1.0);
     }
     return q < 0.0 ? -z : z;
}

double gammad(double x, double p) {
     using std::exp;
     using std::abs;
     using std::lgamma;
     using std::log;
     using std::numeric_limits;
     using std::sqrt;

     static const double elimit = log(numeric_limits<double>::min());
     static const double oflo = sqrt(numeric_limits<double>::max());
     static const double plimit = 1000.0, xbig = 1e6, tol = 1e-8;

     if (x < 0.0 || p <= 0.0)
          throw std::invalid_argument(__func__);
     bool fin;
     double pn1, pn2, pn3, pn4, pn5, pn6,
          a, b, c, arg, an, rn, min, gam;

     if (x == 0.0)
          return 0.0;
     if (p > plimit) {
          pn1 = exp(log(x / p) / 3.0) + 1.0 / (9.0 * p) - 1.0;
          pn1 *= 3.0 * sqrt(p);
          return normal_integral(pn1);
     }
     if (x > xbig)
          return 1.0;
     if (x <= 1.0 || x < p) {
          arg = p * log(x) - x - lgamma(p + 1.0);
          c = gam = 1.0;
          a = p;
          do {
               gam += (c *= x / ++a);
          } while (c > tol);
          if ((arg += log(gam)) >= elimit)
               return exp(arg);
          else
               return 0.0;
     } else {
          arg = p * log(x) - x - lgamma(p);
          b = (a = 1.0 - p) + x + 1.0;
          c = 0.0;
          gam = (pn3 = (pn2 = x) + (pn1 = 1.0)) / (pn4 = x * b);
          fin = false;
          do {
               an = ++a * ++c;
               b += 2.0;
               pn5 = b * pn3 - an * pn1;
               pn6 = b * pn4 - an * pn2;
               if (abs(pn6) > 0.0) {
                    if (tol < (min = tol * (rn = pn5 / pn6)))
                         min = tol;
                    if (abs(gam - rn) <= min)
                         fin = true;
                    else
                         gam = rn;
               }
               if (!fin) {
                    pn1 = pn3;
                    pn2 = pn4;
                    pn3 = pn5;
                    pn4 = pn6;
                    if (abs(pn5) >= oflo) {
                         pn1 /= oflo;
                         pn2 /= oflo;
                         pn3 /= oflo;
                         pn4 /= oflo;
                    }
               }
          } while (!fin);
          if ((arg += log(gam)) >= elimit)
               return 1.0 - exp(arg);
          else
               return 1.0;
     }
}

double probst(const double t, const int df) {
     using SHG::Constants::ipi;
     using std::atan;
     using std::sqrt;

     if (df < 1)
          throw std::invalid_argument(__func__);

     double f = df, c = 1.0;
     const double a = t / sqrt(f), b = f / (f + t * t);
     const int m2 = df - 2, oe = df % 2;
     f = 1.0;
     for (int k = 2 + oe; k <= m2; k += 2)
          f += (c *= b * (k - 1) / k);
     if (oe == 1) {
          if (df == 1)
               f = 0.0;
          return 0.5 + (a * b * f + atan(a)) * ipi<double>;
     }
     return 0.5 + 0.5 * a * sqrt(b) * f;
}

double betain(const double x, const double p, const double q,
              const double beta) {
     using std::abs;
     using std::exp;
     using std::log;

     static const double accuracy = 1e-8;

     bool index;
     int ns;
     double bti = 1.0, ai = 1.0, cx = 1.0 - x, pp, psq = p + q, qq, rx,
          temp, term = 1.0, xx;
     if (p <= 0.0 || q <= 0.0 || x < 0.0 || x > 1.0)
          throw std::invalid_argument(__func__);
     if (x == 0.0 || x == 1.0)
          return x;
     if (p < psq * x) {
          xx = cx; cx = x; pp = q; qq = p; index = true;
     } else {
          xx = x; pp = p; qq = q; index = false;
     }
     ns = static_cast<int>(qq + cx * psq);
     rx = xx / cx;
     temp = qq - ai;
     if (ns == 0)
          rx = xx;
     for (;;) {
          bti += (term *= temp * rx / (pp + ai));
          temp = abs(term);
          if (temp <= accuracy && temp <= accuracy * bti) {
               bti *= exp(pp * log(xx) + (qq - 1.0) * log(cx) - beta) /
                    pp;
               return index ? 1.0 - bti : bti;
          }
          ai++;
          if (--ns < 0) {
               temp = psq++;
          } else {
               temp = qq - ai;
               if (ns == 0) rx = xx;
          }
     }
}

double betain(const double x, const double p, const double q) {
     using std::lgamma;
     const double beta = lgamma(p) + lgamma(q) - lgamma(p + q);
     return betain(x, p, q, beta);
}

double cdffdist(const int m, const int n, double x) {
     if (m < 1 || n < 1)
          throw std::invalid_argument(__func__);
     if (x <= 0.0)
          return 0.0;
     x *= m;
     return betain(x / (x + n), 0.5 * m, 0.5 * n);
}

float digamma(float x) {
     if (x <= 0.0)
          throw std::invalid_argument(__func__);
     if (x <= 1e-5)
          return -0.5772156649f - 1.0f / x;
     float dig = 0.0;
     while (x < 8.5f)
          dig -= 1.0f / x++;
     /* Use Stirling's (actually de Moivre's) expansion if x > 8.5. */
     float r = 1.0f / x;
     dig += std::log(x) - 0.5f * r;
     r *= r;
     // The constants below are the third, fourth and fifth Stirling
     // coefficients.
     dig -= r * (8.333333333E-02f - r *
                 (8.3333333333E-03f - r * 3.968253968E-03f));
     return dig;
}

}       // namespace SHG
