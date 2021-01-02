#include <shg/numalg.h>
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <shg/mconsts.h>
#include <shg/utils.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(numalg_test)

// Since version 1928 Microsoft compiler refuses to compile these two
// test cases arguing that for T = std::complex<double>,
// Vector<T>::operator std::basic_string<T>() cannot be compiled as T
// is not a trivial standard-layout type. However, this operator is
// not instantiated here.

#if defined(_MSC_VER) && (_MSC_VER <= 1927)

// GNU Scientific Library - Reference Manual, section 6.6.
BOOST_AUTO_TEST_CASE(solve_polynomial_test) {
     const Vecdouble a{-1, 0, 0, 0, 0, 1};
     const Veccomplex z{
          {-0.809016994374947451, 0.587785252292473137},
          {-0.809016994374947451, -0.587785252292473137},
          {0.309016994374947451, 0.951056516295153642},
          {0.309016994374947451, -0.951056516295153642},
          {1.000000000000000000, 0.000000000000000000}};
     Veccomplex x;
     BOOST_REQUIRE_NO_THROW(solve_polynomial(a, x));
     BOOST_REQUIRE(x.size() == z.size());
     for (Veccomplex::size_type i = 0; i < x.size(); i++)
          BOOST_CHECK(std::abs(x(i) - z(i)) < 1e-15);
}

BOOST_AUTO_TEST_CASE(solve_polynomial_throws_test) {
     Vecdouble a;
     Veccomplex x;
     BOOST_CHECK_THROW(solve_polynomial(a, x), std::runtime_error);
     a = {0.0};
     BOOST_CHECK_THROW(solve_polynomial(a, x), std::runtime_error);
     a = {0.0, 0.0};
     BOOST_CHECK_THROW(solve_polynomial(a, x), std::runtime_error);
     a = {-1.0, 0.0, 0.0, 1.0, 0.0};
     BOOST_CHECK_THROW(solve_polynomial(a, x), std::runtime_error);
}

#endif  // defined(_MSC_VER) && (_MSC_VER <= 1927)

// GNU Scientific Library - Reference Manual, section 14.15.
BOOST_AUTO_TEST_CASE(solve_linear_test) {
     Matdouble A(4, 4,
                 {0.18, 0.60, 0.57, 0.96, 0.41, 0.24, 0.99, 0.58,
                  0.14, 0.30, 0.97, 0.66, 0.51, 0.13, 0.19, 0.85});
     Vecdouble b{1.0, 2.0, 3.0, 4.0};
     const Vecdouble x{-4.05205, -12.6056, 1.66091, 8.69377};
     BOOST_REQUIRE_NO_THROW(solve_linear(A, b));
     BOOST_REQUIRE(x.size() == b.size());
     for (Vecdouble::size_type i = 0; i < b.size(); i++)
          BOOST_CHECK(faeq(b(i), x(i), 2e-5));
}

BOOST_AUTO_TEST_CASE(solve_linear_throws_test) {
     Matdouble A{diagonal_matrix<double>(2)};
     Vecdouble x{1.0};
     auto is_correct = [](const std::runtime_error& e) {
          return std::strcmp(
                      e.what(),
                      "solve_linear: matrix/vector sizes are not "
                      "conformant") == 0;
     };
     BOOST_CHECK_EXCEPTION(solve_linear(A, x), std::runtime_error,
                           is_correct);
}

using test_types = boost::mpl::list<float, double, long double>;

/**
 * \todo Add testing with other indexed containers.
 */
