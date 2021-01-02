/**
 * \file src/numalg.cc
 * Numerical algorithms.
 */

#include <shg/numalg.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_poly.h>

namespace SHG {

namespace {

int solve_polynomial_internal(const Vecdouble& a, Veccomplex& x) {
     int status = 0;
     if (a.size() < 1)
          return GSL_EINVAL;
     x.resize(a.size() - 1);
     gsl_poly_complex_workspace* w =
          gsl_poly_complex_workspace_alloc(a.size());
     if (w == nullptr) {
          status = GSL_ENOMEM;
     } else {
          status = gsl_poly_complex_solve(
               a.c_vec(), a.size(), w,
               reinterpret_cast<gsl_complex_packed_ptr>(x.c_vec()));
          gsl_poly_complex_workspace_free(w);
     }
     return status;
}

}  // anonymous namespace

void solve_polynomial(const Vecdouble& a, Veccomplex& x) {
     const int status = solve_polynomial_internal(a, x);
     if (status == GSL_SUCCESS)
          return;
     throw std::runtime_error(std::string{"solve_polynomial: "} +
                              gsl_strerror(status));
}

namespace {

int solve_linear_internal(Matdouble& a, Vecdouble& x) {
     gsl_matrix_view av =
          gsl_matrix_view_array(a.c_vec(), a.nrows(), a.ncols());
     gsl_vector_view xv = gsl_vector_view_array(x.c_vec(), x.size());
     gsl_permutation* p = gsl_permutation_alloc(x.size());
     int status = p == nullptr ? GSL_ENOMEM : 0;
     if (!status) {
          int signum;
          status = gsl_linalg_LU_decomp(&av.matrix, p, &signum);
          if (!status)
               status = gsl_linalg_LU_svx(&av.matrix, p, &xv.vector);
     }
     if (p != nullptr)
          gsl_permutation_free(p);
     return status;
}

}  // anonymous namespace

void solve_linear(Matdouble& a, Vecdouble& x) {
     const int status = solve_linear_internal(a, x);
     if (status == GSL_SUCCESS)
          return;
     throw std::runtime_error(std::string{"solve_linear: "} +
                              gsl_strerror(status));
}

void real_roots(const Vecdouble& a, Vecdouble& x) {
     constexpr double eps =
          100.0 * std::numeric_limits<double>::epsilon();
     const int d = degree_of_polynomial(a, eps);
     if (d < 1)
          throw std::invalid_argument(
               "degree of polynomial should be at least 1 in "
               "real_roots");
     Vecdouble a1(d + 1, a.c_vec());
     Veccomplex x_complex;
     solve_polynomial(a1, x_complex);
     std::vector<double> x1;
     for (const auto& z : x_complex)
          if (std::abs(z.imag()) <= eps * std::abs(z.real()))
               x1.push_back(z.real());
     std::sort(x1.begin(), x1.end());
     x = x1;
}

Minimizer_base::Minimizer_base(const gsl_min_fminimizer_type* T)
     : s_(gsl_min_fminimizer_alloc(T)) {
     if (s_ == nullptr)
          throw std::runtime_error(__func__);
}

Minimizer_base::~Minimizer_base() {
     gsl_min_fminimizer_free(s_);
}

gsl_min_fminimizer* Minimizer_base::get() const {
     return s_;
}

Minimizer::Minimizer(const gsl_min_fminimizer_type* T)
     : Minimizer_base(T) {}

void Minimizer::set(gsl_function* f, double x_minimum, double x_lower,
                    double x_upper) {
     const int st =
          gsl_min_fminimizer_set(s_, f, x_minimum, x_lower, x_upper);
     if (st != GSL_SUCCESS)
          throw std::invalid_argument(gsl_strerror(st));
     is_set_ = true;
     iter_ = 0;
}

int Minimizer::iterate(int max_iter, double epsabs, double epsrel) {
     if (!is_set_)
          throw std::logic_error("no function set to minimize");

     iter_ = 0;
     if (max_iter < 1)
          return 1;
     int st;

     do {
          iter_++;
          st = gsl_min_fminimizer_iterate(s_);
          if (st != GSL_SUCCESS)
               goto fail;

          x_minimum_ = gsl_min_fminimizer_x_minimum(s_);
          x_lower_ = gsl_min_fminimizer_x_lower(s_);
          x_upper_ = gsl_min_fminimizer_x_upper(s_);

          f_minimum_ = gsl_min_fminimizer_f_minimum(s_);
          f_lower_ = gsl_min_fminimizer_f_lower(s_);
          f_upper_ = gsl_min_fminimizer_f_upper(s_);

          st = gsl_min_test_interval(x_lower_, x_upper_, epsabs,
                                     epsrel);
          if (st == GSL_SUCCESS)
               return 0;
     } while (st == GSL_CONTINUE && iter_ < max_iter);

     if (st != GSL_CONTINUE)
          goto fail;
     return 1;

fail:
     is_set_ = false;
     throw std::runtime_error(gsl_strerror(st));
}

void Uniform_search_for_minimum::search(
     std::function<double(double)> f, double a, double b,
     double eps) {
     if (b <= a || eps <= 0.0)
          throw std::invalid_argument(__func__);
     const double nd = std::ceil((2.0 / eps) * (b - a));
     if (nd > std::numeric_limits<int>::max())
          throw std::runtime_error(__func__);

     int n = nd;
     if (n < 2)
          n = 2;
     const double h = (b - a) / n;
     result.clear();
     Result r;

     r.x_minimum = a;
     r.x_upper = n == 2 ? b : a + 2 * h;
     r.f_minimum = f(r.x_minimum);
     r.f_upper = f(r.x_upper);

     for (int k = 2; k <= n; k++) {
          r.x_lower = r.x_minimum;
          r.x_minimum = r.x_upper;
          r.x_upper = k == n ? b : a + k * h;
          r.f_lower = r.f_minimum;
          r.f_minimum = r.f_upper;
          r.f_upper = f(r.x_upper);
          if (r.f_lower > r.f_minimum && r.f_minimum < r.f_upper)
               result.push_back(r);
     }
}

Multimin_fminimizer_base::Multimin_fminimizer_base(
     const gsl_multimin_fminimizer_type* T, size_t n)
     : s_(gsl_multimin_fminimizer_alloc(T, n)) {
     if (s_ == nullptr)
          throw std::runtime_error(__func__);
}

Multimin_fminimizer_base::~Multimin_fminimizer_base() {
     gsl_multimin_fminimizer_free(s_);
}

gsl_multimin_fminimizer* Multimin_fminimizer_base::get() const {
     return s_;
}

Multimin_fminimizer::Multimin_fminimizer(
     const gsl_multimin_fminimizer_type* T, size_t n)
     : Multimin_fminimizer_base(T, n),
       n_(n),
       x_(gsl_vector_alloc(n)),
       ss_(gsl_vector_alloc(n)),
       x_minimum_(n),
       f_minimum_() {
     if (x_ == nullptr || ss_ == nullptr) {
          if (ss_ != nullptr)
               gsl_vector_free(ss_);
          if (x_ != nullptr)
               gsl_vector_free(x_);
          throw std::runtime_error(__func__);
     }
}

Multimin_fminimizer::~Multimin_fminimizer() {
     gsl_vector_free(ss_);
     gsl_vector_free(x_);
}

void Multimin_fminimizer::set(gsl_multimin_function* f,
                              const std::vector<double>& x,
                              const std::vector<double>& step) {
     if (x.size() != n_ || step.size() != n_)
          throw std::logic_error(__func__);
     for (std::size_t i = 0; i < n_; i++) {
          gsl_vector_set(x_, i, x[i]);
          gsl_vector_set(ss_, i, step[i]);
     }
     const int st = gsl_multimin_fminimizer_set(s_, f, x_, ss_);
     if (st != GSL_SUCCESS)
          throw std::invalid_argument(gsl_strerror(st));
     is_set_ = true;
     iter_ = 0;
}

int Multimin_fminimizer::iterate(int max_iter, double eps) {
     if (!is_set_)
          throw std::logic_error("no function set to minimize");

     if (eps <= 0.0)
          throw std::invalid_argument(__func__);

     iter_ = 0;
     if (max_iter < 1)
          return 1;
     int st;

     do {
          iter_++;
          st = gsl_multimin_fminimizer_iterate(s_);
          if (st != GSL_SUCCESS)
               goto fail;
          gsl_vector* v = gsl_multimin_fminimizer_x(s_);
          for (std::size_t i = 0; i < n_; i++)
               x_minimum_[i] = gsl_vector_get(v, i);
          f_minimum_ = gsl_multimin_fminimizer_minimum(s_);
          const double size = gsl_multimin_fminimizer_size(s_);
          st = gsl_multimin_test_size(size, eps);
          if (st == GSL_SUCCESS)
               return 0;
     } while (st == GSL_CONTINUE && iter_ < max_iter);

     if (st != GSL_CONTINUE)
          goto fail;
     return 1;

fail:
     is_set_ = false;
     throw std::runtime_error(gsl_strerror(st));
}

}  // namespace SHG
