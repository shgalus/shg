/* unicode.cc: testing Unicode encoding */

/**
 * \file testing/unicode.cc
 * Testing Unicode encoding.
 * \date Created on 21 September 2010.
 */

#include <iostream>
#include <stdexcept>
#include "shg/unicode.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

/**
 * Encodes a Unicode character u in UTF-8. The returned value is the
 * number of bytes in UTF-8 representation, namely 1, 2, 3 or 4. If
 * the returned value is 1, the character c0 is the representation, if
 * the returned value is 2, characters c0 and c1 make the
 * representation and so on. If the returned value is 0, and invalid
 * character u was given, that is u >= 0x110000u.
 */
int utf8_encode(std::uint32_t u,
                char* c0, char* c1, char* c2, char* c3) {
     if (u < 0x0080u) {
          /*
           * 1 byte output
           *
           * 0xxxxxxx will be translated into:
           *
           * +----------+
           * | 0xxxxxxx |
           * +----------+
           * |    c0    |
           * +----------+
           */
          const unsigned char x = u & 0x7fu;
          *c0 = x;
          return 1;
     } else if (u < 0x0800u) {
          /*
           * 2 bytes output
           *
           * 00000yyy xxxxxxxx will be translated into:
           *
           * +----------+----------+
           * | 110yyyxx | 10xxxxxx |
           * +----------+----------+
           * |    c0    |    c1    |
           * +----------+----------+
           */
          const unsigned char y = (u & 0xff00u) / 0x0100u;
          const unsigned char x = u & 0xffu;
          *c0 = static_cast<char>(0xc0u + (y << 2) + (x >> 6));
          *c1 = 0x80u + (x & 0x3f);
          return 2;
     } else if (u < 0x010000u) {
          /*
           * 3 bytes output
           *
           * yyyyyyyy xxxxxxxx will be translated into:
           *
           * +----------+----------+----------+
           * | 1110yyyy | 10yyyyxx | 10xxxxxx |
           * +----------+----------+----------+
           * |    c0    |    c1    |    c2    |
           * +----------+----------+----------+
           */
          const unsigned char y = (u & 0xff00u) / 0x0100u;
          const unsigned char x = u & 0xffu;
          *c0 = 0xe0u + (y >> 4);
          *c1 = static_cast<char>(0x80u + ((y & 0x0f) << 2) + (x >> 6));
          *c2 = 0x80u + (x & 0x3f);
          return 3;
     } else if (u < 0x110000u) {
          /*
           * 4 bytes output
           *
           * 000zzzzz yyyyyyyy xxxxxxxx will be translated into:
           *
           * +----------+----------+----------+----------+
           * | 11110zzz | 10zzyyyy | 10yyyyxx | 10xxxxxx |
           * +----------+----------+----------+----------+
           * |    c0    |    c1    |    c2    |    c3    |
           * +----------+----------+----------+----------+
           */
          const unsigned char z = (u & 0xff0000u) / 0x010000u;
          const unsigned char y = (u & 0xff00u) / 0x0100u;
          const unsigned char x = u & 0xffu;
          *c0 = 0xf0u + (z >> 2);
          *c1 = static_cast<char>(
               0x80u + ((z & 0x03) << 4) + ((y & 0xff) >> 4));
          *c2 = static_cast<char>(
               0x80u + ((y & 0x0f) << 2) + ((x & 0xc0) >> 6));
          *c3 = 0x80u + (x & 0x3f);
          return 4;
     } else {
          return 0;
     }
}

/**
 * Tests auxiliary function utf8_encode.
 */
void test1() {
     char c0, c1, c2, c3;
     int s;

     // Test all ASCII characters - one byte.
     for (int c = 0; c < 0x80; c++) {
          s = utf8_encode(c, &c0, &c1, &c2, &c3);
          SHG_ASSERT(s == 1 && c0 == c);
     }
     // Test U+00A2 - two bytes.
     s = utf8_encode(0x00a2, &c0, &c1, &c2, &c3);
     SHG_ASSERT(s == 2 && c0 == '\xc2' && c1 == '\xa2');
     // Test U+20AC - three bytes.
     s = utf8_encode(0x20ac, &c0, &c1, &c2, &c3);
     SHG_ASSERT(s == 3 && c0 == '\xe2' && c1 == '\x82' && c2 == '\xac');
     // Test U+024B62 - four bytes.
     s = utf8_encode(0x024b62, &c0, &c1, &c2, &c3);
     SHG_ASSERT(s == 4 && c0 == '\xf0' && c1 == '\xa4' &&
                c2 == '\xad' && c3 == '\xa2');
}

/**
 * Tests one character.
 */
