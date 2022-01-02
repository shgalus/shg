#include <shg/terminals.h>
#include <filesystem>
#include <fstream>
#include <shg/utils.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(terminals_test)

using std::istringstream;
using std::ostringstream;
using std::ios_base;
using namespace SHG::PLP;

BOOST_AUTO_TEST_CASE(tab_terminals_test) {
     using Index = std::vector<std::string>::size_type;
     auto const tt = tab_terminals();
     BOOST_CHECK(tt.size() == 1052);
     for (Index i = 0; i < tt.size(); i++) {
          auto const& t = tt[i];
          BOOST_CHECK(t.find_first_of(white_space) ==
                      std::string::npos);
          for (Index j = i + 1; j < tt.size(); j++)
               BOOST_CHECK(tt[j] != t);
     }
     // for (auto const& t : tt)
     // std::cout << t << '\n';
}

constexpr char const* const cookie_jar_test_data = R"(
%% C1
P1
P2

%% C2
P1
P2
P3
%% C3
P1
P2
P3
%%
P1
%% C5
)";

constexpr char const* const cookie_jar_test_result = R"(C1:P1 P2
C2:P1 P2 P3
C3:P1 P2 P3
:P1
C5:
)";

BOOST_AUTO_TEST_CASE(cookie_jar_test) {
     istringstream iss(ios_base::in | ios_base::binary);
     std::string result;

     iss.str("");
     result.clear();
     {
          Cookie_jar cj(iss);
          while (cj.get())
               result += cj.comment() + ":" + cj.contents() + '\n';
     }
     BOOST_CHECK(result.empty());

     iss.clear();
     iss.str("%%\n");
     result.clear();
     {
          Cookie_jar cj(iss);
          while (cj.get())
               result += cj.comment() + ":" + cj.contents() + '\n';
     }

     BOOST_CHECK(result == ":\n");

     iss.clear();
     iss.str(cookie_jar_test_data);
     result.clear();
     {
          Cookie_jar cj(iss);
          while (cj.get())
               result += cj.comment() + ":" + cj.contents() + '\n';
     }
     BOOST_CHECK(result == cookie_jar_test_result);
}

constexpr char const* const get_sentence_test_data = R"(
This is the first sentence.

This
is # comment 1
the# comment 2
#comment 3
second sentence.   # comment 4

#
This is the third sentence.

This is the fourth sentence.
)";

constexpr char const* const get_sentence_test_result =
     R"(This is the first sentence.
This is the second sentence.
This is the third sentence.
This is the fourth sentence.
)";

BOOST_AUTO_TEST_CASE(get_sentence_test) {
     istringstream iss(ios_base::in | ios_base::binary);
     std::string result;

     iss.str("");
     for (std::string s = get_sentence(iss); !s.empty();
          s = get_sentence(iss))
          result += s + '\n';
     BOOST_CHECK(result.empty());
     BOOST_CHECK(iss.eof() && !iss.bad());

     iss.clear();
     iss.str("# comment\n");
     result.clear();
     for (std::string s = get_sentence(iss); !s.empty();
          s = get_sentence(iss))
          result += s + '\n';
     BOOST_CHECK(result.empty());
     BOOST_CHECK(iss.eof() && !iss.bad());

     iss.clear();
     iss.str("#\n#\n#\n");
     result.clear();
     for (std::string s = get_sentence(iss); !s.empty();
          s = get_sentence(iss))
          result += s + '\n';
     BOOST_CHECK(result.empty());
     BOOST_CHECK(iss.eof() && !iss.bad());

     iss.clear();
     iss.str("a#\n#\n#\n");
     result.clear();
     for (std::string s = get_sentence(iss); !s.empty();
          s = get_sentence(iss))
          result += s + '\n';
     BOOST_CHECK(result == "a\n");
     BOOST_CHECK(iss.eof() && !iss.bad());

     iss.clear();
     iss.str("#\na#\n#\n");
     result.clear();
     for (std::string s = get_sentence(iss); !s.empty();
          s = get_sentence(iss))
          result += s + '\n';
     BOOST_CHECK(result == "a\n");
     BOOST_CHECK(iss.eof() && !iss.bad());

     iss.clear();
     iss.str("#\n#\na#\n");
     result.clear();
     for (std::string s = get_sentence(iss); !s.empty();
          s = get_sentence(iss))
          result += s + '\n';
     BOOST_CHECK(result == "a\n");
     BOOST_CHECK(iss.eof() && !iss.bad());

     iss.clear();
     iss.str("#\n#\n#\na");
     result.clear();
     for (std::string s = get_sentence(iss); !s.empty();
          s = get_sentence(iss))
          result += s + '\n';
     BOOST_CHECK(result == "a\n");
     BOOST_CHECK(iss.eof() && !iss.bad());

     iss.clear();
     iss.str(get_sentence_test_data);
     result.clear();
     for (std::string s = get_sentence(iss); !s.empty();
          s = get_sentence(iss))
          result += s + '\n';
     BOOST_CHECK(result == get_sentence_test_result);
     BOOST_CHECK(iss.eof() && !iss.bad());
}

