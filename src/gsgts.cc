/**
 * \file src/gsgts.cc
 * Generating stationary Gaussian time series.
 * Created on 26 February 2017.
 */

#include <shg/gsgts.h>
#include <cmath>
#include <stdexcept>
#include <shg/mconsts.h>

namespace SHG {

using std::complex;
using std::cos;
using std::invalid_argument;
using std::sin;
using std::size_t;
using std::sqrt;
using std::vector;

GSGTS::GSGTS(const std::vector<double>& acf, Cosine_transform f)
     : n(acf.size() - 1), g(acf.size()) {
     if (acf.size() <= 1)
          throw invalid_argument(__func__);
     if (f == nullptr)
          f = cosft;
     f(acf, g);
     for (size_t k = 0; k < g.size(); k++) {
          const double gk = g[k];
          if (gk < 0.0)
               throw invalid_argument(__func__);
          g[k] = sqrt(2.0 * gk);
     }
}

void GSGTS::generate(std::vector<double>& X,
                     std::function<double()> normal,
                     Real_transform f) {
     if (X.size() < g.size())
          throw invalid_argument(__func__);
     if (f == nullptr)
          f = realft;
     /* We may take vector<double> z(2 * g.size()). */
     vector<complex<double>> z(g.size());
     z[0] = SHG::Constants::sqrt2<double> * normal() * g[0];
     z[n] = SHG::Constants::sqrt2<double> * normal() * g[n];
     for (size_t k = 1; k < n; k++) {
          z[k].real(normal() * g[k]);
          z[k].imag(normal() * g[k]);
     }
     f(z, X);
}

void GSGTS::realft(const std::vector<std::complex<double>>& z,
                   std::vector<double>& X) {
     const size_t n = z.size() - 1;
     const double w = SHG::Constants::pi<double> / n;
     const double c = 1.0 / sqrt(n);
     const double z0half = 0.5 * z[0].real();
     const double znhalf = 0.5 * z[n].real();
     size_t j, k;
     double m, p, s;
     int sign = 1;
     for (j = 0; j < X.size(); j++) {
          m = j * w;
          s = 0.0;
          for (k = 1; k < n; k++) {
               p = k * m;
               s += z[k].real() * cos(p) - z[k].imag() * sin(p);
          }
          X[j] = c * (z0half + s + sign * znhalf);
          sign = -sign;
     }
}

void GSGTS::cosft(const std::vector<double>& h,
                  std::vector<double>& H) {
     const size_t n = h.size() - 1;
     const double w = SHG::Constants::pi<double> / n;
     const double h0half = 0.5 * h[0];
     const double hnhalf = 0.5 * h[n];
     size_t j, k;
     double m, s;
     int sign = 1;
     for (k = 0; k <= n; k++) {
          m = k * w;
          s = 0.0;
          for (j = 1; j < n; j++)
               s += h[j] * cos(m * j);
          H[k] = h0half + s + sign * hnhalf;
          sign = -sign;
     }
}

std::vector<double> acfar1(double sigma2, double phi1, size_t n) {
     if (n < 1 || std::abs(phi1) >= 1.0)
          throw invalid_argument(__func__);
     std::vector<double> g(n);
     g[0] = sigma2 / (1.0 - phi1 * phi1);
     for (std::vector<double>::size_type i = 1; i < n; i++)
          g[i] = phi1 * g[i - 1];
     return g;
}

}  // namespace SHG
