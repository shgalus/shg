/**
 * \file include/shg/numerals.h
 * Numerals and derived parts of speech.
 */

#ifndef SHG_NUMERALS_H
#define SHG_NUMERALS_H

#include <shg/dict.h>

namespace SHG::PLP {

/**
 * Checks if \c s starts with a numeral prefix. If a prefix is found,
 * the first position after the prefix is returned. Example:
 * find_numeral_prefix("dwukilogramowy") == 3. The range of numeral
 * prefixes is from 1 to 1999. Each numeral prefix may include prefix
 * \em ponad or suffix \em ipol.
 */
plstring::size_type find_numeral_prefix(plstring const& s);

/**
 * Checks if \c s is a numeral adverb (like \em trzykrotnie).
 * If so, the description is added to \c sd.
 * The string \c s must be in lowercase in Charset encoding.
 * The range of numeral adverbs checked is from 1 to 1999.
 *
 * See \cite szober-1957, page 102-103.
 */
void check_numeral_adverbs(std::string const& s, Setdesc& sd);

}  // namespace SHG::PLP

#endif
