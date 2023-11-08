#include <shg/pcfg.h>
#include <iomanip>
#include <sstream>
#include <shg/mzt.h>
#include <shg/grscfg.h>
#include <shg/utils.h>
#include "tests.h"
#include "cfgdata.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(pcfg_test)

using SHG::PLP::Vecsprod;
using SHG::PLP::PCFG;
using SHG::PLP::add_probabilities;
using SHG::PLP::Vecprod;
using SHG::PLP::Sentence;
using SHG::PLP::Vecsent;
using SHG::PLP::CYK;
using SHG::PLP::Conversion_to_CNF;
using SHG::PLP::full_cnf;
using SHG::PLP::McKenzie;
using SHG::faeq;
using SHG::MZT;
using std::istringstream;
using std::ostringstream;
using std::ios_base;

constexpr double const tol =
     100.0 * std::numeric_limits<double>::epsilon();

std::vector<double> const probs{
     1.0, 0.4, 1.0, 0.7, 0.3, 0.1, 0.18, 0.04, 0.18, 0.1, 1.0, 1.0,
};

constexpr char const* const res = R"(Number of nonterminals: 6
Number of terminals: 6
Number of binary productions: 5
Number of unary productions: 7

Nonterminals:
0 S
1 NP
2 PP
3 VP
4 P
5 V

Terminals:
0 astronomers
1 ears
2 saw
3 stars
4 telescopes
5 with

Binary productions:
1.00000 S --> NP VP
0.40000 NP --> NP PP
1.00000 PP --> P NP
0.30000 VP --> VP PP
0.70000 VP --> V NP

Unary productions:
0.10000 NP --> astronomers
0.18000 NP --> ears
0.04000 NP --> saw
0.18000 NP --> stars
0.10000 NP --> telescopes
1.00000 P --> with
1.00000 V --> saw
)";

char const* const res_cyk =
     "S 1.000000(NP 0.100000(astronomers), VP 0.700000(V "
     "1.000000(saw), NP 0.400000(NP 0.180000(stars), PP 1.000000(P "
     "1.000000(with), NP 0.180000(ears)))))";

BOOST_AUTO_TEST_CASE(basic_test) {
     PCFG pcfg;
     pcfg.set(test_grammars[2], probs);
     std::ostringstream oss;
     oss << std::fixed << std::setprecision(5);
     oss << pcfg;
     BOOST_CHECK(oss.str() == res);
     Sentence const X{"astronomers", "saw", "stars", "with", "ears"};
     double const prob = pcfg.prob(X);
     BOOST_CHECK(faeq(prob, 0.0015876, tol));
     for (PCFG::Index k = 0; k < X.size(); k++) {
          double const prob_outside = pcfg.prob_outside(X, k);
          BOOST_CHECK(faeq(prob_outside, prob, tol));
     }
     double prob2;
     PCFG::Parse_tree t;
     pcfg.cyk(X, prob2, t);
     BOOST_CHECK(faeq(prob2, 0.0009072, tol));
     auto const q = t.to_string();
     BOOST_CHECK(q == res_cyk);
}

BOOST_DATA_TEST_CASE(inside_outside_probabilities_test,
                     bdata::xrange(8) * bdata::xrange(8), xr1, xr2) {
     std::vector<double> const probs{1.0, 0.25, 0.125, 0.75, 0.875};
     Sentence X(xr1 + xr2 + 2, "a");
     for (Sentence::size_type i = xr1 + 1; i < X.size(); i++)
          X[i] = "b";
     PCFG pcfg;
     pcfg.set(test_grammars[20], probs);
     double const prob = pcfg.prob(X);
     for (PCFG::Index k = 0; k < X.size(); k++) {
          double const prob1 = pcfg.prob_outside(X, k);
          BOOST_CHECK(faeq(prob1, prob, tol));
     }
}

constexpr char const* const res_add_probabilities =
     R"(Number of nonterminals: 4
