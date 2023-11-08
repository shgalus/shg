#include <shg/polynomial.h>
#include <shg/utils.h>
#include <shg/binom.h>
#include "buchbdat.h"
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(polynomial_test)

using SHG::ALGEBRA::Term;
using SHG::ALGEBRA::Polynomial;
using SHG::ALGEBRA::from_chars;
using SHG::ALGEBRA::Polynomial_div;
using SHG::ALGEBRA::Buchberger_improved;
using SHG::ALGEBRA::Monomial;
using SHG::ALGEBRA::Monomial_cmp;
using SHG::ALGEBRA::lex_cmp;
using SHG::ALGEBRA::grlex_cmp;
using SHG::ALGEBRA::grevlex_cmp;
using SHG::ALGEBRA::Field_Q;
using SHG::ALGEBRA::Field_Fp;
using SHG::ALGEBRA::Element;
using std::invalid_argument;

BOOST_AUTO_TEST_CASE(term_test) {
     std::stringstream ss;
     Field_Q Q;
     Element z = Q.element(-3, 4);
     Term y(&Q);

     Term x{z, Monomial{1, 2, 1}};
     ss << x;
     ss >> y;
     BOOST_CHECK(x == y);
}

BOOST_AUTO_TEST_CASE(constructor_test) {
     Field_Q const F;
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
          Polynomial p(&F);
          BOOST_CHECK(p.dim() == 1);
          BOOST_CHECK(p.deg() == -1);
          BOOST_CHECK(p.is_zero());
          BOOST_CHECK(p.field() == &F);
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
          Polynomial p(&F, 2);
          BOOST_CHECK(p.dim() == 2);
          BOOST_CHECK(p.deg() == -1);
          BOOST_CHECK(p.is_zero());
          BOOST_CHECK(p.field() == &F);
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
     Field_Q const F;
     {
          // x^2yz + xy^2z + xyz^2
          Polynomial p(&F, 3);
          p += {F.element(1), Monomial{2, 1, 1}};
          p += {F.element(1), Monomial{1, 2, 1}};
          p += {F.element(1), Monomial{1, 1, 2}};
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.deg() == 4);
          BOOST_CHECK(p.terms().size() == 3);
          p -= {F.element(1), Monomial{2, 1, 1}};
          p -= {F.element(1), Monomial{1, 2, 1}};
          p -= {F.element(1), Monomial{1, 1, 2}};
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.deg() == -1);
          BOOST_CHECK(p.terms().size() == 0);
     }
     {  // Multiply by a term.
          // x^2yz + 2xy^2z + 3xyz^2
          Polynomial p(&F, 3);
          p += {F.element(1), Monomial{2, 1, 1}};
          p += {F.element(2), Monomial{1, 2, 1}};
          p += {F.element(3), Monomial{1, 1, 2}};

          // 4x^3y^3z^4 + 8x^2y^4z^4 + 12x^2y^3z^5
          Polynomial q(&F, 3);
          q += {F.element(4), Monomial{3, 3, 4}};
          q += {F.element(8), Monomial{2, 4, 4}};
          q += {F.element(12), Monomial{2, 3, 5}};

          Term const t0{F.element(0), Monomial{1, 2, 3}};
          Term const t1{F.element(4), Monomial{1, 2, 3}};

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
          Polynomial p0(&F, 3);
          p0 += {F.element(1), Monomial{2, 1, 1}};
          p0 += {F.element(2), Monomial{1, 2, 1}};
          p0 += {F.element(3), Monomial{1, 1, 2}};

          // 4x^2yz + 8xy^2z + 12xyz^2
          Polynomial q(&F, 3);
          q += {F.element(4), Monomial{2, 1, 1}};
          q += {F.element(8), Monomial{1, 2, 1}};
          q += {F.element(12), Monomial{1, 1, 2}};

          Polynomial p{p0};
          p *= F.element(4);
          BOOST_CHECK(p == q);
          p *= F.one();
          BOOST_CHECK(p == q);
          p *= F.zero();
          BOOST_CHECK(p.is_zero());

          p = p0 * F.element(4);
          BOOST_CHECK(p == q);
          p = F.element(4) * p0;
          BOOST_CHECK(p == q);
     }
     {
          // (x^2y + yz) * (x + y) = x^3y + xyz + x^2y^2 + y^2z
          Polynomial p(&F, 3), q(&F, 3), r(&F, 3), s(&F, 3);
          p += {F.element(1), Monomial{2, 1, 0}};
          p += {F.element(1), Monomial{0, 1, 1}};
          q += {F.element(1), Monomial{1, 0, 0}};
          q += {F.element(1), Monomial{0, 1, 0}};
          r += {F.element(1), Monomial{3, 1, 0}};
          r += {F.element(1), Monomial{1, 1, 1}};
          r += {F.element(1), Monomial{2, 2, 0}};
          r += {F.element(1), Monomial{0, 2, 1}};

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
Polynomial test_polynomial(Field_Q const* F, int n) {
     using SHG::binom;
     Polynomial p(F, 2);
     for (int k = 0; k <= n; k++) {
          Element const e = F->element(binom(n, k));
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
Polynomial test_polynomial(Field_Q const* F, int a, int b, int c,
                           int n) {
     using SHG::binom;
     Polynomial p(F, 3);
     for (int k1 = 0; k1 <= n; k1++) {
          Element const bin1 = F->element(binom(n, k1));
          Element const ak1 = pow(F->element(a), k1);
          for (int k2 = 0; k2 <= n - k1; k2++) {
               Element const bin2 = F->element(binom(n - k1, k2));
               Element const bk2 = pow(F->element(b), k2);
               Element const ck3 = pow(F->element(c), n - k1 - k2);
               Element const coeff = bin1 * ak1 * bin2 * bk2 * ck3;
               p += {coeff,
                     Monomial{n + k1, n + k2, 2 * n - k1 - k2}};
          }
     }
     return p;
}

BOOST_AUTO_TEST_CASE(polynomial_value_test) {
     Field_Q const F;
     Polynomial p{test_polynomial(&F, 1, 2, 3, 1)};

     // For x = t, y = 2t, z = 3t the polynomial has the value 84t^4.
     Element const e84 = F.element(84);
     for (int t = 0; t < 10; t++) {
          Polynomial::Point const x{F.element(t), F.element(2 * t),
                                    F.element(3 * t)};
          BOOST_CHECK(p(x) == e84 * pow(F.element(t), 4));
     }
}

BOOST_AUTO_TEST_CASE(polynomial_input_output_test) {
     Field_Q const F;
     Polynomial p(&F, 3), q(&F, 3);
     std::stringstream ss;
     for (int n = 1; n <= 10; n++) {
          p = test_polynomial(&F, 1, 2, 3, n);
          ss.clear();
          ss << p;
          BOOST_CHECK(ss);
          ss >> q;
          BOOST_CHECK(ss);
          BOOST_CHECK(p == q);
     }
}

BOOST_AUTO_TEST_CASE(leading_monomial_test) {
     Field_Q const F;
     Polynomial p{&F, 3};
     bool b;

     // \cite cox-little-oshea-2007, page 59.
     // 4*x*y^2*z+4*z^2+-5*x^3+7*x^2*z^2"
     p += {F.element(4), Monomial{1, 2, 1}};
     p += {F.element(4), Monomial{0, 0, 2}};
     p += {F.element(-5), Monomial{3, 0, 0}};
     p += {F.element(7), Monomial{2, 0, 2}};
     p.order(lex_cmp);
     b = p.leading_monomial() == Monomial{3, 0, 0};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == F.element(-5);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{F.element(-5), Monomial{3, 0, 0}};
     BOOST_CHECK(b);
     p.order(grlex_cmp);
     b = p.leading_monomial() == Monomial{2, 0, 2};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == F.element(7);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{F.element(7), Monomial{2, 0, 2}};
     BOOST_CHECK(b);
     p.order(grevlex_cmp);
     b = p.leading_monomial() == Monomial{1, 2, 1};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == F.element(4);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{F.element(4), Monomial{1, 2, 1}};
     BOOST_CHECK(b);

     p.set_to_zero();
     p += F.element(5);
     p.order(lex_cmp);
     b = p.leading_monomial() == Monomial{0, 0, 0};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == F.element(5);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{F.element(5), Monomial{0, 0, 0}};
     BOOST_CHECK(b);
     p.order(grlex_cmp);
     b = p.leading_monomial() == Monomial{0, 0, 0};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == F.element(5);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{F.element(5), Monomial{0, 0, 0}};
     BOOST_CHECK(b);
     p.order(grevlex_cmp);
     b = p.leading_monomial() == Monomial{0, 0, 0};
     BOOST_CHECK(b);
     b = p.leading_coefficient() == F.element(5);
     BOOST_CHECK(b);
     b = p.leading_term() == Term{F.element(5), Monomial{0, 0, 0}};
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

/**
 * Test data and expected results for basic test of division of
 * polynomials. In each case, \f$ f = \sum_{i = 1}^n a_ig_i + r \f$.
 */
struct {
     bool const should_throw;
     char const* f;
     std::vector<const char*> const g;
     char const* r;
     std::vector<const char*> const a;
} const pdtcase[] = {
     // \cite cox-little-oshea-2007, page 61-62.
     {false,
      "2 2 1/1 2 1 2 1/1 2 0 0",  // xy^2 + 1
      {
           "2 2 1/1 2 1 1 1/1 2 0 0",  // xy + 1
           "2 2 1/1 2 0 1 1/1 2 0 0"   // y + 1
      },
      "2 1 2/1 2 0 0",  // 2
      {
           "2 1 1/1 2 0 1",  // y
           "2 1 -1/1 2 0 0"  // -1
      }},
     // \cite cox-little-oshea-2007, page 62-64.
     {false,
      "2 3 1/1 2 2 1 1/1 2 1 2 1/1 2 0 2",  // x^2y + xy^2 + y^2
      {
           "2 2 1/1 2 1 1 -1/1 2 0 0",  // xy - 1
           "2 2 1/1 2 0 2 -1/1 2 0 0",  // y^2 - 1
      },
      "2 3 1/1 2 1 0 1/1 2 0 1 1/1 2 0 0",  // x + y + 1
      {
           "2 2 1/1 2 1 0 1/1 2 0 1",  // x + y
           "2 1 1/1 2 0 0",            // 1
      }},
     // \cite cox-little-oshea-2007, page 66.
     {false,
      "2 3 1/1 2 2 1 1/1 2 1 2 1/1 2 0 2",  // x^2y + xy^2 + y^2
      {
           "2 2 1/1 2 0 2 -1/1 2 0 0",  // y^2 - 1
           "2 2 1/1 2 1 1 -1/1 2 0 0",  // xy - 1
      },
      "2 2 2/1 2 1 0 1/1 2 0 0",  // 2x + 1
      {
           "2 2 1/1 2 1 0 1/1 2 0 0",  // x + 1
           "2 1 1/1 2 1 0"             // x
      }},
     // \cite cox-little-oshea-2007, page 67.
     {false,
      "2 2 1/1 2 1 2 -1/1 2 1 0",  // xy^2 - x
      {
           "2 2 1/1 2 1 1 1/1 2 0 0",   // xy + 1
           "2 2 1/1 2 0 2 -1/1 2 0 0",  // y^2 - 1
      },
      "2 2 -1/1 2 1 0 -1/1 2 0 1",  // -x - y
      {
           "2 1 1/1 2 0 1",  // y
           "2 0"             // 0
      }},
     // \cite cox-little-oshea-2007, page 67.
     {false,
      "2 2 1/1 2 1 2 -1/1 2 1 0",  // xy^2 - x
      {
           "2 2 1/1 2 0 2 -1/1 2 0 0",  // y^2 - 1
           "2 2 1/1 2 1 1 1/1 2 0 0",   // xy + 1
      },
      "2 0",  // 0
      {
           "2 1 1/1 2 1 0",  // x
           "2 0"             // 0
      }},
     // \cite cox-little-oshea-2007, ex. 1a, page 68.
     {false,
      "2 4 1/1 2 7 2 1/1 2 3 2 -1/1 2 0 1 1/1 2 0 0",  // x^7y^2
                                                       // +
                                                       // x^3y^2
                                                       // - y + 1
      {
           "2 2 1/1 2 1 2 -1/1 2 1 0",  // xy^2 - x
           "2 2 1/1 2 1 0 -1/1 2 0 3",  // x - y^3
      },
      "2 3 2/1 2 0 3 -1/1 2 0 1 1/1 2 0 0",  // 2/1 x1^0 x2^3 +
                                             // -1/1 x1^0 x2^1 +
                                             // 1/1 x1^0 x2^0
      {
           "2 10 1/1 2 6 0 1/1 2 5 1 1/1 2 4 2 1/1 2 4 0 1/1 2 "
           "3 1 1/1 2 2 2 2/1 2 2 0 2/1 2 1 1 2/1 2 0 2 2/1 2 0 "
           "0",  // 1/1 x1^6 x2^0 + 1/1 x1^5 x2^1 + 1/1 x1^4 x2^2
                 // + 1/1 x1^4 x2^0 + 1/1 x1^3 x2^1 + 1/1 x1^2
                 // x2^2 + 2/1 x1^2 x2^0 + 2/1 x1^1 x2^1 + 2/1
                 // x1^0 x2^2 + 2/1 x1^0 x2^0
           "2 7 1/1 2 6 0 1/1 2 5 1 1/1 2 4 0 1/1 2 3 1 2/1 2 2 "
           "0 2/1 2 1 1 2/1 2 0 0"  // 1/1 x1^6 x2^0 + 1/1 x1^5
                                    // x2^1 + 1/1 x1^4 x2^0 + 1/1
                                    // x1^3 x2^1 + 2/1 x1^2 x2^0
                                    // + 2/1 x1^1 x2^1 + 2/1 x1^0
                                    // x2^0
      }},
     // \cite cox-little-oshea-2007, ex. 2a, page 68.
     {false,
      "3 3 1/1 3 1 2 2 1/1 3 1 1 0 -1/1 3 0 1 1",  // xy^2z^2 + xy -
                                                   // yz
      {
           "3 2 1/1 3 1 0 0 -1/1 3 0 2 0",  // x - y^2
           "3 2 1/1 3 0 1 0 -1/1 3 0 0 3",  // y - z^3
           "3 2 1/1 3 0 0 2 -1/1 3 0 0 0"   // z^2 - 1
      },
      "3 1 1/1 3 0 0 1",
      {"3 2 1/1 3 0 2 2 1/1 3 0 1 0",
       "3 8 1/1 3 0 3 2 1/1 3 0 2 5 1/1 3 0 2 0 1/1 3 0 1 8 1/1 3 0 "
       "1 3 1/1 3 0 0 11 1/1 3 0 0 6 -1/1 3 0 0 1",
       "3 9 1/1 3 0 0 12 1/1 3 0 0 10 1/1 3 0 0 8 1/1 3 0 0 7 1/1 3 "
       "0 0 6 1/1 3 0 0 5 1/1 3 0 0 4 1/1 3 0 0 3 1/1 3 0 0 1"}},
     // \cite cox-little-oshea-2007, ex. 2a, page 68, first
     // permutation.
     {false,
      "3 3 1/1 3 1 2 2 1/1 3 1 1 0 -1/1 3 0 1 1",  // xy^2z^2 + xy -
                                                   // yz
      {
           "3 2 1/1 3 0 0 2 -1/1 3 0 0 0",  // z^2 - 1
           "3 2 1/1 3 1 0 0 -1/1 3 0 2 0",  // x - y^2
           "3 2 1/1 3 0 1 0 -1/1 3 0 0 3"   // y - z^3
      },
      "3 1 1/1 3 0 0 1",
      {"3 9 1/1 3 1 2 0 1/1 3 0 3 1 1/1 3 0 2 2 1/1 3 0 2 1 1/1 3 0 "
       "2 0 1/1 3 0 1 2 1/1 3 0 1 1 1/1 3 0 1 0 1/1 3 0 0 1",
       "3 2 1/1 3 0 2 0 1/1 3 0 1 0",
       "3 6 1/1 3 0 3 0 1/1 3 0 2 1 1/1 3 0 2 0 1/1 3 0 1 1 1/1 3 0 "
       "1 0 1/1 3 0 0 0"}},
     // \cite cox-little-oshea-2007, ex. 2a, page 68, second
     // permutation.
     {false,
      "3 3 1/1 3 1 2 2 1/1 3 1 1 0 -1/1 3 0 1 1",  // xy^2z^2 + xy -
                                                   // yz
      {
           "3 2 1/1 3 0 1 0 -1/1 3 0 0 3",  // y - z^3
           "3 2 1/1 3 0 0 2 -1/1 3 0 0 0",  // z^2 - 1
           "3 2 1/1 3 1 0 0 -1/1 3 0 2 0"   // x - y^2
      },
      "3 1 1/1 3 0 0 1",
      {"3 8 1/1 3 1 1 2 1/1 3 1 0 5 1/1 3 1 0 0 1/1 3 0 1 1 1/1 3 0 "
       "1 0 1/1 3 0 0 4 1/1 3 0 0 3 -1/1 3 0 0 1",
       "3 9 1/1 3 1 0 6 1/1 3 1 0 4 1/1 3 1 0 2 1/1 3 1 0 1 1/1 3 1 "
       "0 0 1/1 3 0 0 5 1/1 3 0 0 4 1/1 3 0 0 3 1/1 3 0 0 1",
       "3 2 1/1 3 0 0 1 1/1 3 0 0 0"}},
     // x^2 - y^2 = (x - y)(x + y) + 0(x - y)
     {false,
      "2 2 1/1 2 2 0 -1/1 2 0 2",  // x^2 - y^2
      {
           "2 2 1/1 2 1 0 1/1 2 0 1",  // x + y
           "2 2 1/1 2 1 0 -1/1 2 0 1"  // x - y
      },
      "2 0",  // 0
      {
           "2 2 1/1 2 1 0 -1/1 2 0 1",  // x - y
           "2 0"                        // 0
      }},
     // x^2 - y^2 = (x - y)(x + y) + 0(x + y) (g[0] = g[1])
     {false,
      "2 2 1/1 2 2 0 -1/1 2 0 2",  // x^2 - y^2
      {
           "2 2 1/1 2 1 0 1/1 2 0 1",  // x + y
           "2 2 1/1 2 1 0 1/1 2 0 1"   // x + y
      },
      "2 0",  // 0
      {
           "2 2 1/1 2 1 0 -1/1 2 0 1",  // x - y
           "2 0"                        // 0
      }},
     // g.size() == 0
     {true,
      "2 2 1/1 2 2 0 -1/1 2 0 2",  // x^2 - y^2
      {},
      "2 0",  // 0
      {}},
     // g[0].dim() != g[1].dim()
     {true,
      "2 2 1/1 2 2 0 -1/1 2 0 2",  // x^2 - y^2
      {
           "1 1 1/1 1 1",             // x
           "2 2 1/1 2 1 0 1/1 2 0 1"  // x + y
      },
      "2 0",  // 0
      {
           "2 2 1/1 2 1 0 -1/1 2 0 1",  // x - y
           "2 0"                        // 0
      }},
     // g[1] = 0
     {true,
      "2 2 1/1 2 2 0 -1/1 2 0 2",  // x^2 - y^2
      {
           "2 2 1/1 2 1 0 1/1 2 0 1",  // x + y
           "2 0"                       // 0
      },
      "2 0",  // 0
      {
           "2 2 1/1 2 1 0 -1/1 2 0 1",  // x - y
           "2 0"                        // 0
      }},
};

BOOST_DATA_TEST_CASE(polynomial_div_test,
                     bdata::xrange(std::size(pdtcase)), xr) {
     using Sztp = std::vector<Polynomial>::size_type;
     auto const& c = pdtcase[xr];
     auto const s = c.g.size();
     BOOST_REQUIRE(c.a.size() == s);

     Polynomial f, r;
     std::vector<Polynomial> g(s), a(s);
     f = from_chars(c.f);
     r = from_chars(c.r);
     for (Sztp i = 0; i < s; i++) {
          g[i] = from_chars(c.g[i]);
          a[i] = from_chars(c.a[i]);
     }
     Polynomial_div pd;
     if (c.should_throw) {
          BOOST_CHECK_THROW(pd.divide(f, g), invalid_argument);
          return;
     }
     {
          Polynomial p({r});
          for (Sztp i = 0; i < s; i++) {
               Polynomial const q = a[i] * g[i];
               p += q;
               if (!q.is_zero()) {
                    // Check that the leading term of a[i] * g[i] is
                    // not greater than the leading term of f.
                    auto const& qlm = q.leading_monomial();
                    auto const& flm = f.leading_monomial();
                    BOOST_REQUIRE(!f.order()(flm, qlm));
               }
          }
          BOOST_REQUIRE(p == f);

          // Check that no monomial of r is divisible by any of
          // leading terms of g[i].
          auto const& t = r.terms();
          for (auto it = t.cbegin(); it != t.cend(); ++it)
               for (Sztp i = 0; i < s; i++)
                    BOOST_REQUIRE(
                         !g[i].leading_monomial().divides(it->first));
     }
     pd.divide(f, g);
     BOOST_REQUIRE(pd.a.size() == s);
     for (Sztp i = 0; i < s; i++)
          BOOST_CHECK(pd.a[i] == a[i]);
     BOOST_CHECK(pd.r == r);
}

struct Test_data {
     std::vector<Polynomial> f_{};
     std::vector<Polynomial> g_{};
     void init(std::vector<char const*> const f,
               std::vector<char const*> const g,
               char const* ordering);
};

void Test_data::init(std::vector<char const*> const f,
                     std::vector<char const*> const g,
                     char const* ordering) {
     Monomial_cmp cmp;
     if (strcmp(ordering, "Lex_less") == 0)
          cmp = lex_cmp;
     else if (strcmp(ordering, "Grlex_less") == 0)
          cmp = grlex_cmp;
     else if (strcmp(ordering, "Grevlex_less") == 0)
          cmp = grevlex_cmp;
     else
          throw std::runtime_error("invalid monomial ordering");
     for (auto it = f.cbegin(); it != f.cend(); ++it) {
          Polynomial p{from_chars(*it)};
          p.order(cmp);
          f_.push_back(p);
     }
     for (auto it = g.cbegin(); it != g.cend(); ++it) {
          Polynomial p{from_chars(*it)};
          p.order(cmp);
          g_.push_back(p);
     }
}

// At compile time, buchberger_test_data.size() may be unknown. This
// constant is checked inside the test.
constexpr std::vector<Buchberger_test_case>::size_type
     buchberger_test_data_size = 30;

BOOST_DATA_TEST_CASE(buchberger_test,
                     bdata::xrange(buchberger_test_data_size), xr) {
     BOOST_REQUIRE(buchberger_test_data.size() ==
                   buchberger_test_data_size);
     auto const& tc = buchberger_test_data[xr];
     if (strstr(tc.description, "exercise 13b, page 114") !=
              nullptr &&
         strcmp(tc.ordering, "Lex_less") == 0) {
          BOOST_CHECK(true);
          return;  // Takes too long.
     }

     Test_data td;
     td.init(tc.f, tc.g, tc.ordering);
     Buchberger_improved b;
     b.run(td.f_);
     BOOST_CHECK(SHG::have_equal_content(b.g(), td.g_));
     BOOST_CHECK(true);
}

/**
 * Non-zero polynomial equal to zero for all values.
 * \cite cox-little-oshea-2007, exercise 2, page 5.
 */
BOOST_AUTO_TEST_CASE(exercise2_test) {
     Field_Fp F(2);
     Polynomial p(&F, 2);
     p += {F.element(1), Monomial{2, 1}};
     p += {F.element(1), Monomial{1, 2}};
     Polynomial::Point x(2);
     for (int i = 0; i < 2; i++) {
          x[0] = F.element(i);
          for (int j = 0; j < 2; j++) {
               x[1] = F.element(j);
               BOOST_CHECK(p(x) == F.zero());
          }
     }
}

/**
 * Test of division based on
 * \f$(x^n - 1) = (x - 1) (x^{n - 1} + x^{n - 2} +
 * \ldots + x + 1)\f$.
 */
BOOST_DATA_TEST_CASE(polynomial_in_one_variable_first_test,
                     bdata::xrange(10), xr) {
     int const n = xr + 1;
     Field_Q F;
     Polynomial a(&F), b(&F), c(&F);
     // a = x^n - 1
     a += {F.element(-1), Monomial{0}};
     a += {F.element(1), Monomial{n}};
     // b = x^{n - 1} + x^{n - 2} + ... + x + 1
     for (int i = 0; i < n; i++)
          b += {F.element(1), Monomial{i}};
     // c = x - 1
     c += {F.element(-1), Monomial{0}};
     c += {F.element(1), Monomial{1}};

     BOOST_CHECK(b * c == a);
     Polynomial_div d;

     d.divide(a, {c});
     BOOST_CHECK(is_zero(d.r));
     BOOST_CHECK(d.a.size() == 1);
     BOOST_CHECK(d.a[0] == b);

     d.divide(a, {b});
     BOOST_CHECK(is_zero(d.r));
     BOOST_CHECK(d.a.size() == 1);
     BOOST_CHECK(d.a[0] == c);
}

/**
 * Test of division based on
 * \f$(x^n - y^n) = (x - y) (x^{n - 1} + x^{n - 2}y +
 * \ldots + xy^{n - 2} + y^{n - 1})\f$.
 */
BOOST_DATA_TEST_CASE(polynomial_in_one_variable_second_test,
                     bdata::xrange(10), xr) {
     int const n = xr + 1;
     Field_Q F;
     Polynomial a(&F, 2), b(&F, 2), c(&F, 2);
     // a = x^n - y^n
     a += {F.element(1), Monomial{n, 0}};
     a += {F.element(-1), Monomial{0, n}};
     // b = x^{n - 1} + x^{n - 2}y + ... + xy^{n - 2} + y^{n - 1}
     for (int i = 0; i < n; i++)
          b += {F.element(1), Monomial{n - 1 - i, i}};
     // c = x - y
     c += {F.element(1), Monomial{1, 0}};
     c += {F.element(-1), Monomial{0, 1}};

     BOOST_CHECK(b * c == a);
     Polynomial_div d;

     a.order(lex_cmp);
     b.order(lex_cmp);
     c.order(lex_cmp);
     d.divide(a, {c});
     BOOST_CHECK(is_zero(d.r));
     BOOST_CHECK(d.a.size() == 1);
     BOOST_CHECK(d.a[0] == b);
     d.divide(a, {b});
     BOOST_CHECK(is_zero(d.r));
     BOOST_CHECK(d.a.size() == 1);
     BOOST_CHECK(d.a[0] == c);

     a.order(grlex_cmp);
     b.order(grlex_cmp);
     c.order(grlex_cmp);
     d.divide(a, {c});
     BOOST_CHECK(is_zero(d.r));
     BOOST_CHECK(d.a.size() == 1);
     BOOST_CHECK(d.a[0] == b);
     d.divide(a, {b});
     BOOST_CHECK(is_zero(d.r));
     BOOST_CHECK(d.a.size() == 1);
     BOOST_CHECK(d.a[0] == c);

     a.order(grevlex_cmp);
     b.order(grevlex_cmp);
     c.order(grevlex_cmp);
     d.divide(a, {c});
     BOOST_CHECK(is_zero(d.r));
     BOOST_CHECK(d.a.size() == 1);
     BOOST_CHECK(d.a[0] == b);
     d.divide(a, {b});
     BOOST_CHECK(is_zero(d.r));
     BOOST_CHECK(d.a.size() == 1);
     BOOST_CHECK(d.a[0] == c);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
