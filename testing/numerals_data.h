#ifndef NUMERALS_DATA_H
#define NUMERALS_DATA_H

#include <string>
#include <vector>

namespace SHG::Testing {

/**
 * Generates numeral prefixes. The generated vector contains 1999
 * numeral prefixes of the form \em jedno, \em dwu, ..., \em
 * tysiacdziewiecsetdziewiecdziesieciodziewiecio.
 */
std::vector<std::string> generate_numeral_prefixes();

}  // namespace SHG::Testing

#endif
