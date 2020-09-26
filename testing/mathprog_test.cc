#include "shg/mathprog.h"
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(mathprog_test)

BOOST_AUTO_TEST_CASE(simplex_gass_76) {
     const Matdouble A{3,
                       4,
                       {1.0, 2.0, 3.0, 0.0, 2.0, 1.0, 5.0, 0.0, 1.0,
                        2.0, 1.0, 1.0}};
     const Vecdouble b{15.0, 20.0, 10.0};
     const Vecdouble c{-1.0, -2.0, -3.0, 1.0};
     const Simplex::Vecequality e{Simplex::eq, Simplex::eq,
                                  Simplex::eq};
     const Simplex::Direction d = Simplex::min;
     const double eps = 1e-9;

     Simplex s(A.nrows(), A.ncols(), A, b, c, e, d, eps);
     BOOST_REQUIRE(s.status == 0);
     BOOST_CHECK(faeq(s.f, -15.0, eps));
     BOOST_CHECK(faeq(s.x(0), 2.5, eps));
     BOOST_CHECK(faeq(s.x(1), 2.5, eps));
     BOOST_CHECK(faeq(s.x(2), 2.5, eps));
     BOOST_CHECK(faeq(s.x(3), 0.0, eps));
}

BOOST_AUTO_TEST_CASE(simplex_gass_82e) {
     const Matdouble A{4, 5, {0.0, 2.0,  -1.0, -1.0, 1.0,  -2.0, 0.0,
                              2.0, -1.0, 1.0,  1.0,  -2.0, 0.0,  -1.0,
                              1.0, 1.0,  1.0,  1.0,  0.0,  0.0}};
     const Vecdouble b{0.0, 0.0, 0.0, 1.0};
     const Vecdouble c{0.0, 0.0, 0.0, 1.0, -1.0};
     const Simplex::Vecequality e{Simplex::ge, Simplex::ge,
                                  Simplex::ge, Simplex::eq};
     const Simplex::Direction d = Simplex::max;
     const double eps = 1e-9;

     Simplex s(A.nrows(), A.ncols(), A, b, c, e, d, eps);
     BOOST_CHECK(s.status == 0);
     BOOST_CHECK(faeq(s.f, 0.0, eps));
     BOOST_CHECK(faeq(s.x(0), 0.4, eps));
     BOOST_CHECK(faeq(s.x(1), 0.2, eps));
     BOOST_CHECK(faeq(s.x(2), 0.4, eps));
     BOOST_CHECK(faeq(s.x(3), 0.0, eps));
     BOOST_CHECK(faeq(s.x(4), 0.0, eps));
}

BOOST_AUTO_TEST_CASE(simplex_gass_82g) {
     const Matdouble A{2, 3, {-2.0, 1.0, 3.0, 2.0, 3.0, 4.0}};
     const Vecdouble b{2.0, 1.0};
     const Vecdouble c{1.0, -2.0, 3.0};
     const Simplex::Vecequality e{Simplex::eq, Simplex::eq};
     const Simplex::Direction d = Simplex::min;
     const double eps = 1e-9;

     Simplex s(A.nrows(), A.ncols(), A, b, c, e, d, eps);
     BOOST_CHECK(s.status == 2);
}

/*
 * Solves the problem
 *
 * minimize
 *      -6x_1 + 2x_1^2 - 2x_1x_2 + 2x_2^2
 * subject to
 *      x_1 + x_2 <= 2
 *      x_j >= 0
 *
 * Cf. wolfe_grabowski_256
 */
BOOST_AUTO_TEST_CASE(wolfe_example) {
     const Vecdouble p{-6.0, 0.0, 0.0},
          C{2.0, -1.0, 0.0, 2.0, 0.0, 0.0}, b{2.0};
     Vecdouble x(3);
     const Matdouble A(1, 3, {1.0, 1.0, 0.0});
     double f;
     const int st = wolfe(p, C, A, b, x, f);
     BOOST_CHECK(st == 0);
     BOOST_CHECK(faeq(f, -5.5, 1e-16));
     BOOST_CHECK(faeq(x[0], 1.5, 1e-16));
     BOOST_CHECK(faeq(x[1], 0.5, 1e-16));
     BOOST_CHECK(faeq(x[2], 0.0, 1e-16));
}

