/**
 * \file include/shg/mstat.h
 * Mathematical statistics.
 * \date Created on 16 June 2009.
 */

#ifndef SHG_MSTAT_H
#define SHG_MSTAT_H

#include <algorithm>
#include <cmath>
#include <vector>
#include <shg/except.h>
#include <shg/matrix.h>
#include <shg/mconsts.h>
#include <shg/utils.h>

namespace SHG {

/**
 * \defgroup mathematical_statistics Mathematical statistics
 *
 * Classes and functions of probability and mathematical statistics
 * \{
 */

/**
 * Density of one-dimensional standard normal distribution. The
 * density function is defined as \f[ f(x) = \frac{1}{\sqrt{2 \pi}}
 * \exp \left(-\frac{1}{2} x^2 \right). \f]
 */
template <class T>
inline T standard_normal_pdf(T x) {
     static_assert(std::is_floating_point<T>::value,
                   "T must be a floating-point type.");
     return SHG::Constants::isqrt2pi<T> *
            std::exp(static_cast<T>(-0.5) * SHG::sqr(x));
}

/**
 * Density of one-dimensional normal distribution. The density
 * function is defined as \f[ f(x) = \frac{1}{\sigma \sqrt{2 \pi}}
 * \exp \left(-\frac{(x - \mu)^2}{2 \sigma^2} \right). \f]
 */
template <class T>
inline T normal_pdf(T x, T mu, T sigma) {
     return standard_normal_pdf((x - mu) / sigma) / sigma;
}

/**
 * Cumulative distribution function of the Kolmogorov-Smirnov
 * distribution. It is defined as (see \cite fisz-1969, p. 412,
 * \cite press-teukolsky-vetterling-flannery-2007, p. 334, and
 * \cite marsaglia-tsang-wang-2003) \f[ F(x) = 1 - 2 \sum_{i =
 * 1}^{\infty} (-1)^{i - 1} \exp(-2i^2x^2). \f]
 *
 * \implementation See \cite press-teukolsky-vetterling-flannery-2007,
 * p. 334.
 *
 * \date Written on 8 January 2012.
 */
double ksdist(double x);

/**
 * Calculates the mean of data.
 * \note If x.size() <= 0, 0 is returned.
 *
 * \implementation The one-pass algorithm is used (see
 * \cite knuth-2002b, p. 248).
 */
double mean(Vecdouble const& x);

/**
 * Calculates the mean and variance of data. The variance is defined
 * as \f$\frac{1}{n} \sum_{i = 1}^n (x_i - m)^2\f$, where \f$m =
 * \frac{1}{n} \sum_{i = 1}^n x_i\f$. n is read as x.size().
 *
 * \exception std::invalid_argument if x.size() < 1
 *
 * \exception std::runtime_error if the returned value would be
 * negative
 *
 * \implementation The one-pass algorithm is used (see
 * \cite knuth-2002b, p. 248).
 */
void mean_var(Vecdouble const& x, double& mean, double& var);

/**
 * Calculates standard deviation of data. Standard deviation is
 * defined as \f$\sqrt{\frac{1}{n} \sum_{i = 1}^n (x_i - m)^2}\f$,
 * where \f$m = \frac{1}{n} \sum_{i = 1}^n x_i\f$.
 *
 * \exception std::invalid_argument if n = 0
 *
 * \exception std::range_error if !isfinite(returned value)
 *
 * \implementation The one-pass algorithm is used (see
 * \cite knuth-2002b, p. 248).
 */
double stddev(double const* x, std::size_t n);

/**
 * \copydoc stddev(double const*, std::size_t)
 */
inline double stddev(std::vector<double> const& x) {
     return stddev(&x[0], x.size());
}

/**
 * Calculates the mean and unbiased variance of data. The unbiased
 * variance is defined as \f$\frac{1}{n - 1} \sum_{i = 1}^n (x_i -
 * m)^2\f$, where \f$m = \frac{1}{n} \sum_{i = 1}^n x_i\f$. n is read
 * as x.size().
 *
 * \exception std::invalid_argument if x.size() < 2
 *
 * \exception std::runtime_error if the returned value would be
 * negative
 *
 * \implementation The one-pass algorithm is used (see
 * \cite knuth-2002b, p. 248).
 */
void mean_var1(Vecdouble const& x, double& mean, double& var);

/**
 * Calculates the autocorrelation function. The following formulas are
 * used (cf. \cite box-jenkins-1970, p. 32): \f[ r_k =
 * \frac{c_k}{c_0}, \f] \f[c_k = \frac{1}{n} \sum_{i = 0}^{n - 1 - k}
 * (x_i - m) (x_{i + k} - m), \quad k = 0, \ldots, K.\f] n is read as
 * x.size(). If there is no exception, r is resized to K + 1, r[0] is
 * always 1.
 *
 * \exception std::invalid_argument if n < 1 or K < 0
 *
 * \exception std::runtime_error if the calculated variance is not
 * greater than 1e-13
 */
void acf(Vecdouble const& x, int K, Vecdouble& r);

/**
 * Calculates the autocorrelation function. The difference between
 * this function and acf(Vecdouble const&, int, Vecdouble&) is that
 * the mean of the data is known.
 */
void acf(Vecdouble const& x, double mean, int K, Vecdouble& r);

/**
 * Chi-squared test for normality. Mean and standard deviation of
 * normal distribution is estimated with maximum likelihood basing on
 * n observations x[0], ..., x[n - 1]. The line is divided into r
 * equiprobable segments and the statistic \f[\chi^2 = \frac{1}{np}
 * \sum_{k = 0}^{r - 1} (n_k - np)^2\f] is calculated, where \f$n_k\f$
 * is the number of elements in the k-th segment. The statistic
 * \f$\chi^2\f$ has the chi-squared distribution with \f$r - 3\f$
 * degrees of freedom (\cite fisz-1969, p. 458). The p-value is
 * returned, ie. \f$\Pr(\chi^2_{r - 3} > \chi^2)\f$. The number of
 * observations must be at least 2 and the number of segments must not
 * be lower than 4.
 *
 * \exception std::invalid_argument if n < 2 or r < 4
 *
 * \exception std::runtime_error if estimated standard deviation is
 * not greater than 1e-13
 */
double chi2normtest(Vecdouble const& x, int r);

/**
 * Chi-squared test for standard normality. This function works as the
 * function chi2normtest(Vecdouble const&, int), but tests for
 * standard normality. Neither mean nor standard deviation is
 * estimated. The p-value is returned, ie. \f$\Pr(\chi^2_{r - 1} >
 * \chi^2)\f$. The number of observations must be at least 2 and the
 * number of segments must not be lower than 4.
 *
 * \exception std::invalid_argument if n < 2 or r < 4
 */
double chi2stdnormtest(Vecdouble const& x, int r);

/**
 * Kolmogorov-Smirnov test. Given a cumulative continuous distribution
 * function \a cdf and a vector of data \a x, the function performs
 * Kolmogorov-Smirnov test and returns the Kolmogorov-Smirnov
 * statistic D \a d and its p-value \a prob. Small values of prob
 * indicate that the cumulative distribution function of data is
 * significantly different from cdf.
 *
 * \warning After return, x is sorted non-decreasingly.
 *
 * \implementation This function is a modification of the function \a
 * ksone from \cite press-teukolsky-vetterling-flannery-2007, p. 737,
 * to allow function objects as parameters. We do not use
 * approximation (14.3.18) from
 * \cite press-teukolsky-vetterling-flannery-2007, p. 737. See also
 * \cite zielinski-zielinski-1990, p. 306.
 */
template <class T>
void ksone(T& cdf, Vecdouble& x, double& d, double& prob);

/**
 * Kolmogorov-Smirnov test. Does the same as
 * ksone(T&, Vecdouble&, double&, double&), but \a x is not sorted
 * after return (\a x is copied to a local variable and then sorted).
 */
template <class T>
void ksonec(T& cdf, Vecdouble const& x, double& d, double& prob);

/**
 * Kolmogorov-Smirnov test. Given a cumulative distribution function
 * \a cdf of a discrete distribution taking only integer values, a
 * vector of data \a x, the function performs Kolmogorov-Smirnov test
 * and returns the Kolmogorov-Smirnov statistic D \a d and its p-value
 * \a prob.
 *
 * \implementation See implementation of ksone(T& cdf, Vecdouble& x,
 * double& d, double& prob), \cite arnold-emerson-2011 from <a href =
 * "http://journal.r-project.org/">The R Journal</a> and
 * \cite press-teukolsky-vetterling-flannery-2007, page 738.
 */
template <class T>
void ksoned(T& cdf, Vecint& x, double& d, double& prob);

/**
 * Kolmogorov-Smirnov test. Does the same as
 * ksoned(T&, Vecint&, double&, double&), but \a x is
 * not sorted after return (\a x is copied to a local variable and
 * then sorted).
 */
template <class T>
void ksonedc(T& cdf, Vecint const& x, double& d, double& prob);

/**
 * Kolmogorov-Smirnov test for normality.
 *
 * Mean \f$m\f$ and standard deviation \f$s\f$ of normal distribution
 * is estimated with maximum likelihood basing on n observations x[0],
 * ..., x[n - 1]. Then the Kolmogorov-Smirnov test in ksone for
 * standard normal distribution is performed for normalized data, ie.
 * for the set \f$(x_i - m) / s\f$. The function returns the
 * Kolmogorov-Smirnov statistic D \a d and its p-value \a prob.
 *
 * \exception std::runtime_error if estimated standard deviation is
 * not greater than 1e-13
 */
void ksnormtest(Vecdouble const& x, double& d, double& prob);

/**
 * The basic sample statistics.
 *
 * The class provides basic sample statistics based on ordering of the
 * sample. The constructor accepts a vector which contains
 * observations.
 *
 * The methods Sample::histogram() const and
 * Sample::histogram(int) const prepare data for plotting a histogram.
 * The interval [min, max], where min and max are the minimum and
 * maximum values in the sample, is divided into k intervals of equal
 * length h = (max - min) / k:
 *
 * [min + j * h, min + (j + 1) * h), j = 0, 1, ..., k - 1
 *
 * (the last interval being right-closed to contain all observations).
 * The height of each bar is proportional to the number of
 * observations in the respective interval but the histogram is so
 * scaled that the total area of all the bars is equal to one. Thus,
 * the heights of the bars are neither relative frequencies nor the
 * number of elements in each interval. This property allows putting
 * another density on the same picture. The parameterless method
 * chooses k approximately as 2 * (sample size)^(-1/3) *
 * (interquantile range) (see \cite freedman-diaconis-1981); if it is
 * impossible, it tries 2 * (sample size)^(-1/3) * (range). These
 * methods are succesful if f.size() > 0 for f in the returned
 * structure. If f.size() == 0, either k < 1 was given or the length h
 * turned out to be less than Sample::eps.
 */
class Sample {
public:
     /**
      * \param [in] v vector of observations
      * \exception std::invalid_argument if v.size() == 0
      * \note The vector of observations is copied to a local vector.
      */
     explicit Sample(std::vector<double> const& v);
     /**
      * Right-continuous sample cdf.
      */
     double cdf(double x) const;
     /**
      * Left-continuous sample cdf.
      */
     double lcdf(double x) const;
     /**
      * p-th quantile.
      *
      * \exception SHG::Invalid_argument unless 0 < p < 1
      */
     double quantile(double p) const;
     /**
      * Sample range (max() - min()).
      */
     double range() const;
     /**
      * Interquartile range (quantile(0.75) - quantile(0.25)).
      */
     double iqr() const;
     /**
      * Sample minimum.
      */
     double min() const;
     /**
      * Sample maximum.
      */
     double max() const;
     /**
      * Structure returned by Sample::histogram() const and
      * Sample::histogram(int) const.
      */
     struct Histdata {
          Histdata(double min, double max);
          double min;             ///< begin of interval for the plot
          double max;             ///< end of interval for the plot
          double h;               ///< common width of the bars
          double maxheight;       ///< the highest bar
          std::vector<double> f;  ///< height of each bar
     };
     Histdata histogram() const;
     Histdata histogram(int k) const;

private:
     std::vector<double> const data;
     std::size_t const n;
     double const en;
     static double const eps;
     /**
      * Only to have const in front of Sample::data.
      */
     static std::vector<double> srt(std::vector<double> const& v);
};

/**
 * Returns run length distribution.
 *
 * Let \f$x_0, \ldots, x_{n - 1}\f$, \f$n \geq 1\f$, be a sequence
 * such that \f$x_i \in \{0, \ldots, m - 1\}\f$, \f$i = 1, \ldots,
 * n\f$, \f$m \geq 1\f$. The function returns such a vector \f$v\f$
 * that \f$v[i]\f$ contains lengths of all runs of elements \f$i\f$
 * for \f$i = 0, \ldots, m - 1\f$.
 *
 * For example, for \f$x = [0, 0, 1, 0, 2, 2, 1, 0, 1, 2, 2, 1, 0, 0,
 * 0, 0, 1, 1, 2]\f$ the function returns \f$v[0] = [2, 1, 1, 4]\f$,
 * \f$v[1] = [1, 1, 1, 1, 2]\f$, \f$v[2] = [2, 2, 1]\f$.
 *
 * \exception std::invalid_argument if an element \f$x_i\f$ is found
 * such that \f$x_i < 0\f$ or \f$x_i \geq m\f$.
 */
std::vector<std::vector<int>> run_length_distribution(
     std::vector<int> const& x, int m);

/**
 * Maximum likelihood estimation of the parameter of the logarithmic
 * series distribution. Logarithmic series distribution has the
 * probability function \cite devroye-1986, p. 545 \f[\Pr(X = n) =
 * \frac{a}{n}p^n, \quad n \in \mathbf{N}, \quad 0 < p < 1,\f] where
 * \f$a = -1 / \ln(1 - p)\f$. Its mean and variance are, respectively,
 * \f[ \frac{ap}{1 - p}, \quad \frac{ap(1 - ap)}{(1 - p)^2}.\f] The
 * maximum likelihood estimator of \f$p\f$ depends only on the sample
 * mean. Given this mean, the function returns the estimate of
 * \f$p\f$. The sample mean must be greater than 1 (sample mean equal
 * to 1 implies degenerate distribution and sample mean less than 1 is
 * impossible).
 *
 * \param [in] mean sample mean
 *
 * \exception SHG::Invalid_argument unless mean > 1.0
 *
 * \implementation The logarithm of likelihood \f$L\f$ of the sample
 * \f$x_1, \ldots, x_n\f$ is \f[ \ln L(p) = -n \ln(-\ln(1 - p)) -
 * \ln(x_1 \ldots x_n) + n \bar{x} \ln p, \f] where \f$\bar{x} = (x_1
 * + \ldots + x_n) / n\f$. To maximize it, the second term on the
 * right hand side may be left out and the remaining terms may be
 * divided by \f$n\f$. Thus, the maximized function is \f[ f(p) =
 * \bar{x} \ln(p) - \ln(-\ln(1 - p)). \f] This functions has the
 * following derivatives: \f{align*} f'(p) &= \frac{\bar{x}}{p} +
 * \frac{1}{(1 - p) \ln(1 - p)} \\ f''(p) &= - \frac{\bar{x}}{p^2} +
 * \frac{1 + \ln(1 - p)}{[(1 - p) \ln(1 - p)]^2} \f} The algorithm
 * solves the equation \f$f'(p) = 0\f$ by the method of bisection.
 */
double mle_lsd(double mean);

/**
 * Cumulative distribution function of the logarithmic series
 * distribution. Calculates cumulative distribution function of the
 * logarithmic series distribution (see mle_lsd(double)), ie. \f[
 * \Pr(X \leq x) = \sum_{k = 1}^{\lfloor x \rfloor} \frac{a}{k}p^k \f]
 * for \f$x \geq 1\f$ and 0 for \f$x < 1\f$.
 *
 * \param [in] x point at which the function is to be calculated
 * \param [in] p parameter of the distribution, must be in \f$(0,
 * 1)\f$
 *
 * \exception SHG::Invalid_argument unless \f$p \in (0, 1) \f$
 * \exception std::range_error on range error
 */
double cdf_lsd(double x, double p);

/**
 * Maximum likelihood estimation of the parameters of the negative
 * binomial distribution. A random variable \f$X\f$ has a negative
 * binomial distribution if its probability function is of the form
 * \cite devroye-1986, p. 543, \cite knuth-2002b, p. 149 \f[ \Pr(X =
 * n) = \binom{t - 1 + n}{n} p^t (1 - p)^n, \quad n = 0, 1, 2, \ldots,
 * \f] where \f$t > 0\f$ and \f$0 < p < 1\f$ are real parameters. Its
 * mean and variance are, respectively, \f[ \frac{t(1 - p)}{p}, \quad
 * \frac{t(1 - p)}{p^2}. \f] Given the sample, the function calculates
 * the maximum likelihood estimates of \f$t\f$ and \f$p\f$.
 *
 * \param [in] x vector of sample observations
 * \param [out] t estimated parameter \f$t\f$
 * \param [out] p estimated parameter \f$p\f$
 *
 * \exception std::invalid_argument if the sample size is less than 1
 * or if a sample element is negative
 *
 * \exception std::runtime_error if the sample mean or variance is too
 * close to 0
 *
 * \exception std::runtime_error if the point estimator of \f$p\f$
 * turns out to be outside of \f$(0, 1)\f$ or if the estimator of
 * \f$t\f$ turns out to be non-positive
 *
 * \implementation The logarithm of likelihood \f$L\f$ of the sample
 * \f$x_1, \ldots, x_n\f$ is \f[ \ln L(t, p) = \sum_{i = 1}^n [\ln
 * \Gamma(t + x_i) - \ln \Gamma(t)] - \sum_{i = 1}^n \ln x_i! + nt \ln
 * p + n \bar{x} \ln(1 - p), \f] where \f$\bar{x} = (x_1 + \ldots +
 * x_n) / n\f$. Solving \f[ \frac{\partial \ln L}{\partial p} =
 * \frac{nt}{p} - \frac{n\bar{x}}{1 - p} = 0 \f] gives \f[ p =
 * \frac{t}{t + \bar{x}}. \f] The equation \f[ \frac{\partial \ln
 * L}{\partial t} = 0 \f] with \f$p\f$ substituted by \f[\frac{t}{t +
 * \bar{x}}\f] takes the form \f[ f(t) \stackrel{\mathrm{def}}{=}
 * \sum_{i = 1}^n \left[ \frac{\Gamma'(t + x_i)}{\Gamma(t + x_i)} -
 * \frac{\Gamma'(t)}{\Gamma(t)} \right] + n \ln \frac{t}{t + \bar{x}}
 * = 0. \f] Differentiating known formula \f[ \Gamma(t + x) = (t + x -
 * 1)(t + x - 2) \ldots (t + 1) t \Gamma(t), \quad x \in \{1, 2,
 * \ldots \}, \f] we get \f[ \frac{\Gamma'(t + x)}{\Gamma(t + x)} -
 * \frac{\Gamma'(t)}{\Gamma(t)} = \frac{1}{t} + \frac{1}{t + 1} +
 * \ldots \frac{1}{t + x - 2} + \frac{1}{t + x - 1}, \f] so \f[ f(t) =
 * \sum_{\{i \colon x_i \neq 0 \}} \left[ \frac{1}{t} + \frac{1}{t +
 * 1} + \ldots + \frac{1}{t + (x_i - 1)} \right] + n \ln \frac{t}{t +
 * \bar{x}}. \f]
 *
 * The algorithm solves the equation \f$f(t) = 0\f$ by the Brent
 * method \cite press-teukolsky-vetterling-flannery-2007, p. 454--456.
 *
 * It may be noted that \f[ f(t) = \sum_{i = 1}^n \psi(t + x_i) - n
 * \psi(t) + n \ln \frac{t}{t + \bar{x}}, \f] where \f$\psi(t) =
 * \Gamma'(t) / \Gamma(t)\f$ is the logarithmic derivative of
 * \f$\Gamma\f$. The first and second derivatives of \f$f\f$ are easy
 * to calculate, what might be useful implementing an algorithm with
 * derivatives.
 */
void mle_negative_binomial(SHG::Vecint const& x, double& t,
                           double& p);

/**
 * Cumulative distribution function of the negative binomial
 * distribution. Calculates cumulative distribution function of the
 * negative binomial distribution described in
 * mle_negative_binomial(SHG::Vecint const&, double&, double&), ie.
 * \f[ \Pr(X \leq x) = \sum_{k = 0}^{\lfloor x \rfloor} \binom{t - 1 +
 * k}{k} p^t (1 - p)^k \f] for \f$x \geq 0\f$ and 0 for \f$x < 0\f$.
 *
 * \param [in] x point at which the function is to be calculated
 * \param [in] t parameter of the distribution, must be positive
 * \param [in] p parameter of the distribution, must be in \f$(0,
 * 1)\f$
 *
 * \exception std::invalid_argument unless \f$t > 0\f$ and \f$p \in
 * (0, 1)\f$
 */
double cdf_negative_binomial(double x, double t, double p);

/**
 * Assessment of discrete distribution. The class carries
 * Kolmogorov-Smirnov goodness-of-fit test of data for geometric,
 * Poisson, logarithmic and negative binomial distribution.
 */
class Assessment_of_discrete_distribution {
public:
     /**
      * \exception std::invalid_argument in case of degenerate
      * distribution or negative values in the sample
      */
     explicit Assessment_of_discrete_distribution(
          SHG::Vecint const& x);
     /**
      * Performs calculations.
      */
     void run();
     /**
      * Returns p-value of Kolmogorov-Smirnov statistic D for
      * geometric distribution.
      */
     double geometric() const { return geometric_; }
     /**
      * Returns p-value of Kolmogorov-Smirnov statistic D for Poisson
      * distribution.
      */
     double poisson() const { return poisson_; }
     /**
      * Returns p-value of Kolmogorov-Smirnov statistic D for
      * logarithmic distribution.
      */
     double logarithmic() const { return logarithmic_; }
     /**
      * Returns p-value of Kolmogorov-Smirnov statistic D for negative
      * binomial distribution.
      */
     double negbin() const { return negbin_; }

private:
     int const n;
     SHG::Vecint const x;
     double mean_;
     double var_;
     double geometric_;
     double poisson_;
     double logarithmic_;
     double negbin_;
};

/**
 * Univariate Gaussian mixture models.
 *
 * Univariate Gaussian mixture model estimation by
 * expectation-maximization algorithm. After initialization with data
 * vector \a x and the number of components \a K, user must set
 * initial values of Unigaumixmod::pi, Unigaumixmod::mu,
 * Unigaumixmod::sigma, call estep() and mstep(). Then estep() and
 * mstep() must be called alternately until the difference between two
 * consecutive values of Unigaumixmod::loglik returned by estep() is
 * sufficiently small.
 *
 * \implementation The class is modeled after the class Gaumixmod
 * \cite press-teukolsky-vetterling-flannery-2007, p. 845, and based
 * on \cite krzysko-wolynski-gorecki-skorzybut-2008, p. 359.
 *
 * \sa SHG::Laplace::Unilapmixmod
 */
struct Unigaumixmod {
     struct Degenerate_distribution : public virtual Exception {
          Degenerate_distribution();
     };
     Unigaumixmod(Vecdouble const& x, int K);
     /**
      * Performs e-step.
      *
      * \returns difference between new and old values of
      * Unigaumixmod::loglik
      */
     double estep();
     /**
      * Performs m-step.
      *
      * \exception SHG::Unigaumixmod::Degenerate_distribution if in a
      * component distribution sigma = 0
      */
     void mstep();

