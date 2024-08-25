#include <shg/algebra.h>
#include <sstream>
#include <shg/ifact.h>
#include <shg/utils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(algebra_test)

using SHG::ALGEBRA::Element;
using SHG::ALGEBRA::Finite_group;
using SHG::ALGEBRA::Group_Sn;
using SHG::ALGEBRA::Invalid_operation;
using SHG::ALGEBRA::Ring_Z;
using SHG::ALGEBRA::Ring_Zn;
using std::invalid_argument;

BOOST_AUTO_TEST_CASE(invalid_operation_test) {
     auto const is_correct = [](Invalid_operation const& e) {
          return std::strcmp(e.what(), "Invalid operation") == 0;
     };
     BOOST_CHECK_EXCEPTION(throw Invalid_operation(),
                           Invalid_operation, is_correct);
}

Element test_pow(Element const& x, int n) {
     BOOST_REQUIRE(x.is_valid());

     Element y = x.as()->one();
     Element z = x;
     if (n < 0) {
          z = inv(z);
          n = -n;
     }
     for (int i = 0; i < n; i++)
          y *= z;
     return y;
}

BOOST_AUTO_TEST_CASE(element_constructor_test) {
     {
          Element x;
          BOOST_CHECK(!x.is_valid());
          BOOST_CHECK(x.as() == nullptr);
          BOOST_CHECK(!x.value().has_value());
     }
     {
          Ring_Z const A;
          Element x(&A);
          BOOST_CHECK(x.is_valid());
          BOOST_CHECK(x.as() == &A);
          BOOST_CHECK(x.value().has_value());
          BOOST_CHECK(is_zero(x));
     }
     {
          Ring_Z const A;
          Ring_Z::ET const a{5};
          Element x(&A, a);
          BOOST_CHECK(x.is_valid());
          BOOST_CHECK(x.as() == &A);
          BOOST_CHECK(x.value().has_value());
     }
     {
          Ring_Z const A;
          Element x;
          // Constructor requires the type ET, not int.
          BOOST_CHECK_THROW(x = Element(&A, 5), invalid_argument);
          BOOST_CHECK(!x.is_valid());
          BOOST_CHECK(x.as() == nullptr);
          BOOST_CHECK(!x.value().has_value());
     }
     {
          Element x;
          BOOST_CHECK_THROW(x = Element(nullptr, 5),
                            invalid_argument);
          BOOST_CHECK(!x.is_valid());
          BOOST_CHECK(x.as() == nullptr);
          BOOST_CHECK(!x.value().has_value());
     }
}

BOOST_DATA_TEST_CASE(group_s_n_test, bdata::xrange(5), xr) {
     using SHG::ALGEBRA::is_group;
     int const n = xr + 1;
     Group_Sn const G(n);

     BOOST_CHECK(is_group(&G));
     BOOST_CHECK(G.is_abelian() == (n < 3));
     Element const one = G.one();

     Group_Sn::ET v(n);
     std::iota(v.begin(), v.end(), 0);
     BOOST_CHECK(G.element(v) == one);
     do {
          Element const x = G.element(v);
          BOOST_CHECK(G.value(x) == v);
          BOOST_CHECK(one * x == x);
          BOOST_CHECK(x * one == x);
          Element const y = inv(x);
          BOOST_CHECK(x * y == one);
          BOOST_CHECK(y * x == one);
          Element u = one / x;
          BOOST_CHECK(u * x == one);
          u = one / (x * x);
          BOOST_CHECK(u + (x + x) == one);
          for (int i = -2 * n; i <= 2 * n; i++)
               BOOST_CHECK(pow(x, i) == test_pow(x, i));
     } while (std::next_permutation(v.begin(), v.end()));
     std::iota(v.begin(), v.end(), 0);
     v[0]++;
     BOOST_CHECK_THROW(G.element(v), invalid_argument);
}

