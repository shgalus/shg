/**
 * \file src/cattostr.cc
 * Implementation of to_string(Category const& c).
 */

#include <shg/dict.h>
#include <shg/except.h>

namespace SHG::PLP {

#define ERROR() \
     SHG::throw_exception<std::logic_error>(__FILE__, __LINE__)

namespace {

char const* name(Part_of_speech p) {
     switch (p) {
     case Part_of_speech::none:
          return "none";
     case Part_of_speech::noun:
          return "noun";
     case Part_of_speech::adjective:
          return "adjective";
     case Part_of_speech::pronoun:
          return "pronoun";
     case Part_of_speech::numeral:
          return "numeral";
     case Part_of_speech::verb:
          return "verb";
     case Part_of_speech::adjectival_passive_participle:
          return "adjectival_passive_participle";
     case Part_of_speech::adjectival_active_participle:
          return "adjectival_active_participle";
     case Part_of_speech::verbal_noun:
          return "verbal_noun";
     case Part_of_speech::adjectival_past_participle:
          return "adjectival_past_participle";
     case Part_of_speech::adverb:
          return "adverb";
     case Part_of_speech::preposition:
          return "preposition";
     case Part_of_speech::conjunction:
          return "conjunction";
     case Part_of_speech::particle:
          return "particle";
     case Part_of_speech::interjection:
          return "interjection";
     default:
          ERROR();
     };
}

char const* name(Inflexion i) {
     switch (i) {
     case Inflexion::uninflected:
          return "uninflected";
     case Inflexion::substantival:
          return "substantival";
     case Inflexion::adjectival:
          return "adjectival";
     case Inflexion::numeral:
          return "numeral";
     case Inflexion::numeral_1:
          return "numeral_1";
     case Inflexion::numeral_2:
          return "numeral_2";
     case Inflexion::by_case:
          return "by_case";
     case Inflexion::by_gender:
          return "by_gender";
     case Inflexion::conjugation:
          return "conjugation";
     default:
          ERROR();
     };
}

char const* name(Declension_case c) {
     switch (c) {
     case Declension_case::none:
          return "none";
     case Declension_case::nominative:
          return "nominative";
     case Declension_case::genitive:
          return "genitive";
     case Declension_case::dative:
          return "dative";
     case Declension_case::accusative:
          return "accusative";
     case Declension_case::instrumental:
          return "instrumental";
     case Declension_case::locative:
          return "locative";
     case Declension_case::vocative:
          return "vocative";
     default:
          ERROR();
     };
}

char const* name(Number n) {
     switch (n) {
     case Number::none:
          return "none";
     case Number::singular:
          return "singular";
     case Number::plural:
          return "plural";
     default:
          ERROR();
     };
}

char const* name(Gender g) {
     switch (g) {
     case Gender::none:
          return "none";
     case Gender::masculine_personal:
          return "masculine_personal";
     case Gender::masculine_animal:
          return "masculine_animal";
     case Gender::masculine_inanimate:
          return "masculine_inanimate";
     case Gender::masculine:
          return "masculine";
     case Gender::feminine:
          return "feminine";
     case Gender::neuter:
          return "neuter";
     case Gender::non_masculine_personal:
          return "non_masculine_personal";
     case Gender::masculine_animate:
          return "masculine_animate";
     case Gender::masculine_impersonal:
          return "masculine_impersonal";
     case Gender::non_feminine:
          return "non_feminine";
     default:
          ERROR();
     };
}

char const* name(Degree d) {
     switch (d) {
     case Degree::none:
          return "none";
     case Degree::positive:
          return "positive";
     case Degree::comparative:
          return "comparative";
     case Degree::superlative:
          return "superlative";
     default:
          ERROR();
     };
}

char const* name(Aspect a) {
     switch (a) {
     case Aspect::none:
          return "none";
     case Aspect::imperfect:
          return "imperfect";
     case Aspect::perfect:
          return "perfect";
     default:
          ERROR();
     };
}

char const* name(Mood m) {
     switch (m) {
     case Mood::none:
          return "none";
     case Mood::indicative:
          return "indicative";
     case Mood::imperative:
          return "imperative";
     case Mood::subjunctive:
          return "subjunctive";
     default:
          ERROR();
     };
}

char const* name(Tense t) {
     switch (t) {
     case Tense::none:
          return "none";
     case Tense::past:
          return "past";
     case Tense::present:
          return "present";
     case Tense::future:
          return "future";
     default:
          ERROR();
     };
}

char const* name(Person p) {
     switch (p) {
     case Person::none:
          return "none";
     case Person::first:
          return "first";
     case Person::second:
          return "second";
     case Person::third:
          return "third";
     default:
          ERROR();
     };
}

char const* name(Form_of_verb f) {
     switch (f) {
     case Form_of_verb::none:
          return "none";
     case Form_of_verb::infinitive:
          return "infinitive";
     case Form_of_verb::impersonal_form_of_past_tense:
          return "impersonal_form_of_past_tense";
     case Form_of_verb::simultaneous_adverbial_participle:
          return "simultaneous_adverbial_participle";
     case Form_of_verb::anticipatory_adverbial_participle:
          return "anticipatory_adverbial_participle";
     default:
          ERROR();
     };
}

char const* name(Type_of_pronoun t) {
     switch (t) {
     case Type_of_pronoun::none:
          return "none";
     case Type_of_pronoun::personal:
          return "personal";
     case Type_of_pronoun::reflexive:
          return "reflexive";
     case Type_of_pronoun::indefinite:
          return "indefinite";
     case Type_of_pronoun::negative:
          return "negative";
     case Type_of_pronoun::demonstrative:
          return "demonstrative";
     case Type_of_pronoun::possessive:
          return "possessive";
     case Type_of_pronoun::interrogative_relative:
          return "interrogative_relative";
     default:
          ERROR();
     };
}

char const* name(Type_of_numeral t) {
     switch (t) {
     case Type_of_numeral::none:
          return "none";
     case Type_of_numeral::cardinal:
          return "cardinal";
     case Type_of_numeral::fractional:
          return "fractional";
     case Type_of_numeral::multiplicative:
          return "multiplicative";
     case Type_of_numeral::multiple:
          return "multiple";
     case Type_of_numeral::collective:
          return "collective";
     case Type_of_numeral::ordinal:
          return "ordinal";
     default:
          ERROR();
     };
}

}  // anonymous namespace

/// \todo Is c.inflexion for pronouns and numerals really necessary?
std::string to_string(Category const& c) {
     std::string const d = ":";
     std::string s = name(c.part_of_speech);

     switch (c.part_of_speech) {
     case Part_of_speech::none:
          break;
     case Part_of_speech::noun:
          if (c.gender != Gender::none)
               s += d + name(c.gender);
          s += d + name(c.declension_case) + d + name(c.number);
          break;
     case Part_of_speech::adjective:
          s += d + name(c.degree) + d + name(c.gender) + d +
               name(c.declension_case) + d + name(c.number);
          break;
     case Part_of_speech::pronoun:
          s += d + name(c.type_of_pronoun) + d + name(c.inflexion);
          switch (c.inflexion) {
          case Inflexion::uninflected:
               break;
          case Inflexion::substantival:
               s += d + name(c.declension_case) + d + name(c.number);
               break;
          case Inflexion::adjectival:
               s += d + name(c.gender) + d + name(c.declension_case) +
                    d + name(c.number);
               break;
          case Inflexion::numeral:
               s += d + name(c.gender) + d + name(c.declension_case);
               break;
          case Inflexion::by_case:
               s += d + name(c.declension_case);
               break;
          default:
               ERROR();
          }
          break;
     case Part_of_speech::numeral:
          s += d + name(c.type_of_numeral) + d + name(c.inflexion);
          switch (c.inflexion) {
          case Inflexion::uninflected:
               break;
          case Inflexion::substantival:
               s += d + name(c.declension_case) + d + name(c.number);
               break;
          case Inflexion::adjectival:
               s += d + name(c.gender) + d + name(c.declension_case) +
                    d + name(c.number);
               break;
          case Inflexion::numeral:
          case Inflexion::numeral_1:
          case Inflexion::numeral_2:
               s += d + name(c.gender) + d + name(c.declension_case);
               break;
          case Inflexion::by_case:
               s += d + name(c.declension_case);
               break;
          case Inflexion::by_gender:
               s += d + name(c.gender);
               break;
          default:
               ERROR();
          }
          break;
     case Part_of_speech::verb:
          s += d + name(c.aspect);
          if (c.form_of_verb != Form_of_verb::none) {
               s += d + name(c.form_of_verb);
               break;
          }
          s += d + name(c.mood);
          if (c.tense != Tense::none)
               s += d + name(c.tense);
          if (c.gender != Gender::none)
               s += d + name(c.gender);
          s += d + name(c.number);
          s += d + name(c.person);
          break;
     case Part_of_speech::adjectival_passive_participle:
     case Part_of_speech::adjectival_active_participle:
     case Part_of_speech::adjectival_past_participle:
          s += d + name(c.gender) + d + name(c.declension_case) + d +
               name(c.number);
          break;
     case Part_of_speech::verbal_noun:
          s += d + name(c.declension_case) + d + name(c.number);
          break;
     case Part_of_speech::adverb:
          s += d + name(c.degree);
          break;
     case Part_of_speech::preposition:
     case Part_of_speech::conjunction:
     case Part_of_speech::particle:
     case Part_of_speech::interjection:
          break;
     default:
          ERROR();
     }
     return s;
}

}  // namespace SHG::PLP
