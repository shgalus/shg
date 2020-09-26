#include "shg/encoding.h"
#include <set>
#include <stdexcept>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(encoding_test)

using SHG::Encoding::Conversion_error;
using SHG::Encoding::is_valid_codepoint;
using SHG::Encoding::iso88592_to_utf32;
using SHG::Encoding::utf16_length;
using SHG::Encoding::utf16_to_utf32;
using SHG::Encoding::utf32_to_iso88592;
using SHG::Encoding::utf32_to_utf16;
using SHG::Encoding::utf32_to_utf8;
using SHG::Encoding::utf32_to_windows1250;
using SHG::Encoding::utf8_length;
using SHG::Encoding::utf8_to_utf32;
using SHG::Encoding::windows1250_to_utf32;
using std::string, std::u16string, std::u32string;

BOOST_AUTO_TEST_CASE(string_conversions_test) {
     // Polish alphabet in UTF-32
     const u32string pla32 =
          U"a\u0105bc\u0107de\u0119fghijkl\u0142mn"
          "\u0144o\u00F3pqrs\u015Btuvwxyz\u017A\u017C\n"
          "A\u0104BC\u0106DE\u0118FGHIJKL\u0141MN"
          "\u0143O\u00D3PQRS\u015ATUVWXYZ\u0179\u017B\n";
     // Polish alphabet in UTF-16
     const u16string pla16 =
          u"a\u0105bc\u0107de\u0119fghijkl\u0142mn"
          "\u0144o\u00F3pqrs\u015Btuvwxyz\u017A\u017C\n"
          "A\u0104BC\u0106DE\u0118FGHIJKL\u0141MN"
          "\u0143O\u00D3PQRS\u015ATUVWXYZ\u0179\u017B\n";
     // Polish alphabet in UTF-8
     const string pla8 =
          u8"a\u0105bc\u0107de\u0119fghijkl\u0142mn"
          "\u0144o\u00F3pqrs\u015Btuvwxyz\u017A\u017C\n"
          "A\u0104BC\u0106DE\u0118FGHIJKL\u0141MN"
          "\u0143O\u00D3PQRS\u015ATUVWXYZ\u0179\u017B\n";
     // Polish alphabet in ISO 8859-2
     const string plaiso88592 =
          "a\261bc\346de\352fghijkl\263mn"
          "\361o\363pqrs\266tuvwxyz\274\277\n"
          "A\241BC\306DE\312FGHIJKL\243MN"
          "\321O\323PQRS\246TUVWXYZ\254\257\n";
     // Polish alphabet in Windows-1250
     const string plawindows1250 =
          "a\271bc\346de\352fghijkl\263mn"
          "\361o\363pqrs\234tuvwxyz\237\277\n"
          "A\245BC\306DE\312FGHIJKL\243MN"
          "\321O\323PQRS\214TUVWXYZ\217\257\n";

     // Mathematical bold alphabet in UTF-32
     const u32string mba32 =
          U"\U0001d400\U0001d401\U0001d402"
          U"\U0001d403\U0001d404\U0001d405\U0001d406\U0001d407"
          U"\U0001d408\U0001d409\U0001d40a\U0001d40b\U0001d40c"
          U"\U0001d40d\U0001d40e\U0001d40f\U0001d410\U0001d411"
          U"\U0001d412\U0001d413\U0001d414\U0001d415\U0001d416"
          U"\U0001d417\U0001d418\U0001d419";
     // Mathematical bold alphabet in UTF-16
     const u16string mba16 =
          u"\U0001d400\U0001d401\U0001d402"
          u"\U0001d403\U0001d404\U0001d405\U0001d406\U0001d407"
          u"\U0001d408\U0001d409\U0001d40a\U0001d40b\U0001d40c"
          u"\U0001d40d\U0001d40e\U0001d40f\U0001d410\U0001d411"
          u"\U0001d412\U0001d413\U0001d414\U0001d415\U0001d416"
          u"\U0001d417\U0001d418\U0001d419";
     // Mathematical bold alphabet in UTF-8
     const string mba8 =
          u8"\U0001d400\U0001d401\U0001d402"
          u8"\U0001d403\U0001d404\U0001d405\U0001d406\U0001d407"
          u8"\U0001d408\U0001d409\U0001d40a\U0001d40b\U0001d40c"
          u8"\U0001d40d\U0001d40e\U0001d40f\U0001d410\U0001d411"
          u8"\U0001d412\U0001d413\U0001d414\U0001d415\U0001d416"
          u8"\U0001d417\U0001d418\U0001d419";

     BOOST_CHECK(pla32.size() == 72);
     BOOST_CHECK(utf16_length(pla16) == 72);
     BOOST_CHECK(utf8_length(pla8) == 72);
     BOOST_CHECK(plaiso88592.size() == 72);
     BOOST_CHECK(plawindows1250.size() == 72);

     BOOST_CHECK(pla32 == utf16_to_utf32(pla16));
     BOOST_CHECK(pla32 == utf8_to_utf32(pla8));
     BOOST_CHECK(pla32 == iso88592_to_utf32(plaiso88592));
     BOOST_CHECK(pla32 == windows1250_to_utf32(plawindows1250));

     BOOST_CHECK(utf32_to_utf16(pla32) == pla16);
     BOOST_CHECK(utf32_to_utf8(pla32) == pla8);
     BOOST_CHECK(utf32_to_iso88592(pla32) == plaiso88592);
     BOOST_CHECK(utf32_to_windows1250(pla32) == plawindows1250);

     BOOST_CHECK(mba32.size() == 26);
     BOOST_CHECK(utf16_length(mba16) == 26);
     BOOST_CHECK(utf8_length(mba8) == 26);
     BOOST_CHECK(mba32 == utf16_to_utf32(mba16));
     BOOST_CHECK(mba32 == utf8_to_utf32(mba8));
     BOOST_CHECK(utf32_to_utf16(mba32) == mba16);
     BOOST_CHECK(utf32_to_utf8(mba32) == mba8);
}

