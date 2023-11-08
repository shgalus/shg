/**
 * \file include/shg/encoding.h
 * Character encodings and character sets.
 * Created on 7 July 2019.
 */

#ifndef SHG_ENCODING_H
#define SHG_ENCODING_H

#include <limits>
#include <stdexcept>
#include <string>

/**
 * \namespace SHG::Encoding Character encodings and character sets.
 *
 * Unicode defines a codespace of 1114112 code points in the range
 * from U+0000 to U+10FFFF. The codespace is divided into 17 planes,
 * each of which contains 65536 code points. The first plane is called
 * Basic Multilingual Plane (code points U+0000-U+FFFF). Some code
 * points have special meaning. Code points U+D800-U+DFFF are so
 * called surrogate code points and cannot be used. This makes 1114112
 * \- 2048 = 1112064 valid code points. Some code points remain
 * unassigned.
 *
 * \implementation Conversions among UTF-8, UTF-16, UTF-32, as
 * described in \cite josuttis-2012, p. 901, are not used here as \e
 * std::wstring_convert is deprecated in C++17.
 */
namespace SHG::Encoding {

/**
 * \addtogroup miscellaneous_utilities
 *
 * \{
 */

/**
 * \defgroup char_enc_char_sets Character encodings and character sets
 *
 * Functions related to character encodings and character sets.
 *
 * \{
 */

static_assert(std::numeric_limits<unsigned char>::max() == 0xffu);

/**
 * Exception class for encoding error.
 */
class Conversion_error : public std::runtime_error {
public:
     Conversion_error();
};

/**
 * Converts UTF-8 string \e s to UTF-32 string. \exception
 * Conversion_error if the string contains invalid character sequence
 */
std::u32string utf8_to_utf32(std::string const& s);

/**
 * Converts UTF-32 character \e c to UTF-8 string. \exception
 * Conversion_error if \e c is not Unicode character
 */
std::string utf32_to_utf8(char32_t c);

/**
 * Converts UTF-32 string \e s to UTF-8 string. \exception
 * Conversion_error if \e s contains invalid code point
 */
std::string utf32_to_utf8(std::u32string const& s);

/**
 * Converts UTF-16 string \e s to UTF-32 string. \exception
 * Conversion_error if the string contains invalid character sequence
 */
std::u32string utf16_to_utf32(std::u16string const& s);

/**
 * Converts UTF-32 character \e c to UTF-16 string. \exception
 * Conversion_error if \e c is not Unicode character
 */
std::u16string utf32_to_utf16(char32_t c);

/**
 * Converts UTF-32 string \e s to UTF-16 string. \exception
 * Conversion_error if \e s contains invalid code point
 */
std::u16string utf32_to_utf16(std::u32string const& s);

/**
 * Converts ISO 8859-2 character to UTF-32 character.
 */
char32_t iso88592_to_utf32(char c);

/**
 * Converts ISO 8859-2 string to UTF-32 ctring.
 */
std::u32string iso88592_to_utf32(std::string const& s);

/**
 * Converts UTF-32 character to ISO 8859-2 character. \exception
 * Conversion_error if \e c has no representation in ISO 8859-2
 */
char utf32_to_iso88592(char32_t c);

/**
 * Converts UTF-32 string to ISO 8859-2 string. \exception
 * Conversion_error if there exists a character in \e s which has no
 * representation in ISO 8859-2
 */
std::string utf32_to_iso88592(std::u32string const& s);

/**
 * Converts Windows-1250 character to UTF-32 character.
 */
char32_t windows1250_to_utf32(char c);

/**
 * Converts Windows-1250 string to UTF-32 string.
 */
std::u32string windows1250_to_utf32(std::string const& s);

/**
 * Converts UTF-32 character to Windows-1250 character. \exception
 * Conversion_error if \e c has no representation in Windows-1250
 */
char utf32_to_windows1250(char32_t c);

/**
 * Converts UTF-32 string to Windows-1250 string. \exception
 * Conversion_error if there exists a character in \e s which has no
 * representation in Windows-1250
 */
std::string utf32_to_windows1250(std::u32string const& s);

/**
 * Returns the number of characters in the UTF-8 string. \exception
 * Conversion_error if the string contains invalid character sequence
 */
std::string::size_type utf8_length(std::string const& s);

/**
 * Returns the number of characters in the UTF-16 string. \exception
 * Conversion_error if the string contains invalid character sequence
 */
std::u16string::size_type utf16_length(std::u16string const& s);

/**
 * Returns \c true iff \e c is a high surrogate code point.
 */
constexpr bool is_high_surrogate(char32_t c) {
     return c >= 0xd800u && c < 0xdc00u;
}

/**
 * Returns \c true iff \e c is a low surrogate code point.
 */
constexpr bool is_low_surrogate(char32_t c) {
     return c >= 0xdc00u && c < 0xe000u;
}

/**
 * Returns \c true iff \e c is a surrogate code point.
 */
constexpr bool is_surrogate(char32_t c) {
     return c >= 0xd800u && c < 0xe000u;
}

/**
 * Returns \c true iff \e c is a valid code point.
 */
constexpr bool is_valid_codepoint(char32_t c) {
     return c < 0x110000u && !is_surrogate(c);
}

/** \} */  // end of group char_enc_char_sets
/** \} */  // end of group miscellaneous_utilities

}  // namespace SHG::Encoding

#endif
