/**
 * \file src/lexan.cc
 * Lexical analysis.
 */

#include <shg/lexan.h>
#include <fstream>
#include <stdexcept>
#include <ios>
#include <iomanip>
#include <shg/utils.h>
#include <shg/charset.h>
#include <shg/numerals.h>

namespace SHG::PLP {

namespace {

/**
 * Table of writing marks other than letters and digits. For
 * punctuation marks, see \cite polanski-1996, page XCV - CXXIV.
 */
constexpr struct {
     unsigned char const code;
     char const* const terminal_name;
} const punctuation_marks[]{
     {0x21, "exclamation_mark"},
     {0x22, "typewriter_quotation_mark"},
     {0x23, "number_sign"},
     {0x24, "dollar_sign"},
     {0x25, "percent_sign"},
     {0x26, "ampersand"},
     {0x27, "apostrophe"},
     {0x28, "left_round_bracket"},
     {0x29, "right_round_bracket"},
     {0x2a, "asterisk"},
     {0x2b, "plus_sign"},
     {0x2c, "comma"},
     {0x2d, "hyphen"},  // lacznik, minus
     {0x2e, "full_stop"},
     {0x2f, "simple_bracket"},
     {0x3a, "colon"},
     {0x3b, "semicolon"},
     {0x3c, "left_angle_bracket"},
     {0x3d, "equals_sign"},
     {0x3e, "right_angle_bracket"},
     {0x3f, "question_mark"},
     {0x40, "commercial_at"},
     {0x5b, "left_square_bracket"},
     {0x5c, "backslash"},
     {0x5d, "right_square_bracket"},
     {0x5e, "up_arrow"},
     {0x5f, "underline"},
     {0x60, "grave_accent"},
     {0x7b, "left_curly_bracket"},
     {0x7c, "vertical_line"},
     {0x7d, "right_curly_bracket"},
     {0x7e, "tilde"},
     {0x80, "no_break_space"},
     {0x81, "paragraph_sign"},
     {0x82, "copyright_sign"},
     {0x83, "left_angle_quotation_mark"},  // lewy cudz. francuski
     {0x84, "registered_sign"},
     {0x85, "degree_sign"},
     {0x86, "plus_minus_sign"},
     {0x87, "right_angle_quotation_mark"},  // prawy cudz. francuski
     {0xf5, "hyphen"},                      // dywiz, lacznik
     {0xf6, "dash"},                        //  myslnik, polpauza
     {0xf7, "dash"},                        //  myslnik, pauza
     {0xf8, "left_definition_quotation_mark"},
     {0xf9, "right_definition_quotation_mark"},
     {0xfa, "left_definition_quotation_mark"},
     {0xfb, "right_quotation_mark"},  //  prawy cudzyslow
     {0xfc, "left_quotation_mark"},   // lewy cudzyslow
     {0xfd, "dots"},                  // wielokropek
     {0xfe, "minus_sign"},
     {0xff, "replacement_character"},
};

constexpr auto const number = "number";
constexpr char const* const unknown_word = "unknown_word";

}  // anonymous namespace

bool operator==(Token const& lhs, Token const& rhs) {
     return lhs.lexeme == rhs.lexeme && lhs.tags == rhs.tags &&
            lhs.attribs == rhs.attribs;
}

Lexer::Lexer() {
     init_punctuation_marks();
}

bool Lexer::load_dict(std::istream& stream) {
     Dictionary dict;
     try {
          dict.load_word_file(stream);
     } catch (Dictionary_error const&) {
          return false;
     }
     if (stream.bad())
          return false;
     dicts_.push_back(std::move(dict));
     return true;
}

bool Lexer::load_dict(char const* fname) {
     using std::ios_base;
     std::ifstream f(fname, ios_base::in | ios_base::binary);
     if (!f)
          return false;
     return load_dict(f);
}

void Lexer::reset(std::istream& input) {
     input_ = &input;
     std::queue<Token> empty;
     std::swap(q_, empty);
}

std::istream const& Lexer::stream() const {
     check();
     return *input_;
}

Token Lexer::get_token() {
     check();
     if (q_.empty())
          tokenize();
     Token tok;
     if (!q_.empty()) {
          tok = q_.front();
          q_.pop();
     }
     return tok;
}

std::vector<std::string> Lexer::tab_of_tags() {
     auto const v = generate_all_categories();
     std::vector<std::string> t;
     for (auto const& c : v)
          t.push_back(to_string(c));
     for (auto const& pm : punctuation_marks)
          t.push_back(pm.terminal_name);
     t.push_back(number);
     t.push_back(unknown_word);
     remove_duplicates(t);
     return t;
}

std::string Lexer::get_line() {
     std::string s;
     for (;;)
          if (!(std::getline(*input_, s) && trim(s).empty()))
               return s;
}

void Lexer::tokenize() {
     std::string const s = get_line();
     if (s.empty())
          return;
     std::string const t = SHG::PLP::Charset::utf8_to_charset(s);
     char const* p = t.c_str();
     if (std::strlen(p) != t.size())
          throw std::runtime_error("null characters in text");
     char const* p1;
     for (;;) {
          Token tok;
          while (SHG::PLP::Charset::isspace(*p))
               p++;
          if (SHG::PLP::Charset::isalpha(*p)) {
               p1 = p;
               while (SHG::PLP::Charset::isalpha(*++p1))
                    ;
               tok.symbol = Symbol::word;
               tok.lexeme = t.substr(p - t.c_str(), p1 - p);
               auto sd = collect_descriptions(tok.lexeme);
               tok.attribs.reserve(sd.size());
               for (auto it = sd.begin(); it != sd.end();)
                    tok.attribs.push_back(
                         std::move(sd.extract(it++).value()));
               if (tok.attribs.size() == 0)
                    tok.tags.push_back(unknown_word);
               else
                    for (auto const& x : tok.attribs)
                         tok.tags.push_back(to_string(x.category));
               p = p1;
          } else if (SHG::PLP::Charset::ispunct(*p)) {
               tok.symbol = Symbol::punctuation_mark;
               tok.lexeme = t.substr(p - t.c_str(), 1);
               auto const it = punctuation_marks_.find(*p);
               assert(it != punctuation_marks_.cend());
               tok.tags.push_back(it->second);
               p++;
          } else if (SHG::PLP::Charset::isdigit(*p)) {
               p1 = p;
               while (SHG::PLP::Charset::isdigit(*++p1))
                    ;
               tok.symbol = Symbol::number;
               tok.lexeme = t.substr(p - t.c_str(), p1 - p);
               tok.tags.push_back(number);
               p = p1;
          } else if (*p == '\0') {
               break;
          } else {
               // omit characters which are not spaces, letters,
               // punctuation marks or digits
               p++;
          }
          q_.push(tok);
     }
}

void Lexer::check() const {
     if (input_ == nullptr)
          throw std::logic_error("input stream not set");
}

void Lexer::init_punctuation_marks() {
     for (auto const& pm : punctuation_marks) {
          auto const [it, status] =
               punctuation_marks_.insert({pm.code, pm.terminal_name});
          if (!status)
               throw std::logic_error("duplicated terminal");
     }
}

void Lexer::search_dicts(std::string const& s, Setdesc& sd) const {
     for (auto& d : dicts_)
          d.search_charset(s.c_str(), sd);
}

void Lexer::search_numerals(std::string const& s, Setdesc& sd) const {
     auto const len = find_numeral_prefix(s);
     if (len == 0)
          return;
     auto const t = s.substr(len);
     Setdesc sd1;
     search_dicts(t, sd1);
     for (auto const& d : sd1) {
          if (d.category.part_of_speech ==
                   Part_of_speech::adjective &&
              d.category.degree == Degree::positive) {
               Description d1{d};
               d1.main_form = s.substr(0, len) + d1.main_form;
               sd.insert(d1);
          } else if (d.category.part_of_speech ==
                          Part_of_speech::adverb &&
                     d.category.degree == Degree::positive) {
               Description d1{d};
               d1.main_form = s.substr(0, len) + d1.main_form;
               sd.insert(d1);
          }
     }
     if (t == "krotnie") {
          Description desc;
          desc.main_form = s;
          desc.category.part_of_speech = Part_of_speech::adverb;
          desc.category.degree = Degree::positive;
          sd.insert(desc);
     }
}

Setdesc Lexer::collect_descriptions(std::string const& s) const {
     Setdesc sd;
     std::string t{s};
     search_dicts(t, sd);
     SHG::PLP::Charset::capitalize(t);
     search_dicts(t, sd);
     SHG::PLP::Charset::uppercase(t);
     search_dicts(t, sd);
     SHG::PLP::Charset::lowercase(t);
     search_dicts(t, sd);

     if (SHG::PLP::Charset::is_proper_prefix(t, "naj")) {
          t = t.substr(3);
          Setdesc sd1;
          search_dicts(t, sd1);
          for (auto const& d : sd1)
               if (d.category.part_of_speech ==
                        Part_of_speech::adjective &&
                   d.category.degree == Degree::comparative) {
                    Description d1{d};
                    d1.category.degree = Degree::superlative;
                    sd.insert(d1);
               } else if (d.category.part_of_speech ==
                               Part_of_speech::adverb &&
                          d.category.degree == Degree::comparative) {
                    Description d1{d};
                    d1.category.degree = Degree::superlative;
                    sd.insert(d1);
               }
     } else if (SHG::PLP::Charset::is_proper_prefix(t, "nie")) {
          t = t.substr(3);
          Setdesc sd1;
          search_dicts(t, sd1);
          for (auto const& d : sd1)
               if (d.category.part_of_speech ==
                        Part_of_speech::adjective &&
                   d.category.degree == Degree::positive) {
                    sd.insert(d);
               } else if (d.category.part_of_speech ==
                               Part_of_speech::adverb &&
                          d.category.degree == Degree::positive) {
                    sd.insert(d);
               } else if (d.category.part_of_speech ==
                               Part_of_speech::noun ||
                          d.category.part_of_speech ==
                               Part_of_speech::verbal_noun) {
                    sd.insert(d);
               } else if (d.category.part_of_speech ==
                          Part_of_speech::
                               adjectival_active_participle) {
                    sd.insert(d);
               } else if (d.category.part_of_speech ==
                          Part_of_speech::
                               adjectival_passive_participle) {
                    sd.insert(d);
               }
     } else if (SHG::PLP::Charset::is_proper_prefix(t, "anty")) {
          t = t.substr(4);
          Setdesc sd1;
          search_dicts(t, sd1);
          for (auto const& d : sd1)
               if (d.category.part_of_speech ==
                        Part_of_speech::noun ||
                   d.category.part_of_speech ==
                        Part_of_speech::verb ||  // antydatowac
                   d.category.part_of_speech ==
                        Part_of_speech::
                             adjectival_passive_participle ||
                   d.category.part_of_speech ==
                        Part_of_speech::
                             adjectival_active_participle ||
                   d.category.part_of_speech ==
                        Part_of_speech::verbal_noun)
                    sd.insert(d);
               else if (d.category.part_of_speech ==
                             Part_of_speech::adjective &&
                        d.category.degree == Degree::positive) {
                    sd.insert(d);
               } else if (d.category.part_of_speech ==
                               Part_of_speech::adverb &&
                          d.category.degree == Degree::positive) {
                    sd.insert(d);
               }
     } else if (SHG::PLP::Charset::is_proper_prefix(t, "mini")) {
          // Nagorko, p. 178.
          t = t.substr(4);
          Setdesc sd1;
          search_dicts(t, sd1);
          for (auto const& d : sd1)
               if (d.category.part_of_speech == Part_of_speech::noun)
                    sd.insert(d);
     } else if (SHG::PLP::Charset::is_proper_prefix(t, "super")) {
          // Nagorko, p. 178.
          t = t.substr(5);
          Setdesc sd1;
          search_dicts(t, sd1);
          for (auto const& d : sd1)
               if (d.category.part_of_speech == Part_of_speech::noun)
                    sd.insert(d);
     }
     if (SHG::PLP::Charset::is_proper_suffix(t, "\362e")) {
          t = t.substr(0, t.size() - 2);
          Setdesc sd1;
          search_dicts(t, sd1);
          for (auto const& d : sd1)
               if (d.category.part_of_speech == Part_of_speech::verb)
                    if (d.category.mood == Mood::imperative)
                         if (d.category.person == Person::second)
                              if (d.category.number ==
                                  Number::singular)
                                   sd.insert(d);
     }
     if (SHG::PLP::Charset::is_proper_suffix(t, "\362")) {
          t = t.substr(0, t.size() - 1);
          Setdesc sd1;
          search_dicts(t, sd1);
          for (auto const& d : sd1)
               if (d.category.part_of_speech == Part_of_speech::verb)
                    if (d.category.mood == Mood::imperative)
                         if (d.category.number == Number::plural)
                              if (d.category.person ==
                                       Person::first ||
                                  d.category.person == Person::second)
                                   sd.insert(d);
     }
     search_numerals(t, sd);
     return sd;
}

std::ostream& operator<<(std::ostream& stream, Token const& tok) {
     int const width = 18;
     std::string const s =
          SHG::PLP::Charset::charset_to_utf8(tok.lexeme);
     auto const alignment = s.size() - tok.lexeme.size();

     stream << std::setw(width + alignment) << std::left << s
            << tok.tags[0] << '\n';
     for (std::size_t i = 1; i < tok.tags.size(); i++)
          stream << std::setw(width) << "" << tok.tags[i] << '\n';
     return stream;
}

std::string get_sentence(std::istream& stream) {
     std::string s;
     std::string line;
     do {
          if (!std::getline(stream, line))
               return s;
          trim(line);
     } while (line.empty());
     s = line;
     for (;;) {
          if (!std::getline(stream, line))
               return s;
          trim(line);
          if (line.empty())
               return s;
          s += " " + line;
     }
}

std::vector<Token> tokenize_string(std::string const& s,
                                   Lexer& lexer) {
     std::istringstream iss(std::ios_base::in |
                            std::ios_base::binary);
     iss.str(s);
     std::vector<Token> v;
     lexer.reset(iss);
     for (auto tok = lexer.get_token(); !tok.empty();
          tok = lexer.get_token())
          v.push_back(tok);
     return v;
}

}  // namespace SHG::PLP
