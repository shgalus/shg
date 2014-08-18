/* smc.h: semi-Markov chain */

/**
 * \file include/shg/smc.h
 * Semi-Markov chain.
 * \date Created on 12 December 2011.
 */

#ifndef SHG_SMC_H
#define SHG_SMC_H

#include <cstdlib>
#include "shg/except.h"
#include "shg/matrix.h"
#include "shg/rng.h"
#include "shg/utils.h"

namespace SHG {

/**
 * \addtogroup mathematical_statistics
 *
 * \{
 */

/**
 * \defgroup HSMM Hidden semi-Markov models
 *
 * Classes and functions associated with hidden semi-Markov models
 *
 * \warning Objects defined in this module are unfinished and
 * obsolete.
 *
 * \{
 */

/**
 * Semi-Markov chain.
 *
 * \warning This class is unfinished and obsolete.
 */
struct SMC {
     /**
      * First we introduce sojourn time distributions. Struct STD is
      * used to derive five classes of these distributions:
      * 1. distribution with finite number of values,
      * 2. geometric distribution,
      * 3. negative binomial distribution,
      * 4. logarithmic series distribution,
      * 5. %Poisson distribution.
      *
      * Because negative binomial distribution and %Poisson
      * distribution both accept the value 0, at the moment the value
      * returned is increased by 1. It would be better to allow two
      * additional integer parameters A > 0 and B > 0 for %Poisson
      * distribution and return values of Y = AX + B, where X is the
      * value returned by the generator. In similar way we can
      * transform negative binomial distribution and also geometric
      * and logarithmic series distribution.
      */
     struct STD {
          virtual int generate(RNG& g) const = 0;
          virtual ~STD();
     };
     /**
      * x[i] with probability p[i]
      */
     struct Finite : public STD {
          Finite(const Vecint& x, const Vecdouble& p);
          virtual int generate(RNG& g) const;
     private:
          const Vecint x;
          const Vecdouble p;
     };
     /**
      * geometric, 0 < p <= 1
      */
     struct Geometric : public STD {
          explicit Geometric(double p);
          virtual int generate(RNG& g) const;
     private:
          const double p;
     };
     /**
      * negative binomial, t > 0, 0 < p < 1, plus 1
      */
     struct Negative_binomial : public STD {
          Negative_binomial(double t, double p);
          virtual int generate(RNG& g) const;
     private:
          const double t;
          const double p;
     };
     /**
      * logarithmic, 0 < p < 1
      */
     struct Logarithmic : public STD {
          explicit Logarithmic(double p);
          virtual int generate(RNG& g) const;
     private:
          const double p;
     };
     /**
      * Poisson, mu > 0, plus 1
      */
     struct Poisson : public STD {
          explicit Poisson(double mu);
          virtual int generate(RNG& g) const;
     private:
          const double mu;
     };

     explicit SMC(std::size_t s);
     SMC(const SMC&) = delete;
     SMC& operator=(const SMC&) = delete;
     ~SMC();

     /**
      * Generates T-element SMC if cut == true. If cut == false,
      * generates at least T elements to end the sample at the end of
      * the last sojourn time.
      *
      * Returns: 0 - ok, 1 - invalid dimension of alpha_ or P_, 2 -
      * incorrect value of alpha_, 3 - incorrect value of P_, 4 - std_
      * not assigned, 5 - g_ not assigned, 6 - T is not positive.
      *
      * The state space is {0, 1, ..., s_ - 1}. On successful return:
      *
      * Z[t], 0 <= t < T, is the generated SMC,
      *
      * X.size() == S.size() == J.size()
      *
      * The process entered the i-th successive state J[i] at the
      * moment S[i] and remained in this state during successive X[i]
      * moments. S[0] = 0, S[i] = S[i - 1] + X[i - 1] for i >= 1, Z[t]
      * = J[i] for t = S[i], ..., S[i] + X[i] - 1, i = 0, ...,
      * X.size().
      */
     int generate(int T, bool cut = false);

     const std::size_t s_;      ///< number of states: 0, ..., s_ - 1
     Vecdouble alpha_;          ///< initial state distribution
     Matdouble P_;              ///< transition matrix (P(i, i) = 0)
     STD* std_;                 ///< sojourn time distribution
     RNG* g_;                   ///< random number generator
     Vecint X;                  ///< successive sojourn times
     Vecint S;                  ///< time points state changes occur
     Vecint J;                  ///< visited states
     Vecint Z;                  ///< generated SMC

private:
     int check_data();
};

/**
 * This is a class to estimate a mixture of K - 1 univariate normal
 * distributions and one degenerate distribution.
 *
 * \warning This class is unfinished and obsolete.
 *
 * Values of status:
 * 1 - no estimation has been done
 */
struct Unideggaumix {
     Unideggaumix(int n, int K);
     void estimate();
     int get_status() {return status;}
     const int n;               ///< number of observations
     const int K;               ///< number of components
     const int K1;              ///< number of Gaussian components
     Vecdouble x;               ///< observations
     Vecdouble pi;              ///< weights
     Vecdouble mu;              ///< means of normal components
     Vecdouble sigma;           ///< std. dev. of normal components
     double x0;                 ///< point for degenerate distribution
     double fx0;                ///< probability function at x0
     double eps;                ///< when loglikelihoods converged
     int maxit;                 ///< maximum number of iterations
     Matdouble psi;             ///< the matrix psi
     double loglik;             ///< loglikelihood found
     int iter;                  ///< the number of iterations executed
private:
     int status;                ///< status returned by estimate
};

/** \} */       /* end of group HSMM */

/** \} */       /* end of group mathematical_statistics */

}       // namespace SHG

#endif
