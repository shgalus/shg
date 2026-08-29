#include <shg/recjar.h>
#include <set>
#include <fstream>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(recjar_test)

bool equal(SHG::RECJAR::Headers const h, char const* const s[]) {
     SHG::RECJAR::Headers h1;
     for (std::size_t i = 0; s[i] != nullptr; i++)
          BOOST_REQUIRE(h1.insert(s[i]).second);
     return h == h1;
}

BOOST_AUTO_TEST_CASE(recjar_error_test) {
     using SHG::RECJAR::Error;
     Error const e1;
     BOOST_CHECK(std::strcmp(e1.what(), "error") == 0);
     Error const e2("message");
     BOOST_CHECK(std::strcmp(e2.what(), "error: message") == 0);
     Error const e3("message", "file");
     BOOST_CHECK(
          std::strcmp(e3.what(), "error: message, file: file") == 0);
     Error const e4("message", "file", 1);
     BOOST_CHECK(std::strcmp(e4.what(),
                             "error: message, file: file, line: 1") ==
                 0);
}

BOOST_AUTO_TEST_CASE(empty_test) {
     using SHG::RECJAR::read, SHG::RECJAR::print;
     std::stringstream ss;

     auto const r = read("");
     BOOST_CHECK(r.size() == 0);
     print(r, ss);
     BOOST_CHECK(strcmp(ss.str().c_str(), "") == 0);
}

// http://www.catb.org/~esr/writings/taoup/html/ch05s02.html#id2906931
// 16 November 2025
constexpr char const* const raymond =
     "Planet: Mercury\n"
     "Orbital-Radius: 57,910,000 km\n"
     "Diameter: 4,880 km\n"
     "Mass: 3.30e23 kg\n"
     "%%\n"
     "Planet: Venus\n"
     "Orbital-Radius: 108,200,000 km\n"
     "Diameter: 12,103.6 km\n"
     "Mass: 4.869e24 kg\n"
     "%%\n"
     "Planet: Earth\n"
     "Orbital-Radius: 149,600,000 km\n"
     "Diameter: 12,756.3 km\n"
     "Mass: 5.972e24 kg\n"
     "Moons: Luna\n";

constexpr char const* const raymond_headers[] = {
     "Planet", "Orbital-Radius", "Diameter",
     "Mass",   "Moons",          nullptr};

BOOST_AUTO_TEST_CASE(raymond_test) {
     namespace fs = std::filesystem;
     using SHG::RECJAR::read, SHG::RECJAR::print,
          SHG::RECJAR::headers;
     std::stringstream ss;

     auto const r = read(raymond);
     print(r, ss);
     BOOST_CHECK(r.size() == 3);
     BOOST_CHECK(strcmp(ss.str().c_str(), raymond) == 0);
     BOOST_CHECK(equal(headers(r), raymond_headers));

     fs::path const path = tmp_dir_path() / "raymond.txt";
     {
          std::ofstream f(path);
          f << raymond;
          BOOST_REQUIRE(f.good());
     }

     ss.str("");
     ss.clear();
     auto const r1 = read(path);
     print(r1, ss);
     BOOST_CHECK(r.size() == 3);
     BOOST_CHECK(strcmp(ss.str().c_str(), raymond) == 0);
}

// \cite raymond-2004, page 125.
constexpr char const* const raymondp =
     "Planeta: Merkury\n"
     "Promie\305\204-Orbity: 57 910 000 km\n"
     "\305\232rednica: 4880 km\n"
     "Masa: 3,30e23 kg\n"
     "%%\n"
     "Planeta: Wenus\n"
     "Promie\305\204-Orbity: 108 200 000 km\n"
     "\305\232rednica: 12 103,6 km\n"
     "Masa: 4,869e24 kg\n"
     "%%\n"
     "Planeta: Ziemia\n"
     "Promie\305\204-Orbity: 149 600 000 km\n"
     "\305\232rednica: 12 756,3 km\n"
     "Masa: 5,972e24 kg\n"
     "Satelity: Ksi\304\231\305\274yc\n";

