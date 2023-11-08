#include <shg/cfg.h>
#include "tests.h"
#include "cfgdata.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(cfg_test)

using SHG::PLP::Vecprod;
using SHG::PLP::Conversion_to_CNF;
using SHG::PLP::CYK;
using SHG::PLP::Sentence;
using SHG::PLP::full_cnf;
using SHG::PLP::CFG_error;

BOOST_AUTO_TEST_CASE(invalid_grammar_test) {
     Vecprod const vp1{
          {" START", {"A", "B"}},
     };
     Vecprod const vp2{
          {"S", {"A A", "B"}},
     };
     BOOST_CHECK(!is_valid(vp1));
     BOOST_CHECK(!is_valid(vp2));
}

BOOST_DATA_TEST_CASE(valid_grammar_test,
                     bdata::xrange(test_grammars_size), xr) {
     BOOST_REQUIRE(test_grammars_size == std::size(test_grammars));
     BOOST_CHECK(is_valid(test_grammars[xr]));
}

BOOST_DATA_TEST_CASE(conversion_to_cnf_test,
                     bdata::xrange(test_grammars_size), xr) {
     BOOST_REQUIRE(std::size(test_grammars) == test_grammars_size);
     BOOST_REQUIRE(std::size(conversion_results) ==
                   conversion_results_size);
     BOOST_REQUIRE(conversion_results_size == test_grammars_size);
     Vecprod const& vp = test_grammars[xr];
     Conversion_to_CNF conv;
     if (xr == 6) {
          BOOST_CHECK_THROW(conv.convert(vp), CFG_error);
          return;
     }
     BOOST_CHECK_NO_THROW(conv.convert(vp));
     BOOST_CHECK(conv.cfg() == conversion_results[xr]);
}

/*
 * The sentence in this case has two parses:
 * S                         S
 *    B                         A
 *       b                         B
 *    C                               b
 *       A                         A
 *          a                         a
 *       B                      B
 *          C                      C
 *             A                      A
 *                a                      a
 *             B                      B
 *                b                      b
 *          C                      C
 *             a                      a
 */
BOOST_AUTO_TEST_CASE(example_6_7_test) {
     Vecprod const& cfg = test_grammars[0];
     BOOST_CHECK(cfg.size() == 8);
     CYK cyk;
     cyk.set_grammar(cfg);
     Sentence const X = {"b", "a", "a", "b", "a"};
     BOOST_CHECK(cyk.recognize(X));
     cyk.parse(X);
     auto const q = cyk.gen().to_string();
     BOOST_CHECK(q ==
                 "(S(B(b), C(A(a), B(C(A(a), B(b)), C(a)))), "
                 "S(A(B(b), A(a)), B(C(A(a), B(b)), C(a))))");
}

BOOST_AUTO_TEST_CASE(example_earley_ae_test) {
     Vecprod const cfg = test_grammars[1];
     BOOST_CHECK(cfg.size() == 5);
     CYK cyk;
     BOOST_CHECK_THROW(cyk.set_grammar(cfg), CFG_error);
}

/*
 * The sentence in this case has two parses:
 *
 * S                          S
 *   NP                         NP
 *      astronomers                astronomers
 *   VP                         VP
 *       V                         VP
 *         saw                          V
 *      NP                                 saw
 *          NP                         NP
 *              stars                      stars
 *          PP                     PP
 *               P                      P
 *                  with                   with
 *              NP                     NP
 *                  ears                   ears
 */
BOOST_AUTO_TEST_CASE(manning_11_2_test) {
     Vecprod const& cfg = test_grammars[2];
     BOOST_CHECK(cfg.size() == 12);
     CYK cyk;
     cyk.set_grammar(cfg);
     Sentence const X = {"astronomers", "saw", "stars", "with",
                         "ears"};
     BOOST_CHECK(cyk.recognize(X));
     cyk.parse(X);
     auto const q = cyk.gen().to_string();
     BOOST_CHECK(
          q ==
          "(S(NP(astronomers), VP(V(saw), NP(NP(stars), PP(P(with), "
          "NP(ears))))), S(NP(astronomers), VP(VP(V(saw), "
          "NP(stars)), PP(P(with), NP(ears)))))");
}

