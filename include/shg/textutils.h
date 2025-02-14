/**
 * \file include/shg/textutils.h
 * Text processing utility functions.
 */

#ifndef SHG_TEXTUTILS_H
#define SHG_TEXTUTILS_H

#include <shg/utils.h>

namespace SHG {

/**
 * \defgroup text_utilities Text processing utility functions
 *
 * Text processing utility functions.
 *
 * \{
 */

/**
 * Reads logical lines from a text stream.
 */
class Logical_line {
public:
     Logical_line(std::istream& input, char delim = '\n');

     /**
      * A physical line which ends with `[ \f\n\r\t\v]*\\\n` or is
      * followed by a physical line which starts with a space has a
      * continuation on the next physical line.
      */
     bool getline(std::string& str);

     /**
      * A physical line which ends with `[ \f\n\r\t\v]*\\\n` has a
      * continuation on the next physical line.
      */
     static std::istream& getline(std::istream& input,
                                  std::string& str,
                                  char delim = '\n');

private:
     std::istream& input;
     char delim;
     std::string nextline{};
     bool empty{true};
};

/**
 * Extracts delim-separated fields from a string and returns them in a
 * vector. If the delim character is preceded by a backslash, it does
 * not separate fields. Escape sequences \\', \\", \\?, \\\\, \\a,
 * \\b, \\f, \\n, \\r, \\t, \\v are replaced by their respective
 * characters.
 */
bool string_to_fields(std::string const& str, char delim,
                      std::vector<std::string>& v);

/** \} */ /* end of group text_utilities */

}  // namespace SHG

#endif
