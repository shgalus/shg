#include <shg/algebra.h>
#include <sstream>
#include <shg/utils.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(algebra_test)

namespace bdata = boost::unit_test::data;
using namespace SHG::ALGEBRA;
using std::invalid_argument;

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
          const Ring_Z A;
          Element x(&A);
          BOOST_CHECK(x.is_valid());
          BOOST_CHECK(x.as() == &A);
          BOOST_CHECK(x.value().has_value());
          BOOST_CHECK(is_zero(x));
     }
     {
          const Ring_Z A;
          const Ring_Z::ET a{5};
          Element x(&A, a);
          BOOST_CHECK(x.is_valid());
          BOOST_CHECK(x.as() == &A);
          BOOST_CHECK(x.value().has_value());
     }
     {
          const Ring_Z A;
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

BOOST_AUTO_TEST_CASE(finite_strings_semigroup_test) {
     {
          Finite_strings S;
          BOOST_CHECK(S.is_abelian());
          const Element one = S.one();
          BOOST_CHECK(is_one(S.element("")));
          BOOST_CHECK(S.value(one) == "");
          BOOST_CHECK(one * one == one);
          BOOST_CHECK(one + one == one);
     }

     Finite_strings S{{'a', 'b', 'c'}};

     BOOST_CHECK(!S.is_abelian());
     const Element one = S.one();
     const Element zero = S.zero();
     BOOST_CHECK(is_one(one));
     BOOST_CHECK(is_zero(one));
     BOOST_CHECK(is_one(zero));
     BOOST_CHECK(is_zero(zero));

     BOOST_CHECK(S.element("") == one);
     BOOST_CHECK(S.value(one) == "");

     const Element x = S.element("aabbaa");
     BOOST_CHECK(S.value(x) == "aabbaa");
     const Element y = S.element("ccbbcc");
     BOOST_CHECK(S.value(y) == "ccbbcc");
     BOOST_CHECK(x == x);
     BOOST_CHECK(x != y);
     Element z = x * y;
     BOOST_CHECK(S.value(z) == "aabbaaccbbcc");
     BOOST_CHECK(x + y == z);
     z = y * x;
     BOOST_CHECK(S.value(z) == "ccbbccaabbaa");
     BOOST_CHECK(y + x == z);
     z = one * x;
     BOOST_CHECK(z == x);
     BOOST_CHECK(one + x == z);
     z = x * one;
     BOOST_CHECK(z == x);
     BOOST_CHECK(x + one == z);
     z = one * one;
     BOOST_CHECK(z == one);
     BOOST_CHECK(one + one == z);
     BOOST_CHECK_THROW(S.element("aabbad"), invalid_argument);
     BOOST_CHECK_THROW(-x, invalid_argument);
     BOOST_CHECK_THROW(inv(x), invalid_argument);
}

BOOST_DATA_TEST_CASE(group_s_n_test, bdata::xrange(5), xr) {
     int const n = xr + 1;
     Group_Sn const G(n);

     BOOST_CHECK(G.is_abelian() == (n < 3));
     const Element one = G.one();

     Group_Sn::ET v(n);
     std::iota(v.begin(), v.end(), 0);
     BOOST_CHECK(G.element(v) == one);
     do {
          const Element x = G.element(v);
          BOOST_CHECK(G.value(x) == v);
          BOOST_CHECK(one * x == x);
          BOOST_CHECK(x * one == x);
          const Element y = inv(x);
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
     // clang-format off
     const Matrix<int> g0(2, 2, {
               0, 1,
               1, 0});
     // The smallest non-commutative group.
     const Matrix<int> g1(6, 6, {
               0, 1, 2, 3, 4, 5,
               1, 5, 3, 4, 2, 0,
               2, 4, 0, 5, 1, 3,
               3, 2, 1, 0, 5, 4,
               4, 3, 5, 1, 0, 2,
               5, 0, 4, 2, 3, 1});

     // \cite burn-1978, page 278
     const Matrix<int> burn1(6, 6, {
               0, 1, 2, 3, 4, 5,
               1, 2, 0, 4, 5, 3,
               2, 0, 1, 5, 3, 4,
               3, 4, 5, 0, 1, 2,
               4, 5, 3, 2, 0, 1,
               5, 3, 4, 1, 2, 0});

     // \cite burn-1978, page 280
     const Matrix<int> burn2(5, 5, {
               0, 1, 2, 3, 4,
               1, 0, 3, 4, 2,
               2, 4, 0, 1, 3,
               3, 2, 4, 0, 1,
               4, 3, 1, 2, 0});
     // clang-format on

     {
          const Finite_group G;
          BOOST_CHECK(G.is_abelian());
          BOOST_CHECK(G.order() == 1);
          BOOST_CHECK(G.value(G.one()) == 0);
          BOOST_CHECK(inv(G.element(0)) == G.element(0));
          BOOST_CHECK(G.element(0) * G.element(0) == G.element(0));
     }
     {
          const Finite_group G({2, 2, {0, 1, 1, 0}});
          BOOST_CHECK(G.is_abelian());
          BOOST_CHECK(G.order() == 2);
          BOOST_CHECK(G.value(G.one()) == 0);
          BOOST_CHECK(inv(G.element(1)) == G.element(1));
          BOOST_CHECK(G.element(1) * G.element(1) == G.element(0));
     }

     Finite_group G(g1);
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
     const Ring_Z Z;

     BOOST_CHECK(Z.is_abelian());
     const Element zero = Z.zero();
     BOOST_CHECK(Z.value(zero) == 0);
     const Element one = Z.one();
     BOOST_CHECK(Z.value(one) == 1);

     const int n = 20;
     for (int i = -n; i <= n; i++) {
          const Element x = Z.element(i);
          BOOST_CHECK(Z.value(x) == i);
          const Element y = -x;
          BOOST_CHECK(-x == y);
          BOOST_CHECK(x + y == zero);
          BOOST_CHECK(y + x == zero);
          BOOST_CHECK(x + zero == x);
          BOOST_CHECK(zero + x == x);
          for (int j = -n; j <= n; j++) {
               const Element y = Z.element(j);
               BOOST_CHECK(Z.value(x + y) == i + j);
               BOOST_CHECK(Z.value(y + x) == i + j);
               BOOST_CHECK(Z.value(x * y) == i * j);
               BOOST_CHECK(Z.value(y * x) == i * j);
          }
     }
}

BOOST_DATA_TEST_CASE(ring_zn_test, bdata::xrange(10), xr) {
     const int n = xr + 1;
     const Ring_Zn A(n);

     BOOST_CHECK(A.is_abelian());
     const Element zero = A.zero();
     BOOST_CHECK(A.value(zero) == 0);
     const Element one = A.one();
     if (n == 1)
          BOOST_CHECK(A.value(one) == 0);
     else
          BOOST_CHECK(A.value(one) == 1);

     for (int i = 0; i < n; i++) {
          const Element x = A.element(i);
          BOOST_CHECK(A.value(x) == i);
          const Element y = -x;
          BOOST_CHECK(-x == y);
          BOOST_CHECK(x + y == zero);
          BOOST_CHECK(y + x == zero);
          BOOST_CHECK(x + zero == x);
          BOOST_CHECK(zero + x == x);
          for (int j = 0; j < n; j++) {
               const Element y = A.element(j);
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
     const Field_Q F;

     BOOST_CHECK(F.is_abelian());
     const Element zero = F.zero();
     BOOST_CHECK(F.value(zero) == 0);
     const Element one = F.one();
     BOOST_CHECK(F.value(one) == 1);

     BOOST_CHECK(is_zero(zero));
     BOOST_CHECK(!is_one(zero));
     BOOST_CHECK(is_one(one));
     BOOST_CHECK(!is_zero(one));

     const int n = 20;
     for (int i = -n; i <= n; i++) {
          const Element x = F.element(i);
          BOOST_CHECK(F.value(x) == i);
          for (int j = -n; j <= n; j++)
               if (j != 0) {
                    const Element x = F.element(i, j);
                    const Field_Q::ET v = j < 0 ? Field_Q::ET(-i, -j)
                                                : Field_Q::ET(i, j);
                    BOOST_CHECK(F.value(x) == v);
                    const Element y = -x;
                    BOOST_CHECK(-x == y);
                    BOOST_CHECK(x + y == zero);
                    BOOST_CHECK(y + x == zero);
                    BOOST_CHECK(x + zero == x);
                    BOOST_CHECK(zero + x == x);
                    BOOST_CHECK(x * one == x);
                    BOOST_CHECK(one * x == x);
                    if (i != 0) {
                         BOOST_CHECK(!is_zero(x));
                         const Element y = inv(x);
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

BOOST_DATA_TEST_CASE(field_f_p_test, bdata::xrange(10), xr) {
     const int p = xr + 2;

     if (!is_prime(p)) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
#endif

          BOOST_CHECK_THROW((Field_Fp(p)), invalid_argument);

#ifdef __clang__
#pragma clang diagnostic pop
#endif

          return;
     }

     const Field_Fp F(p);

     BOOST_CHECK(F.is_abelian());
     const Element zero = F.zero();
     BOOST_CHECK(F.value(zero) == 0);
     const Element one = F.one();
     BOOST_CHECK(F.value(one) == 1);

     BOOST_CHECK(is_zero(zero));
     BOOST_CHECK(!is_one(zero));
     BOOST_CHECK(is_one(one));
     BOOST_CHECK(!is_zero(one));

     for (int i = 0; i < p; i++) {
          const Element x = F.element(i);
          BOOST_CHECK(F.value(x) == i);
          const Element y = -x;
          BOOST_CHECK(-x == y);
          BOOST_CHECK(x + y == zero);
          BOOST_CHECK(y + x == zero);
          BOOST_CHECK(x + zero == x);
          BOOST_CHECK(zero + x == x);
          BOOST_CHECK(x * one == x);
          BOOST_CHECK(one * x == x);
          if (i != 0) {
               const Element y = inv(x);
               BOOST_CHECK(inv(x) == y);
               BOOST_CHECK(x * y == one);
               BOOST_CHECK(y * x == one);

          } else {
               BOOST_CHECK_THROW(inv(x), invalid_argument);
          }
          for (int j = 0; j < p; j++) {
               const Element y = F.element(j);
               BOOST_CHECK(F.value(x + y) == (i + j) % p);
               BOOST_CHECK(F.value(y + x) == (i + j) % p);
               BOOST_CHECK(F.value(x * y) == (i * j) % p);
               BOOST_CHECK(F.value(y * x) == (i * j) % p);
          }
     }
     BOOST_CHECK_THROW(F.element(-1), invalid_argument);
     BOOST_CHECK_THROW(F.element(p), invalid_argument);
}

BOOST_AUTO_TEST_CASE(direct_product_of_groups_test) {
     Group_Sn Sn(2);
     Direct_product_of_groups G({&Sn, &Sn});
     BOOST_CHECK(G.is_abelian());
     BOOST_CHECK(G.separator() == ";");
     G.separator("\n\n");
     BOOST_CHECK(G.separator() == "\n\n");
     G.separator(";");

     const Element e0 = Sn.element({0, 1});
     const Element e1 = Sn.element({1, 0});

     const Direct_product_of_groups::ET ve{e0, e0};
     const Direct_product_of_groups::ET va{e0, e1};
     const Direct_product_of_groups::ET vb{e1, e0};
     const Direct_product_of_groups::ET vc{e1, e1};

     const Element e = G.element(ve);
     const Element a = G.element(va);
     const Element b = G.element(vb);
     const Element c = G.element(vc);

     BOOST_CHECK(G.one() == e);
     BOOST_CHECK(is_one(e));
     BOOST_CHECK(is_zero(e));
     BOOST_CHECK(G.value(e) == ve);
     BOOST_CHECK(G.value(a) == va);
     BOOST_CHECK(G.value(b) == vb);
     BOOST_CHECK(G.value(c) == vc);

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

BOOST_AUTO_TEST_CASE(finite_strings_input_output_test) {
     Finite_strings S{{'a', 'b', 'c'}};
     Element x(&S), y(&S);
     std::stringstream ss;

     x = S.element("");
     ss.clear();
     ss << x;
     BOOST_CHECK(!ss.fail());
     ss >> y;
     BOOST_CHECK(!ss.fail());
     BOOST_CHECK(x == y);

     x = S.element("aabbaa");
     ss.clear();
     ss << x;
     BOOST_CHECK(!ss.fail());
     ss >> y;
     BOOST_CHECK(!ss.fail());
     BOOST_CHECK(x == y);

     ss.clear();
     ss.str("abd");
     BOOST_CHECK(!ss.fail());
     ss >> x;
     BOOST_CHECK(ss.fail());
     BOOST_CHECK(x == y);
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
     const int i = xr - 20;
     const Ring_Z Z;
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
     const int n = xr + 1;
     const Ring_Zn A(n);
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
     const Ring_Zn A(3);
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

BOOST_DATA_TEST_CASE(field_fp_input_output_test, bdata::xrange(10),
                     xr) {
     const int p = xr + 2;
     if (!is_prime(p)) {
          BOOST_CHECK(true);
          return;
     }
     const Field_Fp F(p);
     Element x;
     Element y(&F);
     std::stringstream ss;

     for (int i = 0; i < p; i++) {
          x = F.element(i);
          ss.clear();
          ss << x;
          BOOST_CHECK(!ss.fail());
          ss >> y;
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(x == y);
     }
}

BOOST_AUTO_TEST_CASE(field_fp_input_fails_test) {
     std::stringstream ss;
     const Field_Fp F(3);
     Element x(&F);

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

BOOST_AUTO_TEST_CASE(direct_product_of_groups_input_output_test) {
     Group_Sn S2(2);
     Group_Sn S3(3);
     Direct_product_of_groups G({&S2, &S3});
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

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
