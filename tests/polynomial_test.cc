#include <shg/polynomial.h>
#include <shg/binom.h>
#include <shg/utils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(polynomial_test)

BOOST_AUTO_TEST_CASE(constructor_test) {
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Ring_Z;
     using SHG::ALGEBRA::Term;
     using SHG::ALGEBRA::grlex_cmp;
     using SHG::ALGEBRA::lex_cmp;
     using std::invalid_argument;
     Ring_Z const Z;
     {
          Polynomial p;
          BOOST_CHECK(p.dim() == 1);
          BOOST_CHECK(p.deg() == -1);
          BOOST_CHECK(p.is_zero());
          BOOST_CHECK(p.terms().empty());
          BOOST_CHECK(p.order() == lex_cmp);
          BOOST_CHECK_THROW(p.order(nullptr), invalid_argument);
          BOOST_CHECK(p.order() == lex_cmp);
          p.order(grlex_cmp);
          BOOST_CHECK(p.order() == grlex_cmp);
          p.set_to_zero();
          BOOST_CHECK(p.is_zero());
          Element const e;
          Term const t;
          Polynomial const y;
          BOOST_CHECK_THROW(p += e, invalid_argument);
          BOOST_CHECK_THROW(p -= e, invalid_argument);
          BOOST_CHECK_THROW(p *= e, invalid_argument);
          BOOST_CHECK_THROW(p += t, invalid_argument);
          BOOST_CHECK_THROW(p -= t, invalid_argument);
          BOOST_CHECK_THROW(p *= t, invalid_argument);
          p += y;
          BOOST_CHECK(p.is_zero());
          p -= y;
          BOOST_CHECK(p.is_zero());
          p *= y;
          BOOST_CHECK(p.is_zero());
     }
     {
          Polynomial p(&Z);
          BOOST_CHECK(p.dim() == 1);
          BOOST_CHECK(p.deg() == -1);
          BOOST_CHECK(p.is_zero());
          BOOST_CHECK(p.as() == &Z);
          BOOST_CHECK(p.terms().empty());
          BOOST_CHECK(p.order() == lex_cmp);
          BOOST_CHECK_THROW(p.order(nullptr), invalid_argument);
          BOOST_CHECK(p.order() == lex_cmp);
          p.order(grlex_cmp);
          BOOST_CHECK(p.order() == grlex_cmp);
          p.set_to_zero();
          BOOST_CHECK(p.is_zero());
          Element const e;
          Term const t;
          Polynomial const y;
          BOOST_CHECK_THROW(p += e, invalid_argument);
          BOOST_CHECK_THROW(p -= e, invalid_argument);
          BOOST_CHECK_THROW(p *= e, invalid_argument);
          BOOST_CHECK_THROW(p += t, invalid_argument);
          BOOST_CHECK_THROW(p -= t, invalid_argument);
          BOOST_CHECK_THROW(p *= t, invalid_argument);
          BOOST_CHECK_THROW(p += y, invalid_argument);
          BOOST_CHECK_THROW(p -= y, invalid_argument);
          BOOST_CHECK_THROW(p *= y, invalid_argument);
     }
     {
          Polynomial p(&Z, 2);
          BOOST_CHECK(p.dim() == 2);
          BOOST_CHECK(p.deg() == -1);
          BOOST_CHECK(p.is_zero());
          BOOST_CHECK(p.as() == &Z);
          BOOST_CHECK(p.terms().empty());
          BOOST_CHECK(p.order() == lex_cmp);
          BOOST_CHECK_THROW(p.order(nullptr), invalid_argument);
          BOOST_CHECK(p.order() == lex_cmp);
          p.order(grlex_cmp);
          BOOST_CHECK(p.order() == grlex_cmp);
          BOOST_CHECK_NO_THROW(p.set_to_zero());
          Element const e;
          Term const t;
          Polynomial const y;
          BOOST_CHECK_THROW(p += e, invalid_argument);
          BOOST_CHECK_THROW(p -= e, invalid_argument);
          BOOST_CHECK_THROW(p *= e, invalid_argument);
          BOOST_CHECK_THROW(p += t, invalid_argument);
          BOOST_CHECK_THROW(p -= t, invalid_argument);
          BOOST_CHECK_THROW(p *= t, invalid_argument);
          BOOST_CHECK_THROW(p += y, invalid_argument);
          BOOST_CHECK_THROW(p -= y, invalid_argument);
          BOOST_CHECK_THROW(p *= y, invalid_argument);
     }
}

