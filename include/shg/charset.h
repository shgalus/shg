/**
 * \file include/shg/charset.h
 * Character set used internally by the class Dictionary.
 */

#ifndef SHG_CHARSET_H
#define SHG_CHARSET_H

#include <string>
#include <algorithm>
#include <stdexcept>
#include <boost/algorithm/string/predicate.hpp>

namespace SHG::PLP {

using plchar = char;
using plstring = std::string;

/**
 * Character set used in Polish language processing. See \ref
 * plpcharset.
 */
namespace Charset {

class Invalid_character_error : public std::runtime_error {
public:
     Invalid_character_error();
};

char32_t unicode(char c);
char const* name(char c);
char unicode_to_char(char32_t c);
std::string utf8_to_charset(std::string const& s);
std::string charset_to_utf8(std::string const& s);

bool isalnum(char c);
bool isalpha(char c);
bool isblank(char c);
bool iscntrl(char c);
bool isdigit(char c);
bool isgraph(char c);
bool islower(char c);
bool isprint(char c);
bool ispunct(char c);
bool isspace(char c);
bool isupper(char c);
bool isxdigit(char c);
char tolower(char c);
char toupper(char c);

int chrcmp(char lhs, char rhs);
int alpha_strcmp(char const* lhs, char const* rhs);

void lowercase(std::string& s);
void uppercase(std::string& s);
void capitalize(std::string& s);

bool is_proper_prefix(std::string const& input,
                      std::string const& test);
bool is_proper_suffix(std::string const& input,
                      std::string const& test);

bool is_lower(plstring const& s);
bool is_upper(plstring const& s);
bool is_capit(plstring const& s);

void print_character_table(std::ostream& f);

/**
 * Returns pointer to 256-element constant array which defines order
 * of characters.
 */
unsigned char const* get_ordtab();

inline void lowercase(std::string& s) {
     std::transform(s.begin(), s.end(), s.begin(), tolower);
}

inline void uppercase(std::string& s) {
     std::transform(s.begin(), s.end(), s.begin(), toupper);
}

inline void capitalize(std::string& s) {
     if (s.size() > 0) {
          lowercase(s);
          s[0] = toupper(s[0]);
     }
}

inline bool is_proper_prefix(std::string const& input,
                             std::string const& test) {
     return input.size() > test.size() &&
            boost::starts_with(input, test);
}

inline bool is_proper_suffix(std::string const& input,
                             std::string const& test) {
     return input.size() > test.size() &&
            boost::ends_with(input, test);
}

inline bool is_lower(plstring const& s) {
     return std::all_of(s.cbegin(), s.cend(), islower);
}

inline bool is_upper(plstring const& s) {
     return std::all_of(s.cbegin(), s.cend(), isupper);
}

}  // namespace Charset

}  // namespace SHG::PLP

#endif
