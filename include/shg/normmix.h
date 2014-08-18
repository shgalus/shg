/* normmix.h: mixtures of normal densities */

/**
 * \file include/shg/normmix.h
 * Mixtures of normal densities.
 * \date Created on 9 October 2010.
 */

#ifndef SHG_NORMMIX_H
#define SHG_NORMMIX_H

#include <cstdlib>
#include "shg/except.h"
#include "shg/vector.h"

namespace SHG {

/**
 * Mixtures of normal densities.
 *
 * Calculates probability density, distribution function, percentage
 * points and moments of the mixture of normal distributions.
 *
 * Let \f$f_i(x; \mu_i, \sigma_i)\f$, \f$i = 1, \ldots, n\f$, \f$n >
 * 0\f$, be probability density functions of normal distributions
 * \f$N(\mu_i, \sigma_i^2)\f$, and \f$w_i\f$ be nonnegative weights
 * summing up to 1. The probability density of the mixture is defined
 * as \f[ f(x) = \sum_{i = 1}^n w_i f_i(x; \mu_i, \sigma_i). \f]
 *
 * The constructor accepts three arguments: \f$w\f$, \f$\mu\f$ and
 * \f$\sigma\f$. If an argument is invalid, the exception
 * Normal_mixture::Error is thrown. The function \c p returns values
 * of the density. The function \c cdf returns values of the
 * corresponding distribution function \f$F\f$. The function \c
 * invcdf(p) returns \f$x\f$ such that \f$F(x) = p\f$ if \f$0 < p <
 * 1\f$ and throws Normal_mixture::Error otherwise. The functions \c
 * mean, \c sdev, \c skew and \c curt return mean, standard deviation,
 * coefficient of skewness and coefficient of curtosis of the mixture
 * distribution. Any of these four functions may throw the exception
 * Normal_mixture::Error if it comes out that variance of the mixture
 * distribution is equal to 0.
 *
 * \ingroup mathematical_statistics
 *
 * \todo In invcdf(double) const choose better initial values for
 * bisection.
 *
 * \implementation
 *
 * The implementation is straightforward. To calculate moments, the
 * first four raw moments \f$m_k\f$ are calculated as \f[ m_k =
 * \sum_{i = 1}^n w_i M_i^{(k)}, \f] where \f$M_i^{(k)}\f$ is the
 * \f$k\f$-th moment of \f$N(\mu_i, \sigma_i^2)\f$ (\cite fisz-1969,
 * page 160)
 * \f{align*}{
 * M_i^{(1)} & = \mu_i \\
 * M_i^{(2)} & = \mu_i^2 + \sigma_i^2 \\
 * M_i^{(3)} & = \mu_i^3 + 3\mu_i\sigma_i^2 \\
 * M_i^{(4)} & = \mu_i^4 + 6\mu_i^2\sigma_i^2 + 3\sigma_i^4
 * \f}
 * Then the following formulas \cite fisz-1969, page 80, are used to
 * calculate central moments:
 * \f{align*}{
 * \nu_2 & = m_2 - m_1^2 \\
 * \nu_3 & = m_3 - 3m_1m_2 + 2m_1^3 \\
 * \nu_4 & = m_4 - 4m_1m_3 + 6m_1^2m_2 - 3m_1^4
 * \f}
 * Finally, mean, standard deviation, coefficient of skewness and
 * coefficient of curtosis are calculated as \f$m_1\f$,
 * \f$\sqrt{\nu_2}\f$, \f$\nu_3 / (\sqrt{\nu_2})^3\f$ and \f$\nu_4 /
 * (\sqrt{\nu_2})^4 - 3\f$, respectively.
 */
class Normal_mixture {
public:
     struct Error : public virtual Exception {
          Error();
     };
     Normal_mixture(const Vecdouble& w,
                    const Vecdouble& mu,
                    const Vecdouble& sigma);
     double p(double x) const;          ///< Probability density.
     double cdf(double x) const;        ///< Distribution function.
     double invcdf(double p) const;     ///< Percentage point.
     double mean();                     ///< Return mean.
     double sdev();                     ///< Return standard deviation.
     double skew();                     ///< Return skewness.
     double curt();                     ///< Return curtosis.
     /**
      * Write this class to an opened binary stream.
      *
      * Only w, mu and sigma are written.
      */
     void write(std::ostream& f) const;
     /**
      * Return object of this class read from an open binary stream.
      *
      * Returned object has not calculated moments.
      */
     static Normal_mixture read(std::istream& f);

private:
     const std::size_t n;               ///< Number of components.
     const Vecdouble w;                 ///< Weights.
     const Vecdouble mu;                ///< Means.
     const Vecdouble sigma;             ///< Standard deviations.
     bool moments_calculated;           ///< True if moments calculated.
     double mean_;                      ///< Mean.
     double sdev_;                      ///< Standard deviation.
     double skew_;                      ///< Skewness.
     double curt_;                      ///< Curtosis.
     void moments();                    ///< Calculate moments.
};

}       // namespace SHG

#endif