Number of terminals: 2
Number of binary productions: 36
Number of unary productions: 8

Nonterminals:
0 _0
1 v1
2 v2
3 v0

Terminals:
0 t0
1 t1

Binary productions:
9.2144e-02 _0 --> v1 v1
7.2148e-02 _0 --> v1 v2
1.1380e-01 _0 --> v1 v0
1.8574e-01 _0 --> v2 v1
7.8501e-02 _0 --> v2 v2
7.8013e-03 _0 --> v2 v0
4.7008e-03 _0 --> v0 v1
1.6349e-02 _0 --> v0 v2
2.8253e-01 _0 --> v0 v0
8.6192e-02 v1 --> v1 v1
4.2359e-02 v1 --> v1 v2
8.1585e-02 v1 --> v1 v0
4.7886e-02 v1 --> v2 v1
6.4353e-03 v1 --> v2 v2
9.0731e-02 v1 --> v2 v0
1.5378e-02 v1 --> v0 v1
1.5768e-01 v1 --> v0 v2
1.7084e-01 v1 --> v0 v0
1.7853e-02 v2 --> v1 v1
1.4401e-02 v2 --> v1 v2
3.8634e-01 v2 --> v1 v0
8.4847e-02 v2 --> v2 v1
2.0492e-03 v2 --> v2 v2
4.8325e-02 v2 --> v2 v0
8.2617e-02 v2 --> v0 v1
6.1975e-02 v2 --> v0 v2
1.0096e-01 v2 --> v0 v0
2.4268e-01 v0 --> v1 v1
1.1106e-02 v0 --> v1 v2
2.2568e-01 v0 --> v1 v0
3.1103e-02 v0 --> v2 v1
2.7078e-02 v0 --> v2 v2
2.4448e-02 v0 --> v2 v0
1.0632e-01 v0 --> v0 v1
2.7799e-01 v0 --> v0 v2
3.8206e-03 v0 --> v0 v0

Unary productions:
4.8889e-02 _0 --> t0
9.7395e-02 _0 --> t1
9.0677e-02 v1 --> t0
2.1024e-01 v1 --> t1
4.3291e-02 v2 --> t0
1.5734e-01 v2 --> t1
2.0900e-03 v0 --> t0
4.7692e-02 v0 --> t1
)";

BOOST_AUTO_TEST_CASE(add_probabilities_test) {
     Conversion_to_CNF conv;
     conv.convert(full_cnf(3, 2));
     auto const vp = conv.cfg();
     BOOST_CHECK_NO_THROW(CYK().set_grammar(vp));
     std::vector<double> prob;
     MZT mzt;
     add_probabilities(vp, prob, mzt);
     PCFG pcfg;
     pcfg.set(vp, prob);
     std::ostringstream oss;
     oss << std::scientific << std::setprecision(4);
     oss << pcfg;
     BOOST_CHECK(oss.str() == res_add_probabilities);
}

char const* const inside_outside_besic_test_result =
     R"(Number of nonterminals: 5
Number of terminals: 2
Number of binary productions: 125
Number of unary productions: 10

Nonterminals:
0 v0
1 v1
2 v2
3 v3
4 v4

Terminals:
0 a
1 b

