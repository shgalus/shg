/**
 * \file include/shg/laplace.h
 * Laplace distribution.
 * \date Created on 6 January 2013.
 */

#ifndef SHG_LAPLACE_H
#define SHG_LAPLACE_H

#include <cstdlib>
#include <shg/except.h>
#include <shg/matrix.h>
#include <shg/rng.h>

namespace SHG {

/**
 * \addtogroup mathematical_statistics
 *
 * \{
 */

/**
 * \defgroup laplace_distribution Laplace distribution
 *
 * Classes and functions associated with Laplace distribution
 * \{
 */

/**
 * Laplace distribution.
 *
 * The Laplace distribution is a probability distribution on
 * \f$\mathbf{R}\f$ given by the density function \f[ f(x) =
 * \frac{1}{2 \lambda} \exp \left( -\frac{|x - \mu|}{\lambda} \right),
 * \quad \mu \in \mathbf{R}, \quad \lambda > 0 \f] (\cite fisz-1969,
 * p. 170). Its probability distribution function is \f[ F(x) =
 * \left\{ \begin{array}{ll} \frac{1}{2} \exp \left( \frac{x -
 * \mu}{\lambda} \right) & \mbox{for $x \leq \mu$,} \\ 1 - \frac{1}{2}
 * \exp \left( -\frac{x - \mu}{\lambda} \right) & \mbox{for $x \geq
 * \mu$.} \end{array} \right. \f] The inverse probability distribution
 * function is \f[ F^{-1}(p) = \left\{ \begin{array}{ll} \mu + \lambda
 * \ln(2p) & \mbox{for $0 < p \leq 0.5$,} \\ \mu - \lambda \ln(2 - 2p)
 * & \mbox{for $0.5 \leq p < 1$.} \end{array} \right. \f]
 */
class Laplace_distribution {
public:
     /**
      * Constructs Laplace distribution. \exception
      * std::invalid_argument if \c lambda is not positive \warning
      * lambda is not the variance!
      */
     explicit Laplace_distribution(double mu = 0, double lambda = 1);
     /**
      * Returns density at \c x.
      */
     double f(double x);
     /**
      * Returns probability distribution function at \c x.
      */
     double cdf(double x);
     /**
      * Returns inverse probability distribution function at \c p.
      * \exception std::invalid_argument if \c p is not in \f$(0,
      * 1)\f$
      *
      * A better starting interval for bisection may be chosen.
      */
     double invcdf(double p);

private:
     double const mu;
     double const lambda;
};

/**
 * %Sample median.
 *
 * Let \f$ x_0 \leq x_1 \leq \ldots \leq x_{n - 1} \f$, \f$n \geq 1
 * \f$ be a sample. The function \c median returns \f$ x_{\lfloor n /
 * 2 \rfloor} \f$ if \f$n\f$ is odd or \f$ 0.5 (x_{n / 2 - 1} + x_{n /
 * 2}) \f$ if \f$n\f$ is even.
 *
 * \exception std::invalid_argument if <tt>x.size() < 1</tt>
 *
 * \warning The function does not check if x is sorted
 * non-decreasingly.
 */
double median(SHG::Vecdouble const& x);

/**
 * Weighted median.
 *
 * Let \f$f \colon \mathbf{R} \rightarrow \mathbf{R}\f$, \f$ f(\mu) =
 * \sum_{i = 1}^n w_i|x_i - \mu|\f$, where \f$w_i \geq 0\f$, \f$x_1
 * \leq x_2 \leq \ldots \leq x_n\f$ and not all \f$w_i\f$ are equal to
 * 0. The function \f$f\f$ reaches its minimum either in a single
 * point or on an interval. The function \c weighted_median returns
 * this point if it is unique. If it is not unique, the function
 * returns the middle point of the interval on which the minimum is
 * reached.
 *
 * \exception std::invalid_argument if not <tt>a.size() == b.size() >
 * 0</tt> or if not <tt>a[i] >= 0</tt> for all \c i or if sum of
 * <tt>a[i]</tt> is not greater than 0 or if <tt>b[i]</tt> are not
 * sorted non-decreasingly.
 *
 * \warning Due to rounding errors, the function may incorrect
 * recognize unique and non-unique minima.
 *
 * \implementation Let us denote \f$x_0 = -\infty\f$, \f$x_{n + 1} =
 * +\infty\f$. On each interval \f$x_{j - 1} \leq \mu \leq x_j\f$,
 * \f$j = 1, \ldots, n + 1\f$, the function \f$f\f$ may be written as
 * \f[ f(\mu) = \sum_{i = 1}^{j - 1} w_i(\mu - x_i) + \sum_{i = j}^n
 * w_i(x_i - \mu) = u_j \mu + v_j, \f] where \f[ u_j = \sum_{i = 1}^{j
 * - 1} w_i - \sum_{i = j}^n w_i. \f] We have \f$u_1 = -\sum_{i = 1}^n
 * w_i < 0\f$, \f$u_{n + 1} = \sum_{i = 1}^n w_i > 0\f$ and \f$u_{j +
 * 1} - u_j = 2w_j \geq 0\f$ for \f$j = 1, \ldots, n\f$, so the
 * sequence \f$u_1, \ldots, u_{n + 1}\f$ is non-decreasing. Let \f[k =
 * \min \left\{ j \in \{1, \ldots, n + 1 \} \colon u_j \geq 0
 * \right\},\f] \f[l = \min \left\{ j \in \{1, \ldots, n + 1 \} \colon
 * u_j > 0 \right\}.\f] For \f$j < k\f$ we have \f$u_j < 0\f$, so
 * \f$f\f$ is strictly decreasing on \f$(-\infty, x_{k - 1}]\f$. For
 * \f$j \geq l\f$ we have \f$u_j > 0 \f$, so \f$f\f$ is strictly
 * increasing on \f$[x_{l - 1}, +\infty)\f$. On \f$[x_{k - 1}, x_{l -
 * 1}]\f$ the function \f$f\f$ is constant. If this interval reduces
 * to a point, ie. \f$k = l\f$, the minimum is uniquely reached at
 * \f$x_{k - 1}\f$.
 */
double weighted_median(SHG::Vecdouble const& x,
                       SHG::Vecdouble const& w);

/**
 * Univariate Laplace mixture models.
 *
 * Univariate Laplace mixture model estimation by
 * expectation-maximization algorithm. After initialization with data
 * vector \a x and the number of components \a K, user must set
 * initial values of Unilapmixmod::pi, Unilapmixmod::mu,
 * Unilapmixmod::lambda, call estep() and mstep(). Then estep() and
 * mstep() must be called alternately until the difference between two
 * consecutive values of Unilapmixmod::loglik returned by estep() is
 * sufficiently small.
 *
 * \implementation The class is modeled after the class Gaumixmod
 * \cite press-teukolsky-vetterling-flannery-2007, p. 845, and based
 * on \cite krzysko-wolynski-gorecki-skorzybut-2008, p. 359.
 *
 * \sa SHG::Unigaumixmod
 */
struct Unilapmixmod {
     struct Degenerate_distribution : public virtual Exception {
          Degenerate_distribution();
     };
     Unilapmixmod(SHG::Vecdouble const& x, int K);
     /**
      * Performs e-step.
      *
      * \returns difference between new and old values of
      * Unilapmixmod::loglik
      */
     double estep();
     /**
      * Performs m-step.
      *
      * \exception SHG::Laplace::Unilapmixmod::Degenerate_distribution
      * if in a component distribution lambda = 0
      */
     void mstep();

