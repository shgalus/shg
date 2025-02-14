#include <shg/mathutils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(mathutils_test)

int fib(int n) {
     if (n < 0)
          throw std::invalid_argument("n < 0 in fib()");
     if (n == 0)
          return 0;
     if (n == 1)
          return 1;
     int fn2 = 0;
     int fn1 = 1;
     int fn;
     for (int i = 2; i <= n; i++) {
          fn = fn1 + fn2;
          fn2 = fn1;
          fn1 = fn;
     }
     return fn;
}

BOOST_AUTO_TEST_CASE(fibonacci_test) {
     using SHG::fibonacci;
     BOOST_CHECK_THROW(fibonacci<int>(-1), std::invalid_argument);
     for (int n = 0; n < 20; n++) {
          BOOST_CHECK(fibonacci<int>(n) == fib(n));
          BOOST_CHECK(fibonacci<boost::multiprecision::cpp_int>(n) ==
                      fib(n));
     }
}

BOOST_AUTO_TEST_CASE(is_fibonacci_test) {
     using boost::multiprecision::cpp_int;
     using SHG::fibonacci;
     using SHG::is_fibonacci;
     BOOST_CHECK(!is_fibonacci(-1));
     for (int n = 0; n < 20; n++)
          BOOST_CHECK(is_fibonacci(fibonacci<int>(n)));
     int k = 0;
     cpp_int f = fibonacci<cpp_int>(k);
     for (int i = 0; i < 1000; i++) {
          cpp_int n = i;
          BOOST_REQUIRE(n <= f);
          if (n == f) {
               BOOST_CHECK(is_fibonacci(n));
               if (k == 1)
                    k = 2;
               f = fibonacci<cpp_int>(++k);
          } else {
               BOOST_CHECK(!is_fibonacci(n));
          }
     }
}

// https://oeis.org/A005117
int const sqfree[] = {
     1,  2,   3,   5,   6,   7,   10,  11,  13,  14,  15, 17,
     19, 21,  22,  23,  26,  29,  30,  31,  33,  34,  35, 37,
     38, 39,  41,  42,  43,  46,  47,  51,  53,  55,  57, 58,
     59, 61,  62,  65,  66,  67,  69,  70,  71,  73,  74, 77,
     78, 79,  82,  83,  85,  86,  87,  89,  91,  93,  94, 95,
     97, 101, 102, 103, 105, 106, 107, 109, 110, 111, 113};

BOOST_AUTO_TEST_CASE(is_square_free_test) {
     using SHG::is_square_free;
     BOOST_CHECK(!is_square_free(0));
     BOOST_CHECK(!is_square_free(4));
     for (std::size_t i = 0; i < std::size(sqfree); i++) {
          BOOST_CHECK(is_square_free(sqfree[i]));
          BOOST_CHECK(is_square_free(-sqfree[i]));
     }
}

BOOST_AUTO_TEST_SUITE(lin_dioph2_test)

using SHG::Lin_Dioph2;

BOOST_AUTO_TEST_CASE(throws_test) {
     BOOST_CHECK_THROW(Lin_Dioph2(0, 0, 0), std::invalid_argument);
     BOOST_CHECK_THROW(Lin_Dioph2(0, 0, 1), std::invalid_argument);
     BOOST_CHECK_THROW(Lin_Dioph2(0, 0, -1), std::invalid_argument);
}

// clang-format off
struct Test_case_no_solution {
     int a1, a2, b;
} const tcns[] = {
     { 8,  2,  9},
     { 8,  2, -9},
     { 8, -2,  9},
     { 8, -2, -9},
     {-8,  2,  9},
     {-8,  2, -9},
     {-8, -2,  9},
     {-8, -2, -9},
     { 8,  0,  9},
     { 8,  0, -9},
     {-8,  0,  9},
     {-8,  0, -9},
};
// clang-format on