BOOST_AUTO_TEST_CASE(character_conversions_test) {
     // Characters undefined in Windows-1250.
     const std::set<unsigned char> win_undef{0x81, 0x83, 0x88, 0x90,
                                             0x98};
     // Windows-1250 characters undefined in ISO 8859-2.
     const std::set<unsigned char> win_undef_in_iso{
          0x80, 0x82, 0x84, 0x85, 0x86, 0x87, 0x89, 0x8b, 0x91,
          0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x99, 0x9b, 0xa6,
          0xa9, 0xab, 0xac, 0xae, 0xb1, 0xb5, 0xb6, 0xb7, 0xbb};

     for (char32_t c = 0; c <= 0x110000; c++) {
          try {
               const string s = utf32_to_utf8(c);
               BOOST_CHECK(utf8_length(s) == 1);
               const u32string t = utf8_to_utf32(s);
               BOOST_CHECK(t.size() == 1 && t[0] == c);
          } catch (const Conversion_error&) {
               BOOST_CHECK(!is_valid_codepoint(c));
          }
          try {
               const u16string s = utf32_to_utf16(c);
               BOOST_CHECK(utf16_length(s) == 1);
               const u32string t = utf16_to_utf32(s);
               BOOST_CHECK(t.size() == 1 && t[0] == c);
          } catch (const Conversion_error&) {
               BOOST_CHECK(!is_valid_codepoint(c));
          }
     }

     for (int i = 0; i < 256; i++) {
          const char c = i;
          const char32_t d = iso88592_to_utf32(c);

          BOOST_CHECK(utf32_to_iso88592(d) == c);
          if (win_undef.find(c) == win_undef.end())
               BOOST_CHECK(utf32_to_windows1250(
                                windows1250_to_utf32(c)) == c);
          else
               BOOST_CHECK_THROW(windows1250_to_utf32(c),
                                 Conversion_error);

          utf32_to_windows1250(d);  // ok, all ISO 8859-2 characters
                                    // are in Windows-1250

          if (win_undef.find(c) == win_undef.end()) {
               const char32_t d = windows1250_to_utf32(c);
               if (win_undef_in_iso.find(c) ==
                   win_undef_in_iso.end()) {
                    utf32_to_iso88592(d);
               } else {
                    BOOST_CHECK_THROW(utf32_to_iso88592(d),
                                      Conversion_error);
               }
          }
     }
}

