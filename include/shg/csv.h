/**
 * \file include/shg/csv.h
 * CSV format.
 */

#ifndef SHG_CSV_H
#define SHG_CSV_H

#include <iostream>
#include <string>
#include <vector>

namespace SHG {

/**
 * Writes records in CSV format. Empty records, ie. for which
 * <tt>rec.size() == 0</tt>, are not written. Each record ends with
 * CRLF. See <a href = "rfc4180.txt">Common Format and MIME Type for
 * Comma-Separated Values (CSV) Files</a> (<a href =
 * "https://tools.ietf.org/html/rfc4180">source</a>, 11 X 2019).
 *
 * \ingroup miscellaneous_utilities
 */
class CSV_writer {
public:
     explicit CSV_writer(std::ostream& ostr = std::cout,
                         char field_separator = ',',
                         char quote_character = '\"');
     void write_record(std::vector<std::string> const& rec);

private:
     char const field_separator_;
     char const quote_character_;
     std::string const special_;
     std::ostream& ostr_;
     std::string rec_{};
     bool first_{};

     void add_field(std::string const& s);
};

/**
 * Reads records written in CSV format. Records may end with CR, LF or
 * CRLF. \sa CSV_writer.
 *
 * \ingroup miscellaneous_utilities
 */
class CSV_reader {
public:
     explicit CSV_reader(std::istream& istr = std::cin,
                         char field_separator = ',',
                         char quote_character = '\"');
     void getrec(std::vector<std::string>& rec);

private:
     char const field_separator_;
     char const quote_character_;
     std::istream& istr_;
     std::string fld_{};
     char c_{};
     bool inquote_{false};
     bool prevquote_{false};

     void handle_quote();
     void handle_separator(std::vector<std::string>& rec);
     bool is_end_of_record();
};

}  // namespace SHG

#endif
