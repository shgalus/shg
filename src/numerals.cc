/**
 * \file src/numerals.cc
 * Numerals and derived parts of speech.
 */

#include <shg/numerals.h>
#include <cstddef>
#include <algorithm>
#include <shg/dict_impl.h>
#include <shg/utils.h>

namespace SHG::PLP {

class Numeral_prefix {
public:
     static plstring::size_type find_num_prefix(std::string const& s);

private:
     static constexpr plchar const* const tab0[]{
          "jedno",   "dwu",       "trzy",
          "cztero",  "pi\315cio", "sze\341cio",
          "siedmio", "o\341mio",  "dziewi\315cio",
     };
     static constexpr plchar const* const tabk[]{
          "dziesi\315cio",    "jedenasto",   "dwunasto",
          "trzynasto",        "czternasto",  "pi\315tnasto",
          "szesnasto",        "siedemnasto", "osiemnasto",
          "dziewi\315tnasto",
     };
     static constexpr plchar const* const tab1[]{
          "dwudziesto",
          "trzydziesto",
          "czterdziesto",
          "pi\315\305dziesi\315cio",
          "sze\341\305dziesi\315cio",
          "siedemdziesi\315cio",
          "osiemdziesi\315cio",
          "dziewi\315\305dziesi\315cio",
     };
     static constexpr plchar const* const tab2[]{
          "stu",       "dwustu",        "trzystu",
          "czterystu", "pi\315\305set", "sze\341\305set",
          "siedemset", "osiemset",      "dziewi\315\305set",
     };

     static constexpr plchar const* const ponad = "ponad";
     static constexpr plchar const* const pol = "p\265\325";
     static constexpr plchar const* const tysiac = "tysi\303c";
     static constexpr plchar const* const ipol = "ip\265\325";
};

plstring::size_type Numeral_prefix::find_num_prefix(
     std::string const& s) {
     using std::find;
     using std::find_if;

     plstring::size_type len{0};
     std::string t{s};

     auto const ispref = [&t](char const* s) {
          return SHG::PLP::Charset::is_proper_prefix(t, s);
     };

     if (ispref(ponad)) {
          t = t.substr(length(ponad));
          len += length(ponad);
     }
     if (ispref(pol))
          return len + length(pol);
     if (ispref(tysiac)) {
          t = t.substr(length(tysiac));
          len += length(tysiac);
     }
     auto it = find_if(std::cbegin(tab2), std::cend(tab2), ispref);
     if (it != std::cend(tab2)) {
          t = t.substr(std::strlen(*it));
          len += std::strlen(*it);
     }
     it = find_if(std::cbegin(tab1), std::cend(tab1), ispref);
     if (it != std::cend(tab1)) {
          t = t.substr(std::strlen(*it));
          len += std::strlen(*it);
          it = find_if(std::cbegin(tab0), std::cend(tab0), ispref);
          if (it != std::cend(tab0)) {
               t = t.substr(std::strlen(*it));
               len += std::strlen(*it);
          }
          if (ispref(ipol)) {
               t = t.substr(length(ipol));
               len += length(ipol);
          }
          return len;
     }
     it = find_if(std::cbegin(tabk), std::cend(tabk), ispref);
     if (it != std::cend(tabk)) {
          t = t.substr(std::strlen(*it));
          len += std::strlen(*it);
     } else {
          it = find_if(std::cbegin(tab0), std::cend(tab0), ispref);
          if (it != std::cend(tab0)) {
               t = t.substr(std::strlen(*it));
               len += std::strlen(*it);
          }
     }
     if (s.substr(0, len) == ponad)
          return 0;
     if (len == 0)
          return 0;
     if (ispref(ipol)) {
          t = t.substr(length(ipol));
          len += length(ipol);
     }
     return len;
}

plstring::size_type find_numeral_prefix(plstring const& s) {
     return Numeral_prefix::find_num_prefix(s);
}

void check_numeral_adverbs(std::string const& s, Setdesc& sd) {
     if (!SHG::PLP::Charset::is_proper_suffix(s, "krotnie"))
          return;
     auto const len = find_numeral_prefix(s);
     if (len == 0)
          return;
     if (s.substr(len) != "krotnie")
          return;
     Description desc;
     desc.main_form = s;
     desc.category.part_of_speech = Part_of_speech::adverb;
     desc.category.degree = Degree::positive;
     sd.insert(desc);
}

}  // namespace SHG::PLP