constexpr char const* const raymondp_headers[] = {
     "Planeta",         "Promie\305\204-Orbity",
     "\305\232rednica", "Masa",
     "Satelity",        nullptr};

BOOST_AUTO_TEST_CASE(raymondp_test) {
     namespace fs = std::filesystem;
     using SHG::RECJAR::read, SHG::RECJAR::print,
          SHG::RECJAR::headers;
     std::stringstream ss;

     auto const r = read(raymondp);
     print(r, ss);
     BOOST_CHECK(r.size() == 3);
     BOOST_CHECK(strcmp(ss.str().c_str(), raymondp) == 0);
     BOOST_CHECK(equal(headers(r), raymondp_headers));

     fs::path const path = tmp_dir_path() / "raymondp.txt";
     {
          std::ofstream f(path);
          f << raymondp;
          BOOST_REQUIRE(f.good());
     }

     ss.str("");
     ss.clear();
     auto const r1 = read(path);
     print(r1, ss);
     BOOST_CHECK(r.size() == 3);
     BOOST_CHECK(strcmp(ss.str().c_str(), raymondp) == 0);
}

BOOST_AUTO_TEST_CASE(exception_test) {
     using SHG::RECJAR::Error;
     using SHG::RECJAR::read;
     std::istringstream iss;
     iss.setstate(std::ios_base::failbit);
     BOOST_CHECK_EXCEPTION(read(iss), Error, [](Error const& e) {
          return std::strcmp(e.what(), "error: stream not found") ==
                 0;
     });
     BOOST_CHECK_EXCEPTION(read("%\n"), Error, [](Error const& e) {
          return std::strcmp(e.what(),
                             "error: invalid start of record, file: "
                             "string, line: 1") == 0;
     });
     BOOST_CHECK_EXCEPTION(read(">\n"), Error, [](Error const& e) {
          return std::strcmp(e.what(),
                             "error: no preceding field, file: "
                             "string, line: 1") == 0;
     });
     BOOST_CHECK_EXCEPTION(read(":\n"), Error, [](Error const& e) {
          return std::strcmp(e.what(),
                             "error: empty field name, file: "
                             "string, line: 1") == 0;
     });
}

BOOST_AUTO_TEST_CASE(empty_field_test) {
     using SHG::RECJAR::read, SHG::RECJAR::print;
     char const* const s = "Planet:\n";
     char const* const t = "Planet: \n";
     std::ostringstream oss;
     auto const r = read(s);
     print(r, oss);
     BOOST_CHECK(strcmp(oss.str().c_str(), t) == 0);
}

BOOST_AUTO_TEST_CASE(empty_field_second_test) {
     using SHG::RECJAR::read, SHG::RECJAR::print;
     char const* const s = "Planet: \n";
     std::ostringstream oss;
     auto const r = read(s);
     print(r, oss);
     BOOST_CHECK(strcmp(oss.str().c_str(), s) == 0);
}

BOOST_AUTO_TEST_CASE(one_letter_field_test) {
     using SHG::RECJAR::read, SHG::RECJAR::print;
     char const* const s = "Planet:M\n";
     char const* const t = "Planet: M\n";
     std::ostringstream oss;
     auto const r = read(s);
     print(r, oss);
     BOOST_CHECK(strcmp(oss.str().c_str(), t) == 0);
}

BOOST_AUTO_TEST_CASE(one_letter_field_second_test) {
     using SHG::RECJAR::read, SHG::RECJAR::print;
     char const* const s = "Planet: M\n";
     std::ostringstream oss;
     auto const r = read(s);
     print(r, oss);
     BOOST_CHECK(strcmp(oss.str().c_str(), s) == 0);
}

BOOST_AUTO_TEST_CASE(empty_records_test) {
     using SHG::RECJAR::read;
     BOOST_CHECK(read("%%").empty());
     BOOST_CHECK(read("%%\n").empty());
     BOOST_CHECK(read("%%\n%%").empty());
     BOOST_CHECK(read("%%\n%%\n").empty());
     BOOST_CHECK(read("%%\n%%\n%%").empty());
     BOOST_CHECK(read("%%\n%%\n%%\n").empty());
}