BOOST_AUTO_TEST_CASE(conversion_to_cnf_earley_ae_test) {
     Vecprod const& vp = test_grammars[1];
     BOOST_CHECK(vp.size() == 5);
     Conversion_to_CNF conv;

     conv.convert(vp);
     CYK cyk;
     cyk.set_grammar(conv.cfg());
     Sentence const X = {"a", "+", "a", "*", "a"};
     BOOST_CHECK(cyk.recognize(X));
     cyk.parse(X);
     BOOST_CHECK(cyk.gen().to_string() ==
                 "(_0(E(a), _3(_1(+), T(T(a), _4(_2(*), P(a))))))");
}

constexpr char const* const ubda_res[] = {
     "(_0(x))",
     "(_0(A(x), A(x)))",
     "(_0(A(x), A(A(x), A(x))), _0(A(A(x), A(x)), A(x)))",
     "(_0(A(x), A(A(x), A(A(x), A(x)))), _0(A(x), A(A(A(x), A(x)), "
     "A(x))), _0(A(A(x), A(x)), A(A(x), A(x))), _0(A(A(x), A(A(x), "
     "A(x))), A(x)), _0(A(A(A(x), A(x)), A(x)), A(x)))",
};

BOOST_DATA_TEST_CASE(ubda_test, bdata::xrange(std::size(ubda_res)),
                     xr) {
     Vecprod const& vp = test_grammars[5];
     BOOST_CHECK(vp.size() == 2);
     Conversion_to_CNF conv;
     conv.convert(vp);
     CYK cyk;
     cyk.set_grammar(conv.cfg());
     Sentence const X(xr + 1, "x");
     BOOST_CHECK(cyk.recognize(X));
     cyk.parse(X);
     auto const q = cyk.gen().to_string();
     BOOST_CHECK(q == ubda_res[xr]);
}

constexpr char const* const pal_res[] = {
     "(_0(x))",
     "",
     "(_0(_1(x), _3(A(x), _1(x))))",
     "",
     "(_0(_1(x), _3(A(_1(x), _3(A(x), _1(x))), _1(x))))",
     "",
     ("(_0(_1(x), _3(A(_1(x), _3(A(_1(x), _3(A(x), _1(x))), _1(x))), "
      "_1(x))))"),
     "",
};

BOOST_DATA_TEST_CASE(pal_test, bdata::xrange(std::size(pal_res)),
                     xr) {
     Vecprod const& vp = test_grammars[7];
     BOOST_CHECK(vp.size() == 2);
     Conversion_to_CNF conv;
     conv.convert(vp);
     CYK cyk;
     cyk.set_grammar(conv.cfg());
     Sentence const X(xr + 1, "x");
     if (xr % 2 == 1) {
          BOOST_CHECK(!cyk.recognize(X));
          return;
     }
     BOOST_CHECK(cyk.recognize(X));
     cyk.parse(X);
     auto const q = cyk.gen().to_string();
     BOOST_CHECK(q == pal_res[xr]);
}