BOOST_AUTO_TEST_CASE(operator_test) {
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Ring_Z;
     using SHG::ALGEBRA::Term;
     Ring_Z const Z;
     {
          // x^2yz + xy^2z + xyz^2
          Polynomial p(&Z, 3);
          p += {Z.element(1), Monomial{2, 1, 1}};
          p += {Z.element(1), Monomial{1, 2, 1}};
          p += {Z.element(1), Monomial{1, 1, 2}};
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.deg() == 4);
          BOOST_CHECK(p.terms().size() == 3);
          p -= {Z.element(1), Monomial{2, 1, 1}};
          p -= {Z.element(1), Monomial{1, 2, 1}};
          p -= {Z.element(1), Monomial{1, 1, 2}};
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.deg() == -1);
          BOOST_CHECK(p.terms().size() == 0);
     }
     {  // Multiply by a term.
          // x^2yz + 2xy^2z + 3xyz^2
          Polynomial p(&Z, 3);
          p += {Z.element(1), Monomial{2, 1, 1}};
          p += {Z.element(2), Monomial{1, 2, 1}};
          p += {Z.element(3), Monomial{1, 1, 2}};

          // 4x^3y^3z^4 + 8x^2y^4z^4 + 12x^2y^3z^5
          Polynomial q(&Z, 3);
          q += {Z.element(4), Monomial{3, 3, 4}};
          q += {Z.element(8), Monomial{2, 4, 4}};
          q += {Z.element(12), Monomial{2, 3, 5}};

          Term const t0{Z.element(0), Monomial{1, 2, 3}};
          Term const t1{Z.element(4), Monomial{1, 2, 3}};

          Polynomial r{p};
          r *= t1;
          BOOST_CHECK(r == q);
          r *= t0;
          BOOST_CHECK(r.is_zero());
          r = p * t1;
          BOOST_CHECK(r == q);
          r = p * t0;
          BOOST_CHECK(r.is_zero());
          r = t1 * p;
          BOOST_CHECK(r == q);
          r = t0 * p;
          BOOST_CHECK(r.is_zero());
     }
     {  // Multiply by a constant.
          // x^2yz + 2xy^2z + 3xyz^2
          Polynomial p0(&Z, 3);
          p0 += {Z.element(1), Monomial{2, 1, 1}};
          p0 += {Z.element(2), Monomial{1, 2, 1}};
          p0 += {Z.element(3), Monomial{1, 1, 2}};

          // 4x^2yz + 8xy^2z + 12xyz^2
          Polynomial q(&Z, 3);
          q += {Z.element(4), Monomial{2, 1, 1}};
          q += {Z.element(8), Monomial{1, 2, 1}};
          q += {Z.element(12), Monomial{1, 1, 2}};

          Polynomial p{p0};
          p *= Z.element(4);
          BOOST_CHECK(p == q);
          p *= Z.one();
          BOOST_CHECK(p == q);
          p *= Z.zero();
          BOOST_CHECK(p.is_zero());

          p = p0 * Z.element(4);
          BOOST_CHECK(p == q);
          p = Z.element(4) * p0;
          BOOST_CHECK(p == q);
     }
     {
          // (x^2y + yz) * (x + y) = x^3y + xyz + x^2y^2 + y^2z
          Polynomial p(&Z, 3), q(&Z, 3), r(&Z, 3), s(&Z, 3);
          p += {Z.element(1), Monomial{2, 1, 0}};
          p += {Z.element(1), Monomial{0, 1, 1}};
          q += {Z.element(1), Monomial{1, 0, 0}};
          q += {Z.element(1), Monomial{0, 1, 0}};
          r += {Z.element(1), Monomial{3, 1, 0}};
          r += {Z.element(1), Monomial{1, 1, 1}};
          r += {Z.element(1), Monomial{2, 2, 0}};
          r += {Z.element(1), Monomial{0, 2, 1}};

          s = p * q;
          BOOST_CHECK(s == r);
          s = q * p;
          BOOST_CHECK(s == r);
          s = p;
          s *= q;
          BOOST_CHECK(s == r);
          s = q;
          s *= p;
          BOOST_CHECK(s == r);
     }
}

