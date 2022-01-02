#include <shg/dict.h>
#include <cstring>
#include <ios>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <shg/utils.h>
#include "dictdata.h"
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(dict_test)

namespace bdata = boost::unit_test::data;
using std::istringstream;
using std::ostringstream;
using std::stringstream;
using std::ios_base;
using namespace SHG::PLP;

struct Dict_fixture {
     void setup();
     static Dictionary dict;
};

void Dict_fixture::setup() {
     istringstream iss(ios_base::in | ios_base::binary);
     iss.str(polish_dict);
     dict.load_source_word_file(iss);
     BOOST_REQUIRE(iss.eof());
     BOOST_REQUIRE(iss.fail());
     BOOST_REQUIRE(!iss.bad());
}

Dictionary Dict_fixture::dict;

BOOST_TEST_GLOBAL_FIXTURE(Dict_fixture);

BOOST_AUTO_TEST_CASE(empty_dict_test) {
     Dictionary const d;
     BOOST_CHECK(!d.has_entry_utf8(""));
     BOOST_CHECK(!d.has_entry_utf8("a"));
     BOOST_CHECK(!d.has_entry_utf8("-"));
     Setdesc sd;
     d.search_utf8("", sd);
     BOOST_CHECK(sd.size() == 0);
     sd.clear();
     d.search_utf8("a", sd);
     BOOST_CHECK(sd.size() == 0);
     sd.clear();
     d.search_utf8("-", sd);
     BOOST_CHECK(sd.size() == 0);
}

BOOST_AUTO_TEST_CASE(move_constructor_test) {
     istringstream iss(ios_base::in | ios_base::binary);
     iss.str(polish_dict);
     Dictionary d;
     d.load_source_word_file(iss);
     BOOST_CHECK(iss.eof());
     BOOST_CHECK(iss.fail());
     BOOST_CHECK(!iss.bad());
     Dictionary const d1{std::move(d)};
     BOOST_CHECK(!d.has_entry_utf8("tylu"));
     BOOST_CHECK(d1.has_entry_utf8("tylu"));
}

BOOST_AUTO_TEST_CASE(move_assignment_test) {
     istringstream iss(ios_base::in | ios_base::binary);
     iss.str(polish_dict);
     Dictionary d;
     d.load_source_word_file(iss);
     BOOST_CHECK(iss.eof());
     BOOST_CHECK(iss.fail());
     BOOST_CHECK(!iss.bad());
     Dictionary d1;
     d1 = std::move(d);
     BOOST_CHECK(!d.has_entry_utf8("tylu"));
     BOOST_CHECK(d1.has_entry_utf8("tylu"));
}

BOOST_AUTO_TEST_CASE(load_write_test) {
     istringstream iss(ios_base::in | ios_base::binary);
     ostringstream oss(ios_base::out | ios_base::binary);
     Dictionary d;

     iss.str(polish_dict);
     d.load_source_word_file(iss);
     BOOST_CHECK(iss.eof());
     BOOST_CHECK(iss.fail());
     BOOST_CHECK(!iss.bad());

     d.write_source_word_file(oss, false);
     BOOST_CHECK(oss.good());
     BOOST_CHECK(oss.str() == polish_dict);

     oss.str("");
     oss.clear();
     d.write_source_word_file(oss, true);
     BOOST_CHECK(oss.good());
     BOOST_CHECK(oss.str() == polish_dict_sorted);

     oss.str("");
     oss.clear();
     d.write_word_file(oss);
     BOOST_CHECK(oss.good());

     iss.str(oss.str());
     iss.clear();
     d.load_word_file(iss);

     oss.str("");
     oss.clear();
     d.write_source_word_file(oss, false);
     BOOST_CHECK(oss.good());
     BOOST_CHECK(oss.str() == polish_dict);
}

struct Test_data {
     char const* const message;
     char const* const entry;
};

Test_data constexpr bad_data[] = {
     {"invalid entry name, line 1",
      "!masculine-personal noun\n"
      "abderyta\n"
      "abderyty\n"
      "abderycie\n"
      "abderyt\304\231\n"
      "abderyt\304\205\n"
      "abderycie\n"
      "abderyto\n"
      "abderyci\n"
      "abderyt\303\263w\n"
      "abderytom\n"
      "abderyt\303\263w\n"
      "abderytami\n"
      "abderytach\n"
      "abderyci\n"
      "\n"},
     {"not enough forms, line 16",
      "masculine-personal noun\n"
      "abderyta\n"
      "abderyty\n"
      "abderycie\n"
      "abderyt\304\231\n"
      "abderyt\304\205\n"
      "abderycie\n"
      "abderyto\n"
      "abderyci\n"
      "abderyt\303\263w\n"
      "abderytom\n"
      "abderyt\303\263w\n"
      "abderytami\n"
      "abderytach\n"
      "\n"},
     {"all forms empty, line 15",
      "masculine-personal noun\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "-\n"
      "\n"},
     {"character not represented in dictionary, line 2",
      "masculine-personal noun\n"
      "ab\302\241eryta\n"  // inverted exclamation mark
      "abderyty\n"
      "abderycie\n"
      "abderyt\304\231\n"
      "abderyt\304\205\n"
      "abderycie\n"
      "abderyto\n"
      "abderyci\n"
      "abderyt\303\263w\n"
      "abderytom\n"
      "abderyt\303\263w\n"
      "abderytami\n"
      "abderytach\n"
      "abderyci\n"
      "\n"},
     {"invalid Unicode character, line 2",
      "masculine-personal noun\n"
      "ab\370\210\200\200\200eryta\n"
      "abderyty\n"
      "abderycie\n"
      "abderyt\304\231\n"
      "abderyt\304\205\n"
      "abderycie\n"
      "abderyto\n"
      "abderyci\n"
      "abderyt\303\263w\n"
      "abderytom\n"
      "abderyt\303\263w\n"
      "abderytami\n"
      "abderytach\n"
      "abderyci\n"
      "\n"},
};

