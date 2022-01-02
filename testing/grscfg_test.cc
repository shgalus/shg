#include <shg/grscfg.h>
#include "testing.h"
#include "cfgdata.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(grscfg_test)

using namespace SHG::PLP;

Vecsent const res_lukasiewicz1{
     {"p"},
     {"q"},
     {"q"},
     {"p"},
     {"p"},
     {"N", "q"},
     {"N", "q"},
     {"N", "p"},
     {"N", "p"},
     {"N", "p"},
     {"N", "N", "q"},
     {"A", "q", "p"},
     {"A", "p", "q"},
     {"A", "p", "p"},
     {"N", "N", "p"},
     {"N", "A", "p", "q"},
     {"A", "N", "q", "q"},
     {"A", "N", "p", "q"},
     {"A", "N", "p", "q"},
     {"A", "p", "N", "q"},
     {"N", "N", "N", "N", "q"},
     {"A", "A", "q", "q", "q"},
     {"N", "N", "A", "p", "q"},
     {"A", "p", "A", "q", "p"},
     {"N", "A", "N", "p", "q"},
     {"A", "p", "A", "p", "N", "p"},
     {"A", "q", "N", "A", "p", "q"},
     {"A", "q", "A", "N", "p", "q"},
     {"A", "p", "N", "A", "p", "q"},
     {"N", "A", "p", "A", "p", "p"},
     {"A", "p", "A", "N", "N", "q", "q"},
     {"N", "N", "A", "A", "q", "q", "p"},
     {"A", "p", "A", "N", "N", "p", "q"},
     {"N", "A", "A", "N", "q", "p", "q"},
     {"A", "N", "N", "q", "A", "q", "p"},
     {"N", "N", "A", "N", "N", "p", "N", "q"},
     {"A", "A", "q", "A", "N", "p", "q", "q"},
     {"N", "N", "A", "N", "q", "A", "p", "q"},
     {"A", "p", "A", "p", "A", "q", "N", "p"},
     {"N", "A", "A", "N", "N", "q", "q", "q"},
};

BOOST_AUTO_TEST_CASE(lukasiewicz1_test) {
     McKenzie gen;
     Vecsent vs;
     gen.set_grammar(test_grammars[18]);
     for (int n = 1; n <= 8; n++) {
          auto const v = gen.generate(5, n);
          vs.insert(vs.end(), v.begin(), v.end());
     }
     BOOST_CHECK(vs == res_lukasiewicz1);
}

Vecsent const res_ae{
     {"a"},
     {"a"},
     {"a"},
     {"a"},
     {"a"},
     {},
     {},
     {},
     {},
     {},
     {"a", "*", "a"},
     {"a", "+", "a"},
     {"a", "*", "a"},
     {"a", "*", "a"},
     {"a", "*", "a"},
     {},
     {},
     {},
     {},
     {},
     {"a", "+", "a", "+", "a"},
     {"a", "*", "a", "*", "a"},
     {"a", "*", "a", "*", "a"},
     {"a", "+", "a", "+", "a"},
     {"a", "+", "a", "+", "a"},
};

BOOST_AUTO_TEST_CASE(ae_test) {
     McKenzie gen;
     Vecsent vs;
     gen.set_grammar(test_grammars[1]);
     for (int n = 1; n <= 5; n++) {
          auto const v = gen.generate(5, n);
          vs.insert(vs.end(), v.begin(), v.end());
     }
     BOOST_CHECK(vs == res_ae);
}

BOOST_AUTO_TEST_CASE(epsilon_prod_test) {
     McKenzie gen;
     BOOST_CHECK_THROW(gen.set_grammar(test_grammars[4]), CFG_error);
}

