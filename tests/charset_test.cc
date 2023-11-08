#include <shg/charset.h>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <fstream>
#include <filesystem>
#include <shg/utils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(charset_test)

BOOST_AUTO_TEST_CASE(unicode_test) {
     using SHG::PLP::Charset::unicode;
     using SHG::PLP::Charset::name;
     for (unsigned i = 0; i < 0x80; i++)
          BOOST_CHECK(unicode(i) == i);
     BOOST_CHECK(unicode('\x80') == 0x00a0);
     BOOST_CHECK(unicode('\xf2') == 0x017c);
     BOOST_CHECK(unicode('\xff') == 0xfffd);
     BOOST_CHECK(std::strcmp(name('\x80'), "NO-BREAK SPACE") == 0);
     BOOST_CHECK(std::strcmp(name('\xf2'),
                             "LATIN SMALL LETTER Z WITH DOT ABOVE") ==
                 0);
     BOOST_CHECK(std::strcmp(name('\xff'), "REPLACEMENT CHARACTER") ==
                 0);
}

BOOST_AUTO_TEST_CASE(unicode_to_char_test) {
     using SHG::PLP::Charset::unicode_to_char;
     using SHG::PLP::Charset::Invalid_character_error;
     for (int i = 0; i < 0x80; i++)
          BOOST_CHECK(unicode_to_char(i) == i);
     BOOST_CHECK(unicode_to_char(0x00a0) == '\x80');
     BOOST_CHECK(unicode_to_char(0x017c) == '\xf2');
     BOOST_CHECK(unicode_to_char(0xfffd) == '\xff');
     BOOST_CHECK_THROW(unicode_to_char(0x00a1),
                       Invalid_character_error);
}

BOOST_AUTO_TEST_CASE(conversion_test) {
     using SHG::PLP::Charset::charset_to_utf8;
     using SHG::PLP::Charset::utf8_to_charset;
     std::string s;
     for (int i = 0; i < 256; i++)
          s += i;
     std::string const t = charset_to_utf8(s);
     std::string const u = utf8_to_charset(t);
     BOOST_CHECK(s == u);
}

BOOST_AUTO_TEST_CASE(ctype_test) {
     using SHG::PLP::Charset::tolower;
     using SHG::PLP::Charset::toupper;
     BOOST_CHECK(tolower('\xf1') == '\xf2');
     BOOST_CHECK(toupper('\xf2') == '\xf1');
}

BOOST_AUTO_TEST_CASE(print_character_table_test) {
     using SHG::PLP::Charset::print_character_table;
     using SHG::rtrim;
     using std::filesystem::path;
     using std::getline;
     using std::ifstream;
     using std::string;
     using std::stringstream;

     stringstream f1(bininpout);
     print_character_table(f1);
     BOOST_REQUIRE(f1.good());
     path const p = path(datadir) / "plcharset.txt";
     ifstream f2(p, bininp);
     BOOST_REQUIRE(f2.good());
     string s1, s2;
     while (getline(f1, s1)) {
          BOOST_REQUIRE(getline(f2, s2));
          BOOST_CHECK(rtrim(s1) == rtrim(s2));
     }
     BOOST_REQUIRE(f1.eof());
     BOOST_REQUIRE(!f1.bad());
     BOOST_REQUIRE(!getline(f2, s2));
     BOOST_REQUIRE(f2.eof());
     BOOST_REQUIRE(!f2.bad());
}

BOOST_AUTO_TEST_CASE(ordtab_test) {
     using SHG::PLP::Charset::get_ordtab;
     int const n = 256;
     std::vector<bool> b(n);
     auto t = get_ordtab();
     for (int i = 0; i < n; i++) {
          BOOST_CHECK(!b[t[i]]);
          b[t[i]] = true;
     }
}

BOOST_DATA_TEST_CASE(chrcmp_test, bdata::xrange(256), xr) {
     using SHG::PLP::Charset::chrcmp;
     using SHG::PLP::Charset::isalpha;
     int const n = 256;
     int const i = xr;
     BOOST_CHECK(chrcmp(i, i) == 0);
     if (!isalpha(i))
          for (int j = 0; j < n; j++)
               if (isalpha(j))
                    BOOST_CHECK(chrcmp(i, j) < 0);
}

