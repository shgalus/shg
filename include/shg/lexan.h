/**
 * \file include/shg/lexan.h
 * Lexical analysis.
 */

#ifndef SHG_LEXAN_H
#define SHG_LEXAN_H

#include <istream>
#include <map>
#include <queue>
#include <shg/dict.h>

namespace SHG::PLP {

enum class Symbol { word, punctuation_mark, number };

/**
 * Struct returned by lexical analyzer.
 *
 * \note See \cite aho-sethi-ullman-2002, pages 67, 81.
 */
struct Token {
     Symbol symbol{};
     std::string lexeme{};
     std::vector<std::string> tags{};
     std::vector<Description> attribs{};
     bool empty() const;
};

bool operator==(Token const& lhs, Token const& rhs);
bool operator!=(Token const& lhs, Token const& rhs);

class Lexer {
public:
     Lexer();
     Lexer(Lexer const&) = delete;
     Lexer& operator=(Lexer const&) = delete;

     bool load_dict(std::istream& stream);
     bool load_dict(char const* fname);

     /**
      * Connects input text for tokenization. Text should be in UTF-8.
      */
     void reset(std::istream& input);
     std::istream const& stream() const;

     /**
      * Returns the next token from input text.
      *
      * Characters which are not letters, punctuation marks or digits
      * are omitted and treated as separators.
      *
      * \throws SHG::Encoding::Conversion_error if invalid UTF-8
      * character is found in a text line
      *
      * \throws SHG::PLP::Charest::Invalid_character_error if a valid
      * UTF-8 character is found in a text line which cannot be
      * translated to the charset
      *
      * \throws std::runtime_error if a text line contains null
      * characters
      *
      * In case of exception, the next call to get_token() starts from
      * the next line of text.
      */
     Token get_token();

     static std::vector<std::string> tab_of_tags();

private:
     std::map<unsigned char, char const*> punctuation_marks_{};
     std::vector<Dictionary> dicts_{};
     std::istream* input_{nullptr};
     std::queue<Token> q_{};

     std::string get_line();
     void tokenize();
     void check() const;
     void init_punctuation_marks();
     void search_dicts(std::string const& s, Setdesc& sd) const;
     void search_numerals(std::string const& s, Setdesc& sd) const;
     Setdesc collect_descriptions(std::string const& s) const;
};

std::ostream& operator<<(std::ostream& stream, Token const& d);

/**
 * Returns the next sentence from the stream. Sentences should be
 * separated by empty lines or lines containing only whitespace
 * characters.
 */
std::string get_sentence(std::istream& stream);

/**
 * Returns vector of tokens output by \c lexer for the string \c s.
 * The string \c s may be the output of get_sentence().
 */
std::vector<Token> tokenize_string(std::string const& s,
                                   Lexer& lexer);

inline bool Token::empty() const {
     return lexeme.size() == 0;
}

inline bool operator!=(Token const& lhs, Token const& rhs) {
     return !(lhs == rhs);
}

}  // namespace SHG::PLP

#endif