Binary productions:
0.1138521099 v0 --> v0 v0
0.0012899317 v0 --> v0 v1
0.0062834811 v0 --> v0 v2
0.0530914819 v0 --> v0 v3
0.0275358670 v0 --> v0 v4
0.0180365963 v0 --> v1 v0
0.0013956020 v0 --> v1 v1
0.0359594760 v0 --> v1 v2
0.0235040385 v0 --> v1 v3
0.0072594538 v0 --> v1 v4
0.0393691620 v0 --> v2 v0
0.0010550444 v0 --> v2 v1
0.0472047902 v0 --> v2 v2
0.1198549792 v0 --> v2 v3
0.0774376536 v0 --> v2 v4
0.1365794887 v0 --> v3 v0
0.0042172912 v0 --> v3 v1
0.0100356876 v0 --> v3 v2
0.0186379211 v0 --> v3 v3
0.0086196105 v0 --> v3 v4
0.0008289185 v0 --> v4 v0
0.0103275746 v0 --> v4 v1
0.0613703671 v0 --> v4 v2
0.0058739527 v0 --> v4 v3
0.0433306148 v0 --> v4 v4
0.0045238230 v1 --> v0 v0
0.0106241092 v1 --> v0 v1
0.0083020031 v1 --> v0 v2
0.0021583340 v1 --> v0 v3
0.0104346895 v1 --> v0 v4
0.0233746820 v1 --> v1 v0
0.0121050240 v1 --> v1 v1
0.0126719381 v1 --> v1 v2
0.0206572488 v1 --> v1 v3
0.0402155260 v1 --> v1 v4
0.0032396203 v1 --> v2 v0
0.0023373145 v1 --> v2 v1
0.0126136414 v1 --> v2 v2
0.0365223339 v1 --> v2 v3
0.0003548019 v1 --> v2 v4
0.0140904481 v1 --> v3 v0
0.0517702293 v1 --> v3 v1
0.0163535765 v1 --> v3 v2
0.0762282501 v1 --> v3 v3
0.0054911061 v1 --> v3 v4
0.0060427779 v1 --> v4 v0
0.0062886062 v1 --> v4 v1
0.0090261626 v1 --> v4 v2
0.0088567954 v1 --> v4 v3
0.0076999472 v1 --> v4 v4
0.0045174833 v2 --> v0 v0
0.0032333888 v2 --> v0 v1
0.0061878689 v2 --> v0 v2
0.1164247911 v2 --> v0 v3
0.0084157145 v2 --> v0 v4
0.0001684213 v2 --> v1 v0
0.0022592139 v2 --> v1 v1
0.0077578012 v2 --> v1 v2
0.0207223671 v2 --> v1 v3
0.0100342388 v2 --> v1 v4
0.0381977207 v2 --> v2 v0
0.0007229240 v2 --> v2 v1
0.0315474376 v2 --> v2 v2
0.0418815465 v2 --> v2 v3
0.0159547188 v2 --> v2 v4
0.0753962246 v2 --> v3 v0
0.0093135418 v2 --> v3 v1
0.0028326640 v2 --> v3 v2
0.0018522554 v2 --> v3 v3
0.0063525682 v2 --> v3 v4
0.0073955635 v2 --> v4 v0
0.0054940531 v2 --> v4 v1
0.0070467233 v2 --> v4 v2
0.0106404881 v2 --> v4 v3
0.0004837268 v2 --> v4 v4
0.0037187951 v3 --> v0 v0
0.0014608374 v3 --> v0 v1
0.0015372566 v3 --> v0 v2
0.0014915413 v3 --> v0 v3
0.0071384787 v3 --> v0 v4
0.0024590406 v3 --> v1 v0
0.0009938916 v3 --> v1 v1
0.0016938636 v3 --> v1 v2
0.0245517479 v3 --> v1 v3
0.0029851986 v3 --> v1 v4
0.0030675554 v3 --> v2 v0
0.0000538600 v3 --> v2 v1
0.0209453032 v3 --> v2 v2
0.0013319691 v3 --> v2 v3
0.0048772411 v3 --> v2 v4
0.0376372709 v3 --> v3 v0
0.0003023774 v3 --> v3 v1
0.0118390212 v3 --> v3 v2
0.0142544879 v3 --> v3 v3
0.0056267747 v3 --> v3 v4
0.0187761599 v3 --> v4 v0
0.0010724189 v3 --> v4 v1
0.0001143858 v3 --> v4 v2
0.0069762302 v3 --> v4 v3
0.0075788165 v3 --> v4 v4
0.0098519996 v4 --> v0 v0
0.0182761602 v4 --> v0 v1
0.0180643774 v4 --> v0 v2
0.0246003708 v4 --> v0 v3
0.0180574364 v4 --> v0 v4
0.0027727801 v4 --> v1 v0
0.0257145409 v4 --> v1 v1
0.0255164217 v4 --> v1 v2
0.0158966280 v4 --> v1 v3
0.0200885582 v4 --> v1 v4
0.0012961431 v4 --> v2 v0
0.0081349328 v4 --> v2 v1
0.0191718439 v4 --> v2 v2
0.0416185951 v4 --> v2 v3
0.0082206175 v4 --> v2 v4
0.0096442651 v4 --> v3 v0
0.0377004482 v4 --> v3 v1
0.1095199440 v4 --> v3 v2
0.0304409485 v4 --> v3 v3
0.0122472596 v4 --> v3 v4
0.0002679206 v4 --> v4 v0
0.0056501112 v4 --> v4 v1
0.0207040059 v4 --> v4 v2
0.0217562083 v4 --> v4 v3
0.0027968889 v4 --> v4 v4

