#include <shg/numerals.h>
#include <numeric>
#include <boost/crc.hpp>
#include <shg/dict_impl.h>
#include "numerals_data.h"
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(numerals_test)

using SHG::PLP::find_numeral_prefix;
using SHG::PLP::check_numeral_adverbs;
using SHG::PLP::Part_of_speech;
using SHG::PLP::Inflexion;
using SHG::PLP::Declension_case;
using SHG::PLP::Number;
using SHG::PLP::Gender;
using SHG::PLP::Degree;
using SHG::PLP::Aspect;
using SHG::PLP::Mood;
using SHG::PLP::Tense;
using SHG::PLP::Person;
using SHG::PLP::Form_of_verb;
using SHG::PLP::Type_of_pronoun;
using SHG::PLP::Type_of_numeral;
using SHG::PLP::Setdesc;
using SHG::PLP::Charset::utf8_to_charset;

BOOST_AUTO_TEST_CASE(generate_numeral_prefixes_test) {
     std::vector<std::string> const v = generate_numeral_prefixes();
     BOOST_CHECK(v.size() == 1999);
     std::string const t =
          std::accumulate(v.cbegin(), v.cend(), std::string());
     boost::crc_32_type result;
     result.process_bytes(t.data(), t.length());
     BOOST_CHECK(result.checksum() == 358752400);
}

BOOST_AUTO_TEST_CASE(find_numeral_prefix_test) {
     char const* const ipol = "ip\303\263\305\202";
     char const* const ponad = "ponad";
     std::string const suff = "kilogramowy";
     std::string::size_type len;
     std::string s;
     std::vector<std::string> const v = generate_numeral_prefixes();

     for (auto const& pref : v) {
          s = pref + suff;
          s = utf8_to_charset(s);
          len = find_numeral_prefix(s);
          BOOST_CHECK(s.substr(len) == suff);

          s = pref + ipol + suff;
          s = utf8_to_charset(s);
          len = find_numeral_prefix(s);
          BOOST_CHECK(s.substr(len) == suff);

          s = ponad + pref + suff;
          s = utf8_to_charset(s);
          len = find_numeral_prefix(s);
          BOOST_CHECK(s.substr(len) == suff);

          s = ponad + pref + ipol + suff;
          s = utf8_to_charset(s);
          len = find_numeral_prefix(s);
          BOOST_CHECK(s.substr(len) == suff);
     }

     char const* const pol = "p\303\263\305\202";
     char const* const s210 = "dwudziestodziesi\304\231cio";
     char const* const s211 = "dwudziestojedenasto";

     s = pol + suff;
     s = utf8_to_charset(s);
     len = find_numeral_prefix(s);
     BOOST_CHECK(len == 3);

     s = ipol + suff;
     s = utf8_to_charset(s);
     len = find_numeral_prefix(s);
     BOOST_CHECK(len == 0);

     s = ponad + suff;
     s = utf8_to_charset(s);
     len = find_numeral_prefix(s);
     BOOST_CHECK(len == 0);

     s = s210 + suff;
     s = utf8_to_charset(s);
     len = find_numeral_prefix(s);
     BOOST_CHECK(len == 10);

     s = s211 + suff;
     s = utf8_to_charset(s);
     len = find_numeral_prefix(s);
     BOOST_CHECK(len == 10);
}

BOOST_AUTO_TEST_CASE(check_numeral_adverbs_test) {
     std::vector<std::string> const v = generate_numeral_prefixes();
     for (auto const& s0 : v) {
          auto const s = s0 + "krotnie";
          Setdesc sd;
          auto const t = utf8_to_charset(s);
          check_numeral_adverbs(t, sd);
          BOOST_REQUIRE(sd.size() == 1);
          auto const& d = *sd.cbegin();
          BOOST_CHECK(d.main_form == t);
          auto const& c = d.category;
          BOOST_CHECK(c.part_of_speech == Part_of_speech::adverb);
          BOOST_CHECK(c.inflexion == Inflexion::uninflected);
          BOOST_CHECK(c.declension_case == Declension_case::none);
          BOOST_CHECK(c.number == Number::none);
          BOOST_CHECK(c.gender == Gender::none);
          BOOST_CHECK(c.degree == Degree::positive);
          BOOST_CHECK(c.aspect == Aspect::none);
          BOOST_CHECK(c.mood == Mood::none);
          BOOST_CHECK(c.tense == Tense::none);
          BOOST_CHECK(c.person == Person::none);
          BOOST_CHECK(c.form_of_verb == Form_of_verb::none);
          BOOST_CHECK(c.type_of_pronoun == Type_of_pronoun::none);
          BOOST_CHECK(c.type_of_numeral == Type_of_numeral::none);
     }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
