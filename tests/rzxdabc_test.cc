#include <shg/rzxdabc.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(rzxdabc_test)

auto const params = bdata::xrange(-3, 6) * bdata::xrange(-5, 6) *
                    bdata::xrange(-5, 6) * bdata::xrange(-6, 7);

BOOST_AUTO_TEST_CASE(characteristics_test) {
     using SHG::ALGEBRA::RZXDABC::Ring_Zxdabc;
     using enum SHG::ALGEBRA::RZXDABC::Ring_Zxdabc::Case;
     using SHG::ALGEBRA::is_ring;
     using SHG::ALGEBRA::is_field;

     {
          Ring_Zxdabc const R(5, 0, 0, 1);
          BOOST_CHECK(R.rcase() == P1);
          BOOST_CHECK(R.is_finite());
          BOOST_CHECK(R.is_zero());
          BOOST_CHECK(!R.is_field());
          BOOST_CHECK(R.size() == 1);
          auto const t = R.tables();
          BOOST_REQUIRE(t.add.rows() == 1);
          BOOST_REQUIRE(t.add.cols() == 1);
          BOOST_REQUIRE(t.mul.rows() == 1);
          BOOST_REQUIRE(t.mul.cols() == 1);
          BOOST_CHECK(is_ring(t.add, t.mul) == 0);
          BOOST_CHECK(is_field(t.add, t.mul) != 0);
     }
     {
          Ring_Zxdabc const R(5, 0, 0, 0);
          BOOST_CHECK(R.rcase() == P2);
          BOOST_CHECK(!R.is_finite());
          BOOST_CHECK(!R.is_zero());
          BOOST_CHECK(!R.is_field());
          BOOST_CHECK(R.size() == 0);
          BOOST_CHECK_THROW(R.tables(), std::invalid_argument);
     }
     {
          Ring_Zxdabc const R(5, 0, 0, 2);
          BOOST_CHECK(R.rcase() == P3);
          BOOST_CHECK(R.is_finite());
          BOOST_CHECK(!R.is_zero());
          BOOST_CHECK(!R.is_field());
          BOOST_CHECK(R.size() == 4);
          auto const t = R.tables();
          BOOST_REQUIRE(t.add.rows() == 4);
          BOOST_REQUIRE(t.add.cols() == 4);
          BOOST_REQUIRE(t.mul.rows() == 4);
          BOOST_REQUIRE(t.mul.cols() == 4);
          BOOST_CHECK(is_ring(t.add, t.mul) == 0);
          BOOST_CHECK(is_field(t.add, t.mul) != 0);
     }
     {
          Ring_Zxdabc const R(5, 2, 1, 2);
          BOOST_CHECK(R.rcase() == P4);
          BOOST_CHECK(R.is_finite());
          BOOST_CHECK(R.is_zero());
          BOOST_CHECK(!R.is_field());
          BOOST_CHECK(R.size() == 1);
          auto const t = R.tables();
          BOOST_REQUIRE(t.add.rows() == 1);
          BOOST_REQUIRE(t.add.cols() == 1);
          BOOST_REQUIRE(t.mul.rows() == 1);
          BOOST_REQUIRE(t.mul.cols() == 1);
          BOOST_CHECK(is_ring(t.add, t.mul) == 0);
          BOOST_CHECK(is_field(t.add, t.mul) != 0);
     }
     {  // Catches omission of C_ calculation in Ring_Zxdabc::reset().
          Ring_Zxdabc const R(4, 2, 2, 0);
          BOOST_CHECK(R.rcase() == P4);
          BOOST_CHECK(R.is_finite());
          BOOST_CHECK(!R.is_zero());
          BOOST_CHECK(!R.is_field());
          BOOST_CHECK(R.size() == 12);
          auto const t = R.tables();
          BOOST_REQUIRE(t.add.rows() == 12);
          BOOST_REQUIRE(t.add.cols() == 12);
          BOOST_REQUIRE(t.mul.rows() == 12);
          BOOST_REQUIRE(t.mul.cols() == 12);
          BOOST_CHECK(is_ring(t.add, t.mul) == 0);
          BOOST_CHECK(is_field(t.add, t.mul) != 0);
     }
     {
          Ring_Zxdabc const R(5, 2, 1, 0);
          BOOST_CHECK(R.rcase() == P4);
          BOOST_CHECK(R.is_finite());
          BOOST_CHECK(!R.is_zero());
          BOOST_CHECK(R.is_field());
          BOOST_CHECK(R.size() == 19);
          auto const t = R.tables();
          BOOST_REQUIRE(t.add.rows() == 19);
          BOOST_REQUIRE(t.add.cols() == 19);
          BOOST_REQUIRE(t.mul.rows() == 19);
          BOOST_REQUIRE(t.mul.cols() == 19);
          BOOST_CHECK(is_ring(t.add, t.mul) == 0);
          BOOST_CHECK(is_field(t.add, t.mul) == 0);
     }
     {
          Ring_Zxdabc const R(9, 2, 6, 0);
          BOOST_CHECK(R.rcase() == P5);
          BOOST_CHECK(!R.is_finite());
          BOOST_CHECK(!R.is_zero());
          BOOST_CHECK(!R.is_field());
          BOOST_CHECK(R.size() == 0);
          BOOST_CHECK_THROW(R.tables(), std::invalid_argument);
     }
}

