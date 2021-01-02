/**
 * \file include/shg/hmm.h
 * Hidden Markov models.
 * \date Created on 1 May 2011.
 */

#ifndef SHG_HMM_H
#define SHG_HMM_H

#include <shg/matrix.h>
#include <shg/mzt.h>

namespace SHG {

/**
 * \addtogroup mathematical_statistics
 *
 * \{
 */

/**
 * \defgroup HMM Hidden Markov models
 *
 * Classes and functions associated with hidden Markov models
 * \{
 */

/**
 * Normal hidden Markov model.
 *
 * See <a href="shg.pdf">Normal hidden Markov models.</a>
 */
class Normal_hmm {
public:
     Normal_hmm(const Matdouble& P, const Vecdouble& p,
                const Vecdouble& mu, const Vecdouble& sigma,
                const Vecdouble& y);
     const std::size_t T_;
     const std::size_t s_;
     Matdouble P;
     Vecdouble p;
     Vecdouble mu;
     Vecdouble sigma;
     const Vecdouble y;
     double logL;
     Matdouble gamma;  ///< pstate
     double logprob;   ///< maximized log(prob) in Viterbi
     Vecint X;         ///< the best sequence given by Viterbi
     int forwardbackward();
     int baumwelch();
     void viterbi();  ///< sets logprob and i0
     void sort();     ///< sort the model by mu1 <= mu2 <= ...

private:
     Matdouble alpha, beta;
     Vecdouble c, d;
     Vecdouble aux;
     double phi(std::size_t t, std::size_t j);
};

/**
 * Generates observations on a normal hidden Markov model.
 *
 * Generates T observations y and X on a normal hidden Markov model
 * with transition matrix P, initial state distribution p, vector of
 * means at each state mu and vector of standard deviations at each
 * state sigma. Uses the generator g. The vectors y and X are resized
 * in this function.
 */
void gen_nhmm(const Matdouble& P, const Vecdouble& p,
              const Vecdouble& mu, const Vecdouble& sigma,
              std::size_t T, Vecdouble& y, Vecint& X, MZT& g);

/**
 * Autocorrelation function of a hidden Markov model.
 *
 * Let \f$(X_t)_{t = 1}^{\infty}\f$ be an \f$m\f$-state hidden Markov
 * model with transition matrix \f$\Gamma\f$ and stationary
 * distribution \f$\delta\f$. For any function \f$g\f$ for which
 * expectations exist \cite zucchini-macdonald-2009, p. 34,
 * \f[
 * E(g(X_t)) = \sum_{i = 1}^m \delta_i G_i = \delta G \mathbf{1}^T,
 * \f]
 * \f[
 * E(g(X_t), g(X_{t + k})) = \sum_{i, j = 1}^m \delta_i G_i G_j
 * \gamma_{ij}(k) = \delta G \Gamma^k G \mathbf{1}^T,
 * \f]
 * where \f$G_i = E(g(X_t) | C_t = i)\f$, \f$G = \mathrm{diag}(G_1,
 * \ldots, G_m)\f$. Thus
 * \f[
 * \mathrm{cov}(g(X_t), g(X_{t + k})) = \delta G \Gamma^k G
 * \mathbf{1}^T - (\delta G \mathbf{1}^T)^2
 * \f]
 * and
 * \f[
 * \mathrm{corr}(g(X_t), g(X_{t + k})) = \mathrm{cov}(g(X_t), g(X_{t +
 * k})) / \mathrm{var}(g(X_t)).
 * \f]
 *
 * This function, given \f$\delta\f$, \f$\Gamma\f$, \f$G\f$, \f$E\f$,
 * \f$v\f$ and \f$K > 0\f$, calculates the autocorrelation function of
 * \f$(X_t)\f$ for the lags \f$0, 1, \ldots, K\f$ and puts the
 * autocorrelation coefficients in <tt>r[K + 1]</tt> so that \c r[k]
 * contains autocorrelation coefficient \f$r_k\f$ for \f$k = 0,
 * \ldots, K\f$. The vector \c r is resized in the function to get
 * proper dimension.
 *
 * \param[in] delta stationary distribution
 * \param[in] gamma transition matrix
 * \param[in] G \f$E(g(X_t)\;|\;C_t = i)\f$
 * \param[in] E \f$E(g(X_t))\f$
 * \param[in] v \f$\mbox{var}(g(X_t))\f$
 * \param[in] K maximum lag
 * \param[out] r autocorrelations, r[0] == 1
 */
void corr_hmm(const Vecdouble& delta, const Matdouble& gamma,
              const Vecdouble& G, double E, double v, int K,
              Vecdouble& r);

/**
 * Autocorrelation function of a normal hidden Markov model.
 *
 * Calculates autocorrelation function of \f$g(X_t)\f$ for a
 * stationary normal hidden Markov model. Three values of \c
 * function_g may be given: 0 for \f$g(X) = X\f$, 1 for \f$g(X) =
 * |X|\f$, 2 for \f$g(X) = X * X\f$.
 *
 * \implementation For \f$g(X) = X\f$ the implementation is
 * straightforward: \f[ E(g(X_t) | C_t = i) = \mu_i, \f] \f[
 * E(g(X_t))= \sum_{i = 1}^m \delta_i \mu_i, \f] \f[
 * \mathrm{var}(g(X_t)) = \sum_{i = 1}^m \delta_i (\sigma_i^2 +
 * \mu_i^2) - [E(g(X_t))]^2. \f]
 * For \f$g(X) = |X|\f$, \f[ E(g(X_t) | C_t = i) = \sigma_i
 * \sqrt{\frac{2}{\pi}} \exp \left( -\frac{\mu_i^2}{2
 * \sigma_i^2}\right) + \mu_i[1 - 2 \Phi(-\mu_i / \sigma_i)], \f] \f[
 * E(g(X_t)) = \sum_{i = 1}^m \delta_i E(g(X_t) | C_t = i), \f] \f[
 * \mathrm{var}(g(X_t)) = E(X_t^2) - [E(g(X_t))]^2 = \sum_{i = 1}^m
 * \delta_i (\mu_i^2 + \sigma_i^2) - [E(g(X_t))]^2, \f] where
 * \f$\Phi\f$ is the cumulative distribution function of standard
 * normal distribution.
 * For \f$g(X) = X^2\f$ (cf. implementation of \c Normal_mixture), \f[
 * E(g(X_t) | C_t = i) = \mu_i^2 + \sigma_i^2, \f] \f[ E(g(X_t)) =
 * \sum_{i = 1}^m \delta_i (\mu_i^2 + \sigma_i^2), \f] \f[
 * \mathrm{var}(g(X_t)) = E(X_t^4) - [E(g(X_t))]^2 = \sum_{i = 1}^m
 * \delta_i (\mu_i^4 + 6 \mu_i^2 \sigma_i^2 + 3 \sigma_i^4) -
 * [E(g(X_t))]^2. \f]
 *
 * \param[in] delta stationary distribution
 * \param[in] gamma transition matrix
 * \param[in] mu state-dependent means
 * \param[in] sigma state-dependent std. dev.
 * \param[in] function_g see above
 * \param[in] K maximum lag
 * \param[out] r autocorrelations, r[0] == 1
 */
void corr_nhmm(const Vecdouble& delta, const Matdouble& gamma,
               const Vecdouble& mu, const Vecdouble& sigma,
               int function_g, int K, Vecdouble& r);

/** \} */ /* end of group HMM */

/** \} */ /* end of group mathematical_statistics */

}  // namespace SHG

#endif