BOOST_AUTO_TEST_CASE(finite_group_test) {
     using SHG::Matrix;
     using SHG::ALGEBRA::is_group;

     // clang-format off
     Matrix<int> const g0(2, 2, {
               0, 1,
               1, 0});
     // The smallest non-commutative group.
     Matrix<int> const g1(6, 6, {
               0, 1, 2, 3, 4, 5,
               1, 5, 3, 4, 2, 0,
               2, 4, 0, 5, 1, 3,
               3, 2, 1, 0, 5, 4,
               4, 3, 5, 1, 0, 2,
               5, 0, 4, 2, 3, 1});

     // \cite burn-1978, page 278
     Matrix<int> const burn1(6, 6, {
               0, 1, 2, 3, 4, 5,
               1, 2, 0, 4, 5, 3,
               2, 0, 1, 5, 3, 4,
               3, 4, 5, 0, 1, 2,
               4, 5, 3, 2, 0, 1,
               5, 3, 4, 1, 2, 0});

     // \cite burn-1978, page 280
     Matrix<int> const burn2(5, 5, {
               0, 1, 2, 3, 4,
               1, 0, 3, 4, 2,
               2, 4, 0, 1, 3,
               3, 2, 4, 0, 1,
               4, 3, 1, 2, 0});
     // clang-format on

     {
          Finite_group const G;
          BOOST_CHECK(is_group(&G));
          BOOST_CHECK(G.is_abelian());
          BOOST_CHECK(G.order() == 1);
          BOOST_CHECK(G.value(G.one()) == 0);
          BOOST_CHECK(inv(G.element(0)) == G.element(0));
          BOOST_CHECK(G.element(0) * G.element(0) == G.element(0));
     }
     {
          Finite_group const G({2, 2, {0, 1, 1, 0}});
          BOOST_CHECK(is_group(&G));
          BOOST_CHECK(G.is_abelian());
          BOOST_CHECK(G.order() == 2);
          BOOST_CHECK(G.value(G.one()) == 0);
          BOOST_CHECK(inv(G.element(1)) == G.element(1));
          BOOST_CHECK(G.element(1) * G.element(1) == G.element(0));
     }

     Finite_group G(g1);
     BOOST_CHECK(is_group(&G));
     BOOST_CHECK(!G.is_abelian());
     BOOST_CHECK(G.order() == 6);
     for (int i = 0; i < G.order(); i++)
          BOOST_CHECK(G.value(G.element(i)) == i);
     BOOST_CHECK(G.value(G.one()) == 0);
     BOOST_CHECK(G.element(4) * G.element(2) == G.element(5));
     BOOST_CHECK(G.element(4) + G.element(2) == G.element(5));
     BOOST_CHECK(inv(G.element(1)) == G.element(5));
     BOOST_CHECK(-G.element(1) == G.element(5));

     BOOST_CHECK_THROW(Finite_group G(burn1), invalid_argument);
     BOOST_CHECK_THROW(Finite_group G(burn2), invalid_argument);
}

BOOST_AUTO_TEST_CASE(ring_z_test) {
     using SHG::ALGEBRA::is_commutative_ring;
     Ring_Z const Z;

     BOOST_CHECK(is_commutative_ring(&Z));
     BOOST_CHECK(!Z.is_field());
     Element const zero = Z.zero();
     BOOST_CHECK(Z.value(zero) == 0);
     Element const one = Z.one();
     BOOST_CHECK(Z.value(one) == 1);

     int const n = 20;
     for (int i = -n; i <= n; i++) {
          Element const x = Z.element(i);
          BOOST_CHECK(Z.value(x) == i);
          Element const y = -x;
          BOOST_CHECK(-x == y);
          BOOST_CHECK(x + y == zero);
          BOOST_CHECK(y + x == zero);
          BOOST_CHECK(x + zero == x);
          BOOST_CHECK(zero + x == x);
          if (i == -1 || i == 1) {
               BOOST_CHECK(inv(x) == x);
               BOOST_CHECK(Z.is_unit(x));
          } else {
               BOOST_CHECK_THROW(inv(x), Invalid_operation);
          }
          if (i == 0) {
               BOOST_CHECK(Z.is_zerodivisor(x));
               BOOST_CHECK(Z.is_nilpotent(x));
          } else {
               BOOST_CHECK(!Z.is_zerodivisor(x));
               BOOST_CHECK(!Z.is_nilpotent(x));
          }
          for (int j = -n; j <= n; j++) {
               Element const y = Z.element(j);
               BOOST_CHECK(Z.value(x + y) == i + j);
               BOOST_CHECK(Z.value(y + x) == i + j);
               BOOST_CHECK(Z.value(x * y) == i * j);
               BOOST_CHECK(Z.value(y * x) == i * j);
          }
     }
}