/**
 * Returns \f$(x^2y + xy^2)^n\f$ for \f$n \geq 0\f$.
 * \implementation \f$ (x^2y + xy^2)^n = \sum_{k = 0}^n \binom{n}{k}
 * x^{n + k} y^{2n - k} \f$.
 */
SHG::ALGEBRA::Polynomial test_polynomial(
     SHG::ALGEBRA::Ring_Z const* Z, int n) {
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Polynomial;
     using SHG::binom;
     Polynomial p(Z, 2);
     for (int k = 0; k <= n; k++) {
          Element const e = Z->element(binom(n, k));
          p += {e, Monomial{n + k, 2 * n - k}};
     }
     return p;
}

/**
 * Returns \f$(ax^2yz + bxy^2z + cxyz^2)^n\f$ for \f$n \geq 0\f$.
 * \implementation \f[ (ax^2yz + bxy^2z + cxyz^2)^n = \sum_{k_1 = 0}^n
 * \binom{n}{k_1}a^{k_1} \sum_{k_2 = 0}^{n - k_1} \binom{n - k_1}{k_2}
 * b^{k_2}c^{n - k_1 - k_2} x^{n + k_1} y^{n + k_2} z^{2n - k_1 - k_2}
 * \f]
 */
SHG::ALGEBRA::Polynomial test_polynomial(
     SHG::ALGEBRA::Ring_Z const* Z, int a, int b, int c, int n) {
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Polynomial;
     using SHG::binom;
     Polynomial p(Z, 3);
     for (int k1 = 0; k1 <= n; k1++) {
          Element const bin1 = Z->element(binom(n, k1));
          Element const ak1 = pow(Z->element(a), k1);
          for (int k2 = 0; k2 <= n - k1; k2++) {
               Element const bin2 = Z->element(binom(n - k1, k2));
               Element const bk2 = pow(Z->element(b), k2);
               Element const ck3 = pow(Z->element(c), n - k1 - k2);
               Element const coeff = bin1 * ak1 * bin2 * bk2 * ck3;
               p += {coeff,
                     Monomial{n + k1, n + k2, 2 * n - k1 - k2}};
          }
     }
     return p;
}

BOOST_AUTO_TEST_CASE(polynomial_value_test) {
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Ring_Z;
     Ring_Z const Z;
     Polynomial p{test_polynomial(&Z, 1, 2, 3, 1)};

     // For x = t, y = 2t, z = 3t the polynomial has the value 84t^4.
     Element const e84 = Z.element(84);
     for (int t = 0; t < 10; t++) {
          Polynomial::Point const x{Z.element(t), Z.element(2 * t),
                                    Z.element(3 * t)};
          BOOST_CHECK(p(x) == e84 * pow(Z.element(t), 4));
     }
}

BOOST_AUTO_TEST_CASE(polynomial_input_output_test) {
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Ring_Z;
     Ring_Z const Z;
     Polynomial p(&Z, 3), q(&Z, 3);
     std::stringstream ss;
     for (int n = 1; n <= 10; n++) {
          p = test_polynomial(&Z, 1, 2, 3, n);
          ss.clear();
          ss << p;
          BOOST_CHECK(ss);
          ss >> q;
          BOOST_CHECK(ss);
          BOOST_CHECK(p == q);
     }
}