constexpr char const* const isprint_order_test_result =
     " !\"#$%&'()*+,-./"
     "0123456789:;<=>?@[\\]^_`{|}~"
     "\302\240\302\247\302\251\302\253\302\256\302\260\302\261\302"
     "\273\342\200\220\342\200\223\342\200\224\342\200\230\342\200"
     "\231\342\200\233\342\200\235\342\200\236\342\200\246\342\210"
     "\222\357\277\275A\304\204\303\201\303\204\303\205\304\202\303"
     "\200\303\202\303\203a\304\205\303\241\303\244\303\245\304\203"
     "\303\240\303\242\303\243BbC\304\206\304\214\303\207c\304\207"
     "\304\215\303\247D\304\220\304\216d\304\221\304\217E\304\230\303"
     "\211\304\232\303\213\303\210\303\212e\304\231\303\251\304\233"
     "\303\253\303\250\303\252FfGgHhI\303\215\303\217\303\214\303\216"
     "i\303\255\303\257\303\254\303\256JjKkL\305\201\304\271\304\275l"
     "\305\202\304\272\304\276MmN\305\203\305\207\303\221n\305\204"
     "\305\210\303\261O\303\230\303\223\303\226\303\222\303\224\303"
     "\225\305\220o\303\270\303\263\303\266\303\262\303\264\303\265"
     "\305\221PpQqR\305\224\305\230r\305\225\305\231S\305\232\305\240"
     "\305\236s\305\233\305\241\305\237\303\237T\305\244\305\242t\305"
     "\245\305\243U\303\232\303\234\305\256\303\231\303\233\305\260u"
     "\303\272\303\274\305\257\303\271\303\273\305\261VvWwXxY\303\235"
     "\305\270y\303\275\303\277Z\305\271\305\273\305\275z\305\272\305"
     "\274\305\276";

constexpr char const* const isalpha_order_test_result =
     "A\304\204\303\201\303\204\303\205\304\202\303\200\303\202\303"
     "\203a\304\205\303\241\303\244\303\245\304\203\303\240\303\242"
     "\303\243BbC\304\206\304\214\303\207c\304\207\304\215\303\247D"
     "\304\220\304\216d\304\221\304\217E\304\230\303\211\304\232\303"
     "\213\303\210\303\212e\304\231\303\251\304\233\303\253\303\250"
     "\303\252FfGgHhI\303\215\303\217\303\214\303\216i\303\255\303"
     "\257\303\254\303\256JjKkL\305\201\304\271\304\275l\305\202\304"
     "\272\304\276MmN\305\203\305\207\303\221n\305\204\305\210\303"
     "\261O\303\230\303\223\303\226\303\222\303\224\303\225\305\220o"
     "\303\270\303\263\303\266\303\262\303\264\303\265\305\221PpQqR"
     "\305\224\305\230r\305\225\305\231S\305\232\305\240\305\236s\305"
     "\233\305\241\305\237\303\237T\305\244\305\242t\305\245\305\243U"
     "\303\232\303\234\305\256\303\231\303\233\305\260u\303\272\303"
     "\274\305\257\303\271\303\273\305\261VvWwXxY\303\235\305\270y"
     "\303\275\303\277Z\305\271\305\273\305\275z\305\272\305\274\305"
     "\276";

BOOST_AUTO_TEST_CASE(order_test) {
     using SHG::PLP::Charset::chrcmp;
     using SHG::PLP::Charset::isalpha;
     using SHG::PLP::Charset::isprint;
     using SHG::PLP::Charset::charset_to_utf8;

     auto const cmp = [](char lhs, char rhs) {
          return chrcmp(lhs, rhs) < 0;
     };
     std::string s;

     for (int i = 0; i < 256; i++)
          if (isprint(i))
               s.push_back(i);
     std::sort(s.begin(), s.end(), cmp);
     BOOST_CHECK(charset_to_utf8(s) == isprint_order_test_result);

     s.clear();
     for (int i = 0; i < 256; i++)
          if (isalpha(i))
               s.push_back(i);
     std::sort(s.begin(), s.end(), cmp);
     BOOST_CHECK(charset_to_utf8(s) == isalpha_order_test_result);
}