BOOST_DATA_TEST_CASE(bad_data_test,
                     bdata::xrange(std::size(bad_data)), xr) {
     auto const& t = bad_data[xr];
     auto is_correct = [&t](const Dictionary_error& e) {
          return std::strcmp(e.what(), t.message) == 0;
     };

     istringstream iss(ios_base::in | ios_base::binary);
     Dictionary dict;

     iss.str(t.entry);
     BOOST_CHECK_EXCEPTION(dict.load_source_word_file(iss),
                           Dictionary_error, is_correct);
}

/**
 * Also testing if swf.txt is sorted.
 */
BOOST_AUTO_TEST_CASE(load_big_source_word_file_test) {
     namespace fs = std::filesystem;
     Dictionary dict;
     fs::path const ip = fs::temp_directory_path() / "swf.txt";
     fs::path const op = fs::temp_directory_path() / "swf2.txt";
     std::ifstream f(ip, ios_base::in | ios_base::binary);
     std::ofstream g(op, ios_base::out | ios_base::binary);
     BOOST_REQUIRE(f.good());
     BOOST_REQUIRE(g.good());
     dict.load_source_word_file(f);
     BOOST_REQUIRE(f.eof());
     BOOST_REQUIRE(f.fail());
     BOOST_REQUIRE(!f.bad());
     f.close();
     dict.write_source_word_file(g, true);
     BOOST_REQUIRE(g.good());
     g.close();
     f.open(ip, ios_base::in | ios_base::binary);
     std::ifstream h(op, ios_base::in | ios_base::binary);
     std::istream_iterator<char> b1(f), e1;
     std::istream_iterator<char> b2(h), e2;
     BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
}

