/* mathprog.cc: testing mathematical programming */

/**
 * \file testing/mathprog.cc
 * Testing mathematical programming.
 * \date Created on 16 July 2014.
 */

#include <cmath>
#include <iomanip>
#include <iostream>
#include "shg/mathprog.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::abs;

// Anonymous namespace for simplex.
namespace {

// Gass, p. 76.
const int m1 = 3;
const int n1 = 4;
const double A1[] = {1, 2, 3, 0, 2, 1, 5, 0, 1, 2, 1, 1};
const double b1[] = {15, 20, 10};
const double c1[] = {-1, -2, -3, 1};
const Simplex::Equality e1[] = {Simplex::eq, Simplex::eq, Simplex::eq};
const Simplex::Direction d1 = Simplex::min;
const bool nofeas1 = false;
const bool nosol1 = false;
const double x1[] = {2.5, 2.5, 2.5, 0.0};
const double f1 = -15;
const double eps1 = 1e-9;

// Gass, p. 82, e.
const int m2 = 4;
const int n2 = 5;
const double A2[] = {0, 2, -1, -1, 1, -2, 0, 2, -1, 1,
                     1, -2, 0, -1, 1, 1, 1, 1, 0, 0};
const double b2[] = {0, 0, 0, 1};
const double c2[] = {0, 0, 0, 1, -1};
const Simplex::Equality e2[] = {Simplex::ge, Simplex::ge, Simplex::ge,
                                Simplex::eq};
const Simplex::Direction d2 = Simplex::max;
const bool nofeas2 = false;
const bool nosol2 = false;
const double x2[] = {0.4, 0.2, 0.4, 0, 0};
const double f2 = 0;
const double eps2 = 1e-9;

// Gass, p. 82, g.
const int m3 = 2;
const int n3 = 3;
const double A3[] = {-2, 1, 3, 2, 3, 4};
const double b3[] = {2, 1};
const double c3[] = {1, -2, 3};
const Simplex::Equality e3[] = {Simplex::eq, Simplex::eq};
const Simplex::Direction d3 = Simplex::min;
const bool nofeas3 = true;
const bool nosol3 = false;
const double x3[] = {0};
const double f3 = 0;
const double eps3 = 1e-9;

// Gass, p. 83, j.
const int m4 = 2;
const int n4 = 3;
const double A4[] = {1, 1, 1, -1, -1, 1};
const double b4[] = {1, 1, 1};
const double c4[] = {1, 1};
const Simplex::Equality e4[] = {Simplex::ge, Simplex::le, Simplex::le};
const Simplex::Direction d4 = Simplex::max;
const bool nofeas4 = false;
const bool nosol4 = true;
const double x4[] = {0};
const double f4 = 0;
const double eps4 = 1e-9;

void test_simplex(const int m,
                  const int n,
                  const double* A,
                  const double* b,
                  const double* c,
                  const Simplex::Equality* e,
                  const Simplex::Direction d,
                  const bool nofeas,
                  const bool nosol,
                  const double* x,
                  const double f,
                  const double eps) {
     const Matdouble A1(m, n, A);
     const Vecdouble b1(m, b);
     const Vecdouble c1(n, c);
     const Simplex::Vecequality e1(m, e);
     Simplex s(m, n, A1, b1, c1, e1, d, eps);
     if (nofeas) {
          SHG_ASSERT(s.status == 2);
     } else if (nosol) {
          SHG_ASSERT(s.status == 3);
     } else {
          SHG_ASSERT(s.status == 0);
          SHG_ASSERT(abs(f - s.f) < eps);
          for (int j = 0; j < n; j++)
               SHG_ASSERT(abs(x[j] - s.x(j)) < eps);
     }
}

void test_simplex() {
     test_simplex(m1, n1, A1, b1, c1, e1, d1, nofeas1, nosol1,
                  x1, f1, eps1);
     test_simplex(m2, n2, A2, b2, c2, e2, d2, nofeas2, nosol2,
                  x2, f2, eps2);
     test_simplex(m3, n3, A3, b3, c3, e3, d3, nofeas3, nosol3,
                  x3, f3, eps3);
     try {
          test_simplex(m3, n3, A3, b3, c3, e3, d3, nofeas3, nosol3,
                       x3, f3, -1.0);
          SHG_ASSERT(false);
     }
     catch (Simplex::Error) {}
     test_simplex(m4, n4, A4, b4, c4, e4, d4, nofeas4, nosol4,
                  x4, f4, eps4);
}

}       // anonymous namespace

