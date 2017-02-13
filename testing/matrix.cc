/* matrix.cc: testing matrix class and auxiliary functions */

/**
 * \file testing/matrix.cc
 * Testing matrix class and auxiliary functions.
 * \date Created on 9 November 2013.
 */

#include <sstream>
#include "shg/except.h"
#include "shg/utils.h"
#include "shg/matrix.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

using std::abs;
using std::initializer_list;
using std::invalid_argument;
using std::ios_base;
using std::move;
using std::out_of_range;
using std::pair;
using std::range_error;
using std::stringstream;
using SHG::Matdouble;
using SHG::Matint;
using SHG::Vecint;
using SHG::alloc_c_matrix;
using SHG::arithmetic_progression;
using SHG::diagonal_matrix;
using SHG::free_c_matrix;
using SHG::hilbert_matrix;

void test_matrix_members() {
     {
          // constructors
          const Matint a1;
          SHG_ASSERT(a1.nrows() == 0 && a1.ncols() == 0);
          const Matint a2(3, 4);
          SHG_ASSERT(a2.nrows() == 3 && a2.ncols() == 4);
          const Matint a3(3, 4, 11);
          SHG_ASSERT(a3.nrows() == 3 && a3.ncols() == 4);
          for (size_t i = 0; i < a3.nrows(); i++)
               for (size_t j = 0; j < a3.ncols(); j++)
                    SHG_ASSERT(a3[i][j] == 11);
          {
               const size_t m = 4;
               const size_t n = 5;
               int** p = alloc_c_matrix<int>(m, n);
               for (size_t i = 0; i < m; i++)
                    for (size_t j = 0; j < n; j++)
                         p[i][j] = i + j;
               Matint a(m, n, p);
               SHG_ASSERT(a.nrows() == m && a.ncols() == n);
               for (size_t i = 0; i < a.nrows(); i++)
                    for (size_t j = 0; j < a.ncols(); j++)
                         SHG_ASSERT(a[i][j] == p[i][j]);
               // Test also Matrix(size_t m, size_t n, const T* a),
               // because *p is a memory block.
               Matint b(m, n, *p);
               free_c_matrix(p);
               SHG_ASSERT(equal(a, b));
               SHG_ASSERT(a == b);
          }
          {
               // copy and move constructors from Vector
               Vecint v(12, 13);
               Matint a(3, 4, v);
               SHG_ASSERT(equal(v, Vecint(12, 13)) &&
                          equal(a, Matint(3, 4, 13)));
               Matint b(3, 4, move(v));
               SHG_ASSERT(v.size() == 0 && equal(b, Matint(3, 4, 13)));
               Matint c(3, 4, Vecint(12, 13));
               SHG_ASSERT(equal(c, Matint(3, 4, 13)));
               Matint d(0, 0, Vecint());
               SHG_ASSERT(equal(d, Matint()));
               try {
                    Matint e(3, 4, Vecint(11, 12));
                    SHG_ASSERT(false);
               } catch (invalid_argument) {}
          }

          {
               const initializer_list<int> il = {0, 1, 2};
               const Matint a(2, 3, il);
               SHG_ASSERT(a.nrows() == 2 && a.ncols() == 3);
               for (size_t i = 0; i < a.nrows(); i++)
                    for (size_t j = 0; j < a.ncols(); j++)
                         SHG_ASSERT(a[i][j] == static_cast<int>(j));
               const Matint b(2, 3, {0, 1, 2});
               SHG_ASSERT(equal(b, a));
               SHG_ASSERT(b == a);
               const Matint c(3, 4, {});
               SHG_ASSERT(c.nrows() == 3 && c.ncols() == 4);
          }
          {
               const Matint a, b(3, 4, 12);
               const Matint a1(a);
               const Matint b1(b);
               SHG_ASSERT(equal(a, a1));
               SHG_ASSERT(equal(b, b1));
          }
          {
               const Matint a(10, 20, 13);
               Matint b(a);
               const Matint c(move(b));
               SHG_ASSERT(b.nrows() == 0 && b.ncols() == 0);
               SHG_ASSERT(equal(c, a));
          }
     }
     {
          // assignment
          const Matdouble a1(hilbert_matrix<double>(3)),
               a2(hilbert_matrix<double>(4));
          Matdouble b1, b2;
          b1 = a1;
          b2 = a2;
          SHG_ASSERT(equal(b1, a1) && equal(b2, a2));
          b1 = move(b2);
          SHG_ASSERT(equal(b1, a2) && equal(b2, a1));
          const Matint c(3, 4, 13);
          Matint d = c;
          d = {};
          SHG_ASSERT(equal(d, c));
          d = {1, 2, 3, 4, 5};
          SHG_ASSERT(d.nrows() == 3 && d.ncols() == 4);
          int k = 1;
          for (size_t i = 0; i < d.nrows(); i++)
               for (size_t j = 0; j < d.ncols(); j++) {
                    SHG_ASSERT(d(i, j) == k);
                    if (++k > 5)
                         k = 1;
               }
     }
     {
          // element access
          Matint a(3, 4, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
          Matint b(a);
          for (size_t i = 0; i < a.nrows(); i++)
               for (size_t j = 0; j < a.ncols(); j++)
                    SHG_ASSERT(a[i][j] == b[i][j]);
          for (size_t i = 0; i < a.nrows(); i++)
               for (size_t j = 0; j < a.ncols(); j++)
                    SHG_ASSERT(a(i, j) == b(i, j));
          for (size_t i = 0; i < a.nrows(); i++)
               for (size_t j = 0; j < a.ncols(); j++)
                    a[i][j] = b[i][j];
          SHG_ASSERT(equal(a, b));
          for (size_t i = 0; i < a.nrows(); i++)
               for (size_t j = 0; j < a.ncols(); j++)
                    a(i, j) = b(i, j);
          SHG_ASSERT(equal(a, b));
          const Matint a1(a), b1(b);
          for (size_t i = 0; i < a.nrows(); i++)
               for (size_t j = 0; j < a.ncols(); j++)
                    SHG_ASSERT(a[i][j] == b(i, j));
          for (size_t i = 0; i < a.nrows(); i++)
               for (size_t j = 0; j < a.ncols(); j++)
                    SHG_ASSERT(a(i, j) == b(i, j));
          try {
               a.at(a.nrows(), 0)++;
               SHG_ASSERT(false);
          } catch (out_of_range) {}
          try {
               const int i = a.at(0, a.ncols());
               SHG_ASSERT(i == a.at(0, a.ncols() - 1));
               SHG_ASSERT(false);
          } catch (out_of_range) {}
     }
     {
          // resize, assign
          Matint a(3, 4);
          a.resize(4, 5);
          SHG_ASSERT(a.nrows() == 4 && a.ncols() == 5);
          a.resize(0, 0);
          SHG_ASSERT(a.nrows() == 0 && a.ncols() == 0);
          a.resize(3, 4);
          SHG_ASSERT(a.nrows() == 3 && a.ncols() == 4);
          a.resize(0, 4);
          SHG_ASSERT(a.nrows() == 0 && a.ncols() == 0);
          a.resize(3, 0);
          SHG_ASSERT(a.nrows() == 0 && a.ncols() == 0);
          a.assign(10, 20, 13);
          SHG_ASSERT(a.nrows() == 10 && a.ncols() == 20);
          for (size_t i = 0; i < a.nrows(); i++)
               for (size_t j = 0; j < a.ncols(); j++)
                    SHG_ASSERT(a(i, j) == 13);
     }
     {
          // c_vec, c_mat, vector
          const Matint
               a(3, 4, arithmetic_progression(12, 0, 1).c_vec());
          const int* pa = a.c_vec();
          for (size_t i = 0, k = 0; i < a.nrows(); i++)
               for (size_t j = 0; j < a.ncols(); j++)
                    SHG_ASSERT(a(i, j) == pa[k++]);
          int const* const* qa = a.c_mat();
          for (size_t i = 0; i < a.nrows(); i++)
               for (size_t j = 0; j < a.ncols(); j++)
                    SHG_ASSERT(a(i, j) == qa[i][j]);
          Matint b(a.nrows(), a.ncols());
          int* pb = b.c_vec();
          for (size_t i = 0, k = 0; i < b.nrows(); i++)
               for (size_t j = 0; j < b.ncols(); j++, k++)
                    pb[k] = pa[k];
          SHG_ASSERT(equal(a, b));
          int* const* qb = b.c_mat();
          b = 0;
          for (size_t i = 0; i < b.nrows(); i++)
               for (size_t j = 0; j < b.ncols(); j++)
                    qb[i][j] = qa[i][j];
          SHG_ASSERT(equal(a, b));
          const Vecint& va = a.vector();
          for (size_t i = 0, k = 0; i < a.nrows(); i++)
               for (size_t j = 0; j < a.ncols(); j++)
                    SHG_ASSERT(a(i, j) == va[k++]);
          Vecint& vb = b.vector();
          vb[0]++;
          SHG_ASSERT(vb[0] == b(0, 0) && b(0, 0) == 1);
          vb[0]--;
          SHG_ASSERT(vb[0] == b(0, 0) && b(0, 0) == 0);
          for (size_t i = 0, k = 0; i < b.nrows(); i++)
               for (size_t j = 0; j < b.ncols(); j++)
                    SHG_ASSERT(b(i, j) == vb[k++]);
     }
     {
          // swap
          const Matint a0(3, 4, {1, 2, 3, 4, 5}),
               b0(2, 3, {-1, -2}), c0;
          Matint a(a0), b(b0), c(c0);
          a.swap(b);
          SHG_ASSERT(equal(a, b0) && equal(b, a0));
          a.swap(b);
          SHG_ASSERT(equal(a, a0) && equal(b, b0));
          a.swap(c);
          SHG_ASSERT(equal(a, c0) && equal(c, a0));
     }
}

void test_matrix_functions() {
     const initializer_list<int> il = {1, 4, 9, 6,
                                       10, 7, 2, 11,
                                       3, 0, 5, 8};
     const Matint a(3, 4, il);
     SHG_ASSERT(sum(a) == 66);
     SHG_ASSERT(min(a) == 0);
     SHG_ASSERT(max(a) == 11);
     const pair<int, int> mm = minmax(a);
     SHG_ASSERT(mm.first == 0 && mm.second == 11);
     const pair<size_t, size_t> minl = minloc(a);
     SHG_ASSERT(minl.first == 2 && minl.second == 1);
     const pair<size_t, size_t> maxl = maxloc(a);
     SHG_ASSERT(maxl.first == 1 && maxl.second == 3);
     const pair<pair<size_t, size_t>, pair<size_t, size_t>>
          minmaxl = minmaxloc(a);
     SHG_ASSERT(minmaxl.first.first == 2 &&
            minmaxl.first.second == 1 &&
            minmaxl.second.first == 1 &&
            minmaxl.second.second == 3);
     {
          // swap, clear
          const Matint a0(a), b0(5, 6, 13), c0;
          Matint a(a0), b(b0), c(c0);
          swap(a, b);
          SHG_ASSERT(equal(a, b0) && equal(b, a0));
          swap(a, b);
          SHG_ASSERT(equal(a, a0) && equal(b, b0));
          swap(a, c);
          SHG_ASSERT(equal(a, c0) && equal(c, a0));
          clear(a);
          clear(b);
          clear(c);
          SHG_ASSERT(equal(a, c0) && equal(b, c0) && equal(c, c0));
     }
     {
          // stream << and >>, print
          {
               Matint a(3, 4, {1, 2, 3, 4, 5}), b;
               stringstream ss;
               ss << a;
               ss >> b;
               SHG_ASSERT(!ss.fail());
               SHG_ASSERT(equal(a, b));
          }
          {
               Matint a, b;
               stringstream ss;
               ss << a;
               ss >> b;
               SHG_ASSERT(!ss.fail());
               SHG_ASSERT(equal(a, b));
          }
          {
               Matint a(3, 4, {1, 2, 3, 4, 5});
               stringstream ss;
               ss << "something";
               ss >> a;
               SHG_ASSERT(ss.fail());
               SHG_ASSERT(equal(a, Matint(3, 4, {1, 2, 3, 4, 5})));
          }
          {
               Matint a(3, 4, {1, 2, 3, 4, 5}), b;
               stringstream ss;
               print(a, ss);
               SHG_ASSERT(ss.str() ==
                          "{1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2}");
          }
          {
               Matint a;
               stringstream ss;
               print(a, ss);
               SHG_ASSERT(ss.str() == "{}");
          }
     }
     {
          // write, read
          const ios_base::openmode mode = ios_base::out | ios_base::in |
               ios_base::binary;
          {
               stringstream ss(mode);
               Matint x(2, 5, 13), y;
               write(x, ss);
               SHG_ASSERT(!ss.fail());
               read(y, ss);
               SHG_ASSERT(!ss.fail());
               SHG_ASSERT(equal(x, y));
          }
          {
               stringstream ss(mode);
               Matdouble x(25, 4,
                           arithmetic_progression(100, 0.0, 0.1)), y;
               write(x, ss);
               SHG_ASSERT(!ss.fail());
               read(y, ss);
               SHG_ASSERT(!ss.fail());
               SHG_ASSERT(equal(x, y));
          }
          {
               stringstream ss(mode);
               Matdouble x, y;
               write(x, ss);
               SHG_ASSERT(!ss.fail());
               read(y, ss);
               SHG_ASSERT(!ss.fail());
               SHG_ASSERT(equal(x, y));
          }
     }
     {
          // maximum_norm_distance
          Matint a(2, 5, 11), b(2, 5, 12);
          SHG_ASSERT(maximum_norm_distance(a, b) == 1);
     }
     {
          // hilbert_matrix
          for (size_t n = 0; n < 10; n++) {
               Matdouble a(n, n);
               for (size_t i = 0; i < n; i++)
                    for (size_t j = 0; j < n; j++)
                         a(i, j) = 1.0 / (1.0 + i + j);
               SHG_ASSERT(maximum_norm_distance(
                           a, hilbert_matrix<double>(n)) < 1e-15);
          }
     }
     {
          // diagonal_matrix
          for (size_t n = 0; n < 5; n++) {
               for (int c = -2; c <= 2; c++) {
                    const Matint a = diagonal_matrix(n, c);
                    SHG_ASSERT(a.nrows() == n && a.ncols() == n);
                    for (size_t i = 0; i < n; i++)
                         for (size_t j = 0; j < n; j++)
                              if (i == j)
                                   SHG_ASSERT(a(i, j) == c);
                              else
                                   SHG_ASSERT(a(i, j) == 0);
               }
               const Matint a = diagonal_matrix<int>(n);
               SHG_ASSERT(a.nrows() == n && a.ncols() == n);
               for (size_t i = 0; i < n; i++)
                    for (size_t j = 0; j < n; j++)
                         if (i == j)
                              SHG_ASSERT(a(i, j) == 1);
                         else
                              SHG_ASSERT(a(i, j) == 0);
          }
     }
     {
          // transpose()
          const Matint a(3, 4, arithmetic_progression(12, 0, 1));
          const Matint b = transpose(a);
          SHG_ASSERT(b.nrows() == a.ncols() && b.ncols() == a.nrows());
          for (size_t j = 0; j < b.ncols(); j++)
               for (size_t i = 0; i < b.nrows(); i++)
                    SHG_ASSERT(b(i, j) == a(j, i));
          const Matint a1;
          const Matint b1 = transpose(a1);
          SHG_ASSERT(b1.nrows() == 0 && b1.ncols() == 0);
     }
     {
          // transpose_in_situ()
          Matint a(3, 3, arithmetic_progression(9, 0, 1));
          const Matint b = transpose_in_situ(a);
          SHG_ASSERT(b.nrows() == 3 && b.ncols() == 3);
          for (size_t j = 0; j < b.ncols(); j++)
               for (size_t i = 0; i < b.nrows(); i++)
                    SHG_ASSERT(b(i, j) == static_cast<int>(3 * j + i));
          SHG_ASSERT(equal(a, b));
          Matint a1;
          transpose_in_situ(a1);
          SHG_ASSERT(a1.nrows() == 0 && a1.ncols() == 0);
          Matint a2(3, 4, arithmetic_progression(12, 0, 1));
          try {
               transpose_in_situ(a2);
               SHG_ASSERT(false);
          }
          catch (invalid_argument) {}
     }
     {
          // multiply
          {
               const Matint a(2, 3, {1, 2, 3, 4, 5, 6});
               const Matint b(3, 2, {3, 6, 2, 5, 1, 4});
               const Matint c = multiply(a, b);
               SHG_ASSERT(equal(c, Matint(2, 2, {10, 28, 28, 73})));
          }
          {
               const Matint a, b;
               const Matint c = multiply(a, b);
               SHG_ASSERT(c.nrows() == 0 && c.ncols() == 0);
          }
          {
               const Matint a(3, 4), b(3, 3);
               try {
                    const Matint c = multiply(a, b);
                    SHG_ASSERT(false);
               } catch (invalid_argument) {}
          }
     }
     {
          // right_multiply_and_assign
          {
               const size_t m = 4, n = 3;
               Matint a(m, n), b(n, n);
               size_t i, j, k;
               k = 0;
               for (j = 0; j < n; j++)
                    for (i = 0; i < m; i++)
                         a(i, j) = ++k;
               k = 0;
               for (j = 0; j < n; j++)
                    for (i = 0; i < n; i++)
                         b(i, j) = ++k;
               const Matint c = multiply(a, b);
               right_multiply_and_assign(a, b);
               SHG_ASSERT(equal(a, c));
          }
          {
               Matint a(3, 4), b(3, 3);
               try {
                    right_multiply_and_assign(a, b);
                    SHG_ASSERT(false);
               }
               catch (invalid_argument) {}
          }
          {
               try {
                    Matint a(3, 3);
                    right_multiply_and_assign(a, a);
                    SHG_ASSERT(false);
               }
               catch (invalid_argument) {}
          }
     }
     {
          // cholesky
          const double eps = 1e-10;
          // Generate n x n Hilbert matrix, invert it, multiple the
          // inversion by the original matrix and check departure from
          // identity matrix.
          for (size_t n = 0; n < 6; n++) {
               Matdouble a(n, n, 9e99);
               // Fill the upper-right triangle with Hilbert matrix.
               for (size_t i = 0; i < n; i++)
                    for (size_t j = i; j < n; j++)
                         a[i][j] = 1.0 / (i + j + 1);
               Matdouble b = a;
               // Fill the lower-left triangle of a.
               for (size_t i = 1; i < n; i++)
                    for (size_t j = 0; j < i; j++)
                         a[i][j] = a[j][i];
               try {
                    cholesky(b);
               } catch (range_error) {
                    SHG_ASSERT(false);
               }
               for (size_t i = 0; i < n; i++)
                    for (size_t j = 0; j < n; j++) {
                         double s = i == j ? -1.0 : 0;
                         for (size_t k = 0; k < n; k++)
                              s += a[i][k] * b[k][j];
                         SHG_ASSERT(abs(s) < eps);
                    }
          }
     }
     {
          // left_multiply_by_transposition()
          for (size_t m = 0; m < 5; m++)
               for (size_t n = 0; n < 5; n++) {
                    const Matint
                         a(m, n, arithmetic_progression(m * n, 1, 1));
                    SHG_ASSERT(equal(left_multiply_by_transposition(a),
                                 multiply(transpose(a), a)));
               }
     }
     {
          // multiply by vector
          {
               const Matint a(2, 3, {1, 2, 3, 4, 5, 6});
               const Vecint v {10, 11, 12};
               const Vecint w = multiply(a, v);
               SHG_ASSERT(equal(w, {68, 167}));
          }
          {
               const Matint a(2, 3);
               const Vecint v(4);
               try {
                    const Vecint w = multiply(a, v);
                    SHG_ASSERT(false);
               } catch (invalid_argument) {}
          }
          {
               const Matint a;
               const Vecint v;
               const Vecint w = multiply(a, v);
               SHG_ASSERT(w.size() == 0);
          }
     }
     {
          // multiply_transposed
          for (size_t m = 0; m < 5; m++)
               for (size_t n = 0; n < 5; n++) {
                    const Matint
                         a(m, n, arithmetic_progression(
                                m * n, 1, 1));
                    const Vecint v = arithmetic_progression(
                         n == 0 ? 0 : m, 1, 1);
                    const Vecint w = multiply_transposed(a, v);
                    const Vecint w1 = multiply(transpose(a), v);
                    SHG_ASSERT(equal(w, w1));
               }
          {
               const Matint a(3, 4);
               const Vecint v(4);
               try {
                    const Vecint w = multiply_transposed(a, v);
                    SHG_ASSERT(false);
               } catch (invalid_argument) {}
          }
     }
}

}       // anonymous namespace

void test_matrix() {
     test_matrix_members();
     test_matrix_functions();
}

}       // namespace Testing
}       // namespace SHG