BOOST_AUTO_TEST_CASE_TEMPLATE(degree_of_polynomial_test, T,
                              test_types) {
     constexpr T eps = std::numeric_limits<T>::min();
     Vector<T> a{1.0, 1.0, 1.0, 1.0};

     BOOST_CHECK(degree_of_polynomial(a, eps) == 3);
     a[3] = 0.0;
     BOOST_CHECK(degree_of_polynomial(a, eps) == 2);
     a[2] = 0.0;
     BOOST_CHECK(degree_of_polynomial(a, eps) == 1);
     a[1] = 0.0;
     BOOST_CHECK(degree_of_polynomial(a, eps) == 0);
     a[0] = 0.0;
     BOOST_CHECK(degree_of_polynomial(a, eps) == -1);

     BOOST_CHECK_THROW(degree_of_polynomial(Vector<T>(), eps),
                       std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(real_roots_test) {
     Vecdouble a, x, x0;
     double eps;
     auto eq = [&eps](double x, double y) {
          return std::abs(x - y) < eps;
     };

     BOOST_CHECK_THROW(real_roots(a, x), std::invalid_argument);
     a = {0.0};
     BOOST_CHECK_THROW(real_roots(a, x), std::invalid_argument);
     a = {1.0};
     BOOST_CHECK_THROW(real_roots(a, x), std::invalid_argument);
     a = {0.0, 0.0};
     BOOST_CHECK_THROW(real_roots(a, x), std::invalid_argument);
     a = {1.0, 0.0};
     BOOST_CHECK_THROW(real_roots(a, x), std::invalid_argument);

     a = {2.0, -3.0, 1.0};
     x0 = {1.0, 2.0};
     eps = 1e-13;
     real_roots(a, x);
     BOOST_CHECK(std::is_sorted(x.cbegin(), x.cend()));
     BOOST_REQUIRE(x.size() == x0.size());
     BOOST_CHECK(std::equal(x.cbegin(), x.cend(), x0.cbegin(), eq));

     a = {2.0, 3.0, 1.0};
     x0 = {-2.0, -1.0};
     eps = 1e-13;
     real_roots(a, x);
     BOOST_CHECK(std::is_sorted(x.cbegin(), x.cend()));
     BOOST_REQUIRE(x.size() == x0.size());
     BOOST_CHECK(std::equal(x.cbegin(), x.cend(), x0.cbegin(), eq));

     a = {1.0, 0.0, 1.0};
     x0 = {};
     eps = 1e-13;
     real_roots(a, x);
     BOOST_CHECK(std::is_sorted(x.cbegin(), x.cend()));
     BOOST_REQUIRE(x.size() == x0.size());
     BOOST_CHECK(std::equal(x.cbegin(), x.cend(), x0.cbegin(), eq));

     a = {1.0, 1.0, 0.0};
     x0 = {-1.0};
     eps = 1e-13;
     real_roots(a, x);
     BOOST_CHECK(std::is_sorted(x.cbegin(), x.cend()));
     BOOST_REQUIRE(x.size() == x0.size());
     BOOST_CHECK(std::equal(x.cbegin(), x.cend(), x0.cbegin(), eq));

     a = {1.0, 1.0};
     x0 = {-1.0};
     eps = 1e-13;
     real_roots(a, x);
     BOOST_CHECK(std::is_sorted(x.cbegin(), x.cend()));
     BOOST_REQUIRE(x.size() == x0.size());
     BOOST_CHECK(std::equal(x.cbegin(), x.cend(), x0.cbegin(), eq));

     a = {-1.0, 0.0, 0.0, 0.0, 1.0};
     x0 = {-1.0, 1.0};
     eps = 1e-13;
     real_roots(a, x);
     BOOST_CHECK(std::is_sorted(x.cbegin(), x.cend()));
     BOOST_REQUIRE(x.size() == x0.size());
     BOOST_CHECK(std::equal(x.cbegin(), x.cend(), x0.cbegin(), eq));

     // GNU Scientific Library - Reference Manual, section 6.5:
     //
     // Note that due to finite precision, roots of higher
     // multiplicity are returned as a cluster of simple roots with
     // reduced accuracy.

     a = {1.0, -4.0, 6.0, -4.0, 1.0};
     x0 = {1.0, 1.0, 1.0, 1.0};
     real_roots(a, x);
     BOOST_CHECK(std::is_sorted(x.cbegin(), x.cend()));
     BOOST_WARN(x.size() == x0.size());
     BOOST_TEST_MESSAGE(
          "Reduced accuracy for roots of higher multiplicity.");

     a = {-120.0, 274.0, -225.0, 85.0, -15.0, 1.0};
     x0 = {1.0, 2.0, 3.0, 4.0, 5.0};
     eps = 2e-13;
     real_roots(a, x);
     BOOST_CHECK(std::is_sorted(x.cbegin(), x.cend()));
     BOOST_REQUIRE(x.size() == x0.size());
     BOOST_CHECK(std::equal(x.cbegin(), x.cend(), x0.cbegin(), eq));

     a = {3003.0 / 12500000.0, -31.0 / 4000.0, 187.0 / 2000.0,
          -1.0 / 2.0, 1.0};
     x0 = {0.11, 0.12, 0.13, 0.14};
     eps = 2e-13;
     real_roots(a, x);
     BOOST_CHECK(std::is_sorted(x.cbegin(), x.cend()));
     BOOST_REQUIRE(x.size() == x0.size());
     BOOST_CHECK(std::equal(x.cbegin(), x.cend(), x0.cbegin(), eq));
}

// \cite [sec.~37.8, p.~417] {galassi-etal-2019}
double fn1(double x, void* params) {
     ignore_unused_variable(params);
     return cos(x) + 1.0;
}

// \cite [sec.~36.4, p.~403] {galassi-etal-2019}
struct fn2_params {
     double a;
     double b;
     double c;
};

double fn2(double x, void* p) {
     struct fn2_params* params = static_cast<struct fn2_params*>(p);
     double a = (params->a);
     double b = (params->b);
     double c = (params->c);

     return (a * x + b) * x + c;
}

BOOST_AUTO_TEST_CASE(minimizer_test) {
     Minimizer m(gsl_min_fminimizer_brent);
     gsl_function F;
     int status;

     F.function = &fn1;
     F.params = nullptr;
     m.set(&F, 2.0, 0.0, 6.0);
     status = m.iterate(100, 1e-7, 0.0);
     BOOST_REQUIRE(status == 0);
     BOOST_CHECK(m.iter() == 9);
     BOOST_CHECK(faeq(m.x_minimum(), Constants::pi<double>, 1e-8));

     struct fn2_params params = {3.0, 2.0, 1.0};
     F.function = &fn2;
     F.params = &params;
     BOOST_CHECK_THROW(m.set(&F, 0.75, 2.0, 6.0),
                       std::invalid_argument);
     m.set(&F, -0.5, -1.0, 0.0);
     status = m.iterate(100, 1e-8, 0.0);
     BOOST_REQUIRE(status == 0);
     BOOST_CHECK(m.iter() == 6);
     BOOST_CHECK(faeq(m.x_minimum(), -1.0 / 3.0, 5e-9));
}

BOOST_AUTO_TEST_CASE(Uniform_search_for_minimum_test) {
     auto f1 = [](double x) { return sqr(x); };
     Uniform_search_for_minimum s;

     s.search(f1, -1.0, 1.0, 0.01);
     BOOST_REQUIRE(s.result.size() == 1);
     const auto it = s.result.cbegin();
     BOOST_CHECK(faeq(it->x_lower, -0.005, tolerance));
     BOOST_CHECK(faeq(it->x_minimum, 0.0, tolerance));
     BOOST_CHECK(faeq(it->x_upper, 0.005, tolerance));
     BOOST_CHECK(faeq(it->f_lower, 0.000025, tolerance));
     BOOST_CHECK(faeq(it->f_minimum, 0.0, tolerance));
     BOOST_CHECK(faeq(it->f_upper, 0.000025, tolerance));

     BOOST_CHECK_THROW(s.search(f1, -1.0, 1.0, -0.01),
                       std::invalid_argument);
     BOOST_CHECK_THROW(s.search(f1, 1.0, -1.0, 0.01),
                       std::invalid_argument);
     const double eps = 3.0 / std::numeric_limits<int>::max();
     BOOST_CHECK_THROW(s.search(f1, -1.0, 1.0, eps),
                       std::runtime_error);
}

// \cite [sec.~39.4, p.~435--436] {galassi-etal-2019}
/* Paraboloid centered on (p[0],p[1]), with
   scale factors (p[2],p[3]) and minimum p[4] */
double my_f(const gsl_vector* v, void* params) {
     double x, y;
     double* p = static_cast<double*>(params);
     x = gsl_vector_get(v, 0);
     y = gsl_vector_get(v, 1);
     return p[2] * (x - p[0]) * (x - p[0]) +
            p[3] * (y - p[1]) * (y - p[1]) + p[4];
}

// \cite [sec.~39.9, p.~441--442] {galassi-etal-2019}
// 10(x - 1)^2 + 20(y - 2)^2 + 30
BOOST_AUTO_TEST_CASE(multi_minimizer_example_test) {
     double par[5] = {1.0, 2.0, 10.0, 20.0, 30.0};

     const gsl_multimin_fminimizer_type* T =
          gsl_multimin_fminimizer_nmsimplex2;
     gsl_multimin_fminimizer* s = NULL;
     gsl_vector *ss, *x;
     gsl_multimin_function minex_func;

     size_t iter = 0;
     int status;
     double size;

     /* Starting point */
     x = gsl_vector_alloc(2);
     gsl_vector_set(x, 0, 5.0);
     gsl_vector_set(x, 1, 7.0);

     /* Set initial step sizes to 1 */
     ss = gsl_vector_alloc(2);
     gsl_vector_set_all(ss, 1.0);

     /* Initialize method and iterate */
     minex_func.n = 2;
     minex_func.f = my_f;
     minex_func.params = par;

     s = gsl_multimin_fminimizer_alloc(T, 2);
     gsl_multimin_fminimizer_set(s, &minex_func, x, ss);

     do {
          iter++;
          status = gsl_multimin_fminimizer_iterate(s);
          if (status)
               break;
          size = gsl_multimin_fminimizer_size(s);
          status = gsl_multimin_test_size(size, 1e-2);
          if (status == GSL_SUCCESS)
               break;
     } while (status == GSL_CONTINUE && iter < 100);
     BOOST_REQUIRE(status == GSL_SUCCESS);
     BOOST_CHECK(iter == 24);
     BOOST_CHECK(faeq(gsl_vector_get(s->x, 0), 1.0, 1e-2));
     BOOST_CHECK(faeq(gsl_vector_get(s->x, 1), 2.0, 1e-2));
     BOOST_CHECK(faeq(s->fval, 30.0, 1e-3));
     gsl_vector_free(x);
     gsl_vector_free(ss);
     gsl_multimin_fminimizer_free(s);
}

BOOST_AUTO_TEST_CASE(multimin_fminimizer_test) {
     double par[5] = {1.0, 2.0, 10.0, 20.0, 30.0};
     Multimin_fminimizer m{gsl_multimin_fminimizer_nmsimplex2, 2};
     gsl_multimin_function F;

     F.n = 2;
     F.f = my_f;
     F.params = par;

     std::vector<double> x{5.0, 7.0};
     std::vector<double> step{1.0, 1.0};
     m.set(&F, x, step);
     int status = m.iterate(100, 1e-2);
     BOOST_REQUIRE(status == 0);
     BOOST_CHECK(m.iter() == 24);
     BOOST_CHECK(faeq(m.x_minimum()[0], 1.0, 1e-2));
     BOOST_CHECK(faeq(m.x_minimum()[1], 2.0, 1e-2));
     BOOST_CHECK(faeq(m.f_minimum(), 30.0, 1e-3));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