Unary productions:
0.0475245951 v0 --> a
0.0795243103 v0 --> b
0.4429635508 v1 --> a
0.1550534600 v1 --> b
0.1142502175 v2 --> a
0.4509163373 v2 --> b
0.6045025842 v3 --> a
0.2130128922 v3 --> b
0.0790722960 v4 --> a
0.4129182979 v4 --> b
)";

// Original probabilities from \cite lari-young-1990, page 44.
std::vector<double> prob0{0.3, 0.3, 0.2, 0.2, 1.0, 1.0, 1.0, 1.0};

BOOST_AUTO_TEST_CASE(inside_outside_basic_test) {
     Vecprod const& vp0 = test_grammars[19];
     Vecsent ts;
     {
          McKenzie gen;
          gen.set_grammar(vp0);
          MZT mzt;
          for (int n = 0; n < 200; n++) {
               // random even number from [2, 40].
               int const k = (mzt.uni(20) + 1) * 2;
               auto const v = gen.generate(1, k);
               ts.insert(ts.end(), v.begin(), v.end());
          }
     }
     Vecprod vp = full_cnf(5, 2);
     for (auto& p : vp) {
          if (p.rhs[0] == "t0")
               p.rhs[0] = "a";
          else if (p.rhs[0] == "t1")
               p.rhs[0] = "b";
     }
     std::vector<double> prob;
     MZT mzt;
     add_probabilities(vp, prob, mzt);
     PCFG pcfg;
     pcfg.set(vp, prob);
     double log_prob = pcfg.log_prob(ts);
     BOOST_CHECK(faeq(log_prob, -9825.9467880, 5e-8));
     int iter;
     double logp;
     int const status =
          pcfg.inside_outside(ts, 1e-8, tol, 20, iter, logp);
     BOOST_CHECK(status == 2);
     BOOST_CHECK(iter == 20);
     BOOST_CHECK(faeq(logp, -3870.5553122869, 5e-11));
     log_prob = pcfg.log_prob(ts);
     BOOST_CHECK(faeq(log_prob, -3870.5553122869, 5e-11));
     std::ostringstream oss;
     oss << std::fixed << std::setprecision(10);
     oss << pcfg;
     BOOST_CHECK(oss.str() == inside_outside_besic_test_result);
}

BOOST_AUTO_TEST_CASE(constructor_test) {
     Vecprod const& vp0 = test_grammars[19];
     PCFG a;
     a.set(vp0, prob0);
     PCFG b(a);
     BOOST_CHECK(a == b);
     b = a;
     BOOST_CHECK(a == b);
     PCFG c{std::move(b)};
     BOOST_CHECK(c == a);
     BOOST_CHECK(!b.is_valid());
     b = a;
     c.clear();
     c = std::move(b);
     BOOST_CHECK(c == a);
     BOOST_CHECK(!b.is_valid());
}