BOOST_AUTO_TEST_CASE(ring_zn_element_type_test) {
     Ring_Zn const Z8_1(8);
     Ring_Zn const Z8_2(8);
     Element const e1 = Z8_1.element(2);
     Element const e2 = Z8_2.element(2);
     Element e;
     bool b;

     BOOST_CHECK_THROW(e = e1 + e2, std::invalid_argument);
     try {
          b = e1 == e2;
          b = false;
     } catch (std::invalid_argument const&) {
          b = true;
     }
     BOOST_CHECK(b);
}

/**
 * Returns true if x is nilpotent in \f$Z_n\f$.
 */
bool is_zn_nilpotent(int x, int n) {
     BOOST_REQUIRE(n > 0);
     BOOST_REQUIRE(x >= 0 && x < n);
     int const x0{x};
     for (int i = 0; i < n; i++) {
          if (x == 0)
               return true;
          x = (x * x0) % n;
     }
     return false;
}

/**
 * Returns true if x is a zerodivisor in \f$Z_n\f$.
 */
bool is_zn_zerodivisor(int x, int n) {
     BOOST_REQUIRE(n > 0);
     BOOST_REQUIRE(x >= 0 && x < n);
     for (int i = 1; i < n; i++)
          if ((x * i) % n == 0)
               return true;
     return false;
}

BOOST_DATA_TEST_CASE(ring_zn_test, bdata::xrange(21), xr) {
     using SHG::ALGEBRA::is_commutative_ring;
     using SHG::gcd;
     using SHG::is_prime;
     int const n = xr + 1;
     Ring_Zn const A(n);

     BOOST_CHECK(is_commutative_ring(&A));
     Element const zero = A.zero();
     BOOST_CHECK(A.value(zero) == 0);
     Element const one = A.one();
     if (n == 1)
          BOOST_CHECK(A.value(one) == 0);
     else
          BOOST_CHECK(A.value(one) == 1);
     BOOST_CHECK(is_prime(n) == A.is_field());

     for (int i = 0; i < n; i++) {
          Element const x = A.element(i);
          BOOST_CHECK(A.value(x) == i);
          if (gcd(i, n) == 1)
               BOOST_CHECK(x * inv(x) == one);
          else
               BOOST_CHECK_THROW(inv(x), Invalid_operation);
          if (A.is_unit(x)) {
               Element const y = inv(x);
               BOOST_CHECK(x * y == one);
          } else {
               BOOST_CHECK_THROW(inv(x), Invalid_operation);
          }
          BOOST_CHECK(A.is_zerodivisor(x) == is_zn_zerodivisor(i, n));
          BOOST_CHECK(A.is_nilpotent(x) == is_zn_nilpotent(i, n));
          Element const y = -x;
          BOOST_CHECK(-x == y);
          BOOST_CHECK(x + y == zero);
          BOOST_CHECK(y + x == zero);
          BOOST_CHECK(x + zero == x);
          BOOST_CHECK(zero + x == x);
          if (A.is_field()) {
               if (i != 0) {
                    Element const y = inv(x);
                    BOOST_CHECK(inv(x) == y);
                    BOOST_CHECK(x * y == one);
                    BOOST_CHECK(y * x == one);
               } else {
                    BOOST_CHECK_THROW(inv(x), Invalid_operation);
               }
          }
          for (int j = 0; j < n; j++) {
               Element const y = A.element(j);
               BOOST_CHECK(A.value(x + y) == (i + j) % n);
               BOOST_CHECK(A.value(y + x) == (i + j) % n);
               BOOST_CHECK(A.value(x * y) == (i * j) % n);
               BOOST_CHECK(A.value(y * x) == (i * j) % n);
          }
     }
     BOOST_CHECK_THROW(A.element(-1), invalid_argument);
     BOOST_CHECK_THROW(A.element(n), invalid_argument);
}