void testone(std::uint32_t c, std::string::size_type z) {
     const std::string s = SHG::utf8_encode(c);
     SHG_ASSERT(s.size() == z);
     SHG_ASSERT(SHG::utf8_strlen(s) == 1);
     std::vector<std::uint32_t> v = SHG::utf8_decode(s);
     SHG_ASSERT(v.size() == 1);
     SHG_ASSERT(v[0] == c);
}

/*
 * Tests first 10, middle 10 and last 10 Unicode characters from the
 * given interval [a, b).
 */
void testab(std::uint32_t a, std::uint32_t b,
            std::string::size_type z) {
     std::uint32_t c;
     for (c = a; c < a + 10; c++)
          testone(c, z);
     for (c = (a + b) / 2; c < (a + b) / 2 + 10; c++)
          /* Skip UTF-16 surrogates. */
          if (c < 0xd800u || c > 0xdfffu)
               testone(c, z);
     for (c = b - 10; c < b; c++)
          testone(c, z);
}

void test2() {
     testab(0, 0x80, 1);
     testab(0x80, 0x800, 2);
     testab(0x800, 0x10000, 3);
     testab(0x10000, 0x200000, 4);
     testab(0x200000, 0x4000000, 5);
     testab(0x4000000, 0x80000000, 6);
}

void test3() {
     using std::uint32_t;
     using std::string;
     for (uint32_t c = 0; c < 0x110000u; c++)
          /* Skip UTF-16 surrogates. */
          if (c < 0xd800u || c > 0xdfffu) {
               const string s = SHG::utf8_encode(c);
               char c0, c1, c2, c3;
               const int r = utf8_encode(c, &c0, &c1, &c2, &c3);
               SHG_ASSERT(r >= 1 && r <= 4);
               SHG_ASSERT(s[0] == c0);
               if (r > 1) {
                    SHG_ASSERT(s[1] == c1);
                    if (r > 2) {
                         SHG_ASSERT(s[2] == c2);
                         if (r > 3)
                              SHG_ASSERT(s[3] == c3);
                    }
               }
          }
}

/**
 * Auxiliary function for test4().
 */
void check(const char* s, bool correct) {
     if (correct) {
          std::vector<std::uint32_t> v = SHG::utf8_decode(s);
     } else {
          try {
               std::vector<std::uint32_t> v = SHG::utf8_decode(s);
               SHG_ASSERT(false);
          } catch (std::invalid_argument) {}
     }
}

/**
 * Performs test based on Markus Kuhn file
 * http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt, 26
 * December 2013.
 */
void test4() {
     /* Section 1. Some correct UTF-8 text. */
     check("\316\272\341\275\271\317\203\316\274\316\265", true);

     /* Section 2. Boundary condition test cases. */
     check("\000", true);
     check("\302\200", true);
     check("\340\240\200", true);
     check("\360\220\200\200", true);
     check("\370\210\200\200\200", true);
     check("\374\204\200\200\200\200", true);
     check("\177", true);
     check("\337\277", true);
     check("\357\277\277", true);
     check("\367\277\277\277", true);
     check("\373\277\277\277\277", true);
     check("\375\277\277\277\277\277", true);
     check("\355\237\277", true);
     check("\356\200\200", true);
     check("\357\277\275", true);
     check("\364\217\277\277", true);
     check("\364\220\200\200", true);

     /* Section 3. Malformed sequences. */
     check("\200", false);
     check("\277", false);
     check("\200\277", false);
     check("\200\277\200", false);
     check("\200\277\200\277", false);
     check("\200\277\200\277\200", false);
     check("\200\277\200\277\200\277", false);
     check("\200\277\200\277\200\277\200", false);
     check("\200\201\202\203\204\205\206\207\210\211\212\213\214"
           "\215\216\217\220\221\222\223\224\225\226\227\230\231"
           "\232\233\234\235\236\237\240\241\242\243\244\245\246"
           "\247\250\251\252\253\254\255\256\257\260\261\262\263"
           "\264\265\266\267\270\271\272\273\274\275\276\277", false);
     check("\300 \301 \302 \303 \304 \305 \306 \307 "
           "\310 \311 \312 \313 \314 \315 \316 \317"
           "\320 \321 \322 \323 \324 \325 \326 \327 "
           "\330 \331 \332 \333 \334 \335 \336 \337 ", false);
     check("\340 \341 \342 \343 \344 \345 \346 \347 "
           "\350 \351 \352 \353 \354 \355 \356 \357 ", false);
     check("\360 \361 \362 \363 \364 \365 \366 \367 ", false);
     check("\370 \371 \372 \373 ", false);
     check("\374 \375 ", false);
     check("\300", false);
     check("\340\200", false);
     check("\360\200\200", false);
     check("\370\200\200\200", false);
     check("\374\200\200\200\200", false);
     check("\337", false);
     check("\357\277", false);
     check("\367\277\277", false);
     check("\373\277\277\277", false);
     check("\375\277\277\277\277", false);
     check("\300\340\200\360\200\200\370\200\200\200\374\200\200"
           "\200\200\337\357\277\367\277\277\373\277\277\277\375"
           "\277\277\277\277", false);
     check("\376", false);
     check("\377", false);
     check("\376\376\377\377", false);

     /* Section 4. Overlong sequences. */
     check("\300\257", false);
     check("\340\200\257", false);
     check("\360\200\200\257", false);
     check("\370\200\200\200\257", false);
     check("\374\200\200\200\200\257", false);
     check("\301\277", false);
     check("\340\237\277", false);
     check("\360\217\277\277", false);
     check("\370\207\277\277\277", false);
     check("\374\203\277\277\277\277", false);
     check("\300\200", false);
     check("\340\200\200", false);
     check("\360\200\200\200", false);
     check("\370\200\200\200\200", false);
     check("\374\200\200\200\200\200", false);

     /* Section 5. Illegal code positions. */
     check("\355\240\200", false);
     check("\355\255\277", false);
     check("\355\256\200", false);
     check("\355\257\277", false);
     check("\355\260\200", false);
     check("\355\276\200", false);
     check("\355\277\277", false);
     check("\355\240\200\355\260\200", false);
     check("\355\240\200\355\277\277", false);
     check("\355\255\277\355\260\200", false);
     check("\355\255\277\355\277\277", false);
     check("\355\256\200\355\260\200", false);
     check("\355\256\200\355\277\277", false);
     check("\355\257\277\355\260\200", false);
     check("\355\257\277\355\277\277", false);
     /* The following two are assumed correct. */
     check("\357\277\276", true);
     check("\357\277\277", true);
}