BOOST_AUTO_TEST_CASE(comparing_test) {
     Vecprod const& vp0 = test_grammars[19];
     std::vector<double> prob1{0.2, 0.4, 0.2, 0.2,
                               1.0, 1.0, 1.0, 1.0};
     PCFG a, b;
     BOOST_CHECK(a == b);
     BOOST_CHECK(!(a != b));
     a.set(vp0, prob0);
     BOOST_CHECK(a != b);
     BOOST_CHECK(!(a == b));
     b.set(vp0, prob0);
     BOOST_CHECK(a == b);
     BOOST_CHECK(!(a != b));
     b.set(vp0, prob1);
     BOOST_CHECK(a != b);
     BOOST_CHECK(!(a == b));
}

/**
 * \cite lari-young-1990, page 44.
 */
Vecsprod const ly{
     {{"S", {"A", "C"}}, 0.3}, {{"S", {"B", "D"}}, 0.3},
     {{"S", {"A", "A"}}, 0.2}, {{"S", {"B", "B"}}, 0.2},
     {{"A", {"a"}}, 1.0},      {{"B", {"b"}}, 1.0},
     {{"C", {"S", "A"}}, 1.0}, {{"D", {"S", "B"}}, 1.0},
};

BOOST_AUTO_TEST_CASE(s_production_test) {
     constexpr char const* const res =
          R"({{"S", {"A", "C"}}, 0.30000},
{{"S", {"B", "D"}}, 0.30000},
{{"S", {"A", "A"}}, 0.20000},
{{"S", {"B", "B"}}, 0.20000},
{{"A", {"a"}}, 1.00000},
{{"B", {"b"}}, 1.00000},
{{"C", {"S", "A"}}, 1.00000},
{{"D", {"S", "B"}}, 1.00000},
)";
     std::ostringstream oss;
     oss << std::fixed << std::setprecision(5);
     oss << ly;
     BOOST_CHECK(oss.str() == res);
}

BOOST_AUTO_TEST_CASE(vecsprod_test) {
     constexpr char const* const res =
          R"({{"S", {"A", "A"}}, 0.20000},
{{"S", {"A", "C"}}, 0.30000},
{{"S", {"B", "B"}}, 0.20000},
{{"S", {"B", "D"}}, 0.30000},
{{"C", {"S", "A"}}, 1.00000},
{{"D", {"S", "B"}}, 1.00000},
{{"A", {"a"}}, 1.00000},
{{"B", {"b"}}, 1.00000},
)";
     PCFG a;
     a.set(test_grammars[19], prob0);
     auto const vsp = a.vecsprod();
     std::ostringstream oss;
     oss << std::fixed << std::setprecision(5);
     oss << vsp;
     BOOST_CHECK(oss.str() == res);
}

/**
 * Return trained grammar \cite lari-young-1990, page 44.
 */
PCFG train_lari_young(int max_iter) {
     Vecprod const& vp0 = test_grammars[19];
     Vecsent ts;
     {
          McKenzie gen;
          gen.set_grammar(vp0);
          MZT mzt;
          for (int n = 0; n < 200; n++) {
               // random even number from [2, 40].
               int const k = (mzt.uni(20) + 1) * 2;
               auto const v = gen.generate(1, k);
               ts.insert(ts.end(), v.begin(), v.end());
          }
     }
     Vecprod vp = full_cnf(5, 2);
     for (auto& p : vp) {
          if (p.rhs[0] == "t0")
               p.rhs[0] = "a";
          else if (p.rhs[0] == "t1")
               p.rhs[0] = "b";
     }
     std::vector<double> prob;
     MZT mzt;
     add_probabilities(vp, prob, mzt);
     PCFG pcfg;
     pcfg.set(vp, prob);
     int iter;
     double logp;
     int const status = pcfg.inside_outside(ts, 1e-8, 2.0 * tol,
                                            max_iter, iter, logp);
     if (status != 2)
          throw std::runtime_error(
               "invalid status in train_lari_young");
     return pcfg;
}

