#include <shg/matrix.h>
#include <iomanip>
#include <shg/utils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(matrix_test)

using SHG::Matint;
using SHG::Matdouble;
using SHG::Vecint;
using SHG::diagonal_matrix;
using SHG::hilbert_matrix;
using SHG::arithmetic_progression;
using SHG::alloc_c_matrix;
using SHG::free_c_matrix;
using SHG::narrow_cast;
using SHG::faeq;

BOOST_AUTO_TEST_CASE(matrix_constructor_test) {
     {
          Matint const a;
          BOOST_CHECK(a.nrows() == 0 && a.ncols() == 0);
     }
     {
          Matint const a(3, 4);
          BOOST_CHECK(a.nrows() == 3 && a.ncols() == 4);
     }
     {
          Matint const a(3, 4, 11);
          BOOST_CHECK(a.nrows() == 3 && a.ncols() == 4);
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a[i][j] == 11);
     }
     {
          int const b[] = {0, 1, 2, 3, 4, 1, 2, 3, 4, 5,
                           2, 3, 4, 5, 6, 3, 4, 5, 6, 7};
          Matint const a(4, 5, b);
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a[i][j] == narrow_cast<int>(i + j));
     }
     {
          std::size_t const m = 4;
          std::size_t const n = 5;
          int** p = alloc_c_matrix<int>(m, n);
          for (std::size_t i = 0; i < m; i++)
               for (std::size_t j = 0; j < n; j++)
                    p[i][j] = i + j;
          Matint a(m, n, p);
          BOOST_CHECK(a.nrows() == m && a.ncols() == n);
          for (std::size_t i = 0; i < a.nrows(); i++)
               for (std::size_t j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a[i][j] == p[i][j]);
          free_c_matrix(p);
     }
     {
          Vecint const v(12, 13);
          Matint const a(3, 4, v);
          BOOST_CHECK(equal(v, Vecint(12, 13)) &&
                      equal(a, Matint(3, 4, 13)));
     }
     {
          Vecint const v(11, 13);
          BOOST_CHECK_THROW(Matint(3, 4, v), std::invalid_argument);
     }
     {
          Vecint v(12, 13);
          Matint a(3, 4, std::move(v));
          BOOST_CHECK(v.size() == 0 && equal(a, Matint(3, 4, 13)));
     }
     {
          Matint const a(2, 3, {0, 1, 2});
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a[i][j] == narrow_cast<int>(j));
     }
     {
          std::initializer_list<int> const il = {0, 1, 2};
          Matint const a(2, 3, il);
          BOOST_CHECK(a.nrows() == 2 && a.ncols() == 3);
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a[i][j] == narrow_cast<int>(j));
     }
     {
          Matint const c(3, 4, {});
          BOOST_CHECK(c.nrows() == 3 && c.ncols() == 4);
     }
     {
          Matint const a;
          Matint const b(a);
          BOOST_CHECK(a == b);
     }
     {
          Matint const a(3, 4, 12);
          Matint const b(a);
          BOOST_CHECK(a == b);
     }
     {
          Matint a;
          Matint const b(std::move(a));
          BOOST_CHECK(a.nrows() == 0 && a.ncols() == 0);
          BOOST_CHECK(b.nrows() == 0 && b.ncols() == 0);
     }
     {
          Matint a(3, 4, 12);
          Matint const b(std::move(a));
          BOOST_CHECK(a.nrows() == 0 && a.ncols() == 0);
          BOOST_CHECK(b == Matint(3, 4, 12));
     }
}

BOOST_AUTO_TEST_CASE(matrix_assignment_test) {
     {
          Matint const a(3, 4, 12);
          Matint b;
          b = a;
          BOOST_CHECK(a == b);
     }
     {
          Matint a(3, 4, 12);
          Matint b;
          b = std::move(a);
          BOOST_CHECK(b == Matint(3, 4, 12));
          BOOST_CHECK(a == Matint());
     }
     {
          Matint a(3, 4, 12);
          a = 13;
          BOOST_CHECK(a == Matint(3, 4, 13));
     }
     {
          Matint a(3, 4, 12);
          a = {1, 2, 3, 4, 5};
          BOOST_CHECK(a == Matint(3, 4,
                                  {1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2,
                                   3, 4, 5, 1}));
     }
     {
          Matint a(3, 4, 12);
          a = {};
          BOOST_CHECK(a == Matint(3, 4, 12));
     }
}