// Unicode codes of ISO_8859-2:1987 character set written in UTF-8.
const char* const l2 =
     // 0x00 to 0x7f, one byte per character
     "\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"
     "\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"
     "\040\041\042\043\044\045\046\047\050\051\052\053\054\055\056\057"
     "\060\061\062\063\064\065\066\067\070\071\072\073\074\075\076\077"
     "\100\101\102\103\104\105\106\107\110\111\112\113\114\115\116\117"
     "\120\121\122\123\124\125\126\127\130\131\132\133\134\135\136\137"
     "\140\141\142\143\144\145\146\147\150\151\152\153\154\155\156\157"
     "\160\161\162\163\164\165\166\167\170\171\172\173\174\175\176\177"
     // 0x80 to 0xff, two bytes per character
     "\302\200\302\201\302\202\302\203\302\204\302\205\302\206\302\207"
     "\302\210\302\211\302\212\302\213\302\214\302\215\302\216\302\217"
     "\302\220\302\221\302\222\302\223\302\224\302\225\302\226\302\227"
     "\302\230\302\231\302\232\302\233\302\234\302\235\302\236\302\237"
     "\302\240\304\204\313\230\305\201\302\244\304\275\305\232\302\247"
     "\302\250\305\240\305\236\305\244\305\271\302\255\305\275\305\273"
     "\302\260\304\205\313\233\305\202\302\264\304\276\305\233\313\207"
     "\302\270\305\241\305\237\305\245\305\272\313\235\305\276\305\274"
     "\305\224\303\201\303\202\304\202\303\204\304\271\304\206\303\207"
     "\304\214\303\211\304\230\303\213\304\232\303\215\303\216\304\216"
     "\304\220\305\203\305\207\303\223\303\224\305\220\303\226\303\227"
     "\305\230\305\256\303\232\305\260\303\234\303\235\305\242\303\237"
     "\305\225\303\241\303\242\304\203\303\244\304\272\304\207\303\247"
     "\304\215\303\251\304\231\303\253\304\233\303\255\303\256\304\217"
     "\304\221\305\204\305\210\303\263\303\264\305\221\303\266\303\267"
     "\305\231\305\257\303\272\305\261\303\274\303\275\305\243\313\231";

void test5() {
     using std::string;
     using std::uint32_t;
     using SHG::l2tou;
     using SHG::utf8_encode;

     const char* t = l2;
     string s = utf8_encode(l2tou(0));
     SHG_ASSERT(s.size() == 1 && s[0] == '\0');
     t++;

     for (uint32_t u = 1; u < 256u; u++) {
          s = SHG::utf8_encode(l2tou(static_cast<char>(u)));
          SHG_ASSERT(s.size() == 1 || s.size() == 2);
          SHG_ASSERT(*t && *t == s[0]);
          t++;
          if (s.size() == 2) {
               SHG_ASSERT(*t && *t == s[1]);
               t++;
          }
     }
     SHG_ASSERT(!*t);
}

}       // anonymous namespace

void test_unicode() {
     test1();
     test2();
     test3();
     test4();
     test5();
}

}       // namespace Testing
}       // namespace SHG
