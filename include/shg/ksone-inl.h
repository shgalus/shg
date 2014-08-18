/* ksone-inl.h: implementation of the template functions ksone */

/**
 * \file include/shg/ksone-inl.h
 * Implementation of the template functions ksone.
 * \date Created on 12 January 2012.
 */

#ifndef SHG_KSONE_INL_H
#define SHG_KSONE_INL_H

#include <cmath>
#include <algorithm>
#include "shg/utils.h"

namespace SHG {

template<class T>
void ksone(T& cdf, Vecdouble& x, double& d, double& prob) {
     const int n = x.size();
     double dt, en = n, ff, fn, fo = 0.0;
     std::sort(x.begin(), x.end());
     d = 0.0;
     for (int j = 0; j < n; j++) {
          fn = (j + 1) / en;
          ff = cdf(x[j]);
          dt = SHG::max(std::abs(fo - ff), std::abs(fn - ff));
          if (dt > d)
               d = dt;
          fo = fn;
     }
     en = std::sqrt(en);
     prob = 1.0 - ksdist(d * en);
     // With approximation (14.3.18) from
     // \cite press-teukolsky-vetterling-flannery-2007, p. 737, the
     // last line would be prob = 1.0 - ksdist(d * (en + 0.12 + 0.11 /
     // en));
}

template<class T>
void ksonec(T& cdf, const Vecdouble& x, double& d, double& prob) {
     Vecdouble v(x);
     ksone(cdf, v, d, prob);
}

template<class T>
void ksoned(T& cdf, Vecint& x, double& d, double& prob) {
     // For discrete distributions with jumps in integer numbers this
     // eps is enough.
     static const double eps = 1e-5;
     const int n = x.size();
     double dt, en = n, ff, fn, fe, fo = 0.0;
     std::sort(x.begin(), x.end());
     d = 0.0;
     for (int j = 0; j < n;) {
          while (++j < n && x[j] == x[j - 1]) {/* VOID */}
          fn = j / en;
          ff = cdf(x[j - 1]);
          fe = cdf(x[j - 1] - eps);
          dt = SHG::max(std::abs(fo - fe), std::abs(fn - ff));
          if (dt > d)
               d = dt;
          fo = fn;
     }
     en = std::sqrt(en);
     prob = 1.0 - ksdist(d * en);
     // With approximation (14.3.18) from
     // \cite press-teukolsky-vetterling-flannery-2007, p. 737, the
     // last line would be prob = 1.0 - ksdist(d * (en + 0.12 + 0.11 /
     // en));
}

template<class T>
void ksonedc(T& cdf, const Vecint& x, double& d, double& prob) {
     Vecint v(x);
     ksoned(cdf, v, d, prob);
}

}       // namespace SHG

#endif