BOOST_AUTO_TEST_CASE(alpha_strcmp_test) {
     using SHG::PLP::Charset::alpha_strcmp;
     BOOST_CHECK(alpha_strcmp("", "") == 0);
     BOOST_CHECK(alpha_strcmp("a", "") > 0);
     BOOST_CHECK(alpha_strcmp("", "a") < 0);
     BOOST_CHECK(alpha_strcmp("a", "a") == 0);
     BOOST_CHECK(alpha_strcmp("\302\245", "\245\302") < 0);
}

BOOST_AUTO_TEST_CASE(boost_starts_ends_test) {
     using boost::starts_with;
     using boost::ends_with;
     std::string const s = "abcd";

     BOOST_CHECK(starts_with("", ""));
     BOOST_CHECK(!starts_with("", "a"));
     BOOST_CHECK(starts_with(s, "a"));
     BOOST_CHECK(starts_with(s, "ab"));
     BOOST_CHECK(starts_with(s, "abc"));
     BOOST_CHECK(starts_with(s, "abcd"));
     BOOST_CHECK(!starts_with(s, "abcde"));
     BOOST_CHECK(!starts_with(s, "x"));

     BOOST_CHECK(ends_with("", ""));
     BOOST_CHECK(!ends_with("", "a"));
     BOOST_CHECK(ends_with(s, "d"));
     BOOST_CHECK(ends_with(s, "cd"));
     BOOST_CHECK(ends_with(s, "bcd"));
     BOOST_CHECK(ends_with(s, "abcd"));
     BOOST_CHECK(!ends_with(s, "abcde"));
     BOOST_CHECK(!ends_with(s, "x"));
}

BOOST_AUTO_TEST_CASE(lowercase_uppercase_capitalize_test) {
     using SHG::PLP::Charset::lowercase;
     using SHG::PLP::Charset::uppercase;
     using SHG::PLP::Charset::capitalize;
     using SHG::PLP::Charset::utf8_to_charset;

     std::string s;
     lowercase(s);
     BOOST_CHECK(s == "");
     uppercase(s);
     BOOST_CHECK(s == "");
     capitalize(s);
     BOOST_CHECK(s == "");
     s = "aBcD";
     lowercase(s);
     BOOST_CHECK(s == "abcd");
     s = "aBcD";
     uppercase(s);
     BOOST_CHECK(s == "ABCD");
     s = "aBcD";
     capitalize(s);
     BOOST_CHECK(s == "Abcd");

     s = "1aBcD";
     lowercase(s);
     BOOST_CHECK(s == "1abcd");
     s = "1aBcD";
     uppercase(s);
     BOOST_CHECK(s == "1ABCD");
     s = "1aBcD";
     capitalize(s);
     BOOST_CHECK(s == "1abcd");

     auto const low =  // \k{a}\'{c}\k{e}{\l}\'{n}\'{o}\'{s}\'{z}\.{z}
          "\304\205\304\207\304\231\305\202\305\204\303\263\305\233"
          "\305\272\305\274";
     auto const upp =  // \k{A}\'{C}\k{E}{\L}\'{N}\'{O}\'{S}\'{Z}\.{Z}
          "\304\204\304\206\304\230\305\201\305\203\303\223\305\232"
          "\305\271\305\273";
     auto const cap =  // \k{A}\'{c}\k{e}{\l}\'{n}\'{o}\'{s}\'{z}\.{z}
          "\304\204\304\207\304\231\305\202\305\204\303\263\305\233"
          "\305\272\305\274";
     auto const mix =  // \k{A}\'{c}\k{E}{\l}\'{N}\'{o}\'{S}\'{z}\.{Z}
          "\304\204\304\207\304\230\305\202\305\203\303\263\305\232"
          "\305\272\305\273";

     s = utf8_to_charset(mix);
     lowercase(s);
     BOOST_CHECK(s == utf8_to_charset(low));
     s = utf8_to_charset(mix);
     uppercase(s);
     BOOST_CHECK(s == utf8_to_charset(upp));
     s = utf8_to_charset(mix);
     capitalize(s);
     BOOST_CHECK(s == utf8_to_charset(cap));
}

