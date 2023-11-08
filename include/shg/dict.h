/**
 * \file include/shg/dict.h
 * Polish dictionary interface.
 */

#ifndef SHG_DICT_H
#define SHG_DICT_H

#include <memory>
#include <string>
#include <stdexcept>
#include <istream>
#include <ostream>
#include <set>
#include <vector>
#include <shg/charset.h>

/**
 * Polish language processing.
 */
namespace SHG::PLP {

/**
 * \defgroup polish_language_processing Polish language processing
 *
 * Polish language processing.
 *
 * \{
 */

class Dictionary_error : public std::runtime_error {
public:
     Dictionary_error();
     explicit Dictionary_error(char const* message);
     Dictionary_error(char const* message, unsigned long line);
};

// clang-format off

enum class Part_of_speech {
     none,
     noun,
     adjective,
     pronoun,
     numeral,
     verb,
     adjectival_passive_participle,
     adjectival_active_participle,
     verbal_noun,
     adjectival_past_participle,
     adverb,
     preposition,
     conjunction,
     particle,
     interjection
};

enum class Inflexion {
     uninflected,
     substantival,
     adjectival,
     numeral,
     numeral_1,
     numeral_2,
     by_case,
     by_gender,
     conjugation
};

enum class Declension_case {
     none,
     nominative,    // mianownik
     genitive,      // dopelniacz
     dative,        // celownik
     accusative,    // biernik
     instrumental,  // narzednik
     locative,      // miejscownik
     vocative       // wolacz
};

enum class Number {
     none,
     singular,
     plural
};

enum class Gender {
     none,
     masculine_personal,
     masculine_animal,
     masculine_inanimate,
     masculine,
     feminine,
     neuter,
     non_masculine_personal,
     masculine_animate,
     masculine_impersonal,
     non_feminine
};

enum class Degree {
     none,
     positive,
     comparative,
     superlative
};

enum class Aspect {
     none,
     imperfect,
     perfect
};

enum class Mood {
     none,
     indicative,
     imperative,
     subjunctive
};

enum class Tense {
     none,
     past,
     present,
     future
};

enum class Person {
     none,
     first,
     second,
     third
};

enum class Form_of_verb {
     none,
     infinitive,
     impersonal_form_of_past_tense,
     simultaneous_adverbial_participle,
     anticipatory_adverbial_participle
};

enum class Type_of_pronoun {
     none,
     personal,
     reflexive,
     indefinite,
     negative,
     demonstrative,
     possessive,
     interrogative_relative
};

enum class Type_of_numeral {
     none,
     cardinal,
     fractional,
     multiplicative,
     multiple,
     collective,
     ordinal
};

// clang-format on

struct Category {
     Part_of_speech part_of_speech{Part_of_speech::none};
     Inflexion inflexion{Inflexion::uninflected};
     Declension_case declension_case{Declension_case::none};
     Number number{Number::none};
     Gender gender{Gender::none};
     Degree degree{Degree::none};
     Aspect aspect{Aspect::none};
     Mood mood{Mood::none};
     Tense tense{Tense::none};
     Person person{Person::none};
     Form_of_verb form_of_verb{Form_of_verb::none};
     Type_of_pronoun type_of_pronoun{Type_of_pronoun::none};
     Type_of_numeral type_of_numeral{Type_of_numeral::none};
};

int catcmp(Category const& c1, Category const& c2);
bool operator==(Category const& c1, Category const& c2);
std::vector<Category> generate_all_categories();
std::string to_string(Category const& c);

struct Description {
     std::string main_form{};
     Category category{};
};

struct Description_cmp {
     bool operator()(Description const& lhs,
                     Description const& rhs) const {
          int const r = Charset::alpha_strcmp(lhs.main_form.c_str(),
                                              rhs.main_form.c_str());
          if (r != 0)
               return r < 0;
          return catcmp(lhs.category, rhs.category) < 0;
     }
};

using Setdesc = std::set<Description, Description_cmp>;

bool operator==(Description const& d1, Description const& d2);

class Dictionary {
public:
     Dictionary();
     ~Dictionary();
     Dictionary(Dictionary const& other) = delete;
     Dictionary(Dictionary&& other);
     Dictionary& operator=(Dictionary const& other) = delete;
     Dictionary& operator=(Dictionary&& other);

     void load_source_word_file(std::istream& input);
     /** Writes source word file. */
     void write_source_word_file(std::ostream& output,
                                 bool do_sort) const;
     void add_source_word_file(std::istream& input);

     void load_word_file(std::istream& input);
     void write_word_file(std::ostream& output) const;

     bool has_entry_utf8(char const* s) const;

     void search_utf8(char const* s, Setdesc& sd) const;

     bool has_entry_charset(char const* s) const;

     void search_charset(char const* s, Setdesc& sd) const;

     struct Table_row {
          char const* entry_type{};
          unsigned long nentries{};
     };
     std::vector<Table_row> report() const;

private:
     class Impl;
     std::unique_ptr<Impl> pimpl_;
};

inline bool operator==(Category const& c1, Category const& c2) {
     return catcmp(c1, c2) == 0;
}

/** \} */ /* end of group polish_language_processing */

}  // namespace SHG::PLP

#endif