BOOST_AUTO_TEST_CASE(leading_monomial_test) {
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Ring_Z;
     using SHG::ALGEBRA::Term;
     using SHG::ALGEBRA::grevlex_cmp;
     using SHG::ALGEBRA::grlex_cmp;
     using SHG::ALGEBRA::lex_cmp;
     using std::invalid_argument;
     Ring_Z const Z;
     Polynomial p{&Z, 3};
     bool b;

     // \cite cox-little-oshea-2007, page 59.
     // 4*x*y^2*z+4*z^2+-5*x^3+7*x^2*z^2"
     p += {Z.element(4), Monomial{1, 2, 1}};
     p += {Z.element(4), Monomial{0, 0, 2}};
     p += {Z.element(-5), Monomial{3, 0, 0}};
     p += {Z.element(7), Monomial{2, 0, 2}};
     p.order(lex_cmp);
     b = p.leading_monomial() == Monomial{3, 0, 0};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == Z.element(-5);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{Z.element(-5), Monomial{3, 0, 0}};
     BOOST_CHECK(b);
     p.order(grlex_cmp);
     b = p.leading_monomial() == Monomial{2, 0, 2};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == Z.element(7);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{Z.element(7), Monomial{2, 0, 2}};
     BOOST_CHECK(b);
     p.order(grevlex_cmp);
     b = p.leading_monomial() == Monomial{1, 2, 1};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == Z.element(4);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{Z.element(4), Monomial{1, 2, 1}};
     BOOST_CHECK(b);

     p.set_to_zero();
     p += Z.element(5);
     p.order(lex_cmp);
     b = p.leading_monomial() == Monomial{0, 0, 0};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == Z.element(5);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{Z.element(5), Monomial{0, 0, 0}};
     BOOST_CHECK(b);
     p.order(grlex_cmp);
     b = p.leading_monomial() == Monomial{0, 0, 0};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == Z.element(5);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{Z.element(5), Monomial{0, 0, 0}};
     BOOST_CHECK(b);
     p.order(grevlex_cmp);
     b = p.leading_monomial() == Monomial{0, 0, 0};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == Z.element(5);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{Z.element(5), Monomial{0, 0, 0}};
     BOOST_CHECK(b);

     p.set_to_zero();
     p.order(lex_cmp);
     BOOST_CHECK_THROW(p.leading_monomial(), invalid_argument);
     BOOST_CHECK_THROW(p.leading_coefficient(), invalid_argument);
     BOOST_CHECK_THROW(p.leading_term(), invalid_argument);
     p.order(grlex_cmp);
     BOOST_CHECK_THROW(p.leading_monomial(), invalid_argument);
     BOOST_CHECK_THROW(p.leading_coefficient(), invalid_argument);
     BOOST_CHECK_THROW(p.leading_term(), invalid_argument);
     p.order(grevlex_cmp);
     BOOST_CHECK_THROW(p.leading_monomial(), invalid_argument);
     BOOST_CHECK_THROW(p.leading_coefficient(), invalid_argument);
     BOOST_CHECK_THROW(p.leading_term(), invalid_argument);

     Polynomial q;
     BOOST_CHECK_THROW(q.leading_monomial(), invalid_argument);
     BOOST_CHECK_THROW(q.leading_coefficient(), invalid_argument);
     BOOST_CHECK_THROW(q.leading_term(), invalid_argument);
}

BOOST_AUTO_TEST_CASE(polynomial1_over_Z_test) {
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Ring_Z;
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::one_var;
     using SHG::to_string;

     char const* const res[] = {
          "1 0",                          // 0
          "1 1 1 1 0",                    // 1
          "1 2 2 1 1 1 1 0",              // 1 + 2x
          "1 3 3 1 2 2 1 1 1 1 0",        // 1 + 2x +3x^2
          "1 4 4 1 3 3 1 2 2 1 1 1 1 0",  // 1 + 2x + 3x^2 + 4x^3
          "1 5 5 1 4 4 1 3 3 1 2 2 1 1 1 1 0",  // 1 + 2x + 3x^2 +
                                                // 4x^3 + 5x^4
     };
     Ring_Z const Z;
     std::vector<Element> a;

     BOOST_CHECK(to_string(Polynomial(&Z, 1)) == "1 0");
     BOOST_CHECK(to_string(one_var(a)) == "1 0");
     for (unsigned i = 0; i < std::size(res); i++) {
          a.resize(i);
          for (unsigned j = 0; j < i; j++)
               a[j] = Z.element(0);
          BOOST_CHECK(to_string(one_var(a)) == "1 0");
          a.resize(i + 1);
          for (unsigned j = 0; j < i; j++)
               a[j] = Z.element(j + 1);
          a[i] = Z.element(0);
          BOOST_CHECK(to_string(one_var(a)) == res[i]);
          a.resize(i + 2, Z.element(0));
          BOOST_CHECK(to_string(one_var(a)) == res[i]);
     }
}