BOOST_AUTO_TEST_CASE(coset_test) {
     using SHG::ALGEBRA::RZXDABC::Ring_Zxdabc;
     Ring_Zxdabc R;
     Ring_Zxdabc::Polynomial p{1, 2, 8};
     Ring_Zxdabc::Element e = R.coset(p);
     BOOST_CHECK(e.a0 == 1 && e.a1 == 2);
     R.reset(2, 0, 0, 0);
     e = R.coset(p);
     BOOST_CHECK(e.a0 == 17 && e.a1 == 2);
     R.reset(2, 3, 4, 5);
     e = R.coset(p);
     BOOST_CHECK(e.a0 == 0 && e.a1 == 0);
}

BOOST_AUTO_TEST_CASE(add_sub_mul_test) {
     using SHG::ALGEBRA::RZXDABC::Ring_Zxdabc;
     using enum SHG::ALGEBRA::RZXDABC::Ring_Zxdabc::Case;
     Ring_Zxdabc const R(2, 9, 3, 6);
     BOOST_CHECK(R.rcase() == P4);
     BOOST_CHECK(R.size() == 9);
     BOOST_CHECK(R.is_field());

     // (1 + x + I) + (2 + 2x + I) = I
     BOOST_CHECK(R.add(R.coset({1, 1}), R.coset({2, 2})) ==
                 R.coset({0, 0}));

     // I - (2 + 2x + I) = (1 + x + I)
     BOOST_CHECK(R.sub(R.coset({0, 0}), R.coset({2, 2})) ==
                 R.coset({1, 1}));

     // I - (1 + x + I) = (2 + 2x + I)
     BOOST_CHECK(R.sub(R.coset({0, 0}), R.coset({1, 1})) ==
                 R.coset({2, 2}));

     // (1 + x + I) * (2 + 2x + I) = x + I
     BOOST_CHECK(R.mul(R.coset({1, 1}), R.coset({2, 2})) ==
                 R.coset({0, 1}));
}

BOOST_DATA_TEST_CASE(neg_test, params, d, a, b, c) {
     using SHG::ALGEBRA::RZXDABC::Ring_Zxdabc;
     using enum SHG::ALGEBRA::RZXDABC::Ring_Zxdabc::Case;

     Ring_Zxdabc::Element const zero(0, 0);
     Ring_Zxdabc const R(d, a, b, c);
     switch (R.rcase()) {
     case P1:
          BOOST_CHECK(R.neg(zero) == zero);
          break;
     case P2:
          for (int r0 = -10; r0 <= 10; r0++)
               for (int r1 = -10; r1 <= 10; r1++) {
                    Ring_Zxdabc::Element const r(r0, r1);
                    auto const s = R.neg(r);
                    auto const u = R.add(r, s);
                    BOOST_CHECK(u == zero);
               }
          break;
     case P3: {
          int const n = R.size();
          for (int i = 0; i < n; i++) {
               auto const r = R.element(i);
               auto const s = R.neg(r);
               auto const u = R.add(r, s);
               BOOST_CHECK(u == zero);
          }
          break;
     }
     case P4: {
          int const n = R.size();
          for (int i = 0; i < n; i++) {
               auto const r = R.element(i);
               auto const s = R.neg(r);
               auto const u = R.add(r, s);
               BOOST_CHECK(u == zero);
          }
          break;
     }
     case P5: {
          int const D = R.D();
          for (int r0 = -10; r0 <= 10; r0++)
               for (int r1 = 0; r1 < D; r1++) {
                    Ring_Zxdabc::Element const r(r0, r1);
                    auto const s = R.neg(r);
                    auto const u = R.add(r, s);
                    BOOST_CHECK(u == zero);
               }
          break;
     }
     default:
          BOOST_REQUIRE(false);
          break;
     }
}