// All words from Dict_fixture::dict.
char const* const hetd[] = {
     "\304\207wier\304\207",
     "\305\274\303\263\305\202\304\207",
     "\305\274\303\263\305\202ci",
     "\305\274\303\263\305\202ci\304\205",
     "\305\274\303\263\305\202ciach",
     "\305\274\303\263\305\202ciami",
     "\305\274\303\263\305\202cie",
     "\305\274\303\263\305\202ciom",
     "absurdaln\304\205",
     "absurdalna",
     "absurdalne",
     "absurdalnego",
     "absurdalnej",
     "absurdalnemu",
     "absurdalni",
     "absurdalny",
     "absurdalnych",
     "absurdalnym",
     "absurdalnymi",
     "ach",
     "ale",
     "aligator",
     "aligator\303\263w",
     "aligatora",
     "aligatorach",
     "aligatorami",
     "aligatorem",
     "aligatorom",
     "aligatorowi",
     "aligatory",
     "aligatorze",
     "aliment\303\263w",
     "alimentach",
     "alimentami",
     "alimentom",
     "alimenty",
     "atrakcyjniejsi",
     "atrakcyjniejsz\304\205",
     "atrakcyjniejsza",
     "atrakcyjniejsze",
     "atrakcyjniejszego",
     "atrakcyjniejszej",
     "atrakcyjniejszemu",
     "atrakcyjniejszy",
     "atrakcyjniejszych",
     "atrakcyjniejszym",
     "atrakcyjniejszymi",
     "byle",
     "da",
     "da\304\207",
     "da\305\202",
     "da\305\202a",
     "da\305\202a\305\233",
     "da\305\202aby",
     "da\305\202aby\305\233",
     "da\305\202abym",
     "da\305\202am",
     "da\305\202by",
     "da\305\202by\305\233",
     "da\305\202bym",
     "da\305\202e\305\233",
     "da\305\202em",
     "da\305\202o",
     "da\305\202o\305\233",
     "da\305\202oby",
     "da\305\202oby\305\233",
     "da\305\202obym",
     "da\305\202om",
     "da\305\202y",
     "da\305\202y\305\233cie",
     "da\305\202y\305\233my",
     "da\305\202yby",
     "da\305\202yby\305\233cie",
     "da\305\202yby\305\233my",
     "dacie",
     "dadz\304\205",
     "daj",
     "dajcie",
     "dajmy",
     "dali",
     "dali\305\233cie",
     "dali\305\233my",
     "daliby",
     "daliby\305\233cie",
     "daliby\305\233my",
     "dam",
     "damy",
     "dan\304\205",
     "dana",
     "dane",
     "danego",
     "danej",
     "danemu",
     "dani",
     "dano",
     "dany",
     "danych",
     "danym",
     "danymi",
     "darz\304\205c\304\205",
     "darz\304\205ca",
     "darz\304\205ce",
     "darz\304\205cego",
     "darz\304\205cej",
     "darz\304\205cemu",
     "darz\304\205cy",
     "darz\304\205cych",
     "darz\304\205cym",
     "darz\304\205cymi",
     "dasz",
     "dawszy",
     "decydowa\305\204",
     "decydowania",
     "decydowaniach",
     "decydowaniami",
     "decydowanie",
     "decydowaniem",
     "decydowaniom",
     "decydowaniu",
     "dojrza\305\202\304\205",
     "dojrza\305\202a",
     "dojrza\305\202e",
     "dojrza\305\202ego",
     "dojrza\305\202ej",
     "dojrza\305\202emu",
     "dojrza\305\202y",
     "dojrza\305\202ych",
     "dojrza\305\202ym",
     "dojrza\305\202ymi",
     "dojrzali",
     "dok\304\205d",
     "dok\304\205dkolwiek",
     "doko\305\202a",
     "dop\303\263ki",
     "dwa",
     "dwaj",
     "dwie",
     "dwiema",
     "dwoje",
     "dwojga",
     "dwojgiem",
     "dwojgu",
     "dwoma",
     "dwu",
     "ich",
     "ile",
     "ile\305\233",
     "iloma",
     "iloma\305\233",
     "ilu",
     "ilu\305\233",
     "im",
     "j\304\205",
     "ja",
     "jacy\305\233",
     "jak\304\205\305\233",
     "jaka\305\233",
     "jaki\305\233",
     "jakich\305\233",
     "jakie\305\233",
     "jakiego\305\233",
     "jakiej\305\233",
     "jakiemu\305\233",
     "jakim\305\233",
     "jakimi\305\233",
     "jakow\304\205",
     "jakowa",
     "jakowe",
     "jakowego",
     "jakowej",
     "jakowemu",
     "jakowi",
     "jakowy",
     "jakowych",
     "jakowym",
     "jakowymi",
     "je",
     "jeden",
     "jedn\304\205",
     "jedna",
     "jednego",
     "jednej",
     "jednemu",
     "jedno",
     "jednym",
     "jego",
     "jej",
     "jemu",
     "kim",
     "kim\305\233",
     "kobiecie",
     "kobiet",
     "kobiet\304\205",
     "kobiet\304\231",
     "kobieta",
     "kobietach",
     "kobietami",
     "kobieto",
     "kobietom",
     "kobiety",
     "kogo",
     "kogo\305\233",
     "kogucisk\303\263w",
     "koguciska",
     "koguciskach",
     "koguciskami",
     "koguciskiem",
     "kogucisko",
     "koguciskom",
     "kogucisku",
     "komu",
     "komu\305\233",
     "kto",
     "kto\305\233",
     "ku\304\207",
     "ku\305\202",
     "ku\305\202a",
     "ku\305\202a\305\233",
     "ku\305\202aby",
     "ku\305\202aby\305\233",
     "ku\305\202abym",
     "ku\305\202am",
     "ku\305\202by",
     "ku\305\202by\305\233",
     "ku\305\202bym",
     "ku\305\202e\305\233",
     "ku\305\202em",
     "ku\305\202o",
     "ku\305\202o\305\233",
     "ku\305\202oby",
     "ku\305\202oby\305\233",
     "ku\305\202obym",
     "ku\305\202om",
     "ku\305\202y",
     "ku\305\202y\305\233cie",
     "ku\305\202y\305\233my",
     "ku\305\202yby",
     "ku\305\202yby\305\233cie",
     "ku\305\202yby\305\233my",
     "kuj",
     "kuj\304\205",
     "kuj\304\205c",
     "kuj\304\231",
     "kujcie",
     "kuje",
     "kujecie",
     "kujemy",
     "kujesz",
     "kujmy",
     "kuli",
     "kuli\305\233cie",
     "kuli\305\233my",
     "kuliby",
     "kuliby\305\233cie",
     "kuliby\305\233my",
     "kuto",
     "m\303\263j",
     "ma\305\202p",
     "ma\305\202p\304\205",
     "ma\305\202p\304\231",
     "ma\305\202pa",
     "ma\305\202pach",
     "ma\305\202pami",
     "ma\305\202pi",
     "ma\305\202pi\304\205",
     "ma\305\202pia",
     "ma\305\202pich",
     "ma\305\202pie",
     "ma\305\202piego",
     "ma\305\202piej",
     "ma\305\202piemu",
     "ma\305\202pim",
     "ma\305\202pimi",
     "ma\305\202po",
     "ma\305\202pom",
     "ma\305\202py",
     "mn\304\205",
     "mnie",
     "moi",
     "moich",
     "moim",
     "moimi",
     "moj\304\205",
     "moja",
     "moje",
     "mojego",
     "mojej",
     "mojemu",
     "my",
     "najatrakcyjniejsi",
     "najatrakcyjniejsz\304\205",
     "najatrakcyjniejsza",
     "najatrakcyjniejsze",
     "najatrakcyjniejszego",
     "najatrakcyjniejszej",
     "najatrakcyjniejszemu",
     "najatrakcyjniejszy",
     "najatrakcyjniejszych",
     "najatrakcyjniejszym",
     "najatrakcyjniejszymi",
     "najniewinniej",
     "nam",
     "nami",
     "nas",
     "ni\304\205",
     "nic",
     "nich",
     "niczego",
     "niczemu",
     "niczyi",
     "niczyich",
     "niczyim",
     "niczyimi",
     "niczyj",
     "niczyj\304\205",
     "niczyja",
     "niczyje",
     "niczyjego",
     "niczyjej",
     "niczyjemu",
     "niczym",
     "niej",
     "niewinnie",
     "niewinniej",
     "nigdy",
     "nim",
     "nimi",
     "on",
     "ona",
     "one",
     "oni",
     "ono",
     "p\303\263\305\202tora",
     "p\303\263\305\202torej",
     "pierwsi",
     "pierwsz\304\205",
     "pierwsza",
     "pierwsze",
     "pierwszego",
     "pierwszej",
     "pierwszemu",
     "pierwszy",
     "pierwszych",
     "pierwszym",
     "pierwszymi",
     "profesor",
     "profesor\303\263w",
     "profesora",
     "profesorach",
     "profesorami",
     "profesorem",
     "profesorom",
     "profesorowi",
     "profesorowie",
     "profesorze",
     "profesorzy",
     "raczej",
     "si\304\231",
     "sob\304\205",
     "sobie",
     "takow\304\205",
     "takowa",
     "takowe",
     "takowego",
     "takowej",
     "takowemu",
     "takowi",
     "takowy",
     "takowych",
     "takowym",
     "takowymi",
     "teatr",
     "teatr\303\263w",
     "teatrach",
     "teatrami",
     "teatrem",
     "teatrom",
     "teatrowi",
     "teatru",
     "teatry",
     "teatrze",
     "trojacy",
     "trojak\304\205",
     "trojaka",
     "trojaki",
     "trojakich",
     "trojakie",
     "trojakiego",
     "trojakiej",
     "trojakiemu",
     "trojakim",
     "trojakimi",
     "trzech",
     "trzej",
     "trzem",
     "trzema",
     "trzy",
     "trzykrotn\304\205",
     "trzykrotna",
     "trzykrotne",
     "trzykrotnego",
     "trzykrotnej",
     "trzykrotnemu",
     "trzykrotni",
     "trzykrotny",
     "trzykrotnych",
     "trzykrotnym",
     "trzykrotnymi",
     "tyle",
     "tyloma",
     "tylu",
     "tysi\304\205c",
     "tysi\304\205ca",
     "tysi\304\205cach",
     "tysi\304\205cami",
     "tysi\304\205ce",
     "tysi\304\205cem",
     "tysi\304\205com",
     "tysi\304\205cowi",
     "tysi\304\205cu",
     "tysi\304\231cy",
     "wo\305\272nego",
     "wo\305\272nemu",
     "wo\305\272ni",
     "wo\305\272ny",
     "wo\305\272nych",
     "wo\305\272nym",
     "wo\305\272nymi",
};

