#include <shg/commalg.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(commalg_test)

BOOST_AUTO_TEST_SUITE(is_commutative_ring_with_unity_test)

using SHG::Linalg::Matint;
using SHG::ALGEBRA::is_commutative_ring_with_unity;

BOOST_AUTO_TEST_CASE(commutative_ring_with_unity_test) {
     int const n = 6;
     Matint add(n, n);
     Matint mul(n, n);

     for (int i = 0; i < n; i++)
          for (int j = 0; j < n; j++) {
               add(i, j) = (i + j) % n;
               mul(i, j) = (i * j) % n;
          }
     // std::cout << add << "\n\n" << mul << '\n';
     BOOST_CHECK(is_commutative_ring_with_unity(add, mul) == 0);
}

BOOST_AUTO_TEST_CASE(commutative_ring_without_unity_test) {
     int const n = 6;
     Matint add(n, n);
     Matint mul(n, n);

     for (int i = 0; i < n; i++)
          for (int j = 0; j < n; j++) {
               add(i, j) = (i + j) % n;
               mul(i, j) = 0;
          }
     BOOST_CHECK(is_commutative_ring_with_unity(add, mul) == 5);
}

/*
 * As a non-commutative ring example we take a ring of 2 x 2
 * upper-triangle matrices over the field \f$F_2\f$. Matrices are
 * denoted as follows:
 *
 * 0  1  2  3  4  5  6  7
 * 00 10 00 01 10 01 11 11
 * 00 01 01 00 00 01 00 01
 */
BOOST_AUTO_TEST_CASE(noncommutative_ring_test) {
     int const n = 8;
     Matint add(n, n);
     Matint mul(n, n);

     // clang-format off

     add << 0, 1, 2, 3, 4, 5, 6, 7,
            1, 0, 4, 7, 2, 6, 5, 3,
            2, 4, 0, 5, 1, 3, 7, 6,
            3, 7, 5, 0, 6, 2, 4, 1,
            4, 2, 1, 6, 0, 7, 3, 5,
            5, 6, 3, 2, 7, 0, 1, 4,
            6, 5, 7, 4, 3, 1, 0, 2,
            7, 3, 6, 1, 5, 4, 2, 0;

     mul << 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 2, 3, 4, 5, 6, 7,
            0, 2, 2, 0, 0, 2, 0, 2,
            0, 3, 3, 0, 0, 3, 0, 3,
            0, 4, 0, 3, 4, 3, 6, 6,
            0, 5, 5, 0, 0, 5, 0, 5,
            0, 6, 3, 3, 4, 0, 6, 1,
            0, 7, 5, 3, 4, 2, 6, 1;

     // clang-format on

     // std::cout << add << "\n\n" << mul << '\n';
     BOOST_CHECK(is_commutative_ring_with_unity(add, mul) == 3);
}

struct ET_less {
     bool operator()(SHG::ALGEBRA::Ring_Zu_quot::ET const& a,
                     SHG::ALGEBRA::Ring_Zu_quot::ET const& b) const {
          if (a.x < b.x)
               return true;
          if (a.x > b.x)
               return false;
          if (a.y < b.y)
               return true;
          return false;
     }
};

