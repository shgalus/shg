#include <shg/zsqrtd.h>
#include <shg/mathutils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(zsqrtd_test)

BOOST_AUTO_TEST_SUITE(ring_zsqrtd_test)

using SHG::ALGEBRA::Ring_Zsqrtd;
using SHG::ALGEBRA::Element;

BOOST_AUTO_TEST_CASE(constructor_test) {
     Ring_Zsqrtd const R;
     BOOST_CHECK(R.d() == -1);
     BOOST_CHECK(Ring_Zsqrtd().d() == -1);
     BOOST_CHECK(Ring_Zsqrtd(2).d() == 2);
     BOOST_CHECK(Ring_Zsqrtd(3).d() == 3);

     Ring_Zsqrtd const S(2);
     BOOST_CHECK(S.d() == 2);

     BOOST_CHECK_THROW(Ring_Zsqrtd(0), std::invalid_argument);
     BOOST_CHECK_THROW(Ring_Zsqrtd(4), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(is_unit_test) {
     Ring_Zsqrtd const S(2);
     BOOST_CHECK(S.is_unit(S.element(1, 0)));
     BOOST_CHECK(S.is_unit(S.element(-1, 0)));
     BOOST_CHECK(S.is_unit(S.element(1, 1)));
     BOOST_CHECK(S.is_unit(S.element(-1, -1)));
     BOOST_CHECK(S.is_unit(S.element(3, 2)));
     BOOST_CHECK(S.is_unit(S.element(-3, -2)));
     BOOST_CHECK(!S.is_unit(S.element(0, 0)));
     BOOST_CHECK(!S.is_unit(S.element(1, 2)));
     BOOST_CHECK(!S.is_unit(S.element(-1, -2)));
}

BOOST_AUTO_TEST_CASE(is_zerodivisor_test) {
     int const N = 5;
     for (int d = -10; d <= 10; d++)
          if (std::abs(d) != 1 && SHG::is_square_free(d)) {
               Ring_Zsqrtd const R(d);
               for (int iqx = -N; iqx <= N; iqx++)
                    for (int iqy = -N; iqy <= N; iqy++) {
                         Element const x = R.element(iqx, iqy);
                         BOOST_CHECK(R.is_zerodivisor(x) ==
                                     (iqx == 0 && iqy == 0));
                    }
          }
}

BOOST_AUTO_TEST_CASE(euclidean_norm_test) {
     Ring_Zsqrtd const S(2);
     BOOST_CHECK(S.euclidean_norm(S.element(0, 0)) == 0);
     BOOST_CHECK(S.euclidean_norm(S.element(1, 0)) == 1);
     BOOST_CHECK(S.euclidean_norm(S.element(0, 1)) == -2);
     BOOST_CHECK(S.euclidean_norm(S.element(2, 3)) == -14);
}

BOOST_AUTO_TEST_CASE(divides_test) {
     Ring_Zsqrtd const S(2);
     BOOST_CHECK(S.divides(S.element(2, 3), S.element(38, 22)));
     BOOST_CHECK(!S.divides(S.element(4, 1), S.element(2, 1)));
     BOOST_CHECK_THROW(S.divides(S.element(0, 0), S.element(1, 1)),
                       std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(divide_test) {
     int const N = 5;
     Element result;

     for (int d = -10; d <= 10; d++)
          if (std::abs(d) != 1 && SHG::is_square_free(d)) {
               Ring_Zsqrtd const R(d);
               for (int iqx = -N; iqx <= N; iqx++)
                    for (int iqy = -N; iqy <= N; iqy++) {
                         Element const z = R.element(iqx, iqy);
                         for (int idx = -N; idx <= N; idx++)
                              for (int idy = -N; idy <= N; idy++) {
                                   Element const y =
                                        R.element(idx, idy);
                                   Element const x = y * z;
                                   try {
                                        if (R.divide(x, y, result))
                                             BOOST_CHECK(result == z);
                                   } catch (
                                        std::invalid_argument const&) {
                                        BOOST_CHECK(R.is_zero(y));
                                   }
                              }
                    }
          }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ring_zsqrtd_quot_test)

using SHG::ALGEBRA::Ring_Zsqrtd_quot;
using SHG::ALGEBRA::is_commutative_ring_with_unity;
using SHG::ALGEBRA::is_field;

BOOST_AUTO_TEST_CASE(constructor_test) {
     Ring_Zsqrtd_quot const R;
     BOOST_CHECK(R.d() == 2);
     BOOST_CHECK(R.a() == 4);
     BOOST_CHECK(R.b() == 2);
     BOOST_CHECK(R.d2() == 2);
     BOOST_CHECK(R.N() == 8);
     BOOST_CHECK(R.absN() == 8);
     BOOST_CHECK(R.d1() == 4);
     BOOST_CHECK(R.x0() == 4);

     Ring_Zsqrtd_quot const S(2, 2, 2);
     BOOST_CHECK(S.d() == 2);
     BOOST_CHECK(S.a() == 2);
     BOOST_CHECK(S.b() == 2);
     BOOST_CHECK(S.d2() == 2);
     BOOST_CHECK(S.N() == -4);
     BOOST_CHECK(S.absN() == 4);
     BOOST_CHECK(S.d1() == 2);
     BOOST_CHECK(S.x0() == 4);

     BOOST_CHECK_THROW(Ring_Zsqrtd_quot(2, 0, 0),
                       std::invalid_argument);
     BOOST_CHECK_THROW(Ring_Zsqrtd_quot(4, 2, 2),
                       std::invalid_argument);
     BOOST_CHECK_THROW(Ring_Zsqrtd_quot(9, 2, 2),
                       std::invalid_argument);
     BOOST_CHECK_THROW(Ring_Zsqrtd_quot(12, 2, 2),
                       std::invalid_argument);
}

// clang-format off

// addition table for Z[sqrt(2)] / (4 + 2 * sqrt(2))
int const res1_add[8][8] = {
     {0,  1,  2,  3,  4,  5,  6,  7},
     {1,  2,  3,  0,  5,  6,  7,  4},
     {2,  3,  0,  1,  6,  7,  4,  5},
     {3,  0,  1,  2,  7,  4,  5,  6},
     {4,  5,  6,  7,  0,  1,  2,  3},
     {5,  6,  7,  4,  1,  2,  3,  0},
     {6,  7,  4,  5,  2,  3,  0,  1},
     {7,  4,  5,  6,  3,  0,  1,  2},
};

// multiplication table for Z[sqrt(2)] / (4 + 2 * sqrt(2))
int const res1_mul[8][8] = {
     {0,  0,  0,  0,  0,  0,  0,  0},
     {0,  1,  2,  3,  4,  5,  6,  7},
     {0,  2,  0,  2,  0,  2,  0,  2},
     {0,  3,  2,  1,  4,  7,  6,  5},
     {0,  4,  0,  4,  2,  6,  2,  6},
     {0,  5,  2,  7,  6,  3,  4,  1},
     {0,  6,  0,  6,  2,  4,  2,  4},
     {0,  7,  2,  5,  6,  1,  4,  3},
};

// clang-format on

BOOST_AUTO_TEST_CASE(tables1_test) {
     Ring_Zsqrtd_quot const R;
     SHG::Linalg::Matint add;
     SHG::Linalg::Matint mul;
     R.tables(add, mul);
     BOOST_CHECK(is_commutative_ring_with_unity(add, mul) == 0);
     BOOST_CHECK(is_field(add, mul) == 10);
     for (int i = 0; i < add.rows(); i++)
          for (int j = 0; j < add.cols(); j++) {
               BOOST_CHECK(add(i, j) == res1_add[i][j]);
               BOOST_CHECK(mul(i, j) == res1_mul[i][j]);
          }
}

// clang-format off

// addition table for Z[sqrt(2)] / (2 + 2 * sqrt(2))
int const res2_add[4][4] = {
     {0, 1, 2, 3},
     {1, 0, 3, 2},
     {2, 3, 0, 1},
     {3, 2, 1, 0},
};

// multiplication table for Z[sqrt(2)] / (2 + 2 * sqrt(2))
int const res2_mul[4][4] = {
     {0, 0, 0, 0},
     {0, 1, 2, 3},
     {0, 2, 0, 2},
     {0, 3, 2, 1},
};

// clang-format on

BOOST_AUTO_TEST_CASE(tables2_test) {
     Ring_Zsqrtd_quot const R(2, 2, 2);
     SHG::Linalg::Matint add;
     SHG::Linalg::Matint mul;
     R.tables(add, mul);
     BOOST_CHECK(is_commutative_ring_with_unity(add, mul) == 0);
     BOOST_CHECK(is_field(add, mul) == 10);
     for (int i = 0; i < add.rows(); i++)
          for (int j = 0; j < add.cols(); j++) {
               BOOST_CHECK(add(i, j) == res2_add[i][j]);
               BOOST_CHECK(mul(i, j) == res2_mul[i][j]);
          }
}

// clang-format off

// addition table for Z[sqrt(2)] / (9 + 6 * sqrt(2))
int const res3_add[9][9] = {
     {0, 1, 2, 3, 4, 5, 6, 7, 8},
     {1, 2, 0, 4, 5, 3, 7, 8, 6},
     {2, 0, 1, 5, 3, 4, 8, 6, 7},
     {3, 4, 5, 6, 7, 8, 0, 1, 2},
     {4, 5, 3, 7, 8, 6, 1, 2, 0},
     {5, 3, 4, 8, 6, 7, 2, 0, 1},
     {6, 7, 8, 0, 1, 2, 3, 4, 5},
     {7, 8, 6, 1, 2, 0, 4, 5, 3},
     {8, 6, 7, 2, 0, 1, 5, 3, 4},
};

// multiplication table for Z[sqrt(2)] / (9 + 6 * sqrt(2))
int const res3_mul[9][9] = {
     {0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 2, 3, 4, 5, 6, 7, 8},
     {0, 2, 1, 6, 8, 7, 3, 5, 4},
     {0, 3, 6, 2, 5, 8, 1, 4, 7},
     {0, 4, 8, 5, 6, 1, 7, 2, 3},
     {0, 5, 7, 8, 1, 3, 4, 6, 2},
     {0, 6, 3, 1, 7, 4, 2, 8, 5},
     {0, 7, 5, 4, 2, 6, 8, 3, 1},
     {0, 8, 4, 7, 3, 2, 5, 1, 6},
};

// clang-format on

BOOST_AUTO_TEST_CASE(tables3_test) {
     Ring_Zsqrtd_quot const R(2, 9, 6);
     SHG::Linalg::Matint add;
     SHG::Linalg::Matint mul;
     R.tables(add, mul);
     // Z[sqrt(2)] / (9 + 6 * sqrt(2)) \cong GF(9).
     BOOST_CHECK(is_commutative_ring_with_unity(add, mul) == 0);
     BOOST_CHECK(is_field(add, mul) == 0);
     for (int i = 0; i < add.rows(); i++)
          for (int j = 0; j < add.cols(); j++) {
               BOOST_CHECK(add(i, j) == res3_add[i][j]);
               BOOST_CHECK(mul(i, j) == res3_mul[i][j]);
          }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