BOOST_AUTO_TEST_CASE(pcg_test) {
     Vecprod const& vp = test_grammars[12];
     BOOST_CHECK(vp.size() == 16);
     Conversion_to_CNF conv;
     conv.convert(vp);
     CYK cyk;
     cyk.set_grammar(conv.cfg());

     // Example of length 1.
     Sentence X{"p"};
     BOOST_REQUIRE(X.size() == 1);
     cyk.parse(X);
     auto q = cyk.gen().to_string();
     BOOST_CHECK(q == "(_0(p))");

     // Example of length 5.
     X = {"(", "p", "&&", "q", ")"};
     BOOST_REQUIRE(X.size() == 5);
     cyk.parse(X);
     q = cyk.gen().to_string();
     BOOST_CHECK(
          q == "(_0(_2((), _9(F(U(p), _13(_7(&&), U(q))), _3()))))");

     // Example of length 13.
     X = {"(", "p",  "\'", "&&", "q", ")", "||",
          "r", "||", "p",  "||", "q", "\'"};
     BOOST_REQUIRE(X.size() == 13);
     cyk.parse(X);
     BOOST_CHECK(cyk.gen().degree() == 1);
     q = cyk.gen().to_string();
     BOOST_CHECK(q ==
                 "(_0(U(_2((), _9(F(U(L(p), _5(')), _13(_7(&&), "
                 "U(q))), _3()))), _10(_6(||), S(U(r), _10(_6(||), "
                 "S(U(p), _11(_6(||), U(L(q), _5(')))))))))");

     // Example of length 26 is corrected by adding the last ")".
     X = {"p",  "->", "(", "(",  "q",  "->", "~", "(", "r",
          "\'", "||", "(", "p",  "&&", "q",  ")", ")", ")",
          "->", "(",  "q", "\'", "||", "r",  ")", ")"};
     BOOST_REQUIRE(X.size() == 26);
     cyk.parse(X);
     BOOST_CHECK(cyk.gen().degree() == 1);
     q = cyk.gen().to_string();
     BOOST_CHECK(
          q ==
          "(_0(U(p), _8(_1(->), U(_2((), _9(F(U(_2((), _9(F(U(q), "
          "_8(_1(->), U(_4(~), U(_2((), _9(F(U(L(r), _5(')), "
          "_11(_6(||), U(_2((), _9(F(U(p), _13(_7(&&), U(q))), "
          "_3()))))), _3())))))), _3()))), _8(_1(->), U(_2((), "
          "_9(F(U(L(q), _5(')), _11(_6(||), U(r))), _3()))))), "
          "_3()))))))");

     // Example of length 17 is omitted as incorrectly printed in the
     // article. Example of length 38.
     X = {"(",  "(", "p",  "&&", "q",  ")",  "||", "(",  "q",  "&&",
          "r",  ")", "||", "(",  "r",  "&&", "p",  "\'", ")",  ")",
          "->", "~", "(",  "(",  "p",  "\'", "||", "q",  "\'", ")",
          "&&", "(", "r",  "\'", "||", "p",  ")",  ")"};
     BOOST_REQUIRE(X.size() == 38);
     cyk.parse(X);
     BOOST_CHECK(cyk.gen().degree() == 1);
     q = cyk.gen().to_string();
     BOOST_CHECK(
          q ==
          "(_0(U(_2((), _9(F(U(_2((), _9(F(U(p), _13(_7(&&), U(q))), "
          "_3()))), _10(_6(||), S(U(_2((), _9(F(U(q), _13(_7(&&), "
          "U(r))), _3()))), _11(_6(||), U(_2((), _9(F(U(r), "
          "_13(_7(&&), U(L(p), _5(')))), _3()))))))), _3()))), "
          "_8(_1(->), U(_4(~), U(_2((), _9(F(U(_2((), _9(F(U(L(p), "
          "_5(')), _11(_6(||), U(L(q), _5(')))), _3()))), "
          "_13(_7(&&), U(_2((), _9(F(U(L(r), _5(')), _11(_6(||), "
          "U(p))), _3()))))), _3())))))))");
}

Sentence gre_test_input(int m, int n) {
     Sentence X;
     for (int i = 0; i < m; i++) {
          X.push_back("e");
          X.push_back("d");
     }
     X.push_back("e");
     X.push_back("a");
     for (int i = 0; i < n; i++)
          X.push_back("b");
     return X;
}

