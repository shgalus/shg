/* rng.h: random number generator */

/**
 * \file include/shg/rng.h
 * Random number generator.
 * \date Created on 11 December 2011.
 */

#ifndef SHG_RNG_H
#define SHG_RNG_H

#include <iostream>
#include "shg/vector.h"

namespace SHG {

/**
 * \defgroup random_number_generator Random number generator
 *
 * Random number generator.
 *
 * \{
 */

/**
 * A base class for defining random number generators. A derived class
 * should define operator() to get random numbers uniformly
 * distributed on [0, 1] and write(), read() methods to save and load
 * generator states using binary streams.
 */
class RNG {
public:
     /**
      * Random number uniformly distributed on [0, 1].
      */
     virtual double operator()() = 0;

     /**
      * Random number uniformly distributed on (0, 1].
      */
     double unipos();

     /**
      * Random number uniformly distributed on (0, 1).
      */
     double uniopen();

     /**
      * Random integer uniformly distributed on [0, b).
      *
      * \exception SHG::Invalid_argument unless b > 0
      */
     int uni(int b);

     /**
      * Random integer uniformly distributed on [a, b).
      *
      * \exception SHG::Invalid_argument unless a < b
      */
     int uni(int a, int b);

     /**
      * Random number exponentially distributed. The probability
      * density function of the exponential distribution is defined as
      * \f[f(x) = \exp(-x) \; \mbox{for $x \geq 0$}.\f]
      *
      * \implementation See \cite knuth-2002b, p. 141.
      */
     double exponential();

     /**
      * Random point uniformly distributed on the surface of a
      * simplex. The simplex is defined as \f[ \{(x_1, \ldots, x_n)
      * \colon x_1 + \ldots + x_n = 1, x_i \geq 0 \}. \f] n is read as
      * x.size().
      *
      * \exception SHG::Invalid_argument unless x.size() > 0
      *
      * \implementation See \cite wieczorkowski-zielinski-1997, p. 104.
      */
     void simplex_surface(Vecdouble& x);

     /**
      * Random number generated from a discrete distribution.
      * Probability function of the distribution is defined as \f[
      * \Pr(X = k) = p_k, 0 \leq k < n. \f] \f$n\f$ is read as
      * p.size(). There should be p.size() > 0 and p[0] + ... + p[n -
      * 1] = 1.
      *
      * \exception SHG::Invalid_argument unless p.size() > 0
      *
      * \implementation See \cite knuth-2002b, p. 127,
      * \cite wieczorkowski-zielinski-1997, p. 66.
      */
     int finite(const Vecdouble& p);

     /**
      * Random number normally distributed with mean 0 and variance 1.
      *
      * \implementation See \cite wieczorkowski-zielinski-1997, p. 48,
      * algorithm 3.2.
      */
     double normal();

     /**
      * Random sample of n from N numbers 0, 1,..., N - 1, 0 < n <= N.
      * Each subset of n elements is equally probable. Sample is
      * increasingly ordered.
      *
      * \exception SHG::Invalid_argument unless n > 0 and n <= N
      *
      * \implementation See \cite knuth-2002b, section 3.4.2,
      * algorithm S.
      */
     void random_sample(int n, int N, Vecint& x);

     /**
      * Random number from logarithmic series distribution.
      * Logarithmic series distribution with parameter 0 < p < 1 has
      * the probability function \f[ \Pr(X = i) = (a / i)p^i, \; i =
      * 1, 2, 3, \ldots, \; a = -\frac{1}{\log(1 - p)}.\f] The
      * expected value and variance are \f[ EX = ap / (1 - p), \; D^2X
      * = ap(1 - ap) / (1 - p)^2. \f]
      *
      * \exception SHG::Invalid_argument unless p > 0 and p < 1.0
      *
      * \exception std::overflow_error if the result exceeds
      * std::numeric_limits<unsigned long>::max(), what may happen for
      * p close to 1
      *
      * \implementation Kemp's second accelerated generator,
      * \cite devroye-1986, p. 548. See also <a href =
      * "http://luc.devroye.org/errors.pdf">an on-line errata</a>.
      */
     unsigned long logarithmic(double p);

     /**
      * Random number from geometric distribution. Geometric
      * distribution has the probability function \f[\Pr(X = n) = (1 -
      * p)^{n - 1}p, \; n = 1, 2, \ldots, \; 0 < p \leq 1.\f]
      *
      * \exception SHG::Invalid_argument unless p > 0.0 and p <= 1.0
      *
      * \exception std::overflow_error if the generated number exceeds
      * std::numeric_limits<unsigned long>::max(), what may happen for
      * p close to 0
      *
      * \implementation See \cite knuth-2002b, p. 145.
      */
     unsigned int geometric(double p);