BOOST_DATA_TEST_CASE(no_solution_int_test,
                     bdata::xrange(std::size(tcns)), xr) {
     using enum SHG::Status;
     BOOST_CHECK(
          Lin_Dioph2(tcns[xr].a1, tcns[xr].a2, tcns[xr].b).status ==
          no_solution);
}

BOOST_DATA_TEST_CASE(no_solution_cpp_int_test,
                     bdata::xrange(std::size(tcns)), xr) {
     using enum SHG::Status;

     boost::multiprecision::cpp_int const a1 = tcns[xr].a1;
     boost::multiprecision::cpp_int const a2 = tcns[xr].a2;
     boost::multiprecision::cpp_int const b = tcns[xr].b;
     BOOST_CHECK(
          Lin_Dioph2(tcns[xr].a1, tcns[xr].a2, tcns[xr].b).status ==
          no_solution);
}

// clang-format off
struct Test_case_success {
     int a1, a2, b, d, x1, x2, c1, c2;
} const tcs[] = {
     { 8,  2,  10, 2, 0,  5,  1, -4},
     { 8, -2,  10, 2, 0, -5, -1, -4},
     {-8,  2,  10, 2, 0,  5,  1,  4},
     {-8, -2,  10, 2, 0, -5, -1,  4},
     {-8, -2, -10, 2, 0,  5, -1,  4},
     {-8,  2, -10, 2, 0, -5,  1,  4},
     { 8, -2, -10, 2, 0,  5, -1, -4},
     { 8,  2, -10, 2, 0, -5,  1, -4},
     { 0,  2,  10, 2, 0,  5,  1,  0},
     { 0, -2,  10, 2, 0, -5, -1,  0},
     { 0,  2, -10, 2, 0, -5,  1,  0},
     { 0, -2, -10, 2, 0,  5, -1,  0},
};
// clang-format on

void check(Test_case_success const& tc) {
     for (int t = -10; t <= 10; t++) {
          int const x1 = tc.x1 + tc.c1 * t;
          int const x2 = tc.x2 + tc.c2 * t;
          BOOST_REQUIRE(tc.a1 * x1 + tc.a2 * x2 == tc.b);
     }
}

BOOST_DATA_TEST_CASE(success_int_test, bdata::xrange(std::size(tcs)),
                     xr) {
     check(tcs[xr]);
     Lin_Dioph2 const ld(tcs[xr].a1, tcs[xr].a2, tcs[xr].b);
     BOOST_CHECK(ld.status == SHG::Status::success);
     BOOST_CHECK(tcs[xr].d == ld.d);
     BOOST_CHECK(tcs[xr].x1 == ld.x1);
     BOOST_CHECK(tcs[xr].x2 == ld.x2);
     BOOST_CHECK(tcs[xr].c1 == ld.c1);
     BOOST_CHECK(tcs[xr].c2 == ld.c2);
}

