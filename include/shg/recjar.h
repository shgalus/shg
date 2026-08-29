/**
 * \file include/shg/recjar.h
 * Record-jar format.
 */

#ifndef SHG_RECJAR_H
#define SHG_RECJAR_H

#include <cstring>
#include <filesystem>
#include <istream>
#include <ostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * Parsing record-jar files.
 * See \cite raymond-2004, p. 125-126. See also:
 * - <a href =
 *   "http://www.catb.org/~esr/writings/taoup/html/ch05s02.html#id2906931">Data
 *   File Metaformats</a>,
 * - <a href =
 *   "https://datatracker.ietf.org/doc/draft-phillips-record-jar/">The
 *   record-jar Format</a> (<a href =
 *   "draft-phillips-record-jar-02.txt">local copy</a>).
 */
namespace SHG::RECJAR {

/** %Exception class for read(). */
class Error : public std::runtime_error {
public:
     Error(char const* message = nullptr, char const* file = nullptr,
           std::size_t line = 0);

private:
     static std::string format_message(char const* message,
                                       char const* file,
                                       std::size_t line);
};

using Field_name = std::string;
using Line = std::string;
struct Field {
     Field_name name{};
     std::vector<Line> lines{};
};
using Record = std::vector<Field>;
using Records = std::vector<Record>;
using Headers = std::set<Field_name>;

bool operator==(Field const& lhs, Field const& rhs);

Records read(std::filesystem::path const& path);
Records read(char const* str);
Records read(std::istream& stream);
void print(Records const& records, std::ostream& stream);
Headers headers(Records const& records);

void build_html(Records const& records, std::string const& lang,
                std::string const& title,
                std::filesystem::path const& path);

}  // namespace SHG::RECJAR

#endif
