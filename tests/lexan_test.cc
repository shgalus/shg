#include <shg/lexan.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <shg/utils.h>
#include <shg/encoding.h>
#include "tests.h"

namespace TESTS {

struct Basic_lexer {
     Basic_lexer();
     SHG::PLP::Lexer lexer{};
};

namespace fs = std::filesystem;

Basic_lexer::Basic_lexer() {
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
using std::stringstream;

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
     fs::path const ip = fs::path(datadir) / "chlopcy.txt";
     std::vector<SHG::PLP::Token> vt, vs;

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
          auto const s = SHG::PLP::get_sentence(f);
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

BOOST_AUTO_TEST_CASE(simple_test) {
     using SHG::rtrim;
     stringstream f;
     {
          istringstream iss(bininp);
          iss.str(simple_test_data);
          lexer.reset(iss);
          BOOST_REQUIRE(lexer.stream().good());
          for (;;) {
               SHG::PLP::Token const tok = lexer.get_token();
               if (tok.empty())
                    break;
               f << tok;
          }
     }
     BOOST_REQUIRE(f.good());
     fs::path const p = fs::path(datadir) / "lexan.txt";
     std::ifstream g(p, bininp);
     f.seekp(0);
     BOOST_REQUIRE(f.good());
     BOOST_REQUIRE(g.good());
     std::string fs, gs;
     while (getline(f, fs)) {
          BOOST_REQUIRE(getline(g, gs));
          BOOST_CHECK(rtrim(fs) == rtrim(gs));
     }
     BOOST_CHECK(!getline(g, gs));
     BOOST_CHECK(gs == "");
     BOOST_REQUIRE(f.eof());
     BOOST_REQUIRE(!f.bad());
     BOOST_REQUIRE(g.eof());
     BOOST_REQUIRE(!g.bad());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