BOOST_AUTO_TEST_CASE(nr_test) {
     using SHG::ALGEBRA::RZXDABC::Ring_Zxdabc;
     using enum SHG::ALGEBRA::RZXDABC::Ring_Zxdabc::Case;
     Ring_Zxdabc const R(2, 9, 3, 6);
     int const max = R.size();
     for (int i = 0; i < max; i++) {
          auto const e = R.element(i);
          BOOST_CHECK(R.nr(e) == i);
     }
     BOOST_CHECK_THROW(R.element(-1), std::invalid_argument);
     BOOST_CHECK_THROW(R.element(max), std::invalid_argument);
}

BOOST_DATA_TEST_CASE(tables_test, params, d, a, b, c) {
     using SHG::ALGEBRA::RZXDABC::Ring_Zxdabc;
     using enum SHG::ALGEBRA::RZXDABC::Ring_Zxdabc::Case;
     using SHG::ALGEBRA::is_field;

     Ring_Zxdabc const R(d, a, b, c);
     if (!R.is_finite()) {
          BOOST_CHECK(R.size() == 0);
          return;
     }
     auto const t = R.tables();
     if (R.is_field())
          BOOST_CHECK(is_field(t.add, t.mul) == 0);
     else
          BOOST_CHECK(is_field(t.add, t.mul) == 10);
}

BOOST_DATA_TEST_CASE(unit_test, params, d, a, b, c) {
     using SHG::ALGEBRA::RZXDABC::Ring_Zxdabc;
     using enum SHG::ALGEBRA::RZXDABC::Ring_Zxdabc::Case;

     Ring_Zxdabc const R(d, a, b, c);
     Ring_Zxdabc::Element const one(1, 0);

     switch (R.rcase()) {
     case P1:
     case P3:
     case P4: {
          int const n = R.size();
          BOOST_REQUIRE(n > 0);
          auto const t = R.tables();
          for (int nr = 0; nr < n; nr++) {
               int nr1 = -1;
               auto const r = R.element(nr);
               for (int i = 0; i < n; i++)
                    if (t.mul(nr, i) == 1) {
                         nr1 = i;
                         break;
                    }
               if (nr1 < 0) {
                    BOOST_CHECK(!R.is_unit(r));
                    auto const s = R.inv(r);
                    BOOST_CHECK(!s.has_value());
               } else {
                    BOOST_CHECK(R.is_unit(r));
                    auto const s = R.inv(r);
                    BOOST_REQUIRE(s.has_value());
                    BOOST_CHECK(*s == R.element(nr1));
                    BOOST_CHECK(R.mul(r, *s) == one);
                    auto const r1 = R.inv(*s);
                    BOOST_REQUIRE(r1.has_value());
                    BOOST_CHECK(r1 == r);
               }
          }
          break;
     }
     case P2:
          for (int r0 = -10; r0 <= 10; r0++)
               for (int r1 = -10; r1 < 10; r1++) {
                    Ring_Zxdabc::Element const r(r0, r1);
                    auto const s = R.inv(r);
                    if (R.is_unit(r)) {
                         BOOST_REQUIRE(s.has_value());
                         BOOST_CHECK(R.mul(r, *s) == one);
                         auto const r1 = R.inv(*s);
                         BOOST_REQUIRE(r1.has_value());
                         BOOST_CHECK(r1 == r);
                    } else {
                         BOOST_CHECK(!s.has_value());
                    }
               }
          break;
     case P5: {
          int const D = R.D();
          for (int r0 = -10; r0 <= 10; r0++)
               for (int r1 = 0; r1 < D; r1++) {
                    Ring_Zxdabc::Element const r(r0, r1);
                    auto const s = R.inv(r);
                    if (R.is_unit(r)) {
                         BOOST_REQUIRE(s.has_value());
                         BOOST_CHECK(R.mul(r, *s) == one);
                         auto const r1 = R.inv(*s);
                         BOOST_REQUIRE(r1.has_value());
                         BOOST_CHECK(r1 == r);
                    } else {
                         BOOST_CHECK(!s.has_value());
                    }
               }
          break;
     }
     default:
          BOOST_REQUIRE(false);
          break;
     }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