Vecsprod const ly_trained{
     {{"v0", {"v0", "v0"}}, 1.1679090918e-22},
     {{"v0", {"v0", "v1"}}, 1.0421582231e-02},
     {{"v0", {"v0", "v2"}}, 1.0367997724e-02},
     {{"v0", {"v0", "v3"}}, 3.0020036669e-01},
     {{"v0", {"v0", "v4"}}, 9.6438987454e-02},
     {{"v0", {"v1", "v0"}}, 5.1052383620e-02},
     {{"v0", {"v1", "v1"}}, 7.8931842364e-03},
     {{"v0", {"v1", "v2"}}, 5.5997055040e-06},
     {{"v0", {"v1", "v3"}}, 1.6240202901e-02},
     {{"v0", {"v1", "v4"}}, 2.9608075998e-05},
     {{"v0", {"v2", "v0"}}, 1.0171886081e-01},
     {{"v0", {"v2", "v1"}}, 2.2003199863e-07},
     {{"v0", {"v2", "v2"}}, 1.0912923837e-02},
     {{"v0", {"v2", "v3"}}, 8.3953814760e-06},
     {{"v0", {"v2", "v4"}}, 1.3097394163e-02},
     {{"v0", {"v3", "v0"}}, 3.4477498191e-01},
     {{"v0", {"v3", "v1"}}, 2.4509421888e-03},
     {{"v0", {"v3", "v2"}}, 6.5414798378e-07},
     {{"v0", {"v3", "v3"}}, 5.2707889573e-03},
     {{"v0", {"v3", "v4"}}, 3.6635175061e-06},
     {{"v0", {"v4", "v0"}}, 2.8979152533e-03},
     {{"v0", {"v4", "v1"}}, 4.7360503391e-05},
     {{"v0", {"v4", "v2"}}, 8.3688391516e-03},
     {{"v0", {"v4", "v3"}}, 2.4486099097e-06},
     {{"v0", {"v4", "v4"}}, 1.7794662211e-02},
     {{"v1", {"v0", "v0"}}, 1.0974406219e-68},
     {{"v1", {"v0", "v1"}}, 1.1470987382e-25},
     {{"v1", {"v0", "v2"}}, 4.4148064501e-27},
     {{"v1", {"v0", "v3"}}, 5.8230929979e-25},
     {{"v1", {"v0", "v4"}}, 9.8414199290e-28},
     {{"v1", {"v1", "v0"}}, 1.6305459887e-25},
     {{"v1", {"v1", "v1"}}, 1.3211078696e-02},
     {{"v1", {"v1", "v2"}}, 2.9948359245e-03},
     {{"v1", {"v1", "v3"}}, 3.3021235369e-02},
     {{"v1", {"v1", "v4"}}, 1.4375697032e-02},
     {{"v1", {"v2", "v0"}}, 2.4499124039e-27},
     {{"v1", {"v2", "v1"}}, 8.7108595660e-04},
     {{"v1", {"v2", "v2"}}, 4.4311237570e-03},
     {{"v1", {"v2", "v3"}}, 2.1307817805e-02},
     {{"v1", {"v2", "v4"}}, 1.1809971049e-04},
     {{"v1", {"v3", "v0"}}, 2.7531333398e-24},
     {{"v1", {"v3", "v1"}}, 8.2252542154e-02},
     {{"v1", {"v3", "v2"}}, 7.0060740559e-03},
     {{"v1", {"v3", "v3"}}, 1.4223738890e-01},
     {{"v1", {"v3", "v4"}}, 3.6914354982e-03},
     {{"v1", {"v4", "v0"}}, 7.1381831351e-28},
     {{"v1", {"v4", "v1"}}, 2.9811043223e-03},
     {{"v1", {"v4", "v2"}}, 3.3467685261e-03},
     {{"v1", {"v4", "v3"}}, 7.1364233155e-03},
     {{"v1", {"v4", "v4"}}, 2.6176134738e-03},
     {{"v2", {"v0", "v0"}}, 5.0199844328e-65},
     {{"v2", {"v0", "v1"}}, 3.0178390370e-24},
     {{"v2", {"v0", "v2"}}, 9.9714983240e-25},
     {{"v2", {"v0", "v3"}}, 2.2190184046e-21},
     {{"v2", {"v0", "v4"}}, 2.1002198127e-25},
     {{"v2", {"v1", "v0"}}, 1.0891423320e-25},
     {{"v2", {"v1", "v1"}}, 5.2583558207e-03},
     {{"v2", {"v1", "v2"}}, 6.2843701505e-03},
     {{"v2", {"v1", "v3"}}, 5.5932701360e-02},
     {{"v2", {"v1", "v4"}}, 1.2017032376e-02},
     {{"v2", {"v2", "v0"}}, 9.9180988716e-24},
     {{"v2", {"v2", "v1"}}, 9.6597917101e-04},
     {{"v2", {"v2", "v2"}}, 1.1529223290e-01},
     {{"v2", {"v2", "v3"}}, 6.2094982815e-02},
     {{"v2", {"v2", "v4"}}, 5.5137486335e-02},
     {{"v2", {"v3", "v0"}}, 1.1037499519e-21},
     {{"v2", {"v3", "v1"}}, 2.4562945086e-02},
     {{"v2", {"v3", "v2"}}, 2.7141698976e-03},
     {{"v2", {"v3", "v3"}}, 4.1953672761e-03},
     {{"v2", {"v3", "v4"}}, 1.0405708094e-02},
     {{"v2", {"v4", "v0"}}, 2.3915820942e-25},
     {{"v2", {"v4", "v1"}}, 8.1628678001e-03},
     {{"v2", {"v4", "v2"}}, 2.4378072368e-02},
     {{"v2", {"v4", "v3"}}, 1.9968224678e-02},
     {{"v2", {"v4", "v4"}}, 1.4369866418e-03},
     {{"v3", {"v0", "v0"}}, 4.0549976822e-72},
     {{"v3", {"v0", "v1"}}, 2.0205395178e-28},
     {{"v3", {"v0", "v2"}}, 1.3499510289e-29},
     {{"v3", {"v0", "v3"}}, 6.6166839939e-27},
     {{"v3", {"v0", "v4"}}, 9.4570584847e-30},
     {{"v3", {"v1", "v0"}}, 2.3734705090e-28},
     {{"v3", {"v1", "v1"}}, 7.9556972576e-04},
     {{"v3", {"v1", "v2"}}, 3.9860654858e-04},
     {{"v3", {"v1", "v3"}}, 3.8046986370e-02},
     {{"v3", {"v1", "v4"}}, 8.8560779655e-04},
     {{"v3", {"v2", "v0"}}, 3.5790197751e-29},
     {{"v3", {"v2", "v1"}}, 1.6499118610e-05},
     {{"v3", {"v2", "v2"}}, 2.6190163995e-03},
     {{"v3", {"v2", "v3"}}, 6.5617014785e-04},
     {{"v3", {"v2", "v4"}}, 7.4631953046e-04},
     {{"v3", {"v3", "v0"}}, 1.3004730945e-25},
     {{"v3", {"v3", "v1"}}, 4.5386274169e-04},
     {{"v3", {"v3", "v2"}}, 4.8081296345e-03},
     {{"v3", {"v3", "v3"}}, 3.1702517672e-02},
     {{"v3", {"v3", "v4"}}, 3.3290763665e-03},
     {{"v3", {"v4", "v0"}}, 2.6638957838e-29},
     {{"v3", {"v4", "v1"}}, 3.5627480559e-04},
     {{"v3", {"v4", "v2"}}, 1.7239715731e-05},
     {{"v3", {"v4", "v3"}}, 4.3399755711e-03},
     {{"v3", {"v4", "v4"}}, 1.1920784645e-03},
     {{"v4", {"v0", "v0"}}, 3.9490112083e-61},
     {{"v4", {"v0", "v1"}}, 1.2038281158e-21},
     {{"v4", {"v0", "v2"}}, 2.8141691423e-22},
     {{"v4", {"v0", "v3"}}, 2.6394984806e-20},
     {{"v4", {"v0", "v4"}}, 6.1440769761e-23},
     {{"v4", {"v1", "v0"}}, 1.0007695597e-22},
     {{"v4", {"v1", "v1"}}, 6.2827702343e-02},
     {{"v4", {"v1", "v2"}}, 2.1935025591e-02},
     {{"v4", {"v1", "v3"}}, 3.9652841827e-02},
     {{"v4", {"v1", "v4"}}, 3.0882215905e-02},
     {{"v4", {"v2", "v0"}}, 2.2009041969e-23},
     {{"v4", {"v2", "v1"}}, 1.0480206449e-02},
     {{"v4", {"v2", "v2"}}, 5.4218795158e-02},
     {{"v4", {"v2", "v3"}}, 4.2850802149e-02},
     {{"v4", {"v2", "v4"}}, 3.0675842351e-02},
     {{"v4", {"v3", "v0"}}, 6.6294109404e-21},
     {{"v4", {"v3", "v1"}}, 8.5655025471e-02},
     {{"v4", {"v3", "v2"}}, 8.6832859462e-02},
     {{"v4", {"v3", "v3"}}, 4.9748751059e-02},
     {{"v4", {"v3", "v4"}}, 2.0837881174e-02},
     {{"v4", {"v4", "v0"}}, 6.8126013253e-25},
     {{"v4", {"v4", "v1"}}, 1.0084363494e-02},
     {{"v4", {"v4", "v2"}}, 6.8115961660e-02},
     {{"v4", {"v4", "v3"}}, 3.7333289002e-02},
     {{"v4", {"v4", "v4"}}, 1.0779766384e-02},
     {{"v0", {"a"}}, 3.6688622370e-08},
     {{"v0", {"b"}}, 4.8296501943e-16},
     {{"v1", {"a"}}, 5.3127174404e-01},
     {{"v1", {"b"}}, 1.2712793147e-01},
     {{"v2", {"a"}}, 2.3436265469e-02},
     {{"v2", {"b"}}, 5.6775625176e-01},
     {{"v3", {"a"}}, 5.5706978421e-01},
     {{"v3", {"b"}}, 3.5256628518e-01},
     {{"v4", {"a"}}, 8.7244858953e-03},
     {{"v4", {"b"}}, 3.2836418463e-01},
};