BOOST_DATA_TEST_CASE(has_entry_utf8_test,
                     bdata::xrange(std::size(hetd)), xr) {
     BOOST_CHECK(Dict_fixture::dict.has_entry_utf8(hetd[xr]));
}

BOOST_AUTO_TEST_CASE(has_entry_utf8_negative_test) {
     BOOST_CHECK(!Dict_fixture::dict.has_entry_utf8("trojakieg"));
     BOOST_CHECK(!Dict_fixture::dict.has_entry_utf8(""));
     BOOST_CHECK(!Dict_fixture::dict.has_entry_utf8("-"));
     BOOST_CHECK(!Dict_fixture::dict.has_entry_utf8("nego"));
     BOOST_CHECK(!Dict_fixture::dict.has_entry_utf8("nemu"));
     BOOST_CHECK(!Dict_fixture::dict.has_entry_utf8("liby"));
}

BOOST_AUTO_TEST_CASE(search_utf8_test) {
     Setdesc sd;
     Dict_fixture::dict.search_utf8("teatrowi", sd);
     BOOST_CHECK(sd.size() == 1);
     sd.clear();
     Dict_fixture::dict.search_utf8("trojakiemu", sd);
     BOOST_CHECK(sd.size() == 3);
     sd.clear();
     Dict_fixture::dict.search_utf8("trojakiego", sd);
     BOOST_CHECK(sd.size() == 4);
     sd.clear();
     Dict_fixture::dict.search_utf8("tylu", sd);
     BOOST_CHECK(sd.size() == 9);
     sd.clear();
     Dict_fixture::dict.search_utf8("teatro", sd);
     BOOST_CHECK(sd.size() == 0);
     sd.clear();
     Dict_fixture::dict.search_utf8("", sd);
     BOOST_CHECK(sd.size() == 0);
}

struct Category_test_data {
     char const* const s;
     std::vector<SHG::PLP::Category> const v;
};

