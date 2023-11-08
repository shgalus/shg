/**
 * \file include/shg/numalg.h
 * Numerical algorithms.
 */

#ifndef SHG_NUMALG_H
#define SHG_NUMALG_H

#include <functional>
#include <limits>
#include <stdexcept>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_multimin.h>
#include <shg/fcmp.h>
#include <shg/matrix.h>
#include <shg/vector.h>

namespace SHG {

/**
 * \addtogroup numerical_analysis
 *
 * \{
 */

/**
 * Degree of polynomial in one variable.
 *
 * \returns \f$\max \{i: a_i \neq 0, i = 0, \ldots, m - 1 \}\f$ if it
 * exists, otherwise -1.
 *
 * \throws std::invalid_argument if a.size() == 0 or a.size() > 32768.
 */
template <template <class> class Indexed_container, class T>
int degree_of_polynomial(Indexed_container<T> const& a, T eps);

/**
 * Calculates real roots of polynomial a[n] x^n + ... + a[2] x^2 +
 * a[1] x^1 + a[0]. Calculated roots are put in x[0] < x[1] < ... <
 * x[m - 1], where m = x.size().
 *
 * \throws std::invalid_argument if degree of polynomial is less than
 * 1
 * \throws std::runtime_error if the GSL routine fails
 */
void real_roots(Vecdouble const& a, Vecdouble& x);

template <template <class> class Indexed_container, class T>
int degree_of_polynomial(Indexed_container<T> const& a, T eps) {
     static_assert(std::numeric_limits<int>::max() >= 32767,
                   "According to the standard, INT_MAX must be at "
                   "least 32767.");
     auto k = a.size();
     if (k == 0 || k > 32768u)
          throw std::invalid_argument(
               "invalid polynomial in degree_of_polynomial");
     while (k-- > 0)
          if (fane(a[k], T(0), eps))
               return k;
     return -1;
}

/**
 * Solves the equation \f$a_0 + a_1x + \ldots + a_{n - 1}x^{n - 1} =
 * 0\f$, \f$n \geq 1\f$.
 */
void solve_polynomial(Vecdouble const& a, Veccomplex& x);

/**
 * Solves system of linear equations.
 */
void solve_linear(Matdouble& a, Vecdouble& x);

/** Light wrapper for GSL minimizer. */
class Minimizer_base {
public:
     explicit Minimizer_base(gsl_min_fminimizer_type const* T);
     Minimizer_base(Minimizer_base const&) = delete;
     Minimizer_base& operator=(Minimizer_base const&) = delete;
     virtual ~Minimizer_base();
     gsl_min_fminimizer* get() const;

private:
     gsl_min_fminimizer* s_;
};

/** Wrapper for GSL minimizer. */
class Minimizer : Minimizer_base {
public:
     explicit Minimizer(gsl_min_fminimizer_type const* T);
     Minimizer(Minimizer const&) = delete;
     Minimizer& operator=(Minimizer const&) = delete;

     bool is_set() const { return is_set_; }

     void set(gsl_function* f, double x_minimum, double x_lower,
              double x_upper);

     int iterate(int max_iter, double epsabs, double epsrel);

     int iter() const { return iter_; }

     double x_minimum() const { return x_minimum_; }
     double x_lower() const { return x_lower_; }
     double x_upper() const { return x_upper_; }
     double f_minimum() const { return f_minimum_; }
     double f_lower() const { return f_lower_; }
     double f_upper() const { return f_upper_; }

private:
     gsl_min_fminimizer* s_{get()};
     int iter_{0};
     bool is_set_{false};
     double x_minimum_{};
     double x_lower_{};
     double x_upper_{};
     double f_minimum_{};
     double f_lower_{};
     double f_upper_{};
};

/**
 * Uniform search for minimum of the function of one variable.
 *
 * The method search() searches uniformly the interval \f$[a, b]\f$
 * for minima of the function \f$f\f$. The method is inspired by
 * \cite bronsztejn-siemiendiajew-musiol-muhlig-2004, section
 * 18.2.4.1, point 1, page 946.
 *
 * First, \f$n\f$ is calculated as \f[ n = \min \{ 2, \lceil
 * \frac{2}{\epsilon} (b - a) \rceil \}, \f] and then the interval
 * \f$[a, b]\f$ is divided into \f$n\f$ intervals \f$[x_k, x_{k +
 * 1}]\f$, \f$k = 0, \ldots n - 1\f$, where \f$x_k = a + kh\f$, \f$h =
 * (b - a) / n\f$. The values of \f$f\f$ are calculated in each
 * \f$x_k\f$ and all values \f[ (x_k, x_{k + 1}, x_{k + 2}, f(x_k),
 * f(x_{k + 1}), f(x_{k + 2})) \f] such that \f[ f(x_k) > f(x_{k + 1})
 * < f(x_{k + 2}) \f] are reported in \f$\var{result}\f$. The length
 * of the interval \f$[x_k, x_{k + 2}]\f$ is not greater than
 * \f$\epsilon\f$. The function is evaluated \f$n + 1\f$times.
 *
 * \throws std::invalid_argument if on entry \f$a \geq b\f$ or
 * \f$\epsilon \leq 0\f$.
 */
struct Uniform_search_for_minimum {
     struct Result {
          double x_lower;
          double x_minimum;
          double x_upper;
          double f_lower;
          double f_minimum;
          double f_upper;
     };
     std::vector<Result> result{};
     void search(std::function<double(double)> f, double a, double b,
                 double eps);
};

/** Light wrapper for GSL minimizer for functions of several
    variables. */
class Multimin_fminimizer_base {
public:
     Multimin_fminimizer_base(gsl_multimin_fminimizer_type const* T,
                              size_t n);
     Multimin_fminimizer_base(Multimin_fminimizer_base const&) =
          delete;
     Multimin_fminimizer_base& operator=(
          Multimin_fminimizer_base const&) = delete;
     virtual ~Multimin_fminimizer_base();
     gsl_multimin_fminimizer* get() const;

private:
     gsl_multimin_fminimizer* s_;
};

/** Wrapper for GSL minimizer for functions of several variables. */
class Multimin_fminimizer : public Multimin_fminimizer_base {
public:
     Multimin_fminimizer(gsl_multimin_fminimizer_type const* T,
                         size_t n);
     Multimin_fminimizer(Multimin_fminimizer const&) = delete;
     Multimin_fminimizer& operator=(Multimin_fminimizer const&) =
          delete;
     virtual ~Multimin_fminimizer();

     bool is_set() const { return is_set_; }

     int iter() const { return iter_; }

     void set(gsl_multimin_function* f, std::vector<double> const& x,
              std::vector<double> const& step);
     int iterate(int max_iter, double eps);

     std::vector<double> const& x_minimum() const {
          return x_minimum_;
     }
     double f_minimum() const { return f_minimum_; }

private:
     gsl_multimin_fminimizer* s_{get()};
     int iter_{0};
     bool is_set_{false};
     std::size_t n_;
     gsl_vector* x_;
     gsl_vector* ss_;
     std::vector<double> x_minimum_;
     double f_minimum_;
};

/** \} */

}  // namespace SHG

#endif
