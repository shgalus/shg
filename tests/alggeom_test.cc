#include <shg/alggeom.h>
#include <shg/utils.h>
#include "buchbdat.h"
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(alggeom_test)

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
     using SHG::ALGEBRA::ALGGEOM::Polynomial_div;
     using SHG::ALGEBRA::Polynomial;
     using SHG::from_string;
     using Sztp = std::vector<Polynomial>::size_type;
     using std::invalid_argument;
     auto const& c = pdtcase[xr];
     auto const s = c.g.size();
     BOOST_REQUIRE(c.a.size() == s);

     Polynomial f, r;
     std::vector<Polynomial> g(s), a(s);
     f = from_string<Polynomial>(c.f);
     r = from_string<Polynomial>(c.r);
     for (Sztp i = 0; i < s; i++) {
          g[i] = from_string<Polynomial>(c.g[i]);
          a[i] = from_string<Polynomial>(c.a[i]);
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
     std::vector<SHG::ALGEBRA::Polynomial> f_{};
     std::vector<SHG::ALGEBRA::Polynomial> g_{};
     void init(std::vector<char const*> const f,
               std::vector<char const*> const g,
               char const* ordering);
};

void Test_data::init(std::vector<char const*> const f,
                     std::vector<char const*> const g,
                     char const* ordering) {
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Monomial_cmp;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::grevlex_cmp;
     using SHG::ALGEBRA::grlex_cmp;
     using SHG::ALGEBRA::lex_cmp;
     using SHG::from_string;

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
          Polynomial p{from_string<Polynomial>(*it)};
          p.order(cmp);
          f_.push_back(p);
     }
     for (auto it = g.cbegin(); it != g.cend(); ++it) {
          Polynomial p{from_string<Polynomial>(*it)};
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
     using SHG::ALGEBRA::ALGGEOM::Buchberger_improved;
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
     using SHG::ALGEBRA::Ring_Zn;
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Polynomial;
     Ring_Zn F(2);
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
     using SHG::ALGEBRA::ALGGEOM::Polynomial_div;
     using SHG::ALGEBRA::Field_Q;
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Polynomial;
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
     using SHG::ALGEBRA::ALGGEOM::Polynomial_div;
     using SHG::ALGEBRA::Field_Q;
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Polynomial;
     using SHG::ALGEBRA::grevlex_cmp;
     using SHG::ALGEBRA::grlex_cmp;
     using SHG::ALGEBRA::lex_cmp;

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