BOOST_AUTO_TEST_CASE(gre_test) {
     Vecprod const& vp = test_grammars[13];
     BOOST_CHECK(vp.size() == 5);
     Conversion_to_CNF conv;
     conv.convert(vp);
     CYK cyk;
     cyk.set_grammar(conv.cfg());
     Sentence X;

     X = gre_test_input(2, 0);
     BOOST_REQUIRE(X.size() == 6);
     BOOST_CHECK(cyk.recognize(X));
     cyk.parse(X);
     BOOST_CHECK(cyk.gen().degree() == 2);
     BOOST_CHECK(
          cyk.gen().to_string() ==
          "(_0(Y(Y(e), _4(_3(d), Y(Y(e), _4(_3(d), Y(e))))), _2(a)), "
          "_0(Y(Y(Y(e), _4(_3(d), Y(e))), _4(_3(d), Y(e))), _2(a)))");

     X = gre_test_input(2, 4);
     BOOST_REQUIRE(X.size() == 10);
     BOOST_CHECK(cyk.recognize(X));
     cyk.parse(X);
     BOOST_CHECK(cyk.gen().degree() == 2);

     X = gre_test_input(2, 10);
     BOOST_REQUIRE(X.size() == 16);
     BOOST_CHECK(cyk.recognize(X));

     X = gre_test_input(2, 200);
     BOOST_REQUIRE(X.size() == 206);
     BOOST_CHECK(cyk.recognize(X));

     X = gre_test_input(4, 2);
     BOOST_REQUIRE(X.size() == 12);
     BOOST_CHECK(cyk.recognize(X));

     X = gre_test_input(7, 2);
     BOOST_REQUIRE(X.size() == 18);
     BOOST_CHECK(cyk.recognize(X));

     X = gre_test_input(8, 2);
     BOOST_REQUIRE(X.size() == 20);
     BOOST_CHECK(cyk.recognize(X));
}

Vecprod const prop_calc_cnf_res{
     {"_0", {"_1", "_9"}},   {"_0", {"_2", "_14"}},
     {"_0", {"_2", "_19"}},  {"_0", {"_2", "_24"}},
     {"_0", {"_2", "_29"}},  {"_0", {"p"}},
     {"_0", {"q"}},          {"_0", {"r"}},
     {"_8", {"S", "_3"}},    {"_9", {"_2", "_8"}},
     {"_10", {"S", "_3"}},   {"_11", {"_2", "_10"}},
     {"_12", {"_4", "_11"}}, {"_13", {"_3", "_12"}},
     {"_14", {"S", "_13"}},  {"_15", {"S", "_3"}},
     {"_16", {"_2", "_15"}}, {"_17", {"_5", "_16"}},
     {"_18", {"_3", "_17"}}, {"_19", {"S", "_18"}},
     {"_20", {"S", "_3"}},   {"_21", {"_2", "_20"}},
     {"_22", {"_6", "_21"}}, {"_23", {"_3", "_22"}},
     {"_24", {"S", "_23"}},  {"_25", {"S", "_3"}},
     {"_26", {"_2", "_25"}}, {"_27", {"_7", "_26"}},
     {"_28", {"_3", "_27"}}, {"_29", {"S", "_28"}},
     {"S", {"_1", "_9"}},    {"S", {"_2", "_14"}},
     {"S", {"_2", "_19"}},   {"S", {"_2", "_24"}},
     {"S", {"_2", "_29"}},   {"_7", {"<->"}},
     {"_1", {"~"}},          {"_2", {"("}},
     {"_3", {")"}},          {"_4", {"or"}},
     {"_5", {"and"}},        {"_6", {"->"}},
     {"S", {"p"}},           {"S", {"q"}},
     {"S", {"r"}},
};

BOOST_AUTO_TEST_CASE(prop_calc_test) {
     Vecprod const& vp = test_grammars[16];
     BOOST_CHECK(vp.size() == 8);
     Conversion_to_CNF conv;
     conv.convert(vp);
     BOOST_CHECK(conv.cfg() == prop_calc_cnf_res);
     CYK cyk;
     cyk.set_grammar(conv.cfg());
     Sentence X;

     X = {"p"};
     cyk.parse(X);
     auto q = cyk.gen().to_string();
     BOOST_CHECK(q == "(_0(p))");

     X = {"(", "p", ")", "or", "(", "q", ")"};
     cyk.parse(X);
     q = cyk.gen().to_string();
     BOOST_CHECK(q ==
                 "(_0(_2((), _14(S(p), _13(_3()), _12(_4(or), "
                 "_11(_2((), _10(S(q), _3()))))))))");
}