BOOST_AUTO_TEST_CASE(field_q_test) {
     using SHG::ALGEBRA::Field_Q;
     using SHG::ALGEBRA::is_commutative_ring;
     Field_Q const F;

     BOOST_CHECK(is_commutative_ring(&F));
     BOOST_CHECK(F.is_field());
     Element const zero = F.zero();
     BOOST_CHECK(F.value(zero) == 0);
     Element const one = F.one();
     BOOST_CHECK(F.value(one) == 1);

     BOOST_CHECK(is_zero(zero));
     BOOST_CHECK(!is_one(zero));
     BOOST_CHECK(is_one(one));
     BOOST_CHECK(!is_zero(one));

     int const n = 20;
     for (int i = -n; i <= n; i++) {
          Element const x = F.element(i);
          BOOST_CHECK(F.value(x) == i);
          if (i == 0) {
               BOOST_CHECK(F.is_nilpotent(x));
               BOOST_CHECK(!F.is_unit(x));
               BOOST_CHECK(F.is_zerodivisor(x));
          } else {
               BOOST_CHECK(!F.is_nilpotent(x));
               BOOST_CHECK(F.is_unit(x));
               BOOST_CHECK(!F.is_zerodivisor(x));
          }
          for (int j = -n; j <= n; j++)
               if (j != 0) {
                    Element const x = F.element(i, j);
                    Field_Q::ET const v = j < 0 ? Field_Q::ET(-i, -j)
                                                : Field_Q::ET(i, j);
                    BOOST_CHECK(F.value(x) == v);
                    Element const y = -x;
                    BOOST_CHECK(-x == y);
                    BOOST_CHECK(x + y == zero);
                    BOOST_CHECK(y + x == zero);
                    BOOST_CHECK(x + zero == x);
                    BOOST_CHECK(zero + x == x);
                    BOOST_CHECK(x * one == x);
                    BOOST_CHECK(one * x == x);
                    if (i != 0) {
                         BOOST_CHECK(!is_zero(x));
                         Element const y = inv(x);
                         BOOST_CHECK(x * y == one);
                         BOOST_CHECK(y * x == one);

                    } else {
                         BOOST_CHECK(is_zero(x));
                         BOOST_CHECK_THROW(inv(x), invalid_argument);
                    }

               } else {
                    BOOST_CHECK_THROW(F.element(i, j),
                                      invalid_argument);
               }
     }
}