bool prepare_word_files() {
     namespace fs = std::filesystem;
     Dictionary dict;
     fs::path ip, op;
     std::ifstream f;
     std::ofstream g;

     ip = fs::temp_directory_path() / "swf.txt";
     op = fs::temp_directory_path() / "swf.wf";
     f.open(ip, ios_base::in | ios_base::binary);
     g.open(op, ios_base::out | ios_base::binary);
     if (!f.good())
          return false;
     if (!g.good())
          return false;
     dict.load_source_word_file(f);
     if (!f.eof())
          return false;
     if (!f.fail())
          return false;
     if (f.bad())
          return false;
     dict.write_word_file(g);
     if (g.bad())
          return false;
     g.close();
     f.close();

     ip = fs::temp_directory_path() / "solaris.swf";
     op = fs::temp_directory_path() / "solaris.wf";
     f.open(ip, ios_base::in | ios_base::binary);
     g.open(op, ios_base::out | ios_base::binary);
     if (!f.good())
          return false;
     if (!g.good())
          return false;
     dict.load_source_word_file(f);
     if (!f.eof())
          return false;
     if (!f.fail())
          return false;
     if (f.bad())
          return false;
     dict.write_word_file(g);
     if (g.bad())
          return false;
     g.close();
     f.close();
     return true;
}

BOOST_AUTO_TEST_CASE(convert_test) {
     namespace fs = std::filesystem;
     BOOST_REQUIRE(prepare_word_files());
     Terminalizer t;
     fs::path ip;
     ip = fs::temp_directory_path() / "swf.wf";
     BOOST_REQUIRE(t.load_dict(ip.string().c_str()));
     ip = fs::temp_directory_path() / "solaris.wf";
     BOOST_REQUIRE(t.load_dict(ip.string().c_str()));
     ip = fs::temp_directory_path() / "solaris.txt";
     std::ifstream f(ip, ios_base::in | ios_base::binary);
     BOOST_REQUIRE(f.good());
     std::string s;
     while (!(s = get_sentence(f)).empty()) {
          std::cout << s << '\n';
          auto r = t.convert(s);
          for (auto x : r)
               std::cout << x;
     }
     BOOST_REQUIRE(f.eof() && !f.bad());
}

BOOST_AUTO_TEST_CASE(check_numerals_test) {
     namespace fs = std::filesystem;
     BOOST_REQUIRE(prepare_word_files());
     Terminalizer t;
     fs::path const ip = fs::temp_directory_path() / "swf.wf";
     BOOST_REQUIRE(t.load_dict(ip.string().c_str()));
     auto r = t.convert(
          "Tysi\304\205ckrotnie, studwukrotnie, "
          "dwudziestotrzykrotnie, osiemnastokrotnie, "
          "jednokrotnie.");
     for (auto x : r)
          std::cout << x;
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(basic_test) {
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
