/**
 * \file src/bdstest.cc
 * BDS test for independence.
 * \date Created on 12 October 2013.
 */

#include <shg/bdstest.h>
#include <stdexcept>
#include <shg/mstat.h>

namespace SHG {

BDS_test::BDS_test(std::vector<double> const& u, int const maxm,
                   std::vector<double> const& eps)
     : maxm_(maxm), eps_(eps), res_() {
     using std::abs;
     using std::erf;
     using std::erfc;
     using std::invalid_argument;
     using std::isfinite;
     using std::pow;
     using std::range_error;
     using std::sqrt;
     using std::vector;

     vdst const n = u.size();

     // The constant maxn_ is set in order to have n * (n - 1) / 2 < n
     // * n / 2 <= std::numeric_limits<vdst>::max().
     if (n < 1 || n > maxn_ || maxm < 2 ||
         static_cast<vdst>(maxm) >= n || eps.size() < 1)
          throw invalid_argument(__func__);
     double const nd = n;
     vector<bool> p(n * (n - 1) / 2);

     auto const sqr = [](double x) { return x * x; };

     // Returns true if |u[i] - u[j]| < eps.
     auto const chi = [&p](vdst i, vdst j) -> bool {
          if (i < j)
               return p[(j - 1) * j / 2 + i];
          else if (i > j)
               return p[(i - 1) * i / 2 + j];
          else
               return true;
     };

     res_.resize(eps.size());

     for (vdst ieps = 0; ieps < eps.size(); ieps++) {
          vdst count;
          double tcount = 0.0;
          // Calculate the matrix of distances and C_{1, n}(\epsilon)
          // by \ref\label{eq:Cmn}.
          {
               double const epsi = eps[ieps];
               vector<bool>::size_type k = 0;
               count = 0;
               for (vdst i = 1; i < n; i++)
                    for (vdst j = 0; j < i; j++)
                         if ((p[k++] = abs(u[i] - u[j]) < epsi))
                              count++;
          }
          double const c1 = 2.0 * count / n / (n - 1);

          // Calculate K by the formula \ref{eq:bds2.13s}.
          for (vdst i = 0; i < n; i++) {
               count = 0;
               for (vdst j = 0; j < n; j++)
                    if (chi(i, j))
                         count++;
               tcount += sqr(count);
          }
          double const k = tcount / (nd * nd * nd);

          // Calculate C defined by \ref{eq:bds2.12}. A simple
          // relationship with C_{1, n}(\epsilon) is used.
          double const c = (n - 1) * c1 / nd;

          res_[ieps].resize(maxm + 1);

          for (int m = 2; m <= maxm; m++) {
               // Calculate C_{m, n} by \ref\label{eq:Cmn}.
               vdst const nm1 = n - m + 1;
               count = 0;
               {
                    vdst i;
                    for (vdst s = 0; s < nm1; s++)
                         for (vdst t = s + 1; t < nm1; t++) {
                              for (i = 0; i < static_cast<vdst>(m) &&
                                          chi(s + i, t + i);
                                   i++) { /* VOID */
                              }
                              if (i == static_cast<vdst>(m))
                                   count++;
                         }
               }
               double const cm = 2.0 * count / nm1 / (nm1 - 1);

               // Calculate V_{m, n} / \sqrt{n} by \ref{eq:simpleV}.
               double v = 0.0;
               for (int j = 1; j < m; j++)
                    v += pow(c, 2 * j) * pow(k, m - j);
               v *= 2.0;
               v += pow(k, m) + sqr((m - 1.0) * pow(c, m)) -
                    sqr(m) * k * sqr(pow(c, m - 1));
               v *= 4.0 / nd;
               v = sqrt(v);
               double w = (cm - pow(c1, m)) / v;
               if (!isfinite(w))
                    throw range_error(__func__);
               res_[ieps][m].stat = w;
               w *= isqrt2;
               res_[ieps][m].pval =
                    w < 0.0 ? 0.5 + 0.5 * erf(w) : 0.5 * erfc(w);
          }
     }
}

BDS_test::BDS_test(std::vector<double> const& u)
     : BDS_test(u, 8, default_eps(u)) {}

std::vector<double> BDS_test::default_eps(
     std::vector<double> const& u) {
     std::vector<double> eps(7);
     double const s = 0.25 * stddev(u);
     for (std::vector<double>::size_type i = 0; i < 7; i++)
          eps[i] = (i + 2.0) * s;
     return eps;
}

double const BDS_test::isqrt2 = 1.0 / std::sqrt(2.0);

BDS_test::vdst const BDS_test::maxn_ = static_cast<vdst>(
     std::sqrt(2.0) * std::sqrt(std::numeric_limits<vdst>::max()));

std::ostream& operator<<(std::ostream& stream, BDS_test const& b) {
     using std::vector;
     for (size_t i = 0; i < b.res().size(); i++) {
          vector<BDS_test::Result> const& r = b.res()[i];
          for (vector<BDS_test::Result>::size_type j = 2;
               j < r.size(); j++)
               stream << b.eps()[i] << ' ' << j << ' ' << r[j].stat
                      << ' ' << r[j].pval << '\n';
     }
     return stream;
}

}  // namespace SHG