     /**
      * Random number from gamma distribution. Gamma distribution has
      * the probability density function \f[ f(x) = \left\{
      * \begin{array} {ll} b^a / \Gamma(a) x^{a - 1} \exp(-bx) &
      * \mbox{for $x > 0$} \\ 0 & \mbox{for $x \leq 0$} \end{array}
      * \right.\f] where \f$b\f$ and \f$a\f$ are positive parameters.
      *
      * \exception SHG::Invalid_argument unless a > 0.0 and b > 0.0
      *
      * \implementation See \cite knuth-2002b, p. 142-143 and solution
      * to exercise 16 on p. 633. See also <a href =
      * "http://www.gnu.org/software/gsl/">GNU Scientific Library</a>,
      * version 1.15, file gsl-1.15/randist/gamma.c.
      */
     double gamma(double a = 1.0, double b = 1.0);

     /**
      * Random number from beta distribution. Beta distribution has
      * the probability density function \f[ f(x) = \left\{
      * \begin{array} {ll} \frac{1}{B(a, b)}x^{a - 1}(1 - x)^{b - 1} &
      * \mbox{for $0 < x < 1$} \\ 0 & \mbox{for $x \leq 0$ or $x \geq
      * 1$} \end{array} \right.\f] where \f$a\f$ and \f$b\f$ are
      * positive parameters and \f$B(a, b) = \Gamma(a)\Gamma(b) /
      * \Gamma(a + b)\f$ (see \cite fisz-1969, p. 167).
      *
      * \exception SHG::Invalid_argument unless a > 0.0 and b > 0.0
      *
      * \implementation See \cite knuth-2002b, p. 143. See also <a
      * href = "http://www.gnu.org/software/gsl/">GNU Scientific
      * Library</a>, version 1.15, file gsl-1.15/randist/beta.c.
      */
     double beta(double a, double b);

     /**
      * Random number from binomial distribution. Binomial
      * distribution has the probability function \f[ \Pr(X = k) =
      * \frac{n!}{k!(n - k)!}p^k(1 - p)^{n - k}, \; k = 0, 1, \ldots,
      * n, \; 0 \leq p \leq 1.\f]
      *
      * \exception SHG::Invalid_argument unless p >= 0.0 and p <= 1.0
      *
      * \implementation See \cite knuth-2002b, p. 143. See also <a
      * href = "http://www.gnu.org/software/gsl/">GNU Scientific
      * Library</a>, version 1.15, file gsl-1.15/randist/binomial.c.
      */
     unsigned int binomial(double p, unsigned int n);

     /**
      * Random number from Poisson distribution. Poisson distribution
      * has the probability function \f[ \Pr(X = n) = \frac{\mu^n}{n!}
      * \exp(-\mu), \; n = 0, 1, 2, \ldots, \f] where \f$\mu > 0\f$ is
      * the mean.
      *
      * \exception SHG::Invalid_argument unless mu > 0.0
      *
      * \implementation See \cite knuth-2002b, p. 145-146. See also <a
      * href = "http://www.gnu.org/software/gsl/">GNU Scientific
      * Library</a>, version 1.15, file gsl-1.15/randist/poisson.c.
      */
     unsigned int poisson(double mu);

     /**
      * Random number from negative binomial distribution. Negative
      * binomial distribution has the probability function \f[ \Pr(X =
      * n) = {{t - 1 + n} \choose {n}} p^t(1 - p)^n, \; n = 0, 1, 2,
      * \ldots,\f] where \f$t > 0\f$ and \f$0 < p < 1\f$.
      *
      * \exception SHG::Invalid_argument unless t > 0.0 and p > 0.0
      * and p < 1.0
      *
      * \implementation See \cite knuth-2002b, p. 149 (exercise 19)
      * and p. 633-634. See also <a href =
      * "http://www.gnu.org/software/gsl/">GNU Scientific Library</a>,
      * version 1.15, file gsl-1.15/randist/nbinomial.c.
      */
     unsigned int negative_binomial(double t, double p);

     /**
      * Random number from Laplace distribution. Laplace distribution
      * has the probability density function \f[f(x) =
      * \frac{1}{2\lambda} \exp \left(-\frac{|x - \mu|}{\lambda}
      * \right).\f] It is asserted that \f$\lambda > 0\f$.
      *
      * \exception SHG::Invalid_argument unless lambda > 0.0
      *
      * \implementation The method of inverse transform sampling is
      * used \cite wieczorkowski-zielinski-1997, p. 41.
      */
     double laplace(double mu, double lambda);

     /**
      * Saves the generator state in a stream. The stream should be
      * open in binary mode. After a call, the state of the stream
      * should be tested: if f.fail() returns true, an error has
      * occured.
      */
     virtual void write(std::ostream& f) const = 0;

     /**
      * Loads the generator state from a stream. The stream should be
      * open in binary mode. After a call, if f.fail() returns true,
      * the operation failed and the state remains the same as before
      * the call.
      */
     virtual void read(std::istream& f) = 0;

     virtual ~RNG();

private:
     double gamma_int(unsigned int a);
     double gamma_large(double a);
     double gamma_frac(double a);
};

/** \} */ /* end of group random_number_generator */

}       // namespace SHG

#endif
