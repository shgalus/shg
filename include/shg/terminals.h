/**
 * \file include/shg/terminals.h
 * Converting text to terminal symbols.
 */

#ifndef SHG_TERMINALS_H
#define SHG_TERMINALS_H

#include <string>
#include <vector>
#include <istream>
#include <map>
#include <shg/dict.h>

namespace SHG::PLP {

/**
 * Returns vector of terminals.
 */
std::vector<std::string> tab_terminals();

/**
 * Reads a stream in a format similar to the cookie-jar format
 * \cite raymond-2004, page 124-125.
 */
class Cookie_jar {
public:
     Cookie_jar(std::istream& stream);
     bool get();
     std::string const& comment() const { return comment_; }
     std::string const& contents() const { return contents_; }

private:
     std::istream& stream_;
     std::string buf_{};
     std::string comment_{};
     std::string contents_{};
};

std::string get_sentence(std::istream& stream);

/**
 * Converts a sequence of characters into terminal symbols.
 */
class Terminalizer {
public:
     using Terminal = std::string;
     using Vecdesc = std::vector<Description>;
     using Setterm = std::vector<Terminal>;

     struct Position {
          std::string s{};
          Vecdesc d{};
          Setterm t{};
     };
     using Result = std::vector<Position>;

     Terminalizer();
     Terminalizer(Terminalizer const&) = delete;
     Terminalizer& operator=(Terminalizer const&) = delete;

     bool load_dict(std::istream& stream);
     bool load_dict(char const* fname);

     /**
      * Converts text from s to a sequence of terminals. Text should
      * be in UTF-8.
      *
      * Characters which are not letters, punctuation marks or digits
      * are omitted and treated as separators.
      *
      * \throws SHG::Encoding::Conversion_error if invalid UTF-8
      * sequence was found in \c s
      *
      * \throws SHG::PLP::Charset::Invalid_character_error if Unicode
      * character is not represented in SHG::PLP::Charset
      *
      * \throws std::invalid_argument if \c s contains null characters
      */
     Result convert(std::string const& s) const;

     /**
      * \todo Implement this.
      */
     static void display_punctuation_marks(std::ostream& stream);

private:
     std::vector<std::string> const tab_terminals_{tab_terminals()};
     std::vector<Dictionary> dicts_{};
     std::map<unsigned char, char const*> punctuation_marks_{};

     void init_punctuation_marks();
     Setdesc collect_descriptions(std::string const& s) const;
     void search_dicts(std::string const& s, Setdesc& sd) const;
     void search_numerals(std::string const& s, Setdesc& sd) const;
};

std::ostream& operator<<(std::ostream& stream,
                         Terminalizer::Position const& p);

}  // namespace SHG::PLP

#endif