BOOST_DATA_TEST_CASE(ring_zu_quot_test,
                     bdata::xrange(21) * bdata::xrange(41), xr1,
                     xr2) {
     using SHG::ALGEBRA::Ring_Zu_quot;
     using SHG::ALGEBRA::is_commutative_ring;
     using SHG::ALGEBRA::Element;

     int const a = static_cast<int>(xr1) - 10;
     int const b = static_cast<int>(xr2) - 20;
     if (std::abs(a) < 2) {
          BOOST_CHECK(true);
          return;
     }
     int const n = a * a;

     Ring_Zu_quot const r(a, b);
     BOOST_CHECK(is_commutative_ring(&r));
     std::map<Ring_Zu_quot::ET, int, ET_less> m;

     bool success;
     success = m.insert({r.value(r.zero()), 0}).second;
     BOOST_CHECK(success);
     success = m.insert({r.value(r.one()), 1}).second;
     BOOST_CHECK(success);
     int const absa = std::abs(r.a());
     int k = 2;
     for (int i = 0; i < absa; i++)
          for (int j = 0; j < absa; j++)
               if (i > 1 || j > 0) {
                    Ring_Zu_quot::ET x(i, j);
                    success = m.insert({x, k++}).second;
                    BOOST_CHECK(success);
               }

     Matint add(n, n);
     Matint mul(n, n);
     for (int i1 = 0; i1 < absa; i1++)
          for (int j1 = 0; j1 < absa; j1++) {
               Ring_Zu_quot::ET x1(i1, j1);
               auto const it1 = m.find(x1);
               BOOST_REQUIRE(it1 != m.cend());
               int const k1 = it1->second;
               Element const e1 = r.element(x1);
               for (int i2 = 0; i2 < absa; i2++)
                    for (int j2 = 0; j2 < absa; j2++) {
                         Ring_Zu_quot::ET x2(i2, j2);
                         auto const it2 = m.find(x2);
                         BOOST_REQUIRE(it2 != m.cend());
                         int const k2 = it2->second;
                         Element const e2 = r.element(x2);

                         Element const e3 = e1 + e2;
                         Ring_Zu_quot::ET const x3 = r.value(e3);
                         auto const it3 = m.find(x3);
                         BOOST_REQUIRE(it3 != m.cend());
                         int const k3 = it3->second;
                         add(k1, k2) = k3;

                         Element const e4 = e1 * e2;
                         Ring_Zu_quot::ET const x4 = r.value(e4);
                         auto const it4 = m.find(x4);
                         BOOST_REQUIRE(it4 != m.cend());
                         int const k4 = it4->second;
                         mul(k1, k2) = k4;
                    }
          }
     // std::cout << add << "\n\n" << mul << '\n';
     BOOST_CHECK(is_commutative_ring_with_unity(add, mul) == 0);
     BOOST_CHECK(SHG::ALGEBRA::is_field(add, mul) == 10);
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(ring_ru_over_z_test) {
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Ring_Z;
     using SHG::ALGEBRA::Ring_Ru;
     using SHG::ALGEBRA::is_zero;
     using SHG::ALGEBRA::is_commutative_ring;

     Ring_Z const Z;
     Ring_Ru const Ru(&Z, 2);
     BOOST_CHECK(is_commutative_ring(&Ru));
     BOOST_CHECK(Ru.base_ring() == &Z);
     BOOST_CHECK(Ru.n() == 2);

     Element const a0{Z.element(1)};
     Element const a1{Z.element(2)};
     Element const b0{Z.element(3)};
     Element const b1{Z.element(4)};
     Ring_Ru::ET const ae{{a0, a1}};
     Ring_Ru::ET const be{{b0, b1}};
     Element const a{Ru.element(ae)};
     Element const b{Ru.element(be)};
     Element const d{
          Ru.element(Ring_Ru::ET{Z.element(3), Z.element(10)})};
     BOOST_CHECK(Ru.is_unit(a));
     BOOST_CHECK(Ru.is_one(a * inv(a)));
     BOOST_CHECK(!Ru.is_unit(b));
     BOOST_CHECK(!Ru.is_unit(d));

     Element c{&Ru};
     BOOST_CHECK(Ru.value(c)[0] == Z.element(0));
     BOOST_CHECK(Ru.value(c)[1] == Z.element(0));
     BOOST_CHECK(is_zero(c));
     BOOST_CHECK(!is_one(c));
     BOOST_CHECK(c != d);
     c.set_to_one();
     BOOST_CHECK(Ru.value(c)[0] == Z.element(1));
     BOOST_CHECK(Ru.value(c)[1] == Z.element(0));
     BOOST_CHECK(is_one(c));
     BOOST_CHECK(c != d);
     c.set_to_zero();
     BOOST_CHECK(Ru.value(c)[0] == Z.element(0));
     BOOST_CHECK(Ru.value(c)[1] == Z.element(0));
     BOOST_CHECK(is_zero(c));
     BOOST_CHECK(!is_one(c));
     BOOST_CHECK(c != d);
     c = a + b;
     BOOST_CHECK(Ru.value(c)[0] == Z.element(4));
     BOOST_CHECK(Ru.value(c)[1] == Z.element(6));
     BOOST_CHECK(!is_zero(c));
     BOOST_CHECK(!is_one(c));
     BOOST_CHECK(c != d);
     c = -c;
     BOOST_CHECK(Ru.value(c)[0] == Z.element(-4));
     BOOST_CHECK(Ru.value(c)[1] == Z.element(-6));
     BOOST_CHECK(!is_zero(c));
     BOOST_CHECK(!is_one(c));
     BOOST_CHECK(c != d);
     c = a * b;
     BOOST_CHECK(Ru.value(c)[0] == Z.element(3));
     BOOST_CHECK(Ru.value(c)[1] == Z.element(10));
     BOOST_CHECK(!is_zero(c));
     BOOST_CHECK(!is_one(c));
     BOOST_CHECK(c == d);

     std::ostringstream oss;
     oss << c;
     BOOST_CHECK(oss.str() == "3 10");
     std::istringstream iss(oss.str());
     Element e{&Ru};
     iss >> e;
     BOOST_CHECK(e == c);
}

BOOST_DATA_TEST_CASE(ring_ru_over_zn_test,
                     bdata::xrange(3) * bdata::xrange(12), xr1, xr2) {
     using SHG::ALGEBRA::Invalid_operation;
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Ring_Zn;
     using SHG::ALGEBRA::Ring_Ru;
     using SHG::ALGEBRA::is_zero;
     using SHG::ALGEBRA::is_commutative_ring;
     using SHG::Varlex;

     int const n = xr1 + 1;
     int const m = xr2 + 1;
     Ring_Zn const Zm(m);
     Ring_Ru const Ru(&Zm, n);
     Element const one = Ru.one();

     BOOST_CHECK(is_commutative_ring(&Ru));
     BOOST_CHECK(Ru.base_ring() == &Zm);
     BOOST_CHECK(Ru.n() == n);

     Varlex va(m, n);
     // For each polynomial a in Z_m[x] / (x^n).
     do {
          std::vector<Element> v(n);
          for (int i = 0; i < n; i++)
               v[i] = Zm.element(va.get()[i]);
          Element const a = Ru.element(v);
          Element b(a);
          if (Ru.is_nilpotent(a)) {
               while (!is_zero(b))
                    b *= a;
          } else {
               int const nm = n * m;
               for (int i = 0; i < nm; i++, b *= a)
                    BOOST_CHECK(!is_zero(b));
          }
          if (Ru.is_unit(a))
               BOOST_CHECK(a * inv(a) == one);
          else
               BOOST_CHECK_THROW(inv(a), Invalid_operation);

          // Check if a in Zm / (x^n) is a zerodivisor.
          Varlex vb(m, n);
          bool found = false;
          do {
               for (int i = 0; i < n; i++)
                    v[i] = Zm.element(vb.get()[i]);
               b = Ru.element(v);
               if (!is_zero(b) && is_zero(a * b))
                    found = true;
          } while (!found && vb.next());
          BOOST_CHECK(Ru.is_zerodivisor(a) == found);
     } while (va.next());
}

BOOST_AUTO_TEST_CASE(basic_test) {
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
