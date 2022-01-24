#include <shg/lexan.h>
#include <filesystem>
#include <fstream>
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

     fs::path const ip = fs::temp_directory_path() / "basic.swf";
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
     fs::path const ip = fs::temp_directory_path() / "solaris.txt";
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
     fs::path const ip = fs::temp_directory_path() / "chlopcy.txt";
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
     fs::path const ip = fs::temp_directory_path() / "chlopcy.txt";
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

/*
BOOST_AUTO_TEST_CASE(nkjp2words_test) {
     namespace fs = std::filesystem;
     using SHG::PLP::Charset::Invalid_character_error;
     fs::path const ip = fs::temp_directory_path() / "nkjp2words.txt";
     std::ifstream f(ip, bininp);
     BOOST_REQUIRE(f.good());
     lexer.reset(f);
     for (;;) {
          try {
               auto const tok = lexer.get_token();
               if (tok.empty())
                    break;
               std::cout << tok;
          } catch (Invalid_character_error const&) {
          }
     }
     BOOST_REQUIRE(f.eof() && !f.bad());
}
*/

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