BOOST_AUTO_TEST_CASE(matrix_element_access_test) {
     {
          Matint a(3, 4, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
          Matint b(a);
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a[i][j] == b[i][j]);
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a(i, j) == b(i, j));
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    a[i][j] = b[i][j];
          BOOST_CHECK(equal(a, b));
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    a(i, j) = b(i, j);
          BOOST_CHECK(equal(a, b));
     }
     {
          Matint const a(3, 4,
                         {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
          Matint const b(a);
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a[i][j] == b(i, j));
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a(i, j) == b(i, j));
     }
     {
          Matint a(3, 4, 12);
          Matint const b(a);
          BOOST_CHECK_THROW(a.at(a.nrows(), 0)++, std::out_of_range);
          BOOST_CHECK_THROW(b.at(0, a.ncols()), std::out_of_range);
     }
}

BOOST_AUTO_TEST_CASE(matrix_memeber_functions_test) {
     {
          Matint a(3, 4);
          a.resize(4, 5);
          BOOST_CHECK(a.nrows() == 4 && a.ncols() == 5);
          a.resize(0, 0);
          BOOST_CHECK(a.nrows() == 0 && a.ncols() == 0);
          a.resize(3, 4);
          BOOST_CHECK(a.nrows() == 3 && a.ncols() == 4);
          a.resize(0, 4);
          BOOST_CHECK(a.nrows() == 0 && a.ncols() == 0);
          a.resize(3, 0);
          BOOST_CHECK(a.nrows() == 0 && a.ncols() == 0);
     }
     {
          Matint const a0(3, 4,
                          {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
          Matint const b1(2, 3, {1, 2, 3, 5, 6, 7});
          Matint const b2(2, 2, {1, 2, 5, 6});
          Matint a;
          a = a0;
          a.conservative_resize(2, 3);
          BOOST_CHECK(a == b1);
          a = a0;
          a.conservative_resize(2, 5);
          for (Matint::size_type i = 0; i < b1.nrows(); i++)
               for (Matint::size_type j = 0; j < b1.ncols(); j++)
                    BOOST_CHECK(a(i, j) == b1(i, j));
          a = a0;
          a.conservative_resize(2, 2);
          BOOST_CHECK(a == b2);
          a = a0;
          a.conservative_resize(4, 2);
          for (Matint::size_type i = 0; i < b2.nrows(); i++)
               for (Matint::size_type j = 0; j < b2.ncols(); j++)
                    BOOST_CHECK(a(i, j) == b2(i, j));
     }
     {
          Matint a;
          a.assign(10, 20, 13);
          BOOST_CHECK(a.nrows() == 10 && a.ncols() == 20);
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a(i, j) == 13);
     }
     {
          Matint const a(3, 4,
                         arithmetic_progression(12, 0, 1).c_vec());
          int const* pa = a.c_vec();
          for (Matint::size_type i = 0, k = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a(i, j) == pa[k++]);
          int const* const* qa = a.c_mat();
          for (Matint::size_type i = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a(i, j) == qa[i][j]);
          Matint b(a.nrows(), a.ncols());
          int* pb = b.c_vec();
          for (Matint::size_type i = 0, k = 0; i < b.nrows(); i++)
               for (Matint::size_type j = 0; j < b.ncols(); j++, k++)
                    pb[k] = pa[k];
          BOOST_CHECK(equal(a, b));
          int* const* qb = b.c_mat();
          b = 0;
          for (Matint::size_type i = 0; i < b.nrows(); i++)
               for (Matint::size_type j = 0; j < b.ncols(); j++)
                    qb[i][j] = qa[i][j];
          BOOST_CHECK(equal(a, b));
          Vecint const& va = a.vector();
          for (Matint::size_type i = 0, k = 0; i < a.nrows(); i++)
               for (Matint::size_type j = 0; j < a.ncols(); j++)
                    BOOST_CHECK(a(i, j) == va[k++]);
          Vecint& vb = b.vector();
          vb[0]++;
          BOOST_CHECK(vb[0] == b(0, 0) && b(0, 0) == 1);
          vb[0]--;
          BOOST_CHECK(vb[0] == b(0, 0) && b(0, 0) == 0);
          for (Matint::size_type i = 0, k = 0; i < b.nrows(); i++)
               for (Matint::size_type j = 0; j < b.ncols(); j++)
                    BOOST_CHECK(b(i, j) == vb[k++]);
     }
     {
          Matint const a0(3, 4, {1, 2, 3, 4, 5}), b0(2, 3, {-1, -2}),
               c0;
          Matint a(a0), b(b0), c(c0);
          a.swap(b);
          BOOST_CHECK(equal(a, b0) && equal(b, a0));
          a.swap(b);
          BOOST_CHECK(equal(a, a0) && equal(b, b0));
          a.swap(c);
          BOOST_CHECK(equal(a, c0) && equal(c, a0));
     }
}

BOOST_AUTO_TEST_CASE(matrix_non_member_functions_test) {
     {
          std::initializer_list<int> const il = {1, 4,  9, 6, 10, 7,
                                                 2, 11, 3, 0, 5,  8};
          Matint const a(3, 4, il);
          BOOST_CHECK(!is_zero(a));
          BOOST_CHECK(sum(a) == 66);
          BOOST_CHECK(min(a) == 0);
          BOOST_CHECK(max(a) == 11);
          auto const mm = minmax(a);
          BOOST_CHECK(mm.first == 0 && mm.second == 11);
          auto const minl = minloc(a);
          BOOST_CHECK(minl.first == 2 && minl.second == 1);
          auto const maxl = maxloc(a);
          BOOST_CHECK(maxl.first == 1 && maxl.second == 3);
          auto const minmaxl = minmaxloc(a);
          BOOST_CHECK(minmaxl.first.first == 2 &&
                      minmaxl.first.second == 1 &&
                      minmaxl.second.first == 1 &&
                      minmaxl.second.second == 3);
     }
     {
          Matint a;
          BOOST_CHECK(!is_zero(a));
          for (Matint::size_type m = 1; m <= 3; m++)
               for (Matint::size_type n = 1; n <= 4; n++) {
                    a.assign(m, n, 0);
                    BOOST_CHECK(is_zero(a));
               }
     }
     {
          Matint a(3, 4, 12);
          clear(a);
          BOOST_CHECK(a == Matint());
          clear(a);
          BOOST_CHECK(a == Matint());
     }
     {
          Matint a(3, 4, 12), b(2, 5, 11), c;
          swap(a, b);
          BOOST_CHECK(a == Matint(2, 5, 11));
          BOOST_CHECK(b == Matint(3, 4, 12));
          swap(a, c);
          BOOST_CHECK(a == Matint());
          BOOST_CHECK(c == Matint(2, 5, 11));
     }
     {
          Matint a(3, 4, {1, 2, 3, 4, 5}), b;
          std::stringstream ss;
          ss << a;
          ss >> b;
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(equal(a, b));
     }
     {
          Matint a, b;
          std::stringstream ss;
          ss << a;
          ss >> b;
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(equal(a, b));
     }
     {
          Matint a(3, 4, {1, 2, 3, 4, 5});
          std::stringstream ss;
          ss << "something";
          ss >> a;
          BOOST_CHECK(ss.fail());
          BOOST_CHECK(equal(a, Matint(3, 4, {1, 2, 3, 4, 5})));
     }
     {
          Matint a(3, 4, {1, 2, 3, 4, 5}), b;
          std::stringstream ss;
          print(a, ss);
          BOOST_CHECK(ss.str() ==
                      "{1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2}");
     }
     {
          Matint a;
          std::stringstream ss;
          print(a, ss);
          BOOST_CHECK(ss.str() == "{}");
     }
     {
          std::stringstream ss(bininpout);
          Matint x(2, 5, 13), y;
          write(x, ss);
          BOOST_CHECK(!ss.fail());
          read(y, ss);
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(equal(x, y));
     }
     {
          std::stringstream ss(bininpout);
          Matdouble x(25, 4, arithmetic_progression(100, 0.0, 0.1)),
               y;
          write(x, ss);
          BOOST_CHECK(!ss.fail());
          read(y, ss);
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(equal(x, y));
     }
     {
          std::stringstream ss(bininpout);
          Matdouble x, y;
          write(x, ss);
          BOOST_CHECK(!ss.fail());
          read(y, ss);
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(equal(x, y));
     }
     {
          Matint a(2, 5, 11), b(2, 5, 12);
          BOOST_CHECK(maximum_norm_distance(a, b) == 1);
     }
     {
          for (Matint::size_type n = 0; n < 5; n++) {
               for (int c = -2; c <= 2; c++) {
                    Matint const a = diagonal_matrix(n, c);
                    BOOST_CHECK(a.nrows() == n && a.ncols() == n);
                    for (Matint::size_type i = 0; i < n; i++)
                         for (Matint::size_type j = 0; j < n; j++)
                              if (i == j)
                                   BOOST_CHECK(a(i, j) == c);
                              else
                                   BOOST_CHECK(a(i, j) == 0);
               }
               Matint const a = diagonal_matrix<int>(n);
               BOOST_CHECK(a.nrows() == n && a.ncols() == n);
               for (Matint::size_type i = 0; i < n; i++)
                    for (Matint::size_type j = 0; j < n; j++)
                         if (i == j)
                              BOOST_CHECK(a(i, j) == 1);
                         else
                              BOOST_CHECK(a(i, j) == 0);
          }
     }
     {
          Matint const a(3, 4, arithmetic_progression(12, 0, 1));
          Matint const b = transpose(a);
          BOOST_CHECK(b.nrows() == a.ncols() &&
                      b.ncols() == a.nrows());
          for (Matint::size_type j = 0; j < b.ncols(); j++)
               for (Matint::size_type i = 0; i < b.nrows(); i++)
                    BOOST_CHECK(b(i, j) == a(j, i));
          Matint const a1;
          Matint const b1 = transpose(a1);
          BOOST_CHECK(b1.nrows() == 0 && b1.ncols() == 0);
     }
     {
          Matint a(3, 3, arithmetic_progression(9, 0, 1));
          Matint const b = transpose_in_situ(a);
          BOOST_CHECK(b.nrows() == 3 && b.ncols() == 3);
          for (Matint::size_type j = 0; j < b.ncols(); j++)
               for (Matint::size_type i = 0; i < b.nrows(); i++)
                    BOOST_CHECK(b(i, j) ==
                                static_cast<int>(3 * j + i));
          BOOST_CHECK(equal(a, b));
          Matint a1;
          transpose_in_situ(a1);
          BOOST_CHECK(a1.nrows() == 0 && a1.ncols() == 0);
          Matint a2(3, 4, arithmetic_progression(12, 0, 1));
          BOOST_CHECK_THROW(transpose_in_situ(a2),
                            std::invalid_argument);
     }
     {
          Matint const a(2, 3, {1, 2, 3, 4, 5, 6});
          Matint const b(3, 2, {3, 6, 2, 5, 1, 4});
          Matint const c = multiply(a, b);
          BOOST_CHECK(equal(c, Matint(2, 2, {10, 28, 28, 73})));
     }
     {
          Matint const a, b;
          Matint const c = multiply(a, b);
          BOOST_CHECK(c.nrows() == 0 && c.ncols() == 0);
     }
     {
          Matint const a(3, 4), b(3, 3);
          BOOST_CHECK_THROW(multiply(a, b), std::invalid_argument);
     }
     {
          Matint::size_type const m = 4, n = 3;
          Matint a(m, n), b(n, n);
          Matint::size_type i, j, k;
          k = 0;
          for (j = 0; j < n; j++)
               for (i = 0; i < m; i++)
                    a(i, j) = ++k;
          k = 0;
          for (j = 0; j < n; j++)
               for (i = 0; i < n; i++)
                    b(i, j) = ++k;
          Matint const c = multiply(a, b);
          right_multiply_and_assign(a, b);
          BOOST_CHECK(equal(a, c));
     }
     {
          Matint a(3, 4), b(3, 3);
          BOOST_CHECK_THROW(right_multiply_and_assign(a, b),
                            std::invalid_argument);
     }
     {
          Matint a(3, 3);
          BOOST_CHECK_THROW(right_multiply_and_assign(a, a),
                            std::invalid_argument);
     }
     {
          for (Matint::size_type m = 0; m < 5; m++)
               for (Matint::size_type n = 0; n < 5; n++) {
                    Matint const a(
                         m, n, arithmetic_progression(m * n, 1, 1));
                    BOOST_CHECK(
                         equal(left_multiply_by_transposition(a),
                               multiply(transpose(a), a)));
               }
     }
     {
          Matint const a(2, 3, {1, 2, 3, 4, 5, 6});
          Vecint const v{10, 11, 12};
          Vecint const w = multiply(a, v);
          BOOST_CHECK(equal(w, {68, 167}));
     }
     {
          Matint const a(2, 3);
          Vecint const v(4);
          BOOST_CHECK_THROW(multiply(a, v), std::invalid_argument);
     }
     {
          Matint const a;
          Vecint const v;
          Vecint const w = multiply(a, v);
          BOOST_CHECK(w.size() == 0);
     }
     {
          Matint const a(3, 4);
          Vecint const v(4);
          BOOST_CHECK_THROW(multiply_transposed(a, v),
                            std::invalid_argument);
     }
}

BOOST_DATA_TEST_CASE(matrix_hilbert_test, bdata::xrange(10), xr) {
     Matdouble::size_type const n = xr;
     Matdouble a(n, n);
     for (Matdouble::size_type i = 0; i < n; i++)
          for (Matdouble::size_type j = 0; j < n; j++)
               a(i, j) = 1.0 / (1.0 + i + j);
     BOOST_CHECK(maximum_norm_distance(a, hilbert_matrix<double>(n)) <
                 1e-15);
}

BOOST_DATA_TEST_CASE(matrix_cholesky_test, bdata::xrange(7), xr) {
     Matdouble::size_type const n = xr;
     double const eps = n > 5 ? 5e-10 : 1e-10;

     // Generate n x n Hilbert matrix, invert it, multiple the
     // inversion by the original matrix and check departure from
     // identity matrix.

     Matdouble a(n, n, 9e99);
     // Fill the upper-right triangle with Hilbert matrix.
     for (Matdouble::size_type i = 0; i < n; i++)
          for (Matdouble::size_type j = i; j < n; j++)
               a[i][j] = 1.0 / (i + j + 1);
     Matdouble b = a;
     // Fill the lower-left triangle of a.
     for (Matdouble::size_type i = 1; i < n; i++)
          for (Matdouble::size_type j = 0; j < i; j++)
               a[i][j] = a[j][i];
     BOOST_CHECK_NO_THROW(cholesky(b));
     for (Matdouble::size_type i = 0; i < n; i++)
          for (Matdouble::size_type j = 0; j < n; j++) {
               double s = i == j ? -1.0 : 0;
               for (Matdouble::size_type k = 0; k < n; k++)
                    s += a[i][k] * b[k][j];
               BOOST_CHECK(faeq(s, 0.0, eps));
          }
}

BOOST_DATA_TEST_CASE(matrix_multiply_transposed_test,
                     bdata::xrange(5) * bdata::xrange(5), xr1, xr2) {
     std::size_t const m = xr1;
     std::size_t const n = xr2;
     Matint const a(m, n, arithmetic_progression(m * n, 1, 1));
     Vecint const v = arithmetic_progression(n == 0 ? 0 : m, 1, 1);
     Vecint const w = multiply_transposed(a, v);
     Vecint const w1 = multiply(transpose(a), v);
     BOOST_CHECK(equal(w, w1));
}

BOOST_AUTO_TEST_CASE(constructor_from_c_matrix_example) {
     int const m = 4, n = 5;
     int** p = new int*[m];
     for (int i = 0; i < m; i++)
          p[i] = new int[n];
     for (int i = 0; i < m; i++)
          for (int j = 0; j < n; j++)
               p[i][j] = i + j;
     Matint a(m, n, p);
     for (int i = m - 1; i >= 0; i--)
          delete[] p[i];
     delete[] p;
     std::stringstream ss;
     ss << a;
     std::string s;
     std::getline(ss, s);
     BOOST_CHECK(s == "4 5");
     std::getline(ss, s);
     BOOST_CHECK(s == "0 1 2 3 4");
     std::getline(ss, s);
     BOOST_CHECK(s == "1 2 3 4 5");
     std::getline(ss, s);
     BOOST_CHECK(s == "2 3 4 5 6");
     std::getline(ss, s);
     BOOST_CHECK(s == "3 4 5 6 7");
     BOOST_CHECK(!getline(ss, s));
}

BOOST_AUTO_TEST_CASE(output_operator_example) {
     std::stringstream ss;
     ss << std::setw(4) << Matint(2, 3, {1, 11, 111, 1111, 111, 11});
     std::string s;
     std::getline(ss, s);
     BOOST_CHECK(s == "2 3");
     std::getline(ss, s);
     BOOST_CHECK(s == "   1   11  111");
     std::getline(ss, s);
     BOOST_CHECK(s == "1111  111   11");
     BOOST_CHECK(!getline(ss, s));
}

BOOST_AUTO_TEST_CASE(output_as_initializer_list_example) {
     std::stringstream ss;
     print(Matint(2, 3, {1, 11, 111, 1111, 111, 11}), ss);
     BOOST_CHECK(ss.str() == "{1, 11, 111, 1111, 111, 11}");
}

BOOST_AUTO_TEST_CASE(multiply_transposed_example) {
     Matint a(2, 3, {1, 2, 3, 4, 5, 6});
     Vecint v{7, 8};
     std::stringstream ss;
     ss << multiply_transposed(a, v);
     std::string s;
     std::getline(ss, s);
     BOOST_CHECK(s == "3");
     std::getline(ss, s);
     BOOST_CHECK(s == "39");
     std::getline(ss, s);
     BOOST_CHECK(s == "54");
     std::getline(ss, s);
     BOOST_CHECK(s == "69");
     BOOST_CHECK(!getline(ss, s));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
