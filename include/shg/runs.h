/**
 * \file include/shg/runs.h
 * Wald-Wolfowitz runs test.
 * \date Created on 28 September 2013.
 */

#ifndef SHG_RUNS_H
#define SHG_RUNS_H

#include <shg/vector.h>

namespace SHG {

/**
 * \addtogroup mathematical_statistics
 *
 * \{
 */

/**
 * Wald-Wolfowitz runs test. The function returns \f$p_1 = \Pr(K \leq
 * k \mid N_1 = n_1, N_2 = n_2)\f$, \f$p_2 = \Pr(K \geq k \mid N_1 =
 * n_1, N_2 = n_2)\f$, where \f$K\f$, \f$N_1\f$, \f$N_2\f$ are random
 * variables representing number of runs, number of elements of type I
 * and number of elements of type II in a sample, respectively. If
 * \f$\mathit{exact} = \mathit{true}\f$ and \f$n_1 \geq 1, n_2 \geq
 * 1\f$, the asymptotic distribution is used, otherwise the exact
 * distribution of the number of runs is used.
 *
 * \exception std::range_error if \f$\mathit{exact} = \mathit{true}\f$
 * and exact calculation results in underflow
 *
 * \implementation
 *
 * The following exact formulae have been used \cite fisz-1969, p.
 * 437, \cite wald-wolfowitz-1940, p. 150: \f[ \Pr(K = k \mid N_1 =
 * n_1, N_2 = n_2) = 2 \binom{n_1 - 1}{l - 1} \binom{n_2 - 1}{l - 1} /
 * \binom{n}{n_1} \f] for \f$k = 2l\f$ and \f{multline*} \Pr(K = k
 * \mid N_1 = n_1, N_2 = n_2) = \\ \left[ \binom{n_1 - 1}{l - 1}
 * \binom{n_2 - 1}{l} + \binom{n_1 - 1}{l} \binom{n_2 - 1}{l - 1}
 * \right] / \binom{n}{n_1} \f} for \f$k = 2l + 1\f$, where \f$n = n_1
 * + n_2\f$ and \f$\binom{n}{k} = 0\f$ for \f$n < k\f$.
 *
 * The possible numbers of runs in a sample are included between 2 and
 * \f$2 \min(n_1, n_2) + 1\f$ if \f$n_1 \neq n_2\f$ and 2 and \f$n\f$
 * if \f$n_1 = n_2\f$.
 *
 * The expected value and variance of \f$K\f$ are \f{align*} E(K \mid
 * N_1 = n_1, N_2 = n_2) &= \frac{2n_1n_2 + n}{n}, \\ D^2(K \mid N_1 =
 * n_1, N_2 = n_2) &= \frac{2n_1n_2(2n_1n_2 - n)}{(n - 1)n^2}. \f}
 *
 * For asymptotic distribution, Wolf and Wolfowitz result
 * \cite wald-wolfowitz-1940, p. 151 has been used that as \f$n_1\f$,
 * \f$n_2\f$ tend to infinity with \f$n_1 / n_2\f$ being constant,
 * \f$K\f$ is asymptotically normally distributed with mean \f$E(K
 * \mid N_1 = n_1, N_2 = n_2)\f$ and variance \f$D^2(K \mid N_1 = n_1,
 * N_2 = n_2)\f$.
 *
 * \note At the time of writing, the original article
 * \cite wald-wolfowitz-1940 was available at <a href =
 * "http://projecteuclid.org/">Project Euclid</a>.
 */
void runs(std::size_t n1, std::size_t n2, std::size_t k, bool exact,
          double& p1, double& p2);

/**
 * Returns the number of runs in a sample.
 *
 * The function returns the number of elements less than \f$z\f$ in
 * the vector \f$x\f$ \f$n_1\f$, the number of elements greater or
 * equal to \f$z\f$ \f$n_2\f$ and the number of runs \f$k\f$. The
 * operation \f$<\f$ must be defined for the type \f$T\f$.
 */
template <class T>
void count_runs(SHG::Vector<T> const& x, T z, std::size_t& n1,
                std::size_t& n2, std::size_t& k) {
     auto const n = x.size();
     n1 = n2 = k = 0;
     if (n == 0)
          return;
     bool s = x[0] < z;
     k = 1;
     for (auto const a : x)
          if (a < z) {
               n1++;
               if (!s) {
                    s = true;
                    k++;
               }
          } else if (s) {
               s = false;
               k++;
          }
     n2 = n - n1;
}

/** \} */ /* end of group mathematical_statistics */

}  // namespace SHG

#endif