BOOST_AUTO_TEST_CASE(polynomial1_over_Z_addition_test) {
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Ring_Z;
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::one_var;
     using SHG::to_string;

     Ring_Z const Z;
     std::vector<Element> a;

     {
          constexpr char const* const res{
               "1 3 5 1 2 6 1 1 4 1 0"};  // 4 + 6x + 5x^2
          a.resize(2);
          a[0] = Z.element(1);
          a[1] = Z.element(2);
          Polynomial const f(one_var(a));  // 1 + 2x
          a.resize(3);
          a[0] = Z.element(3);
          a[1] = Z.element(4);
          a[2] = Z.element(5);
          Polynomial const g(one_var(a));  // 3 + 4x + 5x^2
          BOOST_CHECK(to_string(f + g) == res);
          BOOST_CHECK(to_string(g + f) == res);
          BOOST_CHECK(f + g == g + f);
     }
     {
          constexpr char const* const res{
               "1 2 2 1 1 1 1 0"};  // 1 + 2x
          a.resize(2);
          a[0] = Z.element(1);
          a[1] = Z.element(2);
          Polynomial const f(one_var(a));  // 1 + 2x
          Polynomial const g(&Z);          // 0
          BOOST_CHECK(to_string(f + g) == res);
          BOOST_CHECK(to_string(g + f) == res);
          BOOST_CHECK(f + g == g + f);
          BOOST_CHECK(f + g == f);
          BOOST_CHECK(g + f == f);
     }
     {
          a.resize(2);
          a[0] = Z.element(1);
          a[1] = Z.element(2);
          Polynomial const f(one_var(a));  // 1 + 2x
          a[0] = Z.element(-1);
          a[1] = Z.element(-2);
          Polynomial const g(one_var(a));  // -1 - 2x
          BOOST_CHECK(to_string(f + g) == "1 0");
          BOOST_CHECK(to_string(g + f) == "1 0");
          BOOST_CHECK(f + g == g + f);
          BOOST_CHECK(-f == g);
          BOOST_CHECK(f == -g);
     }
}

BOOST_AUTO_TEST_CASE(polynomial1_over_Z_multiplication_test) {
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Ring_Z;
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::one_var;
     using SHG::to_string;

     Ring_Z const Z;
     std::vector<Element> a;

     {
          constexpr char const* const res{
               "1 4 10 1 3 13 1 2 10 1 1 3 1 0"};  // 3 + 10x + 13x^2
                                                   // + 10x^3
          a.resize(2);
          a[0] = Z.element(1);
          a[1] = Z.element(2);
          Polynomial const f(one_var(a));  // 1 + 2x
          a.resize(3);
          a[0] = Z.element(3);
          a[1] = Z.element(4);
          a[2] = Z.element(5);
          Polynomial const g(one_var(a));  // 3 + 4x + 5x^2
          BOOST_CHECK(to_string(f * g) == res);
          BOOST_CHECK(to_string(g * f) == res);
          BOOST_CHECK(f * g == g * f);
     }
     {
          constexpr char const* const res{"1 0"};  // 0
          a.resize(2);
          a[0] = Z.element(1);
          a[1] = Z.element(2);
          Polynomial const f(one_var(a));  // 1 + 2x
          Polynomial const g(&Z);          // 0
          BOOST_CHECK(to_string(f * g) == res);
          BOOST_CHECK(to_string(g * f) == res);
          BOOST_CHECK(f * g == g * f);
     }
}

BOOST_AUTO_TEST_CASE(polynomial1_over_Z_value_operator_test) {
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Ring_Z;
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::one_var;

     Ring_Z const Z;
     std::vector<Element> const a{Z.element(1), Z.element(2),
                                  Z.element(3)};

     Polynomial const f(one_var(a));  // 1 + 2x + 3x^2
     BOOST_CHECK(f(Z.element(0)) == Z.element(1));
     BOOST_CHECK(f(Z.element(4)) == Z.element(57));
     Polynomial const g(&Z);  // 0
     BOOST_CHECK(g(Z.element(0)) == Z.element(0));
     BOOST_CHECK(g(Z.element(4)) == Z.element(0));
}

/// \cite rutkowski-2012, exercise 578, page 144.
BOOST_AUTO_TEST_CASE(polynomial1_over_Z6_test) {
     using SHG::ALGEBRA::Ring_Zn;
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Polynomial;
     using SHG::to_string;

     Ring_Zn const Z6(6);
     std::vector<Element> a;
     a.resize(2);
     a[0] = Z6.element(1);
     a[1] = Z6.element(2);
     Polynomial const f(one_var(a));  // 1 + 2x
     a[1] = Z6.element(3);
     Polynomial const g(one_var(a));                      // 1 + 3x
     BOOST_CHECK(to_string(f * g) == "1 2 5 1 1 1 1 0");  // 1 + 5x
}

