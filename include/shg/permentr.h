/**
 * \file include/shg/permentr.h
 * Permutation entropy.
 * \date Created on 5 May 2014.
 */

#ifndef SHG_PERMENTR_H
#define SHG_PERMENTR_H

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <map>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace SHG {

/**
 * \addtogroup mathematical_statistics
 *
 * \{
 */

/**
 * Calculates permutation entropy of a time series.
 *
 * Let \f$(x_i)_{i = 0}^{N - 1}\f$ be a time series and let \f$2 \leq
 * L \leq N\f$ be a size of a sliding window. Let \f$\pi = (\pi_0,
 * \pi_1, \ldots, \pi_{L - 1})\f$ be a permutation of the set \f$\{0,
 * 1, \ldots, L - 1\}\f$. The window \f$(x_i)_{i = n}^{n + L - 1}\f$
 * is of type \f$\pi\f$ if \f$x_{n + \pi_0} \prec x_{n + \pi_1} \prec
 * \ldots \prec x_{n + \pi_{L - 1}}\f$, where \f$x_i \prec x_j\f$ if
 * \f$x_i < x_j\f$ or \f$x_i = x_j\f$ and \f$i < j\f$. The relative
 * frequency of \f$\pi\f$ in \f$(x_i)_{i = 0}^{N - 1}\f$ is then
 * \f[\hat{p}(\pi) = \frac{1}{N - L + 1}|\{n \colon 0 \leq n \leq N -
 * L, (x_i)_{i = n}^{n + L - 1} \mbox{is of type $\pi$} \}|\f] and the
 * permutation entropy is \f[-\sum_{\pi} \hat{p}(\pi)\log_2
 * \hat{p}(\pi).\f] Conf. \cite amigo-2010, page 29--30, and
 * \cite bandt-pompe-2002, page 2.
 *
 * \tparam T linearly ordered set
 * \param[in] x time series
 * \param[in] L size of sliding window
 * \returns permutation entropy
 *
 * \exception std::invalid_argument if \f$L < 2\f$ or \f$L >
 * \mathrm{x.size()}\f$.
 */
template <class T>
double permutation_entropy(std::vector<T> const& x, std::size_t L);

template <class T>
double permutation_entropy(std::vector<T> const& x,
                           std::size_t const L) {
     if (L < 2 || L > x.size())
          throw std::invalid_argument(__func__);
     typedef std::vector<std::size_t> Permutation;
     std::size_t const nl1 = x.size() - L + 1;
     std::map<Permutation, std::size_t> c;
     Permutation p(L), p0(L);
     std::iota(p0.begin(), p0.end(), 0);
     for (std::size_t n = 0; n < nl1; n++) {
          p = p0;
          std::stable_sort(p.begin(), p.end(),
                           [n, &x](std::size_t i, std::size_t j) {
                                return x[n + i] < x[n + j];
                           });
          c[p]++;
     }
     double const NL1 = nl1;
     double s = 0.0;
     for (auto const& x : c) {
          double const p = x.second / NL1;
          s -= p * std::log(p);
     }
     return 1.4426950408889634073599246810018921374266  // 1 / ln 2
            * s;
}

/** \} */ /* end of group mathematical_statistics */

}  // namespace SHG

#endif