BOOST_AUTO_TEST_CASE(direct_product_test) {
     using SHG::ALGEBRA::AS;
     using SHG::ALGEBRA::Direct_product;
     {
          std::vector<AS const*> v;
          Direct_product P;
          BOOST_CHECK_THROW(Direct_product{v}, std::invalid_argument);
          BOOST_CHECK_THROW(P.reset(v), std::invalid_argument);
          v.resize(2);
          v[0] = v[1] = nullptr;
          BOOST_CHECK_THROW(Direct_product{v}, std::invalid_argument);
          BOOST_CHECK_THROW(P.reset(v), std::invalid_argument);
     }
     Group_Sn const Sn(2);
     Direct_product const G({&Sn, &Sn});

     Element const e0 = Sn.element({0, 1});
     Element const e1 = Sn.element({1, 0});

     Direct_product::ET const ve{e0, e0};
     Direct_product::ET const va{e0, e1};
     Direct_product::ET const vb{e1, e0};
     Direct_product::ET const vc{e1, e1};

     Element const e = G.element(ve);
     Element const a = G.element(va);
     Element const b = G.element(vb);
     Element const c = G.element(vc);

     BOOST_CHECK(G.one() == e);
     BOOST_CHECK(is_one(e));
     BOOST_CHECK(is_zero(e));
     BOOST_CHECK(G.value(e) == ve);
     BOOST_CHECK(G.value(a) == va);
     BOOST_CHECK(G.value(b) == vb);
     BOOST_CHECK(G.value(c) == vc);

     BOOST_CHECK(e == e);
     BOOST_CHECK(a != e);
     BOOST_CHECK(b != e);
     BOOST_CHECK(c != e);
     BOOST_CHECK(e != a);
     BOOST_CHECK(a == a);
     BOOST_CHECK(b != a);
     BOOST_CHECK(c != a);
     BOOST_CHECK(e != b);
     BOOST_CHECK(a != b);
     BOOST_CHECK(b == b);
     BOOST_CHECK(c != b);
     BOOST_CHECK(e != c);
     BOOST_CHECK(a != c);
     BOOST_CHECK(b != c);
     BOOST_CHECK(c == c);

     BOOST_CHECK(e * e == e);
     BOOST_CHECK(e * a == a);
     BOOST_CHECK(e * b == b);
     BOOST_CHECK(e * c == c);
     BOOST_CHECK(a * e == a);
     BOOST_CHECK(a * a == e);
     BOOST_CHECK(a * b == c);
     BOOST_CHECK(a * c == b);
     BOOST_CHECK(b * e == b);
     BOOST_CHECK(b * a == c);
     BOOST_CHECK(b * b == e);
     BOOST_CHECK(b * c == a);
     BOOST_CHECK(c * e == c);
     BOOST_CHECK(c * a == b);
     BOOST_CHECK(c * b == a);
     BOOST_CHECK(c * c == e);

     BOOST_CHECK(inv(e) == e);
     BOOST_CHECK(inv(a) == a);
     BOOST_CHECK(inv(b) == b);
     BOOST_CHECK(inv(c) == c);

     BOOST_CHECK(e / e == e);
     BOOST_CHECK(a / a == e);
     BOOST_CHECK(b / b == e);
     BOOST_CHECK(c / c == e);
     BOOST_CHECK(a / e == a);
     BOOST_CHECK(e / a == a);
     BOOST_CHECK(c / b == a);
     BOOST_CHECK(b / c == a);
     BOOST_CHECK(b / e == b);
     BOOST_CHECK(c / a == b);
     BOOST_CHECK(e / b == b);
     BOOST_CHECK(a / c == b);
     BOOST_CHECK(c / e == c);
     BOOST_CHECK(b / a == c);
     BOOST_CHECK(a / b == c);
     BOOST_CHECK(e / c == c);

     BOOST_CHECK(e + e == e);
     BOOST_CHECK(e + a == a);
     BOOST_CHECK(e + b == b);
     BOOST_CHECK(e + c == c);
     BOOST_CHECK(a + e == a);
     BOOST_CHECK(a + a == e);
     BOOST_CHECK(a + b == c);
     BOOST_CHECK(a + c == b);
     BOOST_CHECK(b + e == b);
     BOOST_CHECK(b + a == c);
     BOOST_CHECK(b + b == e);
     BOOST_CHECK(b + c == a);
     BOOST_CHECK(c + e == c);
     BOOST_CHECK(c + a == b);
     BOOST_CHECK(c + b == a);
     BOOST_CHECK(c + c == e);

     BOOST_CHECK(-e == e);
     BOOST_CHECK(-a == a);
     BOOST_CHECK(-b == b);
     BOOST_CHECK(-c == c);

     BOOST_CHECK(e - e == e);
     BOOST_CHECK(a - a == e);
     BOOST_CHECK(b - b == e);
     BOOST_CHECK(c - c == e);
     BOOST_CHECK(a - e == a);
     BOOST_CHECK(e - a == a);
     BOOST_CHECK(c - b == a);
     BOOST_CHECK(b - c == a);
     BOOST_CHECK(b - e == b);
     BOOST_CHECK(c - a == b);
     BOOST_CHECK(e - b == b);
     BOOST_CHECK(a - c == b);
     BOOST_CHECK(c - e == c);
     BOOST_CHECK(b - a == c);
     BOOST_CHECK(a - b == c);
     BOOST_CHECK(e - c == c);

     Element x(&G);
     x.set_to_zero();
     BOOST_CHECK(is_zero(x));
     x.set_to_one();
     BOOST_CHECK(is_one(x));
}