/**
 * Data from <a href =
 * "https://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt">
 * <em>UTF-8 decoder capability and stress test</em></a> by <a href =
 * "https://www.cl.cam.ac.uk/~mgk25/">Markus Kuhn</a> downloaded in
 * July 2019 (<a href = "utf8test.txt">local copy</a>).
 */
const struct {
     const string s;
     const bool correct;
} kuhn_data[] = {
     // 1. Some correct UTF-8 text.
     {"\316\272\341\275\271\317\203\316\274\316\265", true},
     // 2. Boundary condition test cases.
     // 2.1. First possible sequence of a certain length.
     {"\000", true},
     {"\302\200", true},
     {"\340\240\200", true},
     {"\360\220\200\200", true},
     {"\370\210\200\200\200", false},      // out of codespace
     {"\374\204\200\200\200\200", false},  // out of codespace
     // 2.2. Last possible sequence of a certain length.
     {"\177", true},
     {"\337\277", true},
     {"\357\277\277", true},
     {"\367\277\277\277", false},          // out of codespace
     {"\373\277\277\277\277", false},      // out of codespace
     {"\375\277\277\277\277\277", false},  // out of codespace
     // 2.3. Other boundary conditions.
     {"\355\237\277", true},
     {"\356\200\200", true},
     {"\357\277\275", true},
     {"\364\217\277\277", true},
     {"\364\220\200\200", false},  // out of codespace
     // 3. Malformed sequences.
     // 3.1. Unexpected continuation bytes.
     {"\200", false},
     {"\277", false},
     {"\200\277", false},
     {"\200\277\200", false},
     {"\200\277\200\277", false},
     {"\200\277\200\277\200", false},
     {"\200\277\200\277\200\277", false},
     {"\200\277\200\277\200\277\200", false},
     {"\200\201\202\203\204\205\206\207\210\211\212\213\214"
      "\215\216\217\220\221\222\223\224\225\226\227\230\231"
      "\232\233\234\235\236\237\240\241\242\243\244\245\246"
      "\247\250\251\252\253\254\255\256\257\260\261\262\263"
      "\264\265\266\267\270\271\272\273\274\275\276\277",
      false},
     // 3.2. Lonely start characters.
     {"\300 \301 \302 \303 \304 \305 \306 \307 "
      "\310 \311 \312 \313 \314 \315 \316 \317"
      "\320 \321 \322 \323 \324 \325 \326 \327 "
      "\330 \331 \332 \333 \334 \335 \336 \337 ",
      false},
     {"\340 \341 \342 \343 \344 \345 \346 \347 "
      "\350 \351 \352 \353 \354 \355 \356 \357 ",
      false},
     {"\360 \361 \362 \363 \364 \365 \366 \367 ", false},
     {"\370 \371 \372 \373 ", false},
     {"\374 \375 ", false},
     // 3.3. Sequences with last continuation byte missing.
     {"\300", false},
     {"\340\200", false},
     {"\360\200\200", false},
     {"\370\200\200\200", false},
     {"\374\200\200\200\200", false},
     {"\337", false},
     {"\357\277", false},
     {"\367\277\277", false},
     {"\373\277\277\277", false},
     {"\375\277\277\277\277", false},
     // 3.4. Concatenation of incomplete sequences.
     {"\300\340\200\360\200\200\370\200\200\200\374\200\200"
      "\200\200\337\357\277\367\277\277\373\277\277\277\375"
      "\277\277\277\277",
      false},
     // 3.5. Impossible bytes.
     {"\376", false},
     {"\377", false},
     {"\376\376\377\377", false},
     // 4. Overlong sequences.
     // 4.1. Examples of an overlong ASCII character.
     {"\300\257", false},
     {"\340\200\257", false},
     {"\360\200\200\257", false},
     {"\370\200\200\200\257", false},
     {"\374\200\200\200\200\257", false},
     // 4.2. Maximum overlong sequences.
     {"\301\277", false},
     {"\340\237\277", false},
     {"\360\217\277\277", false},
     {"\370\207\277\277\277", false},
     {"\374\203\277\277\277\277", false},
     // 4.3. Overlong representation of the NUL character.
     {"\300\200", false},
     {"\340\200\200", false},
     {"\360\200\200\200", false},
     {"\370\200\200\200\200", false},
     {"\374\200\200\200\200\200", false},
     // 5. Illegal code positions.
     // 5.1. Single UTF-16 surrogates.
     {"\355\240\200", false},
     {"\355\255\277", false},
     {"\355\256\200", false},
     {"\355\257\277", false},
     {"\355\260\200", false},
     {"\355\276\200", false},
     {"\355\277\277", false},
     // 5.2. Paired UTF-16 surrogates.
     {"\355\240\200\355\260\200", false},
     {"\355\240\200\355\277\277", false},
     {"\355\255\277\355\260\200", false},
     {"\355\255\277\355\277\277", false},
     {"\355\256\200\355\260\200", false},
     {"\355\256\200\355\277\277", false},
     {"\355\257\277\355\260\200", false},
     {"\355\257\277\355\277\277", false},
     // 5.3 Noncharacter code positions.
     {"\357\277\276", true},
     {"\357\277\277", true},
     {"\357\267\220\357\267\221\357\267\222\357\267\223\357"
      "\267\224\357\267\225\357\267\226\357\267\227\357\267"
      "\230\357\267\231\357\267\232\357\267\233\357\267\234"
      "\357\267\235\357\267\236\357\267\237\357\267\240\357"
      "\267\241\357\267\242\357\267\243\357\267\244\357\267"
      "\245\357\267\246\357\267\247\357\267\250\357\267\251"
      "\357\267\252\357\267\253\357\267\254\357\267\255\357"
      "\267\256\357\267\257",
      true},
     {"\360\237\277\276\360\237\277\277\360\257\277\276\360"
      "\257\277\277\360\277\277\276\360\277\277\277\361\217"
      "\277\276\361\217\277\277\361\237\277\276\361\237\277"
      "\277\361\257\277\276\361\257\277\277\361\277\277\276"
      "\361\277\277\277\362\217\277\276\362\217\277\277\362"
      "\237\277\276\362\237\277\277\362\257\277\276\362\257"
      "\277\277\362\277\277\276\362\277\277\277\363\217\277"
      "\276\363\217\277\277\363\237\277\276\363\237\277\277"
      "\363\257\277\276\363\257\277\277\363\277\277\276\363"
      "\277\277\277\364\217\277\276\364\217\277\277",
      true}};

BOOST_AUTO_TEST_CASE(kuhn_test) {
     for (const auto& [s, correct] : kuhn_data)
          if (correct)
               utf8_to_utf32(s);
          else
               BOOST_CHECK_THROW(utf8_to_utf32(s), Conversion_error);
}

const u16string invalid_utf16[] = {
     // high surrogate not followed by low surrogate
     u"\xd800\xd800", u"\xd801\xd800", u"\xdbff\xd800",
     u"\xd800\xd801", u"\xd801\xd801", u"\xdbff\xd801",
     u"\xd800\x0001", u"\xd801\x0001", u"\xdbff\x0001",
     // start with low surrogate
     u"\xdc00", u"\xdc01", u"\xdfff"};

BOOST_AUTO_TEST_CASE(invalid_utf16_strings_test) {
     for (const auto& s : invalid_utf16)
          BOOST_CHECK_THROW(utf16_to_utf32(s), Conversion_error);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
