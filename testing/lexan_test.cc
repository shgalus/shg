#include <shg/lexan.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <shg/utils.h>
#include <shg/encoding.h>
#include "testing.h"

namespace SHG::Testing {

struct Basic_lexer {
     Basic_lexer();
     SHG::PLP::Lexer lexer{};
};

Basic_lexer::Basic_lexer() {
     using std::ios_base;
     namespace fs = std::filesystem;
     SHG::PLP::Dictionary dict;

     fs::path const ip = fs::path(datadir) / "basic.swf";
     fs::path const op = fs::temp_directory_path() / "basic.wf";
     std::ifstream f(ip, bininp);
     BOOST_REQUIRE(f.good());
     dict.load_source_word_file(f);
     BOOST_REQUIRE(f.eof());
     BOOST_REQUIRE(!f.bad());
     f.close();
     std::ofstream g(op, binout);
     BOOST_REQUIRE(g.good());
     dict.write_word_file(g);
     BOOST_REQUIRE(g.good());
     g.close();
     BOOST_REQUIRE(lexer.load_dict(op.string().c_str()));
}

BOOST_FIXTURE_TEST_SUITE(lexan_test, Basic_lexer)

using std::istringstream;
using std::ostringstream;
using namespace SHG::PLP;

BOOST_AUTO_TEST_CASE(lexer_basic_test) {
     istringstream iss(bininp);
     iss.str("Bogumi\305\202 jedzie do lasu na rowerze 20 km/godz.");
     lexer.reset(iss);
     BOOST_CHECK(lexer.stream().good());
     int n = 0;
     while (!lexer.get_token().empty())
          n++;
     BOOST_CHECK(n == 11);
     BOOST_CHECK(lexer.stream().eof());
     BOOST_CHECK(lexer.stream().fail());
     BOOST_CHECK(!lexer.stream().bad());

     iss.clear();
     iss.str("");
     lexer.reset(iss);
     BOOST_CHECK(lexer.stream().good());
     n = 0;
     while (!lexer.get_token().empty())
          n++;
     BOOST_CHECK(n == 0);
     BOOST_CHECK(lexer.stream().eof());
     BOOST_CHECK(lexer.stream().fail());
     BOOST_CHECK(!lexer.stream().bad());
}

BOOST_AUTO_TEST_CASE(lexer_solaris_test) {
     namespace fs = std::filesystem;
     fs::path const ip = fs::path(datadir) / "solaris.txt";
     std::ifstream f(ip, bininp);
     BOOST_REQUIRE(f.good());
     lexer.reset(f);
     int n = 0;
     while (!lexer.get_token().empty())
          n++;
     BOOST_REQUIRE(f.eof() && !f.bad());
     BOOST_CHECK(n == 1284);
}

BOOST_AUTO_TEST_CASE(lexer_chlopcy_test) {
     namespace fs = std::filesystem;
     fs::path const ip = fs::path(datadir) / "chlopcy.txt";
     std::ifstream f(ip, bininp);
     BOOST_REQUIRE(f.good());
     lexer.reset(f);
     int n = 0;
     while (!lexer.get_token().empty())
          n++;
     BOOST_REQUIRE(f.eof() && !f.bad());
     BOOST_CHECK(n == 1252);
}

BOOST_AUTO_TEST_CASE(chlopcy_by_sentence_test) {
     namespace fs = std::filesystem;
     fs::path const ip = fs::path(datadir) / "chlopcy.txt";
     std::vector<Token> vt, vs;

     std::ifstream f(ip, bininp);
     BOOST_REQUIRE(f.good());
     lexer.reset(f);
     for (;;) {
          auto const tok = lexer.get_token();
          if (tok.empty())
               break;
          vt.push_back(tok);
     }
     BOOST_REQUIRE(f.eof() && !f.bad());
     f.close();
     f.open(ip, bininp);
     BOOST_REQUIRE(f.good());
     for (;;) {
          auto const s = get_sentence(f);
          if (s.empty())
               break;
          auto const v = tokenize_string(s, lexer);
          vs.insert(vs.end(), v.cbegin(), v.cend());
     }
     BOOST_REQUIRE(f.eof() && !f.bad());
     f.close();
     BOOST_CHECK(vs == vt);
}

BOOST_AUTO_TEST_CASE(check_numerals_test) {
     istringstream iss(bininp);
     iss.str(
          "Tysi\304\205ckrotnie studwukrotnie "
          "dwudziestotrzykrotnie osiemnastokrotnie "
          "jednokrotnie");
     lexer.reset(iss);
     for (;;) {
          auto const tok = lexer.get_token();
          if (tok.empty())
               break;
          BOOST_REQUIRE(tok.tags.size() == 1);
          BOOST_CHECK(tok.tags[0] == "adverb:positive");
     }
     BOOST_REQUIRE(iss.eof() && !iss.bad());
}

constexpr char const* const simple_test_data =
     "Jan idzie piechot\304\205 do domu.\n"
     "Piotr jedzie rowerem do kina.\n"
     "Maria je chleb.\n"
     "Dziewczynka jad\305\202a chleb z mas\305\202em.\n"
     "Ch\305\202opiec wypije szklank\304\231 wody.\n"
     "Jan by\305\202 w kinie, a Piotr by\305\202 w teatrze.\n"
     "Poniewa\305\274 zacz\304\205\305\202 poda\304\207 deszcz, Jan "
     "roz\305\202o\305\274y\305\202 parasol i schowa\305\202 "
     "si\304\231 pod dachem.\n"
     "Maria kupi\305\202a sobie zapiekank\304\231, poniewa\305\274 "
     "by\305\202a g\305\202odna.\n"
     "Jan szybko zjad\305\202 pyszn\304\205 zup\304\231 "
     "grzybow\304\205.\n";

// clang-format off

constexpr char const* const simple_test_data_result =
     "Jan               noun:masculine_personal:nominative:singular\n"
     "                  noun:feminine:genitive:plural\n"
     "idzie             verb:imperfect:indicative:present:singular:third\n"
     "piechot\304\205          noun:feminine:instrumental:singular\n"
     "do                preposition\n"
     "domu              noun:masculine_inanimate:genitive:singular\n"
     "                  noun:masculine_inanimate:locative:singular\n"
     "                  noun:masculine_inanimate:vocative:singular\n"
     ".                 full_stop\n"
     "Piotr             noun:masculine_personal:nominative:singular\n"
     "jedzie            verb:imperfect:indicative:present:singular:third\n"
     "rowerem           noun:masculine_inanimate:instrumental:singular\n"
     "do                preposition\n"
     "kina              noun:neuter:nominative:plural\n"
     "                  noun:neuter:genitive:singular\n"
     "                  noun:neuter:accusative:plural\n"
     "                  noun:neuter:vocative:plural\n"
     ".                 full_stop\n"
     "Maria             noun:feminine:nominative:singular\n"
     "je                none\n"
     "                  verb:imperfect:indicative:present:singular:third\n"
     "                  pronoun:personal:adjectival:neuter:accusative:singular\n"
     "                  pronoun:personal:adjectival:non_masculine_personal:accusative:plural\n"
     "chleb             noun:masculine_inanimate:nominative:singular\n"
     "                  noun:masculine_inanimate:accusative:singular\n"
     ".                 full_stop\n"
     "Dziewczynka       noun:feminine:nominative:singular\n"
     "jad\305\202a             noun:neuter:nominative:plural\n"
     "                  noun:neuter:genitive:singular\n"
     "                  noun:neuter:accusative:plural\n"
     "                  noun:neuter:vocative:plural\n"
     "                  verb:imperfect:indicative:past:feminine:singular:third\n"
     "chleb             noun:masculine_inanimate:nominative:singular\n"
     "                  noun:masculine_inanimate:accusative:singular\n"
     "z                 preposition\n"
     "mas\305\202em            noun:neuter:instrumental:singular\n"
     ".                 full_stop\n"
     "Ch\305\202opiec          noun:masculine_personal:nominative:singular\n"
     "wypije            verb:perfect:indicative:future:singular:third\n"
     "szklank\304\231          noun:feminine:accusative:singular\n"
     "wody              noun:feminine:nominative:plural\n"
     "                  noun:feminine:genitive:singular\n"
     "                  noun:feminine:accusative:plural\n"
     "                  noun:feminine:vocative:plural\n"
     ".                 full_stop\n"
     "Jan               noun:masculine_personal:nominative:singular\n"
     "                  noun:feminine:genitive:plural\n"
     "by\305\202               verb:imperfect:indicative:past:masculine:singular:third\n"
     "                  verb:perfect:indicative:past:masculine:singular:third\n"
     "w                 preposition\n"
     "kinie             noun:neuter:locative:singular\n"
     ",                 comma\n"
     "a                 conjunction\n"
     "                  particle\n"
     "Piotr             noun:masculine_personal:nominative:singular\n"
     "by\305\202               verb:imperfect:indicative:past:masculine:singular:third\n"
     "                  verb:perfect:indicative:past:masculine:singular:third\n"
     "w                 preposition\n"
     "teatrze           noun:masculine_inanimate:locative:singular\n"
     "                  noun:masculine_inanimate:vocative:singular\n"
     ".                 full_stop\n"
     "Poniewa\305\274          conjunction\n"
     "zacz\304\205\305\202            verb:perfect:indicative:past:masculine:singular:third\n"
     "poda\304\207             verb:perfect:infinitive\n"
     "deszcz            noun:masculine_inanimate:nominative:singular\n"
     "                  noun:masculine_inanimate:accusative:singular\n"
     ",                 comma\n"
     "Jan               noun:masculine_personal:nominative:singular\n"
     "                  noun:feminine:genitive:plural\n"
     "roz\305\202o\305\274y\305\202          verb:perfect:indicative:past:masculine:singular:third\n"
     "parasol           noun:masculine_inanimate:nominative:singular\n"
     "                  noun:masculine_inanimate:accusative:singular\n"
     "i                 conjunction\n"
     "                  interjection\n"
     "schowa\305\202           verb:perfect:indicative:past:masculine:singular:third\n"
     "si\304\231               pronoun:reflexive:by_case:genitive\n"
     "                  pronoun:reflexive:by_case:accusative\n"
     "pod               preposition\n"
     "dachem            noun:masculine_inanimate:instrumental:singular\n"
     ".                 full_stop\n"
     "Maria             noun:feminine:nominative:singular\n"
     "kupi\305\202a            verb:perfect:indicative:past:feminine:singular:third\n"
     "sobie             pronoun:reflexive:by_case:dative\n"
     "                  pronoun:reflexive:by_case:locative\n"
     "                  pronoun:reflexive:by_case:dative\n"
     "                  pronoun:reflexive:by_case:locative\n"
     "zapiekank\304\231        noun:feminine:accusative:singular\n"
     ",                 comma\n"
     "poniewa\305\274          conjunction\n"
     "by\305\202a              verb:imperfect:indicative:past:feminine:singular:third\n"
     "                  verb:perfect:indicative:past:feminine:singular:third\n"
     "                  adjectival_past_participle:feminine:nominative:singular\n"
     "                  adjectival_past_participle:feminine:vocative:singular\n"
     "g\305\202odna            adjective:positive:feminine:nominative:singular\n"
     "                  adjective:positive:feminine:vocative:singular\n"
     ".                 full_stop\n"
     "Jan               noun:masculine_personal:nominative:singular\n"
     "                  noun:feminine:genitive:plural\n"
     "szybko            noun:feminine:vocative:singular\n"
     "                  adverb:positive\n"
     "zjad\305\202             verb:perfect:indicative:past:masculine:singular:third\n"
     "pyszn\304\205            adjective:positive:feminine:accusative:singular\n"
     "                  adjective:positive:feminine:instrumental:singular\n"
     "zup\304\231              noun:feminine:accusative:singular\n"
     "grzybow\304\205          adjective:positive:feminine:accusative:singular\n"
     "                  adjective:positive:feminine:instrumental:singular\n"
     ".                 full_stop\n";

// clang-format on

BOOST_AUTO_TEST_CASE(simple_test) {
     istringstream iss(bininp);
     ostringstream oss;
     iss.str(simple_test_data);
     lexer.reset(iss);
     BOOST_CHECK(lexer.stream().good());
     for (;;) {
          Token const tok = lexer.get_token();
          if (tok.empty())
               break;
          oss << tok;
     }
     BOOST_CHECK(lexer.stream().eof());
     BOOST_CHECK(lexer.stream().fail());
     BOOST_CHECK(!lexer.stream().bad());
     BOOST_CHECK(oss.str() == simple_test_data_result);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
