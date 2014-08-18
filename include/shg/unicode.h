/* unicode.h: Unicode encoding */

/**
 * \file include/shg/unicode.h
 * Unicode encoding.
 * \date Created on 21 September 2010.
 */

#ifndef SHG_UNICODE_H
#define SHG_UNICODE_H

#include <cstdint>
#include <string>
#include <vector>

namespace SHG {

/**
 * \addtogroup miscellaneous_utilities
 *
 * \{
 */

/**
 * \defgroup unicode_encoding Unicode encoding
 *
 * Functions related to Unicode.
 *
 * \{
 */

/**
 * Encodes Unicode character in UTF-8.
 */
std::string utf8_encode(std::uint32_t c);

/**
 * Decodes UTF-8 string into Unicode.
 */
std::vector<std::uint32_t> utf8_decode(const std::string& s);

/**
 * Number of Unicode characters in UTF-8 string.
 */
std::string::size_type utf8_strlen(const std::string& s);

/**
 * Encodes an ISO_8859-2:1987 character in Unicode.
 */
std::uint32_t l2tou(char c);

/** \} */       // end of group unicode_encoding
/** \} */       // end of group miscellaneous_utilities

}       // namespace SHG

#endif