BOOST_AUTO_TEST_CASE(is_proper_prefix_suffix_test) {
     using SHG::PLP::Charset::is_proper_prefix;
     using SHG::PLP::Charset::is_proper_suffix;

     BOOST_CHECK(is_proper_prefix("niebrzydki", ""));
     BOOST_CHECK(is_proper_prefix("niebrzydki", "nie"));
     BOOST_CHECK(!is_proper_prefix("niebrzydki", "nia"));
     BOOST_CHECK(!is_proper_prefix("nie", "nie"));

     BOOST_CHECK(is_proper_suffix("niebrzydki", ""));
     BOOST_CHECK(is_proper_suffix("niebrzydki", "dki"));
     BOOST_CHECK(!is_proper_suffix("niebrzydki", "aki"));
     BOOST_CHECK(!is_proper_suffix("nie", "nie"));
}

BOOST_AUTO_TEST_CASE(completeness_test) {
     using SHG::PLP::Charset::isalpha;
     using SHG::PLP::Charset::islower;
     using SHG::PLP::Charset::isupper;
     for (int i = 0; i < 256; i++) {
          bool const a = isalpha(i);
          bool const b = islower(i);
          bool const c = isupper(i);
          BOOST_CHECK(a == b || c);
     }
}

BOOST_AUTO_TEST_CASE(is_lower_test) {
     using SHG::PLP::Charset::is_lower;
     BOOST_CHECK(is_lower(""));
     BOOST_CHECK(is_lower("a"));
     BOOST_CHECK(is_lower("abc"));
     BOOST_CHECK(!is_lower("Abc"));
     BOOST_CHECK(!is_lower("aBc"));
     BOOST_CHECK(!is_lower("abC"));
     BOOST_CHECK(is_lower("\303\305\315"));   // ace
     BOOST_CHECK(!is_lower("\302\305\315"));  // Ace
     BOOST_CHECK(!is_lower("\303\304\315"));  // aCe
     BOOST_CHECK(!is_lower("\303\305\314"));  // acE
}

BOOST_AUTO_TEST_CASE(is_upper_test) {
     using SHG::PLP::Charset::is_upper;
     BOOST_CHECK(is_upper(""));
     BOOST_CHECK(is_upper("A"));
     BOOST_CHECK(is_upper("ABC"));
     BOOST_CHECK(!is_upper("aBC"));
     BOOST_CHECK(!is_upper("AbC"));
     BOOST_CHECK(!is_upper("ABc"));
     BOOST_CHECK(is_upper("\302\304\314"));   // ACE
     BOOST_CHECK(!is_upper("\303\304\314"));  // aCE
     BOOST_CHECK(!is_upper("\302\305\314"));  // AcE
     BOOST_CHECK(!is_upper("\302\304\315"));  // ACe
}

BOOST_AUTO_TEST_CASE(is_capit_test) {
     using SHG::PLP::Charset::is_capit;
     BOOST_CHECK(is_capit(""));
     BOOST_CHECK(is_capit("A"));
     BOOST_CHECK(is_capit("Abc"));
     BOOST_CHECK(!is_capit("ABc"));
     BOOST_CHECK(!is_capit("AbC"));
     BOOST_CHECK(!is_capit("ABC"));
     BOOST_CHECK(is_capit("\302"));           // A
     BOOST_CHECK(is_capit("\302\305\315"));   // Ace
     BOOST_CHECK(!is_capit("\302\304\315"));  // ACe
     BOOST_CHECK(!is_capit("\302\305\314"));  // AcE
     BOOST_CHECK(!is_capit("\302\304\314"));  // ACE
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
