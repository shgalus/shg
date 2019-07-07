/* encoding.cc: character encodings and character sets */

/**
 * \file src/encoding.cc
 * Character encodings and character sets.
 * Created on 7 July 2019.
 */

/*
 * Unicode defines a codespace of 1114112 code points in the range
 * from U+0000 to U+10FFFF. The codespace is divided into 17 planes,
 * each of which contains 65536 code points. The first plane is called
 * Basic Multilingual Plane (code points U+0000–U+​FFFF). Some code
 * points have special meaning. Code points U+D800-U+DFFF are so
 * called surrogate code points and cannot be used. Some code points
 * remain unassigned.
 */

#include <cassert>
#include <stdexcept>
#include "shg/encoding.h"

namespace SHG::Encoding {

using std::u16string, std::u32string;

u8string utf32_to_utf8(char32_t c) {
     u8string::size_type nchars;
     char8_t w[4];

     /*
      * 00000000-0000007F: 
      * 00000000 00000000 00000000 0xxxxxxx (7 bits)
      * is translated to
      * 0xxxxxxx
      *
      * 00000080-000007FF:
      * 00000000 00000000 00000xxx xxxxxxxx (11 bits)
      * is translated to
      * 110xxxxx 10xxxxxx
      *
      * 00000800-0000FFFF: 
      * 00000000 00000000 xxxxxxxx xxxxxxxx (16 bits)
      * is translated to
      * 1110xxxx 10xxxxxx 10xxxxxx
      *        
      * 00010000-001FFFFF: 
      * 00000000 000xxxxx xxxxxxxx xxxxxxxx (21 bits)
      * is translated to 
      * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
      */
     
     if (c < 0x80u) {
          w[0] = static_cast<char8_t>(c);
          nchars = 1;
     } else if (c < 0x0800u) {
          w[0] = static_cast<char8_t>(0xc0u + c / 0x40u);
          w[1] = 0x80u + c % 0x40u;
          nchars = 2;
     } else if (c < 0x10000u) {
          if (c >= 0xd800u && c < 0xe000u)
               goto error;      // UTF-16 surrogate
          w[0] = static_cast<char8_t>(0xe0u + c / 0x1000u);
          c %= 0x1000u;
          w[1] = static_cast<char8_t>(0x80u + c / 0x40u);
          w[2] = 0x80u + c % 0x40u;
          nchars = 3;
     } else if (c <= 0x10ffffu) {
          w[0] = static_cast<char8_t>(0xf0u + c / 0x40000u);
          c %= 0x40000u;
          w[1] = static_cast<char8_t>(0x80u + c / 0x1000u);
          c %= 0x1000u;
          w[2] = static_cast<char8_t>(0x80u + c / 0x40u);
          w[3] = 0x80u + c % 0x40u;
          nchars = 4;
     } else
          goto error;
     return u8string(reinterpret_cast<char8_t*>(w), nchars);
error:
     throw std::invalid_argument(__func__);
}

namespace {

/**
 * Decodes UTF-8 sequence contained in s starting from position i. If
 * the sequence is correct, its Unicode code is returned and the
 * position i is set one character before the beginning of the next
 * sequence. If the sequence is incorrect, std::invalid_argument is
 * thrown and the position in i remains undefinite.
 */
char32_t getc(const u8string& s, u8string::size_type& i) {
     char32_t u;
     int nbytes;
     char8_t b = s[i];

     if (b < 0x80u) {
          u = b;
          nbytes = 0;
     } else if (b < 0xc0u) {
          goto error;
     } else if (b < 0xe0u) {
          u = b & 0x1fu;
          nbytes = 1;
     } else if (b < 0xf0u) {
          u = b & 0x0fu;
          nbytes = 2;
     } else if (b < 0xf8u) {
          u = b & 0x07u;
          nbytes = 3;
     } else {
          goto error;
     }
     for (int j = 0; j < nbytes; j++)
          if (++i >= s.size() ||
              ((b = s[i]) & 0xc0u) != 0x80u)
               goto error;
          else
               (u <<= 6) += b & 0x3fu;
     /* Check for overlong sequence. */
     if (u < 0x80u) {
          if (nbytes > 0)
               goto error;
     } else if (u < 0x0800u) {
          if (nbytes > 1)
               goto error;
     } else if (u < 0x10000u) {
          if (nbytes > 2)
               goto error;
     } else if (u > 0x10ffffu) {
          goto error;
     }
     /* Check for UTF-16 surrogate. */
     if (u >= 0xd800u && u < 0xe000u)
          goto error;
     return u;
error:
     throw std::invalid_argument(__func__);
}

}       // anonymous namespace

u32string utf8_to_utf32(const u8string& s) {
     u32string v;
     for (u8string::size_type i = 0; i < s.size(); i++)
          v += getc(s, i);
     return v;
}

u8string::size_type utf8len(const u8string& s) {
     u8string::size_type n = 0;
     for (u8string::size_type i = 0; i < s.size(); i++, n++)
          getc(s, i);
     return n;
}

}       // namespace SHG::Encoding