BOOST_DATA_TEST_CASE(group_s_n_input_output_test, bdata::xrange(5),
                     xr) {
     int const n = xr + 1;
     Group_Sn const G(n);
     Element y(&G);
     Group_Sn::ET v(n);
     std::stringstream ss;

     std::iota(v.begin(), v.end(), 0);
     do {
          Element const x = G.element(v);
          ss.clear();
          ss << x;
          BOOST_CHECK(!ss.fail());
          ss >> y;
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(x == y);
     } while (std::next_permutation(v.begin(), v.end()));
}

BOOST_AUTO_TEST_CASE(group_s_n_input_fails_test) {
     std::stringstream ss;
     Group_Sn const G(3);
     Element x(&G);

     ss.clear();
     ss.str("");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     ss.clear();
     ss.str("3 0 1 3");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     ss.clear();
     ss.str("301 2");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     ss.clear();
     ss.str("2 0 1");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
}

BOOST_AUTO_TEST_CASE(finite_group_input_output_test) {
     Finite_group const G({3, 3, {0, 1, 2, 1, 2, 0, 2, 0, 1}});
     Element x(&G), y(&G);
     std::stringstream ss;

     for (int i = 0; i < 3; i++) {
          x = G.element(i);
          ss.clear();
          ss << x;
          BOOST_CHECK(!ss.fail());
          ss >> y;
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(x == y);
     }
     ss.clear();
     ss.str("3");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
}

BOOST_DATA_TEST_CASE(ring_z_input_output_test, bdata::xrange(41),
                     xr) {
     int const i = xr - 20;
     Ring_Z const Z;
     Element const x = Z.element(i);
     Element y(&Z);
     std::stringstream ss;

     ss << x;
     BOOST_CHECK(!ss.fail());

     if (i == 0)
          BOOST_CHECK(ss.str() == "0");
     if (i == -1)
          BOOST_CHECK(ss.str() == "-1");
     if (i == 1)
          BOOST_CHECK(ss.str() == "1");

     ss >> y;
     BOOST_CHECK(!ss.fail());
     BOOST_CHECK(x == y);
}

BOOST_AUTO_TEST_CASE(ring_z_input_fails_test) {
     std::stringstream ss;
     Ring_Z Z;
     Element x(&Z);

     ss.clear();
     ss.str("xxx");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     ss.clear();
     ss.str("");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
}

BOOST_DATA_TEST_CASE(ring_zn_input_output_test, bdata::xrange(10),
                     xr) {
     int const n = xr + 1;
     Ring_Zn const A(n);
     Element x;
     Element y(&A);
     std::stringstream ss;

     for (int i = 0; i < n; i++) {
          x = A.element(i);
          ss.clear();
          ss << x;
          BOOST_CHECK(!ss.fail());
          ss >> y;
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(x == y);
     }
}

BOOST_AUTO_TEST_CASE(ring_zn_input_fails_test) {
     std::stringstream ss;
     Ring_Zn const A(3);
     Element x(&A);

     ss.clear();
     ss.str("-1");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     ss.clear();
     ss.str("3");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     ss.clear();
     ss.str("xxx");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     ss.clear();
     ss.str("");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
}

BOOST_DATA_TEST_CASE(field_q_input_output_test,
                     bdata::xrange(41) * bdata::xrange(41), xr1,
                     xr2) {
     using SHG::ALGEBRA::Field_Q;
     int const i = xr1 - 20;
     int const j = xr2 - 20;

     if (j == 0) {
          BOOST_CHECK(true);
          return;
     }

     Field_Q Q;
     Element const x = Q.element(i, j);
     Element y(&Q);
     std::stringstream ss;

     ss << x;
     BOOST_CHECK(!ss.fail());

     if (i == 0)
          BOOST_CHECK(ss.str() == "0");
     if (i == 10 && j == 2)
          BOOST_CHECK(ss.str() == "5");
     if (i == -10 && j == 2)
          BOOST_CHECK(ss.str() == "-5");
     if (i == 10 && j == -2)
          BOOST_CHECK(ss.str() == "-5");
     if (i == -10 && j == -3)
          BOOST_CHECK(ss.str() == "10/3");
     if (i == -10 && j == 3)
          BOOST_CHECK(ss.str() == "-10/3");
     if (i == 10 && j == -3)
          BOOST_CHECK(ss.str() == "-10/3");
     if (i == 10 && j == 3)
          BOOST_CHECK(ss.str() == "10/3");

     ss >> y;
     BOOST_CHECK(!ss.fail());
     BOOST_CHECK(x == y);
}