// namespace for wolfe
namespace {

// Output result of comparison.
void result(int st,
            int n,
            const Vecdouble& x,
            const Vecdouble& x0,
            double f,
            double f0) {
     logfile << std::setw(6) << st;
     if (st == 0) {
          SHG_ASSERT(abs(f - f0) < 5e-8);
     } else {
          logfile << '\n';
          return;
     }
     double max = 0.0, d;
     for (int i = 0; i < n; i++)
          if ((d = abs(x[i] - x0[i])) > max)
               max = d;
     logfile << ' ' << std::scientific  << std::setprecision(3)
             << std::setw(6) << max << ' ' << abs(f - f0) << '\n';
}

// Initialize arrays.
void init_arr(int m, int n,
              Vecdouble& p, Vecdouble& C,
              Matdouble& A, Vecdouble& b,
              Vecdouble& x, Vecdouble& x0) {
     p.assign(n, 0.0);
     C.assign(n * (n + 1) / 2, 0.0);
     A.assign(m, n, 0.0);
     b.assign(m, 0.0);
     x.assign(n, 0.0);
     x0.assign(n, 0.0);
}

// Grabowski, p. 247.
void grabowski1() {
     int m = 2, n = 4;
     int st;
     Vecdouble p, C, b, x, x0;
     Matdouble A;
     double f, f0 = -100.0;
     init_arr(m, n, p, C, A, b, x, x0);
     p[0] = -10.0; p[1] = -25.0;
     C[0] = 10.0; C[1] = 2.0; C[4] = 1.0;
     A[0][0] = 1.0; A[0][1] = 2.0; A[0][2] = 1.0;
     A[1][0] = 1.0; A[1][1] = 1.0; A[1][3] = 1.0;
     b[0] = 10.0; b[1] = 9.0;
     x0[1] = 5.0; x0[3] = 4.0;
     st = wolfe(p, C, A, b, x, f);
     result(st, n, x, x0, f, f0);
}

// Grabowski, p. 256.
void grabowski2() {
     int m = 1, n = 3;
     int st;
     Vecdouble p, C, b, x, x0;
     Matdouble A;
     double f, f0 = -5.5;
     init_arr(m, n, p, C, A, b, x, x0);
     p(0) = -6.0;
     C(0) = C(3) = 2.0; C(1) = -1.0;
     A(0, 0) = A(0, 1) = A(0, 2) = 1.0;
     b(0) = 2.0;
     x0(0) = 1.5; x0(1) = 0.5;
     st = wolfe(p, C, A, b, x, f);
     result(st, n, x, x0, f, f0);
}

// Gass, p. 285.  C is not positive definite.
void gass() {
     int m = 2, n = 4;
     int st;
     Vecdouble p, C, b, x, x0;
     Matdouble A;
     double f, f0 = -22.0 / 9.0;
     init_arr(m, n, p, C, A, b, x, x0);
     p(0) = -2.0; p(1) = -1.0;
     C(0) = 1.0;
     A(0, 0) = 2.0; A(0, 1) = 3.0; A(0, 2) = 1.0;
     A(1, 0) = 2.0; A(1, 1) = 1.0; A(1, 3) = 1.0;
     b(0) = 6.0; b(1) = 4.0;
     x0(0) = 2.0 / 3.0; x0(1) = 14.0 / 9.0; x0(3) = 10.0 / 9.0;
     st = wolfe(p, C, A, b, x, f);
     result(st, n, x, x0, f, f0);
}

/*
 * \sum_{i = 1}^n (alpha[i] * x[i] - beta[i])^2, n big, alpha[i],
 * beta[i] >= 0 minimizes to zero at x[i] = beta[i] / alpha[i].
 */
void simple(int m, int n) {
     int st;
     Vecdouble p, C, b, x, x0;
     Matdouble A;
     double f, f0;
     int i, j, k;
     Vecdouble alpha(n), beta(n);
     init_arr(m, n, p, C, A, b, x, x0);

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
     // alpha and beta can be released here
     st = wolfe(p, C, A, b, x, f);
     result(st, n, x, x0, f, f0);
}

/*
 * The test data are generated in the following way. For a positive
 * definite symmetric matrix H and such matrix U, vector v and vector z
 * >= 0 that Uz = v, the form (Ux - v)^(T)H(Ux - v) has a minimum 0 at x
 * = z. Thus the form p^(T)x + x^(T)C x, where C = U^(T)HU, p =
 * -2v^(T)HU, has a minimum -v^(T)Hv at the same point. As a set of
 * linear equality restrictions, a subset of the first m equalities from
 * Ux = v may be taken. If U is a square invertible matrix, the minimum
 * is unique.
 *
 * Here we take a Hilbert matrix as H, that is H(i, j) = 1 / (i + j +
 * 1), i, j = 0, 1, ..., n - 1, and an upper triangle matrix of the form
 * u[i][j] = 0 for i > j, u[i][j] = w^(j - i) for i <= j, i, j = 0, 1,
 * ..., n - 1, w = 1.01, as U. We also take z = (1 1 ... 1)^(T).
 */
void complex(int m, int n) {
     const double w = 1.01;
     int st;
     Vecdouble p, C, b, x, x0,
          &v = x;               // v will be kept in x
     Matdouble A, CC;           // auxiliary, analogous to C
     double f, f0;
     double s, d;
     int i, j, k;
     init_arr(m, n, p, C, A, b, x, x0);
     CC.assign(n, n, 0.0);

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
                    s +=  d / (i + k + 1);
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

     st = wolfe(p, C, A, b, x, f);
     result(st, n, x, x0, f, f0);
}

const int nn[8] = {1, 5, 10, 20, 50, 100, 200, 500};
const int mm[2] = {1, 3};

void test_wolfe() {
     int i, j;
     logfile << "     Method      Status max(x-x0)   f-f0   \n"
             << "---------------- ------ --------- ---------\n";
     logfile << "Grabowski1:      ";
     grabowski1();
     logfile << "Grabowski2:      ";
     grabowski2();
     logfile << "Gass:            ";
     gass();
     for (i = 0; i < 8; i++)
          for (j = 0; j < 2; j++) {
               logfile << "simple(" << mm[j] << ", "
                       << std::setw(3) << nn[i] << "):  ";
               simple(mm[j], nn[i]);
          }
     for (i = 0; i < 5; i++)
          for (j = 0; j < 2; j++) {
               logfile << "complex(" << mm[j] << ", "
                       << std::setw(3) << nn[i] << "): ";
               complex(mm[j], nn[i]);
          }
}

}  // anonymous namespace

void test_mathprog() {
     header("test_mathprog");
     test_simplex();
     test_wolfe();
     footer("test_mathprog");
}

}       // namespace Testing
}       // namespace SHG