Vecsprod const remove_productions_test_result{
     {{"v0", {"v0", "v3"}}, 4.0203923227e-01},
     {{"v0", {"v2", "v0"}}, 1.3622559212e-01},
     {{"v0", {"v3", "v0"}}, 4.6173517561e-01},
     {{"v1", {"v3", "v3"}}, 1.7765526382e-01},
     {{"v2", {"v2", "v2"}}, 1.6879070152e-01},
     {{"v1", {"a"}}, 6.6356126597e-01},
     {{"v1", {"b"}}, 1.5878347022e-01},
     {{"v2", {"b"}}, 8.3120929848e-01},
     {{"v3", {"a"}}, 6.1240951514e-01},
     {{"v3", {"b"}}, 3.8759048486e-01},
     {{"v4", {"b"}}, 1.0000000000e+00},
};

BOOST_AUTO_TEST_CASE(remove_productions_test) {
     PCFG pcfg;
     auto const p = split(ly_trained);
     pcfg.set(p.first, p.second, 1e-10);
     pcfg.remove_productions(1e-1);
     auto const vsp = pcfg.vecsprod();
     BOOST_REQUIRE(vsp.size() ==
                   remove_productions_test_result.size());
     for (Vecsprod::size_type i = 0; i < vsp.size(); i++)
          BOOST_CHECK(
               std::abs(vsp[i].prob -
                        remove_productions_test_result[i].prob) <
               1e-10);

     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