Category_test_data const ctd[]{
     {
          "wo\305\272ny",
          {{Part_of_speech::noun, Inflexion::substantival,
            Declension_case::nominative, Number::singular,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::noun, Inflexion::substantival,
            Declension_case::vocative, Number::singular,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "wo\305\272nego",
          {{Part_of_speech::noun, Inflexion::substantival,
            Declension_case::genitive, Number::singular,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::noun, Inflexion::substantival,
            Declension_case::accusative, Number::singular,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "wo\305\272nemu",
          {{Part_of_speech::noun, Inflexion::substantival,
            Declension_case::dative, Number::singular,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "wo\305\272nym",
          {{Part_of_speech::noun, Inflexion::substantival,
            Declension_case::instrumental, Number::singular,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::noun, Inflexion::substantival,
            Declension_case::locative, Number::singular,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::noun, Inflexion::substantival,
            Declension_case::dative, Number::plural,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "wo\305\272ni",
          {{Part_of_speech::noun, Inflexion::substantival,
            Declension_case::nominative, Number::plural,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::noun, Inflexion::substantival,
            Declension_case::vocative, Number::plural,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "wo\305\272nych",
          {{Part_of_speech::noun, Inflexion::substantival,
            Declension_case::genitive, Number::plural,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::noun, Inflexion::substantival,
            Declension_case::accusative, Number::plural,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::noun, Inflexion::substantival,
            Declension_case::locative, Number::plural,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "wo\305\272nymi",
          {{Part_of_speech::noun, Inflexion::substantival,
            Declension_case::instrumental, Number::plural,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "absurdalny",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::nominative, Number::singular,
            Gender::masculine_animate, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::vocative, Number::singular,
            Gender::masculine_animate, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::nominative, Number::singular,
            Gender::masculine_inanimate, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::accusative, Number::singular,
            Gender::masculine_inanimate, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::vocative, Number::singular,
            Gender::masculine_inanimate, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "absurdalnego",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::genitive, Number::singular,
            Gender::masculine_animate, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::accusative, Number::singular,
            Gender::masculine_animate, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::genitive, Number::singular,
            Gender::masculine_inanimate, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::genitive, Number::singular,
            Gender::neuter, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "absurdalnemu",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::dative, Number::singular,
            Gender::masculine_animate, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::dative, Number::singular,
            Gender::masculine_inanimate, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::dative, Number::singular, Gender::neuter,
            Degree::positive, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "absurdalnym",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::instrumental, Number::singular,
            Gender::masculine_animate, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::locative, Number::singular,
            Gender::masculine_animate, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::instrumental, Number::singular,
            Gender::masculine_inanimate, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::locative, Number::singular,
            Gender::masculine_inanimate, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::instrumental, Number::singular,
            Gender::neuter, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::locative, Number::singular,
            Gender::neuter, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::dative, Number::plural,
            Gender::masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::dative, Number::plural,
            Gender::non_masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "absurdalna",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::nominative, Number::singular,
            Gender::feminine, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::vocative, Number::singular,
            Gender::feminine, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "absurdalnej",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::genitive, Number::singular,
            Gender::feminine, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::dative, Number::singular,
            Gender::feminine, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::locative, Number::singular,
            Gender::feminine, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "absurdaln\304\205",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::accusative, Number::singular,
            Gender::feminine, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::instrumental, Number::singular,
            Gender::feminine, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "absurdalne",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::nominative, Number::singular,
            Gender::neuter, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::accusative, Number::singular,
            Gender::neuter, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::vocative, Number::singular,
            Gender::neuter, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::nominative, Number::plural,
            Gender::non_masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::accusative, Number::plural,
            Gender::non_masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::vocative, Number::plural,
            Gender::non_masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "absurdalni",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::nominative, Number::plural,
            Gender::masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::vocative, Number::plural,
            Gender::masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "absurdalnych",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::genitive, Number::plural,
            Gender::masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::accusative, Number::plural,
            Gender::masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::locative, Number::plural,
            Gender::masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::genitive, Number::plural,
            Gender::non_masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::locative, Number::plural,
            Gender::non_masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "absurdalnymi",
          {{Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::instrumental, Number::plural,
            Gender::masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::instrumental, Number::plural,
            Gender::non_masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "trzy",
          {{Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::nominative, Number::none,
            Gender::non_masculine_personal, Degree::none,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::accusative, Number::none,
            Gender::non_masculine_personal, Degree::none,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::vocative, Number::none,
            Gender::non_masculine_personal, Degree::none,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal}},
     },
     {
          "trzech",
          {{Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::genitive, Number::none,
            Gender::non_masculine_personal, Degree::none,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::locative, Number::none,
            Gender::non_masculine_personal, Degree::none,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::genitive, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::accusative, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::locative, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal}},
     },
     {
          "trzem",
          {{Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::dative, Number::none,
            Gender::non_masculine_personal, Degree::none,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::dative, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal}},
     },
     {
          "trzema",
          {{Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::instrumental, Number::none,
            Gender::non_masculine_personal, Degree::none,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::instrumental, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal}},
     },
     {
          "trzej",
          {{Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::nominative, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral,
            Declension_case::vocative, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal}},
     },
     {
          "jeden",
          {{Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::nominative, Number::none,
            Gender::masculine_animate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::vocative, Number::none,
            Gender::masculine_animate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::nominative, Number::none,
            Gender::masculine_inanimate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::accusative, Number::none,
            Gender::masculine_inanimate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::vocative, Number::none,
            Gender::masculine_inanimate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal}},
     },
     {
          "jednego",
          {{Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::genitive, Number::none,
            Gender::masculine_animate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::accusative, Number::none,
            Gender::masculine_animate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::genitive, Number::none,
            Gender::masculine_inanimate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::genitive, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal}},
     },
     {
          "jednemu",
          {{Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::dative, Number::none,
            Gender::masculine_animate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::dative, Number::none,
            Gender::masculine_inanimate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::dative, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal}},
     },
     {
          "jednym",
          {{Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::instrumental, Number::none,
            Gender::masculine_animate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::locative, Number::none,
            Gender::masculine_animate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::instrumental, Number::none,
            Gender::masculine_inanimate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::locative, Number::none,
            Gender::masculine_inanimate, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::instrumental, Number::none,
            Gender::neuter, Degree::none, Aspect::none, Mood::none,
            Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::locative, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal}},
     },
     {
          "jedna",
          {{Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::nominative, Number::none,
            Gender::feminine, Degree::none, Aspect::none, Mood::none,
            Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::vocative, Number::none, Gender::feminine,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal}},
     },
     {
          "jednej",
          {{Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::genitive, Number::none, Gender::feminine,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::dative, Number::none, Gender::feminine,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::locative, Number::none, Gender::feminine,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal}},
     },
     {
          "jedn\304\205",
          {{Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::accusative, Number::none,
            Gender::feminine, Degree::none, Aspect::none, Mood::none,
            Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::instrumental, Number::none,
            Gender::feminine, Degree::none, Aspect::none, Mood::none,
            Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal}},
     },
     {
          "jedno",
          {{Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::nominative, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::accusative, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_1,
            Declension_case::vocative, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal}},
     },
     {
          "dwa",
          {{Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::nominative, Number::none,
            Gender::masculine_impersonal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::accusative, Number::none,
            Gender::masculine_impersonal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::vocative, Number::none,
            Gender::masculine_impersonal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::nominative, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::accusative, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::vocative, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal}},
     },
     {
          "dwu",
          {{Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::genitive, Number::none,
            Gender::masculine_impersonal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::dative, Number::none,
            Gender::masculine_impersonal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::locative, Number::none,
            Gender::masculine_impersonal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::genitive, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::dative, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::accusative, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::locative, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::genitive, Number::none, Gender::feminine,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::dative, Number::none, Gender::feminine,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::locative, Number::none, Gender::feminine,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::genitive, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::dative, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::locative, Number::none, Gender::neuter,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal}},
     },
     {
          "dwoma",
          {{Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::instrumental, Number::none,
            Gender::masculine_impersonal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::instrumental, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::instrumental, Number::none,
            Gender::neuter, Degree::none, Aspect::none, Mood::none,
            Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal}},
     },
     {
          "dwaj",
          {{Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::nominative, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::vocative, Number::none,
            Gender::masculine_personal, Degree::none, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal}},
     },
     {
          "dwie",
          {{Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::nominative, Number::none,
            Gender::feminine, Degree::none, Aspect::none, Mood::none,
            Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::accusative, Number::none,
            Gender::feminine, Degree::none, Aspect::none, Mood::none,
            Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal},
           {Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::vocative, Number::none, Gender::feminine,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::cardinal}},
     },
     {
          "dwiema",
          {{Part_of_speech::numeral, Inflexion::numeral_2,
            Declension_case::instrumental, Number::none,
            Gender::feminine, Degree::none, Aspect::none, Mood::none,
            Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::cardinal}},
     },
     {
          "nic",
          {{Part_of_speech::pronoun, Inflexion::by_case,
            Declension_case::nominative, Number::none, Gender::none,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none,
            Type_of_pronoun::negative, Type_of_numeral::none},
           {Part_of_speech::pronoun, Inflexion::by_case,
            Declension_case::accusative, Number::none, Gender::none,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none,
            Type_of_pronoun::negative, Type_of_numeral::none},
           {Part_of_speech::pronoun, Inflexion::by_case,
            Declension_case::vocative, Number::none, Gender::none,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none,
            Type_of_pronoun::negative, Type_of_numeral::none}},
     },
     {
          "niczego",
          {{Part_of_speech::pronoun, Inflexion::by_case,
            Declension_case::genitive, Number::none, Gender::none,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none,
            Type_of_pronoun::negative, Type_of_numeral::none}},
     },
     {
          "niczemu",
          {{Part_of_speech::pronoun, Inflexion::by_case,
            Declension_case::dative, Number::none, Gender::none,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none,
            Type_of_pronoun::negative, Type_of_numeral::none}},
     },
     {
          "niczym",
          {{Part_of_speech::pronoun, Inflexion::by_case,
            Declension_case::instrumental, Number::none, Gender::none,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none,
            Type_of_pronoun::negative, Type_of_numeral::none},
           {Part_of_speech::pronoun, Inflexion::by_case,
            Declension_case::locative, Number::none, Gender::none,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none,
            Type_of_pronoun::negative, Type_of_numeral::none}},
     },
     {
          "p\303\263\305\202tora",
          {{Part_of_speech::numeral, Inflexion::by_gender,
            Declension_case::none, Number::none, Gender::non_feminine,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::fractional}},
     },
     {
          "p\303\263\305\202torej",
          {{Part_of_speech::numeral, Inflexion::by_gender,
            Declension_case::none, Number::none, Gender::feminine,
            Degree::none, Aspect::none, Mood::none, Tense::none,
            Person::none, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::fractional}},
     },
     {
          "ku\304\207",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::none, Gender::none,
            Degree::none, Aspect::imperfect, Mood::none, Tense::none,
            Person::none, Form_of_verb::infinitive,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kuj\304\231",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular, Gender::none,
            Degree::none, Aspect::imperfect, Mood::indicative,
            Tense::present, Person::first, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kujesz",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular, Gender::none,
            Degree::none, Aspect::imperfect, Mood::indicative,
            Tense::present, Person::second, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kuje",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular, Gender::none,
            Degree::none, Aspect::imperfect, Mood::indicative,
            Tense::present, Person::third, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kujemy",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural, Gender::none,
            Degree::none, Aspect::imperfect, Mood::indicative,
            Tense::present, Person::first, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kujecie",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural, Gender::none,
            Degree::none, Aspect::imperfect, Mood::indicative,
            Tense::present, Person::second, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kuj\304\205",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural, Gender::none,
            Degree::none, Aspect::imperfect, Mood::indicative,
            Tense::present, Person::third, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kuj",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular, Gender::none,
            Degree::none, Aspect::imperfect, Mood::imperative,
            Tense::none, Person::second, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kujmy",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural, Gender::none,
            Degree::none, Aspect::imperfect, Mood::imperative,
            Tense::none, Person::first, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kujcie",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural, Gender::none,
            Degree::none, Aspect::imperfect, Mood::imperative,
            Tense::none, Person::second, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "ku\305\202bym",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular,
            Gender::masculine, Degree::none, Aspect::imperfect,
            Mood::subjunctive, Tense::none, Person::first,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "ku\305\202by\305\233",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular,
            Gender::masculine, Degree::none, Aspect::imperfect,
            Mood::subjunctive, Tense::none, Person::second,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "ku\305\202by",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular,
            Gender::masculine, Degree::none, Aspect::imperfect,
            Mood::subjunctive, Tense::none, Person::third,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "ku\305\202abym",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular, Gender::feminine,
            Degree::none, Aspect::imperfect, Mood::subjunctive,
            Tense::none, Person::first, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "ku\305\202aby\305\233",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular, Gender::feminine,
            Degree::none, Aspect::imperfect, Mood::subjunctive,
            Tense::none, Person::second, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "ku\305\202aby",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular, Gender::feminine,
            Degree::none, Aspect::imperfect, Mood::subjunctive,
            Tense::none, Person::third, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "ku\305\202obym",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular, Gender::neuter,
            Degree::none, Aspect::imperfect, Mood::subjunctive,
            Tense::none, Person::first, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "ku\305\202oby\305\233",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular, Gender::neuter,
            Degree::none, Aspect::imperfect, Mood::subjunctive,
            Tense::none, Person::second, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "ku\305\202oby",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::singular, Gender::neuter,
            Degree::none, Aspect::imperfect, Mood::subjunctive,
            Tense::none, Person::third, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kuliby\305\233my",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural,
            Gender::masculine_personal, Degree::none,
            Aspect::imperfect, Mood::subjunctive, Tense::none,
            Person::first, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "kuliby\305\233cie",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural,
            Gender::masculine_personal, Degree::none,
            Aspect::imperfect, Mood::subjunctive, Tense::none,
            Person::second, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "kuliby",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural,
            Gender::masculine_personal, Degree::none,
            Aspect::imperfect, Mood::subjunctive, Tense::none,
            Person::third, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "ku\305\202yby\305\233my",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural,
            Gender::non_masculine_personal, Degree::none,
            Aspect::imperfect, Mood::subjunctive, Tense::none,
            Person::first, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "ku\305\202yby\305\233cie",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural,
            Gender::non_masculine_personal, Degree::none,
            Aspect::imperfect, Mood::subjunctive, Tense::none,
            Person::second, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "ku\305\202yby",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::plural,
            Gender::non_masculine_personal, Degree::none,
            Aspect::imperfect, Mood::subjunctive, Tense::none,
            Person::third, Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
     {
          "kuto",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::none, Gender::none,
            Degree::none, Aspect::imperfect, Mood::none, Tense::none,
            Person::none, Form_of_verb::impersonal_form_of_past_tense,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "kuj\304\205c",
          {{Part_of_speech::verb, Inflexion::conjugation,
            Declension_case::none, Number::none, Gender::none,
            Degree::none, Aspect::imperfect, Mood::none, Tense::none,
            Person::none,
            Form_of_verb::simultaneous_adverbial_participle,
            Type_of_pronoun::none, Type_of_numeral::none}},
     },
     {
          "ma\305\202pie",
          {{Part_of_speech::noun, Inflexion::substantival,
            Declension_case::dative, Number::singular,
            Gender::feminine, Degree::none, Aspect::none, Mood::none,
            Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::noun, Inflexion::substantival,
            Declension_case::locative, Number::singular,
            Gender::feminine, Degree::none, Aspect::none, Mood::none,
            Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::nominative, Number::singular,
            Gender::neuter, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::accusative, Number::singular,
            Gender::neuter, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::vocative, Number::singular,
            Gender::neuter, Degree::positive, Aspect::none,
            Mood::none, Tense::none, Person::none, Form_of_verb::none,
            Type_of_pronoun::none, Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::nominative, Number::plural,
            Gender::non_masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::accusative, Number::plural,
            Gender::non_masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none},
           {Part_of_speech::adjective, Inflexion::adjectival,
            Declension_case::vocative, Number::plural,
            Gender::non_masculine_personal, Degree::positive,
            Aspect::none, Mood::none, Tense::none, Person::none,
            Form_of_verb::none, Type_of_pronoun::none,
            Type_of_numeral::none}},
     },
};

BOOST_DATA_TEST_CASE(category_test, bdata::xrange(std::size(ctd)),
                     xr) {
     Setdesc sd;
     Dict_fixture::dict.search_utf8(ctd[xr].s, sd);
     std::vector<Category> vc;
     for (auto const& d : sd)
          vc.push_back(d.category);
     BOOST_CHECK(SHG::have_equal_content(vc, ctd[xr].v));
}

BOOST_AUTO_TEST_CASE(main_form_test) {
     Setdesc sd;
     Dict_fixture::dict.search_utf8("trzy", sd);
     BOOST_CHECK(sd.size() > 0);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "trzy");
     sd.clear();
     Dict_fixture::dict.search_utf8("trzech", sd);
     BOOST_CHECK(sd.size() > 0);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "trzy");
     sd.clear();
     Dict_fixture::dict.search_utf8("trzem", sd);
     BOOST_CHECK(sd.size() > 0);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "trzy");
     sd.clear();
     Dict_fixture::dict.search_utf8("trzema", sd);
     BOOST_CHECK(sd.size() > 0);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "trzy");
     sd.clear();
     Dict_fixture::dict.search_utf8("trzej", sd);
     BOOST_CHECK(sd.size() > 0);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "trzy");

     sd.clear();
     Dict_fixture::dict.search_utf8("alimenty", sd);
     BOOST_CHECK(sd.size() > 0);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "alimenty");
     sd.clear();
     Dict_fixture::dict.search_utf8("aliment\303\263w", sd);
     BOOST_CHECK(sd.size() > 0);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "alimenty");
     sd.clear();
     Dict_fixture::dict.search_utf8("alimentom", sd);
     BOOST_CHECK(sd.size() > 0);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "alimenty");
     sd.clear();
     Dict_fixture::dict.search_utf8("alimentami", sd);
     BOOST_CHECK(sd.size() > 0);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "alimenty");
     sd.clear();
     Dict_fixture::dict.search_utf8("alimentach", sd);
     BOOST_CHECK(sd.size() > 0);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "alimenty");

     sd.clear();
     Dict_fixture::dict.search_utf8("ale", sd);
     BOOST_CHECK(sd.size() == 1);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "ale");

     sd.clear();
     Dict_fixture::dict.search_utf8("wo\305\272nymi", sd);
     BOOST_CHECK(sd.size() == 1);
     for (auto const& d : sd)
          BOOST_CHECK(d.main_form == "wo\305\272ny");
}

BOOST_AUTO_TEST_CASE(generate_all_categories_test) {
     auto const v = generate_all_categories();
     BOOST_CHECK(v.size() == 1002);
     // std::cout << v.size() << '\n';
}

BOOST_AUTO_TEST_CASE(cat_to_string_test) {
     using Index = std::vector<std::string>::size_type;
     auto const v = generate_all_categories();
     BOOST_CHECK(v.size() == 1002);
     std::vector<std::string> w;
     for (auto const& c : v)
          w.push_back(to_string(c));
     for (Index i = 0; i < w.size(); i++)
          for (Index j = i + 1; j < w.size(); j++)
               BOOST_CHECK(w[i] != w[j]);
     // for (auto const& c : w)
     //     std::cout << c << '\n';
}

BOOST_AUTO_TEST_CASE(empty_dict_report_test) {
     Dictionary const dict;
     auto const v = dict.report();
     BOOST_CHECK(v.size() == 51);
     for (auto const& et : v)
          BOOST_CHECK(et.nentries == 0);
}

BOOST_AUTO_TEST_CASE(report_test) {
     using Index = std::vector<Dictionary::Table_row>::size_type;
     auto const v = Dict_fixture::dict.report();
     BOOST_CHECK(v.size() == 51);
     for (Index i = 0; i < v.size(); i++)
          switch (i) {
          case 0:
               BOOST_CHECK(v[i].nentries == 3);
               break;
          case 3:
               BOOST_CHECK(v[i].nentries == 4);
               break;
          case 6:
               BOOST_CHECK(v[i].nentries == 2);
               break;
          default:
               BOOST_CHECK(v[i].nentries == 1);
               break;
          }
}

/**
 * Displays report on swf.txt.
 */
void swf_stat() {
     namespace fs = std::filesystem;
     Dictionary dict;
     fs::path const ip = fs::temp_directory_path() / "swf.txt";
     std::ifstream f(ip, ios_base::in | ios_base::binary);
     BOOST_REQUIRE(f.good());
     dict.load_source_word_file(f);
     BOOST_CHECK(f.eof());
     BOOST_CHECK(f.fail());
     BOOST_CHECK(!f.bad());
     auto const v = dict.report();
     BOOST_CHECK(v.size() == 51);
     unsigned long total = 0;
     for (auto const& r : v) {
          std::cout << std::setw(53) << std::left << r.entry_type
                    << "   " << std::setw(7) << std::right
                    << r.nentries << '\n';
          total += r.nentries;
     }
     std::cout << std::setw(53) << std::left << "total"
               << "   " << std::setw(7) << std::right << total
               << '\n';
}

BOOST_AUTO_TEST_CASE(setdesc_test) {
     constexpr auto const res = R"(
trojaki  numeral:multiple:adjectival:masculine_inanimate:dative:singular
trojaki  numeral:multiple:adjectival:neuter:dative:singular
trojaki  numeral:multiple:adjectival:masculine_animate:dative:singular
)";
     Setdesc sd;
     Dict_fixture::dict.search_utf8("trojakiemu", sd);
     BOOST_CHECK(sd.size() == 3);
     std::string s{'\n'};
     for (auto const& w : sd)
          s += w.main_form + "  " + to_string(w.category) + '\n';
     BOOST_CHECK(s == res);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
