/* encoding.h: character encodings and character sets */

/**
 * \file include/shg/encoding.h
 * Character encodings and character sets.
 * Created on 7 July 2019.
 */

#ifndef SHG_ENCODING_H
#define SHG_ENCODING_H

#include <string>

namespace SHG::Encoding {

// With C++20 char8_t this will change.
typedef unsigned char char8_t;
typedef std::basic_string<char8_t> u8string;

/** Returns UTF-8 representation of Unicode character c. Throws
    std::invalid_argument if c > 0x10ffffu (code point out of
    codespace) or if 0xd800u <= c < 0xe000u (UTF-16 surrogate). */
u8string utf32_to_utf8(char32_t c);

std::u32string utf8_to_utf32(const u8string& s);

u8string::size_type utf8len(const u8string& s);

}       // namespace SHG::Encoding

#endif