     int const n;              ///< number of observations
     int const K;              ///< number of components
     SHG::Vecdouble const& x;  ///< observations
     SHG::Vecdouble pi;        ///< weights
     SHG::Vecdouble mu;        ///< mus of Laplace components
     SHG::Vecdouble lambda;    ///< lambdas of Laplace components
     Matdouble psi;            ///< the matrix psi
     double loglik;            ///< loglikelihood found
     SHG::Vecdouble x_sorted;
     SHG::Vecint order;
};

/**
 * Mixtures of Laplace densities.
 *
 * Calculates probability density, distribution function, percentage
 * points and moments of the mixture of Laplace distributions.
 *
 * \implementation
 *
 * Implementetion is the same as of SHG::Normal_mixture. The first for
 * moments about zero of Laplace distribution \f$M_i^{(k)}\f$ are
 * (\cite kotz-kozubowski-podgorski-2001, page 20)
 * \f{align*}{
 * M_i^{(1)} & = \mu_i \\
 * M_i^{(2)} & = \mu_i^2 + 2 \lambda_i^2 \\
 * M_i^{(3)} & = \mu_i^3 + 6\mu_i\lambda_i^2 \\
 * M_i^{(4)} & = \mu_i^4 + 12\mu_i^2\lambda_i^2 + 24\lambda_i^4
 * \f}
 */
class Laplace_mixture {
public:
     struct Error : public virtual Exception {
          Error();
     };
     Laplace_mixture(SHG::Vecdouble const& w,
                     SHG::Vecdouble const& mu,
                     SHG::Vecdouble const& lambda);
     double p(double x) const;       ///< Probability density.
     double cdf(double x) const;     ///< Distribution function.
     double invcdf(double p) const;  ///< Percentage point.
     double mean();                  ///< Return mean.
     double sdev();                  ///< Return standard deviation.
     double skew();                  ///< Return skewness.
     double curt();                  ///< Return curtosis.
     /**
      * Generates a series from the mixture.
      *
      * Given random number generator \c rng, generates a series of
      * length \c n from this mixture and puts it in \c x. The vector
      * \c x is resized within this method. If \c n < 1, an empty
      * vector \c x is returned.
      */
     void generate(SHG::RNG& rng, int n, SHG::Vecdouble& x);
     /**
      * Write this class to an opened binary stream.
      *
      * Only w, mu and lambda are written.
      */
     void write(std::ostream& f) const;
     /**
      * Return object of this class read from an open binary stream.
      *
      * Returned object has not calculated moments.
      */
     static Laplace_mixture read(std::istream& f);

private:
     std::size_t const n_;         ///< Number of components.
     SHG::Vecdouble const w;       ///< Weights.
     SHG::Vecdouble const mu;      ///< Parameters mu.
     SHG::Vecdouble const lambda;  ///< Parameters lambda.
     bool moments_calculated;      ///< True if moments calculated.
     double mean_;                 ///< Mean.
     double sdev_;                 ///< Standard deviation.
     double skew_;                 ///< Skewness.
     double curt_;                 ///< Curtosis.
     void moments();               ///< Calculate moments.
};

/** \} */ /* end of group laplace_distribution */

/** \} */ /* end of group mathematical_statistics */

}  // namespace SHG

#endif