/// \cite sharp-2000, exercise 1.18, page 10.
BOOST_AUTO_TEST_CASE(Z7_test) {
     using SHG::ALGEBRA::Ring_Zn;
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::one_var;

     Ring_Zn const Z7(7);
     std::vector<Element> a;
     a.resize(8);
     for (int i = 0; i < 8; i++)
          a[i] = Z7.element(0);
     a[1] = Z7.element(6);
     a[7] = Z7.element(1);
     Polynomial const f(one_var(a));  // x^7 - x
     for (int i = 0; i < 7; i++)
          BOOST_CHECK(f(Z7.element(i)) == Z7.element(0));
}

/// \cite rutkowski-2012, example 75, page 143.
BOOST_AUTO_TEST_CASE(Z8_test) {
     using SHG::ALGEBRA::Ring_Zn;
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::one_var;
     using SHG::to_string;

     constexpr char const* const res[] = {
          "1 3 1 1 3 1 1 2 2 1 1",        // 2x + x^2 + x^3
          "1 4 7 1 3 3 1 2 2 1 1 2 1 0",  // 2 + 2x + 3x^2 + 7x^3
          "1 6 2 1 5 4 1 4 7 1 3 1 1 2 6 1 1 7 1 0",  // 7 + 6x + x^2
                                                      // + 7x^3 + 4x^4
                                                      // + 2x^5"
     };

     Ring_Zn const Z8(8);
     std::vector<Element> a;
     a.resize(3);
     a[0] = Z8.element(5);
     a[1] = Z8.element(6);
     a[2] = Z8.element(2);
     Polynomial const f(one_var(a));  // 5 + 6x + 2x^2
     a.resize(4);
     a[0] = Z8.element(3);
     a[1] = Z8.element(4);
     a[2] = Z8.element(7);
     a[3] = Z8.element(1);
     Polynomial const g(one_var(a));  // 3 + 4x + 7x^2 + x^3
     BOOST_CHECK(to_string(f + g) == res[0]);
     BOOST_CHECK(to_string(f - g) == res[1]);
     BOOST_CHECK(to_string(f * g) == res[2]);
}

BOOST_AUTO_TEST_CASE(polynomial_ring_test) {
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Field_Q;
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::Polynomial_ring;
     using SHG::ALGEBRA::is_commutative_ring;
     using SHG::to_string;

     Field_Q Q;
     Polynomial_ring ring(&Q);
     BOOST_CHECK(is_commutative_ring(&ring));

     BOOST_CHECK(ring.as() == &Q);
     BOOST_CHECK(ring.dim() == 1);
     Polynomial p(ring.as(), ring.dim());
     p += {Q.element(1), Monomial{3}};
     p += Q.element(-1);
     BOOST_CHECK(to_string(p) == "1 2 1 1 3 -1 1 0");
     Element x = ring.element(p);
     BOOST_CHECK(ring.value(x) == p);
     Element y{x};
     BOOST_CHECK(y == x);
     Element z = pow(x, 5);
     BOOST_CHECK(to_string(z) ==
                 "1 6 1 1 15 -5 1 12 10 1 9 -10 1 6 5 1 3 -1 1 0");
}

BOOST_AUTO_TEST_CASE(polynomial_ring_reset_test) {
     using SHG::ALGEBRA::Field_Q;
     using SHG::ALGEBRA::Ring_Z;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::one_var;
     using SHG::ALGEBRA::Polynomial_ring;
     using SHG::ALGEBRA::Element;
     using SHG::to_string;

     Field_Q const Q;
     Ring_Z const Z;
     std::vector<Element> a;
     Polynomial_ring R(&Q, 1);
     BOOST_CHECK(R.as() == &Q);
     BOOST_CHECK(R.dim() == 1);
     a.resize(2);
     a[0] = Q.element(1, 2);
     a[1] = Q.element(1, 4);
     Polynomial const p1 = one_var(a);
     BOOST_CHECK(to_string(p1) == "1 2 1/4 1 1 1/2 1 0");
     R.reset(&Z, 1);
     BOOST_CHECK(R.as() == &Z);
     BOOST_CHECK(R.dim() == 1);
     a[0] = Z.element(1);
     a[1] = Z.element(2);
     Polynomial const p2 = one_var(a);
     BOOST_CHECK(to_string(p1) == "1 2 1/4 1 1 1/2 1 0");
     BOOST_CHECK(to_string(p2) == "1 2 2 1 1 1 1 0");
     BOOST_CHECK_THROW(p1 + p2, std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