     int const n;         ///< number of observations
     int const K;         ///< number of components
     Vecdouble const& x;  ///< observations
     Vecdouble pi;        ///< weights
     Vecdouble mu;        ///< means of normal components
     Vecdouble sigma;     ///< std. dev. of normal components
     Matdouble psi;       ///< the matrix psi
     double loglik;       ///< loglikelihood found
};

template <class T>
void ksone(T& cdf, Vecdouble& x, double& d, double& prob) {
     int const n = x.size();
     double dt, en = n, ff, fn, fo = 0.0;
     std::sort(x.begin(), x.end());
     d = 0.0;
     for (int j = 0; j < n; j++) {
          fn = (j + 1.0) / en;
          ff = cdf(x[j]);
          dt = std::max(std::abs(fo - ff), std::abs(fn - ff));
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

template <class T>
void ksonec(T& cdf, Vecdouble const& x, double& d, double& prob) {
     Vecdouble v(x);
     ksone(cdf, v, d, prob);
}

template <class T>
void ksoned(T& cdf, Vecint& x, double& d, double& prob) {
     // For discrete distributions with jumps in integer numbers this
     // eps is enough.
     static double const eps = 1e-5;
     int const n = x.size();
     double dt, en = n, ff, fn, fe, fo = 0.0;
     std::sort(x.begin(), x.end());
     d = 0.0;
     for (int j = 0; j < n;) {
          while (++j < n && x[j] == x[j - 1]) { /* VOID */
          }
          fn = j / en;
          ff = cdf(x[j - 1]);
          fe = cdf(x[j - 1] - eps);
          dt = std::max(std::abs(fo - fe), std::abs(fn - ff));
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

template <class T>
void ksonedc(T& cdf, Vecint const& x, double& d, double& prob) {
     Vecint v(x);
     ksoned(cdf, v, d, prob);
}

/**
 * Returns correlation coefficient between \f$x\f$ and \f$y\f$.
 *
 * Uses formula \f[ r = \frac{\sum_{i = 0}^{n - 1} (x_i - \bar{x})(y_i
 * - \bar{y})}{\sqrt{\sum_{i = 0}^{n - 1} (x_i - \bar{x})^2 \sum_{i =
 * 0}^{n - 1} (y_i - \bar{y})^2}}. \f]
 *
 * \implementation
 * See \cite bronsztejn-siemiendiajew-musiol-muhlig-2004, formula
 * 16.152b, page 849 or
 * \cite press-teukolsky-vetterling-flannery-2007, formula 14.5.1,
 * page 745.
 */
template <typename T>
T correlation_coefficient(std::vector<T> const& x,
                          std::vector<T> const& y) {
     static_assert(std::is_floating_point<T>::value);
     using Index = std::vector<T>::size_type;
     Index const n = x.size();
     SHG_VALIDATE(n >= 2);
     SHG_VALIDATE(y.size() == n);
     T const mx = std::accumulate(x.cbegin(), x.cend(), T(0)) / n;
     T const my = std::accumulate(y.cbegin(), y.cend(), T(0)) / n;
     T sxx = T(0), syy = T(0), sxy = T(0);
     for (Index i = 0; i < n; i++) {
          T const xt = x[i] - mx;
          T const yt = y[i] - my;
          sxy += xt * yt;
          sxx += SHG::sqr(xt);
          syy += SHG::sqr(yt);
     }
     T const r = sxy / std::sqrt(sxx * syy);
     if (!std::isfinite(r))
          SHG_THROW(std::runtime_error, "zero variation");
     return r;
}

/** \} */ /* end of group mathematical_statistics */

}  // namespace SHG

#endif