BOOST_AUTO_TEST_CASE(simple_continuation_test) {
     using SHG::RECJAR::read, SHG::RECJAR::print;
     char const* const s = "Planet: Mer\n> cury\n";
     std::ostringstream oss;
     auto const r = read(s);
     print(r, oss);
     BOOST_CHECK(strcmp(oss.str().c_str(), s) == 0);
}

BOOST_AUTO_TEST_CASE(no_space_after_less_continuation_test) {
     using SHG::RECJAR::read, SHG::RECJAR::print;
     char const* const s = "Planet: Mer\n>cury\n";
     char const* const t = "Planet: Mer\n> cury\n";
     std::ostringstream oss;
     auto const r = read(s);
     print(r, oss);
     BOOST_CHECK(strcmp(oss.str().c_str(), t) == 0);
}

BOOST_AUTO_TEST_CASE(empty_continuation_test) {
     using SHG::RECJAR::read, SHG::RECJAR::print;
     char const* const s = "Planet: Mercury\n>\n";
     char const* const t = "Planet: Mercury\n> \n";
     std::ostringstream oss;
     auto const r = read(s);
     print(r, oss);
     BOOST_CHECK(strcmp(oss.str().c_str(), t) == 0);
}

BOOST_AUTO_TEST_CASE(continuation_is_one_space_test) {
     using SHG::RECJAR::read, SHG::RECJAR::print;
     char const* const s = "Planet: Mercury\n> \n";
     std::ostringstream oss;
     auto const r = read(s);
     print(r, oss);
     BOOST_CHECK(strcmp(oss.str().c_str(), s) == 0);
}

BOOST_AUTO_TEST_CASE(comparing_test) {
     SHG::RECJAR::Field f1, f2;
     SHG::RECJAR::Records r1, r2;
     BOOST_CHECK(f1 == f2);
     BOOST_CHECK(r1 == r2);
}

/*
BOOST_AUTO_TEST_CASE(language_subtag_registry_test) {
     using SHG::RECJAR::read;
     using Headers = std::set<SHG::RECJAR::Field_name>;
     std::filesystem::path const path = "language-subtag-registry";
     Headers duplicated_headers;

     auto const r = read(path);
     std::cout << "Number of records: " << r.size() << '\n';

     for (auto it = r.cbegin(); it != r.cend(); ++it) {
          Headers d;
          for (auto it1 = it->cbegin(); it1 != it->cend(); ++it1) {
               if (!d.insert(it1->name).second)
                    duplicated_headers.insert(it1->name);
          }
     }
     std::cout << "\nDuplicated field names:\n";
     for (auto it = duplicated_headers.cbegin();
          it != duplicated_headers.cend(); ++it)
          std::cout << *it << '\n';
     BOOST_CHECK(true);
}
*/

constexpr char const* const html_test_input =
     "(i): x \342\210\274 x (zwrotno\305\233\304\207)\n"
     "(ii): x \342\210\274 x\342\200\262 \342\207\222 x\342\200\262 "
     "\342\210\274 x (symetria)\n"
     "(iii): x \342\210\274 x\342\200\262 \342\210\247  "
     "x\342\200\262 \342\210\274 x\342\200\263 \342\207\222 x "
     "\342\210\274 x\342\200\263 (przechodnio\305\233\304\207)\n"
     "%%\n"
     "Definition: Let R be a commutatiove ring. A subset I of R is "
     "said\n"
     "> to be an ideal of R precisely when the following conditions "
     "are\n"
     "> satisfied: (i) I \342\211\240 \342\210\205; (ii) whenever a, "
     "b \342\210\210 I, then a + b \342\210\210 I also;\n"
     "> and (iii) whenever a \342\210\210 I and r \342\210\210 R, "
     "then ra \342\210\210 I also.\n";