Vecsent const res_pal{
     {"x"},
     {"x"},
     {"x"},
     {"x"},
     {"x"},
     {},
     {},
     {},
     {},
     {},
     {"x", "x", "x"},
     {"x", "x", "x"},
     {"x", "x", "x"},
     {"x", "x", "x"},
     {"x", "x", "x"},
     {},
     {},
     {},
     {},
     {},
     {"x", "x", "x", "x", "x"},
     {"x", "x", "x", "x", "x"},
     {"x", "x", "x", "x", "x"},
     {"x", "x", "x", "x", "x"},
     {"x", "x", "x", "x", "x"},
};

BOOST_AUTO_TEST_CASE(pal_test) {
     McKenzie gen;
     Vecsent vs;
     gen.set_grammar(test_grammars[7]);
     for (int n = 1; n <= 5; n++) {
          auto const v = gen.generate(5, n);
          vs.insert(vs.end(), v.begin(), v.end());
     }
     // std::cout << to_string(vs);
     BOOST_CHECK(vs == res_pal);
}

Vecsent const res_manning_11_2{
     {},
     {},
     {},
     {},
     {},
     {},
     {},
     {},
     {},
     {},
     {"telescopes", "saw", "telescopes"},
     {"stars", "saw", "astronomers"},
     {"telescopes", "saw", "telescopes"},
     {"telescopes", "saw", "stars"},
     {"telescopes", "saw", "ears"},
     {},
     {},
     {},
     {},
     {},
     {"telescopes", "saw", "astronomers", "with", "saw"},
     {"stars", "saw", "saw", "with", "stars"},
     {"ears", "saw", "astronomers", "with", "stars"},
     {"saw", "saw", "stars", "with", "saw"},
     {"saw", "with", "saw", "saw", "saw"},
};

BOOST_AUTO_TEST_CASE(manning_11_2_test) {
     McKenzie gen;
     Vecsent vs;
     gen.set_grammar(test_grammars[2]);
     for (int n = 1; n <= 5; n++) {
          auto const v = gen.generate(5, n);
          vs.insert(vs.end(), v.begin(), v.end());
     }
     BOOST_CHECK(vs == res_manning_11_2);
}

Vecsent const res_long_sentence{
     {"N", "A", "A", "N", "A", "A", "p", "p", "A", "q",
      "q", "q", "N", "A", "A", "N", "q", "N", "N", "N",
      "p", "A", "A", "A", "q", "N", "q", "A", "A", "A",
      "q", "p", "A", "p", "p", "N", "N", "N", "p", "p"},
     {"N", "N", "A", "A", "A", "A", "N", "q", "A", "N",
      "q", "N", "p", "q", "A", "A", "A", "N", "A", "N",
      "N", "N", "p", "N", "N", "A", "p", "p", "A", "A",
      "q", "A", "q", "A", "p", "p", "q", "q", "q", "p"},
     {"A", "A", "q", "N", "A", "p", "A", "N", "N", "A",
      "p", "q", "p", "A", "q", "A", "A", "N", "q", "q",
      "A", "A", "p", "N", "p", "A", "N", "A", "A", "A",
      "A", "q", "q", "A", "q", "p", "N", "q", "q", "p"},
     {"N", "N", "A", "N", "A", "A", "q", "A", "A", "N",
      "q", "p", "A", "N", "p", "A", "N", "p", "A", "q",
      "A", "p", "A", "p", "A", "q", "N", "p", "p", "A",
      "A", "N", "N", "A", "A", "p", "q", "p", "q", "q"},
     {"A", "A", "A", "A", "N", "p", "N", "A", "q", "N",
      "p", "p", "N", "A", "A", "q", "q", "N", "A", "N",
      "p", "A", "N", "q", "N", "N", "A", "A", "A", "q",
      "A", "q", "N", "p", "N", "p", "A", "q", "p", "p"},
};

BOOST_AUTO_TEST_CASE(long_sentence_test) {
     McKenzie gen;
     Vecsent vs;
     gen.set_grammar(test_grammars[18]);
     auto const v = gen.generate(5, 40);
     BOOST_CHECK(v == res_long_sentence);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
