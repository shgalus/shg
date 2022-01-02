/**
 * \file src/complcat.cc
 * Implementation of complete_category().
 */

#include <shg/dict.h>

namespace SHG::PLP {

namespace {

void complete_category_substantival(Category& c,
                                    unsigned short form) {
     c.declension_case = static_cast<Declension_case>(form % 7 + 1);
     c.number = form < 7 ? Number::singular : Number::plural;
}

void complete_category_adjectival(Category& c, unsigned short form) {
     c.declension_case = static_cast<Declension_case>(form % 7 + 1);
     c.number = form < 28 ? Number::singular : Number::plural;
     switch (form / 7) {
     case 0:
          c.gender = Gender::masculine_animate;
          break;
     case 1:
          c.gender = Gender::masculine_inanimate;
          break;
     case 2:
          c.gender = Gender::feminine;
          break;
     case 3:
          c.gender = Gender::neuter;
          break;
     case 4:
          c.gender = Gender::masculine_personal;
          break;
     case 5:
          c.gender = Gender::non_masculine_personal;
          break;
     default:
          throw Dictionary_error();
          break;
     }
}

void complete_category_numeral(Category& c, unsigned short form) {
     c.declension_case = static_cast<Declension_case>(form % 7 + 1);
     if (form < 7)
          c.gender = Gender::non_masculine_personal;
     else
          c.gender = Gender::masculine_personal;
}

void complete_category_numeral_1(Category& c, unsigned short form) {
     c.declension_case = static_cast<Declension_case>(form % 7 + 1);
     if (form < 7)
          c.gender = Gender::masculine_animate;
     else if (form < 14)
          c.gender = Gender::masculine_inanimate;
     else if (form < 21)
          c.gender = Gender::feminine;
     else
          c.gender = Gender::neuter;
}

void complete_category_numeral_2(Category& c, unsigned short form) {
     c.declension_case = static_cast<Declension_case>(form % 7 + 1);
     if (form < 7)
          c.gender = Gender::masculine_impersonal;
     else if (form < 14)
          c.gender = Gender::masculine_personal;
     else if (form < 21)
          c.gender = Gender::feminine;
     else
          c.gender = Gender::neuter;
}

void complete_category_by_case(Category& c, unsigned short form) {
     c.declension_case = static_cast<Declension_case>(form % 7 + 1);
}

void complete_category_by_gender(Category& c, unsigned short form) {
     if (form == 0)
          c.gender = Gender::non_feminine;
     else if (form == 1)
          c.gender = Gender::feminine;
}

void complete_category_conjugation(Category& c, unsigned short form) {
     if (form == 0) {
          c.form_of_verb = Form_of_verb::infinitive;
          return;
     }
     if (form >= 1 && form <= 6) {
          c.mood = Mood::indicative;
          if (c.aspect == Aspect::imperfect)
               c.tense = Tense::present;
          else if (c.aspect == Aspect::perfect)
               c.tense = Tense::future;
          c.number = form < 4 ? Number::singular : Number::plural;
          c.person = static_cast<Person>((form - 1) % 3 + 1);
     }
     if (form >= 7 && form <= 21) {
          c.mood = Mood::indicative;
          c.tense = Tense::past;
          c.number = form < 16 ? Number::singular : Number::plural;
          if (form < 10)
               c.gender = Gender::masculine;
          else if (form < 13)
               c.gender = Gender::feminine;
          else if (form < 16)
               c.gender = Gender::neuter;
          else if (form < 19)
               c.gender = Gender::masculine_personal;
          else
               c.gender = Gender::non_masculine_personal;
          c.person = static_cast<Person>((form - 1) % 3 + 1);
     }
     if (form == 22) {
          c.mood = Mood::imperative;
          c.number = Number::singular;
          c.person = Person::second;
     }
     if (form == 23) {
          c.mood = Mood::imperative;
          c.number = Number::plural;
          c.person = Person::first;
     }
     if (form == 24) {
          c.mood = Mood::imperative;
          c.number = Number::plural;
          c.person = Person::second;
     }
     if (form >= 25 && form <= 39) {
          c.mood = Mood::subjunctive;
          c.number = form < 34 ? Number::singular : Number::plural;
          if (form < 28)
               c.gender = Gender::masculine;
          else if (form < 31)
               c.gender = Gender::feminine;
          else if (form < 34)
               c.gender = Gender::neuter;
          else if (form < 37)
               c.gender = Gender::masculine_personal;
          else
               c.gender = Gender::non_masculine_personal;
          c.person = static_cast<Person>((form - 1) % 3 + 1);
     }
     if (form == 40) {
          c.form_of_verb =
               Form_of_verb::impersonal_form_of_past_tense;
     }
     if (form == 41) {
          if (c.aspect == Aspect::imperfect)
               c.form_of_verb =
                    Form_of_verb::simultaneous_adverbial_participle;
          else if (c.aspect == Aspect::perfect)
               c.form_of_verb =
                    Form_of_verb::anticipatory_adverbial_participle;
     }
}

}  // anonymous namespace

void complete_category(Category& c, Inflexion infl,
                       unsigned short form) {
     switch (infl) {
     case Inflexion::substantival:
          complete_category_substantival(c, form);
          break;
     case Inflexion::adjectival:
          complete_category_adjectival(c, form);
          break;
     case Inflexion::numeral:
          complete_category_numeral(c, form);
          break;
     case Inflexion::numeral_1:
          complete_category_numeral_1(c, form);
          break;
     case Inflexion::numeral_2:
          complete_category_numeral_2(c, form);
          break;
     case Inflexion::by_case:
          complete_category_by_case(c, form);
          break;
     case Inflexion::by_gender:
          complete_category_by_gender(c, form);
          break;
     case Inflexion::conjugation:
          complete_category_conjugation(c, form);
          break;
     case Inflexion::uninflected:
          break;
     default:
          throw Dictionary_error();
     }
}

}  // namespace SHG::PLP
