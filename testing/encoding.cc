/* encoding.cc: testing character encodings and character sets */

/**
 * \file testing/encoding.cc
 * Testing character encodings and character sets.
 * Created on  7 July 2019.
 */

#include <stdexcept>
#include "shg/encoding.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using namespace SHG::Encoding;

namespace {

void test1() {
     u8string s;
     std::u32string u;
     for (char32_t c = 0; c <= 0x10ffff; c++) {
          try {
               s = utf32_to_utf8(c);

               try {
                    u = utf8_to_utf32(s);
                    SHG_ASSERT(u.size() == 1);
                    SHG_ASSERT(u[0] == c);
               } catch (const std::invalid_argument&) {
                    SHG_ASSERT(0);
               }

          } catch (const std::invalid_argument&) {
               SHG_ASSERT(c >= 0xd800u && c < 0xe000u);
          }
     }
}

}       // anonymous namespace

void test_encoding() {

     test1();

}

}       // namespace Testing
}       // namespace SHG
