/**
 * \file src/drbnwtsn.cc
 * Durbin-Watson statistic.
 * \date Created on 12 July 2010.
 */

#include <shg/drbnwtsn.h>
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <vector>
#include <shg/except.h>
#include <shg/mconsts.h>

using std::abs;
using std::atan;
using std::sin;
using std::sqrt;
using std::vector;

namespace SHG {

namespace {

// Integrand in \ref{eq:dwcdfintegral}.
double f(double const u, vector<double> const& w, int const m) {
     if (u) {
          double theta = 0.0, rho = 1.0, z;
          for (int i = 0; i < m; i++) {
               z = u * w[i];
               theta += atan(z);
               rho *= (1.0 + z * z);
          }
          theta /= 2.0;
          rho = sqrt(sqrt(rho));
          return sin(theta) / (u * rho);
     } else {
          double theta = 0.0;
          for (int i = 0; i < m; i++)
               theta += w[i];
          return theta / 2.0;
     }
}

}  // anonymous namespace

double dwcdf(int const n, int const k, double const x,
             bool const lower, double const eps, int const iter) {
     using SHG::Constants::pi;
     static double const tol = 1e-12;

     if (n <= 0 || k <= 0 || n - k - 1 <= 1 || eps <= 0.0 || iter < 1)
          throw std::invalid_argument(__func__);

     // From n - k - 1 > 1 it follows that at least one element of x -
     // lambda(i), i = 1, ..., n - 1, will be non-zero, so w will have
     // at least one element.

     if (x <= 0.0)
          return 0.0;
     if (x >= 4.0)
          return 1.0;

     vector<double> w(n - k - 1);
     int il, iu;
     double z;

     // Calculate w[i]. The number of w[i] is m.
     double h = pi<double> / n;
     int m = 0;
     if (lower) {
          il = 1;
          iu = n - k - 1;
     } else {
          il = k + 1;
          iu = n - 1;
     }
     double y = x - 2.0;
     for (int i = il; i <= iu; i++) {
          z = y + 2.0 * std::cos(h * i);
          if (abs(z) > tol)
               w[m++] = z;
     }
     SHG_ASSERT(m < n - k);

     // Calculate prob as defined by \ref{eq:dwcdfprob}.
     double const eps2 = pi<double> * eps / 2.0;
     // Determine the upper limit of integration b by
     // \ref{eq:dwcdfupperlimit}.
     z = 2.0 / m;
     double b = 1.0;
     for (int i = 0; i < m; i++)
          b /= w[i];
     b = sqrt(abs(b));
     b = b * z / (pi<double> * eps2);
     b = std::pow(b, z);
     SHG_ASSERT(b != HUGE_VAL);

     // Calculate the integral \ref{eq:dwcdfintegral} by the Simpson
     // method.
     double const a = 0.0;
     h = b - a;
     int i = 0;
     int l = 1;
     double s1 = f(a, w, m) + f(b, w, m);
     double s2 = f((a + b) / 2.0, w, m);
     double i2 = (h / 6.0) * (s1 + 4.0 * s2);
     for (;;) {
          double const i1 = i2;
          s1 += 2.0 * s2;
          h /= 2.0;
          y = a + h / 2.0;
          i++;
          if (i > iter)
               throw std::range_error(__func__);
          l *= 2;
          s2 = 0.0;
          for (int j = 0; j < l; j++)
               s2 += f(y + j * h, w, m);
          i2 = (h / 6.0) * (s1 + 4.0 * s2);
          if (abs(i1 - i2) <= eps2) {
               z = 0.5 + i2 / pi<double>;
               if (z < 0.0)
                    z = 0.0;
               else if (z > 1.0)
                    z = 1.0;
               return z;
          }
     }
}

void ppdw(int const n, int const k, double const p, double* xl,
          double* xu, double const eps) {
     static double const epscdf = 1e-6;
     static int const iter = 15;

     if (k <= 0 || n <= 0 || n - k - 1 <= 1 || p <= 0.0 || p >= 1.0)
          throw std::invalid_argument(__func__);

     // Do for lower = false, true.
     bool lower = false;
     for (;;) {
          double a = 0.0, b = 4.0, x, fx;
          // Bisection.
          while (b - a > eps) {
               x = (a + b) / 2.0;
               fx = dwcdf(n, k, x, lower, epscdf, iter);
               if (fx < p)
                    a = x;
               else
                    b = x;
          }
          if (lower) {
               *xl = (a + b) / 2.0;
               return;
          } else {
               *xu = (a + b) / 2.0;
               lower = true;
          }
     }
}

void swtbl(std::ostream& f) {
     static double const eps = 4.9e-5;

     using SHG::ppdw;
     using std::fixed;
     using std::setprecision;
     using std::setw;

     f << fixed << setprecision(3);

     for (int level = 1; level <= 2; level++) {
          f << "DURBIN-WATSON STATISTIC: " << (level == 1 ? 1 : 5)
            << " PER CENT SIGNIFICANCE POINTS OF DL AND DU\n";
          double const alpha = level == 1 ? 1e-2 : 5e-2;
          for (int page = 1; page <= 4; page++) {
               int const i = 1 + 5 * (page - 1);
               f << "   ";
               for (int k = i; k <= i + 4; k++)
                    f << "     K =" << setw(2) << k << "   ";
               f << '\n';
               f << "  N";
               for (int k = 0; k <= 4; k++)
                    f << "   DL     DU ";
               f << '\n';
               f << "---";
               for (int k = 0; k <= 4; k++)
                    f << "-------------";
               f << '\n';
               for (int line = i; line <= 49; line++) {
                    int n;
                    if (line <= 35)
                         n = 5 + line;
                    else if (line <= 47)
                         n = 45 + 5 * (line - 36);
                    else
                         n = 150 + 50 * (line - 48);
                    int j = 0;
                    double xl[5], xu[5];
                    for (int k = i; k <= i + 4; k++) {
                         if (n - k - 1 <= 3)
                              break;
                         j++;
                         ppdw(n, k, alpha, &xl[j - 1], &xu[j - 1],
                              eps);
                    }
                    f << setw(3) << n;
                    for (int k = 0; k < j; k++)
                         f << ' ' << setw(6) << xl[k] << setw(6)
                           << xu[k];
                    for (int k = 0; k < 5 - j; k++)
                         f << "  ----- -----";
                    f << '\n';
               }
               f << "---";
               for (int k = 0; k <= 4; k++)
                    f << "-------------";
               f << '\n';
               if (page == 4)
                    f << "K IS THE NUMBER OF REGRESSORS EXCLUDING "
                      << "THE INTERCEPT\n";
               f << '\n';
          }
     }
}

}  // namespace SHG