constexpr char const* const html_test_output[] = {
     "<!doctype html>",
     "<html lang=\"pl\">",
     "<head>",
     "<meta charset=\"utf-8\">",
     "<meta name=\"viewport\" content=\"width=device-width, "
     "initial-scale=1.0\">",
     "<title>HTML test</title>",
     "<style>",
     "body {font: 12pt serif; color: black; background: white;",
     "  margin-left: 10%%; margin-right: 10%%;}",
     "h1, h2, h3, h4, h5, h6 {color: #900020; background: white;}",
     "h2.rozm {font-family: \"Comic Sans MS\", sans-serif;}",
     "p.footer {font-style: italic;}",
     "span.attention {font-style: italic; color: red; background: "
     "white;}",
     "div.new {color: black; background: yellow;}",
     "div.record {",
     "    margin: 5px;",
     "    padding: 5px;",
     "    border: 1px solid black;",
     "    border-radius: 5px;",
     "}",
     "div.field {",
     "}",
     "</style>",
     "</head>",
     "<body>",
     "<div class=\"record\">",
     "<div class=\"field\">",
     "<strong>(i)</strong>: x \342\210\274 x "
     "(zwrotno\305\233\304\207)",
     "</div>",
     "<div class=\"field\">",
     "<strong>(ii)</strong>: x \342\210\274 x\342\200\262 "
     "\342\207\222 x\342\200\262 \342\210\274 x (symetria)",
     "</div>",
     "<div class=\"field\">",
     "<strong>(iii)</strong>: x \342\210\274 x\342\200\262 "
     "\342\210\247  x\342\200\262 \342\210\274 x\342\200\263 "
     "\342\207\222 x \342\210\274 x\342\200\263 "
     "(przechodnio\305\233\304\207)",
     "</div>",
     "</div>",
     "<div class=\"record\">",
     "<div class=\"field\">",
     "<strong>Definition</strong>: Let R be a commutatiove ring. A "
     "subset I of R is said to be an ideal of R precisely when the "
     "following conditions are satisfied: (i) I \342\211\240 "
     "\342\210\205; (ii) whenever a, b \342\210\210 I, then a + b "
     "\342\210\210 I also; and (iii) whenever a \342\210\210 I and r "
     "\342\210\210 R, then ra \342\210\210 I also.",
     "</div>",
     "</div>",
     "</body>",
     "</html>",
     nullptr};

BOOST_AUTO_TEST_CASE(build_html_test) {
     namespace fs = std::filesystem;
     using SHG::RECJAR::read, SHG::RECJAR::build_html;
     fs::path const path = tmp_dir_path() / "html_test.html";
     auto const r = read(html_test_input);
     BOOST_CHECK_NO_THROW(build_html(r, "pl", "HTML test", path));
     std::ifstream f;
     std::string s;
     char const* const* line = html_test_output;
     f.open(path);
     while (std::getline(f, s)) {
          BOOST_REQUIRE(*line != nullptr);
          BOOST_CHECK(strcmp(s.c_str(), *line) == 0);
          line++;
     }
     BOOST_CHECK(f.eof() && !f.bad());
     BOOST_CHECK(*line == nullptr);
}

BOOST_AUTO_TEST_CASE(extra_spaces_test) {
     using SHG::RECJAR::read;
     std::string s{"   "};
     for (char const* c = raymond; *c != '\0'; c++) {
          if (*c == '\n')
               s += "\n   ";
          else if (*c == ':')
               s += "   :   ";
          else
               s += *c;
     }
     BOOST_CHECK(read(s.c_str()) == read(raymond));
}

BOOST_AUTO_TEST_CASE(spaces_in_field_names_test) {
     using SHG::RECJAR::read, SHG::RECJAR::print;
     constexpr char const* const s =
          "x x:yy\n"
          "z z  z   z  : uuu\n";
     constexpr char const* const t =
          "x x: yy\n"
          "z z  z   z: uuu\n";
     auto const r = read(s);
     std::stringstream ss;
     print(r, ss);
     BOOST_CHECK(strcmp(ss.str().c_str(), t) == 0);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