BOOST_AUTO_TEST_CASE(wolfe_grabowski_247) {
     const int m = 2, n = 4;
     Vecdouble p(n, 0.0);
     Vecdouble C(n * (n + 1) / 2, 0.0);
     Vecdouble b(m, 0.0);
     Vecdouble x(n, 0.0);
     Vecdouble x0(n, 0.0);
     Matdouble A(m, n, 0.0);
     double f;
     double f0 = -100.0;
     p[0] = -10.0;
     p[1] = -25.0;
     C[0] = 10.0;
     C[1] = 2.0;
     C[4] = 1.0;
     A[0][0] = 1.0;
     A[0][1] = 2.0;
     A[0][2] = 1.0;
     A[1][0] = 1.0;
     A[1][1] = 1.0;
     A[1][3] = 1.0;
     b[0] = 10.0;
     b[1] = 9.0;
     x0[1] = 5.0;
     x0[3] = 4.0;
     const int st = wolfe(p, C, A, b, x, f);
     BOOST_REQUIRE(st == 0);
     BOOST_CHECK(faeq(f, f0, 1e-16));
     for (int i = 0; i < n; i++)
          BOOST_CHECK(faeq(x[i], x0[i], 1e-16));
}

BOOST_AUTO_TEST_CASE(wolfe_grabowski_256) {
     const int m = 1, n = 3;
     Vecdouble p(n, 0.0);
     Vecdouble C(n * (n + 1) / 2, 0.0);
     Vecdouble b(m, 0.0);
     Vecdouble x(n, 0.0);
     Vecdouble x0(n, 0.0);
     Matdouble A(m, n, 0.0);
     double f;
     double f0 = -5.5;
     p(0) = -6.0;
     C(0) = C(3) = 2.0;
     C(1) = -1.0;
     A(0, 0) = A(0, 1) = A(0, 2) = 1.0;
     b(0) = 2.0;
     x0(0) = 1.5;
     x0(1) = 0.5;
     const int st = wolfe(p, C, A, b, x, f);
     BOOST_REQUIRE(st == 0);
     BOOST_CHECK(faeq(f, f0, 1e-16));
     for (int i = 0; i < n; i++)
          BOOST_CHECK(faeq(x[i], x0[i], 1e-16));
}

/**
 * Quadratic programming problem. The problem \f{align*}
 * \mbox{minimize} & \quad -6x_1 + 2x_1^2 - 2x_1x_2 + 2x_2^2 \\
 * \mbox{subject to} & \quad x_1 + x_2 \leq 2, \\ & \quad x_j \geq 0
 * \f} (\cite gass-1980, p. 285). \f$C\f$ is not positive definite.
 */
BOOST_AUTO_TEST_CASE(wolfe_gass_285) {
     const int m = 2, n = 4;
     Vecdouble p(n, 0.0);
     Vecdouble C(n * (n + 1) / 2, 0.0);
     Vecdouble b(m, 0.0);
     Vecdouble x(n, 0.0);
     Vecdouble x0(n, 0.0);
     Matdouble A(m, n, 0.0);
     double f;
     double f0 = -22.0 / 9.0;
     p(0) = -2.0;
     p(1) = -1.0;
     C(0) = 1.0;
     A(0, 0) = 2.0;
     A(0, 1) = 3.0;
     A(0, 2) = 1.0;
     A(1, 0) = 2.0;
     A(1, 1) = 1.0;
     A(1, 3) = 1.0;
     b(0) = 6.0;
     b(1) = 4.0;
     x0(0) = 2.0 / 3.0;
     x0(1) = 14.0 / 9.0;
     x0(3) = 10.0 / 9.0;
     const int st = wolfe(p, C, A, b, x, f);
     BOOST_REQUIRE(st == 0);
     BOOST_CHECK(faeq(f, f0, 5e-16));
     for (int i = 0; i < n; i++)
          BOOST_CHECK(faeq(x[i], x0[i], 5e-16));
}

namespace bdata = boost::unit_test::data;

/*
 * \sum_{i = 1}^n (alpha[i] * x[i] - beta[i])^2, n big, alpha[i],
 * beta[i] >= 0 minimizes to zero at x[i] = beta[i] / alpha[i].
 */
BOOST_DATA_TEST_CASE(wolfe_simple,
                     bdata::xrange(2) * bdata::xrange(8), xr1, xr2) {
     const Vecint mm{1, 3};
     const Vecint nn{1, 5, 10, 20, 50, 100, 200, 500};
     const int m = mm(xr1);
     const int n = nn(xr2);
     Vecdouble p(n, 0.0);
     Vecdouble C(n * (n + 1) / 2, 0.0);
     Vecdouble b(m, 0.0);
     Vecdouble x(n, 0.0);
     Vecdouble x0(n, 0.0);
     Matdouble A(m, n, 0.0);
     double f, f0;
     int i, j, k;
     Vecdouble alpha(n), beta(n);
     for (i = 0; i < n; i++) {
          alpha(i) = i + 1;
          beta(i) = (i + 1) * (i + 1);
     }
     j = n;
     k = 0;
     for (i = 0; i < n; i++) {
          p(i) = -2.0 * alpha(i) * beta(i);
          x0(i) = beta(i) / alpha(i);
          C(k) = alpha(i) * alpha(i);
          k += j--;
     }
     for (i = 0; i < m; i++) {
          for (j = 0; j < n; j++)
               A(i, j) = 1;
          f = 0.0;
          for (j = 0; j < n; j++)
               f += beta(j) / alpha(j);
          b(i) = f;
     }
     f0 = 0.0;
     for (i = 0; i < n; i++)
          f0 -= beta(i) * beta(i);

     const int st = wolfe(p, C, A, b, x, f);
     BOOST_REQUIRE(st == 0);
     BOOST_CHECK(faeq(f, f0, 1e-16));
     for (int i = 0; i < n; i++)
          BOOST_CHECK(faeq(x[i], x0[i], 1e-16));
}