/**
 * \todo Fix cases commented out.
 */
BOOST_AUTO_TEST_CASE(field_q_input_fails_test) {
     using SHG::ALGEBRA::Field_Q;
     std::stringstream ss;
     Field_Q Q;
     Element x(&Q);

     ss.clear();
     ss.str("xxx");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     ss.clear();
     ss.str("");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     /*
      * These two pass in boost_1_76_0 but not in boost_1_78_0.
      *
      * ss.clear();
      * ss.str("1/-2");
      * BOOST_CHECK(!ss.fail());
      * ss >> x;
      * BOOST_CHECK(ss.fail());
      * ss.clear();
      * ss.str("1/0");
      * BOOST_CHECK(!ss.fail());
      * ss >> x;
      * BOOST_CHECK(ss.fail());
      */
}

BOOST_AUTO_TEST_CASE(direct_product_input_output_test) {
     using SHG::ALGEBRA::Direct_product;
     Group_Sn S2(2);
     Group_Sn S3(3);
     Direct_product G({&S2, &S3});
     Element x(&G), y(&G);
     std::stringstream ss;
     Group_Sn::ET v2(2), v3(3);

     std::iota(v2.begin(), v2.end(), 0);
     do {
          Element const x2 = S2.element(v2);
          std::iota(v3.begin(), v3.end(), 0);
          do {
               Element const x3 = S3.element(v3);
               x = G.element({x2, x3});
               ss.clear();
               ss << x;
               BOOST_CHECK(!ss.fail());
               ss >> y;
               BOOST_CHECK(!ss.fail());
               BOOST_CHECK(x == y);
          } while (std::next_permutation(v3.begin(), v3.end()));
     } while (std::next_permutation(v2.begin(), v2.end()));

     ss.clear();
     ss.str("");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     ss.clear();
     ss.str("2 0 1 3 0 1 3");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
}

BOOST_AUTO_TEST_CASE(group_sn_reset_test) {
     Group_Sn Sn(3);

     BOOST_CHECK(Sn.n() == 3);
     Element const e1 = Sn.element({1, 0, 2});
     Element const e2 = Sn.element({2, 1, 0});
     BOOST_CHECK((Sn.value(e1 * e2) == Group_Sn::ET{2, 0, 1}));
     Sn.reset(4);
     BOOST_CHECK(Sn.n() == 4);
     // vector subscript out of range in line below:
     // auto const v = Sn.value(e1 * e2);
}

BOOST_AUTO_TEST_CASE(ring_zn_reset_test) {
     Ring_Zn Zn(8);

     BOOST_CHECK(Zn.n() == 8);
     Element const e1 = Zn.element(5);
     Element const e2 = Zn.element(7);
     BOOST_CHECK(Zn.value(e1 + e2) == 4);
     Zn.reset(6);
     BOOST_CHECK(Zn.n() == 6);
     BOOST_CHECK(Zn.value(e1 + e2) == 6);  // (5 + 7) - 6 = 6
}

BOOST_AUTO_TEST_CASE(direct_product_reset_test) {
     using SHG::ALGEBRA::Direct_product;
     using SHG::to_string;

     Ring_Zn const Z2(2);
     Ring_Zn const Z3(3);
     Group_Sn const S2(2);
     Group_Sn const S3(3);

     Direct_product G({&Z2, &Z3});
     Element const e1{G.element({Z2.element(0), Z3.element(0)})};
     Element const e2{G.element({Z2.element(1), Z3.element(2)})};
     BOOST_CHECK(to_string(e1) == "0 0");
     BOOST_CHECK(to_string(e2) == "1 2");
     BOOST_CHECK(to_string(e1 + e2) == "1 2");
     G.reset({&S2, &S3});
     BOOST_CHECK(to_string(e1) == "0 0");
     BOOST_CHECK(to_string(e2) == "1 2");
     // memory access violation at address: 0x0: no mapping at fault
     // address:

     // Element const e = e1 + e2;
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
