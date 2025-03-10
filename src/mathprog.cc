/**
 * \file src/mathprog.cc
 * Mathematical programming.
 * \date Created on 16 July 2014.
 */

#include <shg/mathprog.h>
#include <cstdlib>
#include <new>

namespace SHG {

using std::size_t;

int revsimplex(Matdouble const& A, Vecdouble const& b,
               Vecdouble const& c, Vecdouble& x, double& f,
               double const eps) {
     size_t const m = A.nrows();
     size_t const n = A.ncols();
     if (m < 1 || n < 1 || eps <= 0.0 || b.size() != m ||
         c.size() != n || x.size() != n)
          return 1;  // invalid input

     size_t const m2 = m + 2, m1 = m + 1;
     // Dummy initialization of k and l for compiler.
     size_t i, j, k = 0, l = 0, p = m2, p1 = m1;
     double d, s;
     Matdouble u(m2, m2, 0.0);
     Vector<size_t> w(m);
     Vecdouble y(m2), a1(n), a2(n), z(m2);
     bool ex, phase = true;

     for (j = 0; j < n; j++) {
          a1(j) = c(j);
          s = 0.0;
          for (i = 0; i < m; i++)
               s -= A(i, j);
          a2(j) = s;
     }
     s = 0.0;
     for (i = 0; i < m; i++) {
          w(i) = n + i;
          if (b(i) < 0.0)
               return 1;  // invalid input
          s -= (z(i) = b(i));
     }
     z(m) = 0.0;
     z(m1) = s;
     for (i = 0; i < m2; i++)
          u(i, i) = 1.0;
     for (;;) {
          if (z(m1) >= -eps && phase) {
               phase = false;
               p = p1--;
          }
          d = 0.0;
          for (j = 0; j < n; j++) {
               s = u(p1, m) * a1(j) + u(p1, m1) * a2(j);
               for (i = 0; i < m; i++)
                    s += u(p1, i) * A(i, j);
               if (d > s) {
                    d = s;
                    k = j;
               }
          }
          if (d > -eps) {
               if (phase)
                    return 2;  // no feasible solution
               f = -z(p1);
               x = 0.0;
               for (i = 0; i < m; i++)
                    // w may contain artificial variables with value 0
                    if (w(i) < n)
                         x(w(i)) = z(i);
               return 0;  // solution found
          }
          for (i = 0; i < p; i++) {
               s = u(i, m) * a1(k) + u(i, m1) * a2(k);
               for (j = 0; j < m; j++)
                    s += u(i, j) * A(j, k);
               y(i) = s;
          }
          ex = true;
          for (i = 0; i < m; i++)
               if (y(i) >= eps) {
                    s = z(i) / y(i);
                    if (ex || s < d) {
                         d = s;
                         l = i;
                    }
                    ex = false;
               }
          if (ex)
               return 3;  // unbounded solution
          w(l) = k;
          s = 1.0 / y(l);
          for (j = 0; j < m; j++)
               u(l, j) *= s;
          for (i = 0; i < p; i++)
               if (i != l) {
                    s = y(i);
                    z(i) -= d * s;
                    for (j = 0; j < m; j++)
                         u(i, j) -= u(l, j) * s;
               }
          z(l) = d;
     }
}

Simplex::Error::Error()
     : Exception("invalid argument in Simplex::Simpex()") {}

Simplex::Simplex(size_t const m, size_t const n, Matdouble const& A,
                 Vecdouble const& b, Vecdouble const& c,
                 Vecequality const& e, Direction const d,
                 double const eps)
     : status(), f(), x() {
     if (m <= 0 || n <= 0 || A.nrows() < m || A.ncols() < n ||
         b.size() < m || c.size() < n || e.size() < m || eps <= 0.0)
          throw Error();

     size_t i, j, k = 0;
     for (i = 0; i < m; i++)
          if (e(i) == le || e(i) == ge)
               k++;
          else if (e(i) != eq)
               throw Error();
     // There will be k slack variables.
     size_t const n1 = n + k;
     k = 0;
     Matdouble A1(m, n1);
     Vecdouble b1(m);
     Vecdouble c1(n1, 0.0);
     Vecdouble x1(n1);
     for (i = 0; i < m; i++)
          for (j = n; j < n1; j++)
               A1(i, j) = 0.0;
     for (i = 0; i < m; i++)
          if (b(i) >= 0.0) {
               for (j = 0; j < n; j++)
                    A1(i, j) = A(i, j);
               b1(i) = b(i);
               if (e(i) != eq)
                    A1(i, n + k++) = e(i) == le ? 1.0 : -1.0;
          } else {
               for (j = 0; j < n; j++)
                    A1(i, j) = -A(i, j);
               b1(i) = -b(i);
               if (e(i) != eq)
                    A1(i, n + k++) = e(i) == ge ? 1.0 : -1.0;
          }
     if (d == min) {
          for (j = 0; j < n; j++)
               c1(j) = c(j);
     } else if (d == max) {
          for (j = 0; j < n; j++)
               c1(j) = -c(j);
     } else {
          throw Error();
     }
     status = revsimplex(A1, b1, c1, x1, f, eps);
     if (status == 0) {
          x.resize(n);
          for (j = 0; j < n; j++)
               x(j) = x1(j);
          if (d == max)
               f = -f;
     }
}

int wolfe(Vecdouble const& p, Vecdouble const& C, Matdouble const& A,
          Vecdouble const& b, Vecdouble& x, double& f) {
     static double const eps = 1e-11;
     static int const maxiter = 10000;
     int const m = A.nrows();
     int const n = A.ncols();
     if (m <= 0 || n <= 0)
          return 1;  // invalid parameter m or n
     {
          size_t const ms = m;
          size_t const ns = n;
          if (p.size() != ns || C.size() != ns * (ns + 1) / 2 ||
              A.nrows() != ms || A.ncols() != ns || b.size() != ms ||
              x.size() != ns)
               return 1;  // invalid dimensions
     }
     int const g = m + n;      // added row in simplex table
     int const h = 2 * n + m;  // added column in simplex table
     int const                 // intervals of variable numbers
                               // x:  0 .. nv - 1
          nv = n,              // v: nv .. nu - 1
          nu = 2 * n,          // u: nu .. nz - 1
          nz = 2 * n + m,      // z: nz .. nw - 1
          nw = 3 * n + m;      // w: nw .. 3 * n + 2 * m - 1
     Matdouble t;              // simplex table
     Vecint B, N;              // basis and non-basis variables
     int q = m;                // last row to leave the basis
     bool initiation = true;   // true = initiation, false = recursion
     int iter = 0;
     int i, j, k, l = 0;  // Dummy initialization for compiler.
     double d, s;

     try {
          t.resize(g + 1, h + 1);
          B.resize(g);
          N.resize(h);
     } catch (std::bad_alloc const&) {
          return 2;  // not enough memory
     }

     // Initialize basis and non-basis variable numbers.
     for (i = 0; i < g; i++)
          B[i] = i < m ? nw + i : nz + (i - m);
     for (i = 0; i < h; i++)
          N[i] = i;

     // Initialize first m rows of the simplex table.
     for (i = 0; i < m; i++) {
          if (b[i] < 0.0) {
               for (j = 0; j < n; j++)
                    t[i][j] = -A[i][j];
               t[i][h] = -b[i];
          } else {
               for (j = 0; j < n; j++)
                    t[i][j] = A[i][j];
               t[i][h] = b[i];
          }
          for (j = n; j < h; j++)
               t[i][j] = 0.0;
     }

     // Initialize next n rows of the simplex table.
     k = 0;
     for (i = m; i < g; i++) {
          t[i][i - m] = 2.0 * C[k++];
          for (j = i - m + 1; j < n; j++)
               t[i][j] = t[m + j][i - m] = 2.0 * C[k++];
          for (j = n; j < 2 * n; j++)
               t[i][j] = 0.0;
          t[i][n + (i - m)] = -1.0;
          for (j = 2 * n; j < h; j++)
               t[i][j] = A[j - 2 * n][i - m];
          t[i][h] = -p[i - m];
          if (t[i][h] < 0.0) {
               for (j = 0; j <= h; j++)
                    t[i][j] = -t[i][j];
          }
     }

     // Initialize the g-th row of the simplex table.
     for (j = 0; j < n; j++) {
          s = 0.0;
          for (i = 0; i < m; i++)
               s += t[i][j];
          t[g][j] = s;
     }
     for (j = n; j < h; j++)
          t[g][j] = 0.0;
     s = 0.0;
     for (i = 0; i < m; i++)
          s += t[i][h];
     t[g][h] = s;

     for (;;) {  // for initiation and recursion
          if (++iter > maxiter)
               return 3;  // too many iteration steps

          // Find variable to enter the basis.
          s = 0.0;
          if (initiation) {
               for (j = 0; j < h; j++)
                    // exclude u, v, z
                    if (N[j] < nv || N[j] >= nw)
                         if (t[g][j] > s)
                              s = t[g][l = j];
          } else {
               for (j = 0; j < h; j++)
                    if (N[j] < nw) {
                         // exclude w
                         if (N[j] >= nv && N[j] < nu) {
                              // side condition for v
                              k = N[j] - n;
                              for (i = 0; i < g && B[i] != k; i++)
                                   ;
                              if (i < g)
                                   continue;
                         } else if (N[j] < nv) {
                              // side condition for x
                              k = N[j] + n;
                              for (i = 0; i < g && B[i] != k; i++)
                                   ;
                              if (i < g)
                                   continue;
                         } else if (N[j] >= nu && N[j] < nz &&
                                    t[g][j] < 0.0) {
                              // adjust u, as we have u1 and u2
                              for (i = 0; i <= g; i++)
                                   t[i][j] = -t[i][j];
                         }
                         if (t[g][j] > s)
                              s = t[g][l = j];
                    }
          }

          // If s < eps, there are three possibilities:
          // - no feasible solution,
          // - the method finishes successfully in recursion phase,
          // - the phase is changed from initiation to recursion.

          if (s < eps) {
               if (t[g][h] > eps)
                    // no feasible solution
                    return initiation ? 4 : 5;
               if (!initiation)
                    break;  // finished successfully

               // Change the phase.
               initiation = false;
               q = g;

               // Discard unused components of z.
               for (i = m; i < g; i++)
                    if (t[i][h] < 0.0) {
                         for (j = 0; j <= h; j++)
                              t[i][j] = -t[i][j];
                    }

               // Calculate last row for the new objective function.
               for (j = 0; j <= h; j++) {
                    s = 0.0;
                    for (i = 0; i < g; i++)
                         if (B[i] >= nz && B[i] < nw)
                              s += t[i][j];
                    if (j < h && N[j] >= nz && N[j] < nw)
                         s -= 1.0;
                    t[g][j] = s;
               }
               continue;
          }

          // Find variable to leave the basis.
          k = -1;
          for (i = 0; i < q; i++)
               // discard w in recursion
               if ((initiation || B[i] < nw) && t[i][l] >= eps) {
                    d = t[i][h] / t[i][l];
                    if (k < 0 || d < s) {
                         k = i;
                         s = d;
                    }
               }

          if (k < 0)
               // unbounded solution
               return initiation ? 6 : 7;

          // Change variables.
          i = B[k];
          B[k] = N[l];
          N[l] = i;

          // Transform the simplex table around t[k][l].
          s = t[k][l];
          t[k][l] = 1.0;
          for (j = 0; j <= h; j++)
               t[k][j] /= s;
          for (i = 0; i <= g; i++)
               if (i != k) {
                    d = t[i][l];
                    t[i][l] = 0.0;
                    for (j = 0; j <= h; j++)
                         t[i][j] -= t[k][j] * d;
               }
     }  // for initiation and recursion

     // Put the solution.
     for (i = 0; i < n; i++)
          x[i] = 0.0;

     for (i = 0; i < g; i++)
          if (B[i] < n)
               x[B[i]] = t[i][h];

     // Calculate minimum value.
     f = 0.0;
     k = 0;
     for (i = 0; i < n; i++) {
          s = 0.0;
          d = C[k++];
          for (j = i + 1; j < n; j++)
               s += C[k++] * x[j];
          f += x[i] * (2.0 * s + d * x[i] + p[i]);
     }
     return 0;
}

}  // namespace SHG