/*
 * The test data are generated in the following way. For a positive
 * definite symmetric matrix H and such matrix U, vector v and vector
 * z >= 0 that Uz = v, the form (Ux - v)^(T)H(Ux - v) has a minimum 0
 * at x = z. Thus the form p^(T)x + x^(T)C x, where C = U^(T)HU, p =
 * -2v^(T)HU, has a minimum -v^(T)Hv at the same point. As a set of
 * linear equality restrictions, a subset of the first m equalities
 * from Ux = v may be taken. If U is a square invertible matrix, the
 * minimum is unique.
 *
 * Here we take a Hilbert matrix as H, that is H(i, j) = 1 / (i + j +
 * 1), i, j = 0, 1, ..., n - 1, and an upper triangle matrix of the
 * form u[i][j] = 0 for i > j, u[i][j] = w^(j - i) for i <= j, i, j =
 * 0, 1, ..., n - 1, w = 1.01, as U. We also take z = (1 1 ... 1)^(T).
 */
BOOST_DATA_TEST_CASE(wolfe_complex,
                     bdata::xrange(2) * bdata::xrange(5), xr1, xr2) {
     const Vecint mm{1, 3};
     const Vecint nn{1, 5, 10, 20, 50};
     const Matdouble eps{2,
                         5,
                         {1e-16, 1.5e-9, 1.3, 3.4, 9.8, 1e-16,
                          1.5e-11, 1.1, 2.9, 7.5}};
     const int m = mm(xr1);
     const int n = nn(xr2);
     const double w = 1.01;
     Vecdouble p(n, 0.0);
     Vecdouble C(n * (n + 1) / 2, 0.0);
     Vecdouble b(m, 0.0);
     Vecdouble x(n, 0.0);
     Vecdouble x0(n, 0.0);
     Matdouble A(m, n, 0.0);
     Matdouble CC(n, n, 0.0);  // auxiliary, analogous to C
     double f, f0;
     double s, d;
     int i, j, k;
     Vecdouble& v = x;  // v will be kept in x

     // The first m rows of A and b.
     for (i = 0; i < m; i++) {
          s = 0.0;
          d = 1.0;
          for (j = i; j < n; j++) {
               A(i, j) = d;
               s += d;
               d *= w;
          }
          b(i) = s;
     }

     // Calculate v = Uz = U * (1 1 ... 1).
     for (i = 0; i < n; i++) {
          s = 0.0;
          d = 1.0;
          for (j = i; j < n; j++) {
               s += d;
               d *= w;
          }
          v(i) = s;
     }

     // C <-- HU
     for (i = 0; i < n; i++)
          for (j = 0; j < n; j++) {
               s = 0.0;
               d = 1.0;
               for (k = j; k >= 0; k--) {
                    s += d / (i + k + 1);
                    d *= w;
               }
               CC(i, j) = s;
          }

     // p <-- -2v^(T)C
     for (j = 0; j < n; j++) {
          s = 0.0;
          for (i = 0; i < n; i++)
               s -= v(i) * CC(i, j);
          p(j) = 2.0 * s;
     }

     // Calculate f = -v^(T)Hv.
     f0 = 0.0;
     for (i = 0; i < n; i++) {
          s = 0.0;
          for (j = i + 1; j < n; j++)
               s += v(j) / (i + j + 1);
          s = 2.0 * s + v(i) / (2.0 * i + 1);
          f0 -= s * v(i);
     }

     // C <-- H^(T)C
     for (i = 1; i < n; i++)
          for (j = 0; j < n; j++)
               CC(i, j) += CC(i - 1, j) * w;
     for (i = 0; i < n; i++)
          x0(i) = 1.0;

     k = 0;
     for (i = 0; i < n; i++)
          for (j = i; j < n; j++)
               C(k++) = CC(i, j);

     const int st = wolfe(p, C, A, b, x, f);
     BOOST_REQUIRE(st == 0);
     BOOST_CHECK(faeq(f, f0, 3e-11));
     for (int i = 0; i < n; i++)
          BOOST_CHECK(faeq(x[i], x0[i], eps[xr1][xr2]));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