BOOST_AUTO_TEST_CASE(lukasiewicz_notation_test) {
     Vecprod const& vp = test_grammars[17];
     BOOST_CHECK(vp.size() == 22);
     CYK cyk;
     cyk.set_grammar(vp);
     Sentence X;

     X = {"p"};
     cyk.parse(X);
     BOOST_CHECK(cyk.gen().degree() == 1);
     BOOST_CHECK(cyk.gen().to_string() == "(S(p))");

     X = {"C", "K", "C", "p", "q", "p", "q"};
     cyk.parse(X);
     BOOST_CHECK(cyk.gen().degree() == 1);
     BOOST_CHECK(cyk.gen().to_string() ==
                 "(S(FC(C), B(W(FK(K), B(W(FC(C), B(W(p), W(q))), "
                 "W(p))), W(q))))");

     X = {"C", "K", "C", "p", "q", "N", "q", "N", "p"};
     cyk.parse(X);
     BOOST_CHECK(cyk.gen().degree() == 1);
     BOOST_CHECK(cyk.gen().to_string() ==
                 "(S(FC(C), B(W(FK(K), B(W(FC(C), B(W(p), W(q))), "
                 "W(FN(N), W(q)))), W(FN(N), W(p)))))");

     X = {"E", "N", "A", "p", "q", "K", "N", "p", "N", "q"};
     cyk.parse(X);
     BOOST_CHECK(cyk.gen().degree() == 1);
     BOOST_CHECK(cyk.gen().to_string() ==
                 "(S(FE(E), B(W(FN(N), W(FA(A), B(W(p), W(q)))), "
                 "W(FK(K), B(W(FN(N), W(p)), W(FN(N), W(q)))))))");
}

Vecprod const full_cnf_test_result{
     {"v0", {"v0", "v0"}}, {"v0", {"v0", "v1"}}, {"v0", {"v0", "v2"}},
     {"v0", {"v1", "v0"}}, {"v0", {"v1", "v1"}}, {"v0", {"v1", "v2"}},
     {"v0", {"v2", "v0"}}, {"v0", {"v2", "v1"}}, {"v0", {"v2", "v2"}},
     {"v0", {"t0"}},       {"v0", {"t1"}},       {"v1", {"v0", "v0"}},
     {"v1", {"v0", "v1"}}, {"v1", {"v0", "v2"}}, {"v1", {"v1", "v0"}},
     {"v1", {"v1", "v1"}}, {"v1", {"v1", "v2"}}, {"v1", {"v2", "v0"}},
     {"v1", {"v2", "v1"}}, {"v1", {"v2", "v2"}}, {"v1", {"t0"}},
     {"v1", {"t1"}},       {"v2", {"v0", "v0"}}, {"v2", {"v0", "v1"}},
     {"v2", {"v0", "v2"}}, {"v2", {"v1", "v0"}}, {"v2", {"v1", "v1"}},
     {"v2", {"v1", "v2"}}, {"v2", {"v2", "v0"}}, {"v2", {"v2", "v1"}},
     {"v2", {"v2", "v2"}}, {"v2", {"t0"}},       {"v2", {"t1"}},
};

BOOST_AUTO_TEST_CASE(full_cnf_test) {
     auto const v = full_cnf(3, 2);
     BOOST_CHECK(v == full_cnf_test_result);
     BOOST_CHECK_THROW(full_cnf(0, 0), CFG_error);
     BOOST_CHECK_THROW(full_cnf(1, 0), CFG_error);
     BOOST_CHECK_THROW(full_cnf(0, 1), CFG_error);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