BOOST_DATA_TEST_CASE(success_cpp_int_test,
                     bdata::xrange(std::size(tcs)), xr) {
     check(tcs[xr]);
     boost::multiprecision::cpp_int const a1 = tcs[xr].a1;
     boost::multiprecision::cpp_int const a2 = tcs[xr].a2;
     boost::multiprecision::cpp_int const b = tcs[xr].b;
     Lin_Dioph2 const ld(a1, a2, b);
     BOOST_CHECK(ld.status == SHG::Status::success);
     BOOST_CHECK(tcs[xr].d == ld.d);
     BOOST_CHECK(tcs[xr].x1 == ld.x1);
     BOOST_CHECK(tcs[xr].x2 == ld.x2);
     BOOST_CHECK(tcs[xr].c1 == ld.c1);
     BOOST_CHECK(tcs[xr].c2 == ld.c2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(lindioph_test)

BOOST_AUTO_TEST_CASE(invalid_argument_test) {
     SHG::Lin_Dioph<int> ld;
     SHG::Matrix<int> A;
     SHG::Vector<int> b;
     BOOST_CHECK_THROW(ld.solve(A, b), std::invalid_argument);
     A.resize(2, 3);
     BOOST_CHECK_THROW(ld.solve(A, b), std::invalid_argument);
     A.resize(0, 0);
     b.resize(3);
     BOOST_CHECK_THROW(ld.solve(A, b), std::invalid_argument);
     A.resize(2, 3);
     b.resize(3);
     BOOST_CHECK_THROW(ld.solve(A, b), std::invalid_argument);
}

struct Test_case {
     SHG::Matrix<int> const A;
     SHG::Vector<int> const b;
     bool const result;
     SHG::Vector<int> const x0;
     SHG::Matrix<int> const C;
};

Test_case const tc[] = {
     {
          // \cite knuth-2002b, section 4.5.2, pages 367-369.
          {2, 4, {10, 3, 3, 8, 6, -7, 0, -5}},
          {1, 2},
          true,
          {-1, -4, -3, 4},
          {4, 2, {4, -1, 7, 2, -7, 12, -5, -4}},
     },
     {
          // \cite narkiewicz-1990, page 35.
          {1, 3, {2, 3, 5}},
          {11},
          true,
          {-1, 1, 2},
          {3, 2, {-1, 2, -1, -3, 1, 1}},
     },
     {
          // \cite narkiewicz-1990, page 35.
          {2, 3, {1, -2, 3, 2, 1, -2}},
          {6, 9},
          true,
          {5, 1, 1},
          {3, 1, {-1, -8, -5}},
     },
     {
          // \cite gilbert-pathria-1990, pages 5-6.
          {2, 3, {5, 6, 8, 6, -11, 7}},
          {1, 9},
          true,
          {-5, -1, 4},
          {3, 1, {10, 1, -7}},
     },
     {
          // \cite bronsztejn-siemiendiajew-musiol-muhlig-2004, page
          // 354.
          {1, 3, {2, 4, 3}},
          {3},
          true,
          {0, 0, 1},
          {3, 2, {1, -3, 1, 0, -2, 2}},
     },
     {
          // linear dependence
          {2, 2, {8, 2, 4, 1}},
          {10, 5},
          true,
          {1, 1},
          {2, 1, {-1, 4}},
     },
     {
          {2, 2, {8, 2, 4, 1}},
          {9, 5},
          false,
          {},
          {},
     },
     {
          // https://en.wikipedia.org/wiki/Wilson_matrix, 17 XII 2024.
          {4, 4, {5, 7, 6, 5, 7, 10, 8, 7, 6, 8, 10, 9, 5, 7, 9, 10}},
          {23, 32, 33, 31},
          true,
          {1, 1, 1, 1},
          {},
     },
     {
          // 3rd = 1st * (-1) + 2nd * 2
          {3, 3, {2, 3, 5, 2, 4, 7, 2, 5, 9}},
          {5, 2, -1},
          true,
          {5, 5, -4},
          {3, 1, {1, -4, 2}},
     },
     {
          // \cite lazebnik-1996, page 3.
          {2, 3, {2, 1, 4, -5, 2, 6}},
          {17, -13},
          true,
          {5, 3, 1},
          {3, 1, {2, 32, -9}},
     },
     {
          // m > n
          {3, 2, {2, 3, 4, 5, 6, 7}},
          {8, 9, 10},
          false,
          {},
          {},
     },
     {
          // \cite kameswari-belay-2021, page 54.
          {1, 5, {2, -3, 48, -5, 1}},
          {-36},
          true,
          {0, 1, -1, -3, 0},
          {5, 4, {1, -1, -2, 4,  1, 1,  -3, 0, 0, 0,
                  0, -1, 0,  -1, 1, -8, 1,  0, 0, 0}},
     },
     {
          // \cite kameswari-belay-2021, page 54.
          {3, 5, {3, 4, 0, 22, -8, 6, 0, 0, 46, -12, 0, 4, 3, -1, 9}},
          {25, 2, 26},
          true,
          {-65, 7, 8, 8, -2},
          {5, 2, {52, 92, 0, -3, -11, 0, -6, -12, 3, 0}},
     },
     {
          {2, 2, {5, 5, 0, 1}},
          {0, 3},
          true,
          {-3, 3},
          {},
     },
     {
          {2, 2, {3, -5, 3, -5}},
          {4, 4},
          true,
          {3, 1},
          {2, 1, {5, 3}},
     },
     {
          {1, 1, {3}},
          {6},
          true,
          {2},
          {},
     },
     {
          {1, 1, {3}},
          {7},
          false,
          {},
          {},
     },
     {
          {1, 2, {0, 0}},
          {0},
          true,
          {0, 0},
          {2, 2, {1, 0, 0, 1}},
     },
};

template <typename T>
bool check(SHG::Matrix<T> const& A, SHG::Vector<T> const& b,
           SHG::Vector<T> const& x0, SHG::Matrix<T> const& C,
           bool has_solution) {
     using Ind = std::size_t;
     int const n = 5;
     if (has_solution) {
          if (C.nrows() == 0) {
               for (Ind i = 0; i < A.nrows(); i++) {
                    T s = 0;
                    for (Ind j = 0; j < A.ncols(); j++)
                         s += A(i, j) * x0(j);
                    if (s != b(i))
                         return false;
               }
               return true;
          } else {
               if (!is_zero(SHG::multiply(A, C)))
                    return false;
          }
     } else {
          int const m = n / 2;
          SHG::Varlex v(n, A.ncols());
          do {
               bool all_equal = true;
               for (Ind i = 0; i < A.nrows(); i++) {
                    T s = 0;
                    for (Ind j = 0; j < A.ncols(); j++)
                         s += A(i, j) * (v.get()[j] - m);
                    if (s != b(i)) {
                         all_equal = false;
                         break;
                    }
               }
               if (all_equal)
                    return false;
          } while (v.next());
     }
     return true;
}

// Permutes rows and columns and then tests.
template <typename T>
bool perm_test(SHG::Matrix<T> const& A, SHG::Vector<T> const& b) {
     using Ind = std::size_t;
     SHG::Lin_Dioph<T> ld;
     Ind const m = A.nrows();
     Ind const n = A.ncols();
     SHG::Vector<Ind> prow(m);
     std::iota(prow.begin(), prow.end(), 0);
     // Enumerate all permutations of rows.
     do {
          SHG::Matrix<T> A1(m, n);
          SHG::Vector<T> b1(m);
          for (Ind i = 0; i < m; i++) {
               auto const t = prow(i);
               for (Ind j = 0; j < n; j++)
                    A1(t, j) = A(i, j);
               b1(t) = b(i);
          }
          SHG::Vector<Ind> pcol(n);
          std::iota(pcol.begin(), pcol.end(), 0);
          // Enumerate all permutations of columns.
          do {
               SHG::Matrix<T> A2(m, n);
               SHG::Vector<T> b2{b1};
               for (Ind j = 0; j < n; j++) {
                    auto const t = pcol(j);
                    for (Ind i = 0; i < m; i++)
                         A2(i, t) = A1(i, j);
               }
               bool const has_solution = ld.solve(A2, b2);
               bool const result =
                    check(A2, b2, ld.x0, ld.C, has_solution);
               if (!result)
                    return false;
          } while (std::next_permutation(pcol.begin(), pcol.end()));
     } while (std::next_permutation(prow.begin(), prow.end()));
     return true;
}

BOOST_DATA_TEST_CASE(lin_dioph_test, bdata::xrange(std::size(tc)),
                     xr) {
     SHG::Lin_Dioph<int> ld;
     BOOST_REQUIRE(ld.solve(tc[xr].A, tc[xr].b) == tc[xr].result);
     BOOST_CHECK(ld.x0 == tc[xr].x0);
     BOOST_CHECK(ld.C == tc[xr].C);
     BOOST_CHECK(
          check(tc[xr].A, tc[xr].b, ld.x0, ld.C, tc[xr].result));
     BOOST_CHECK(perm_test(tc[xr].A, tc[xr].b));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(basic_test) {
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
