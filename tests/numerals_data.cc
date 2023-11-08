#include "numerals_data.h"

namespace TESTS {

class Numeral_prefixes {
public:
     using Vecstring = std::vector<std::string>;
     static Vecstring generate();

private:
     static constexpr char const* const tab0[]{
          "jedno",   "dwu",           "trzy",
          "cztero",  "pi\304\231cio", "sze\305\233cio",
          "siedmio", "o\305\233mio",  "dziewi\304\231cio",
     };

     static constexpr char const* const tab1[]{
          "dziesi\304\231cio", "jedenasto",
          "dwunasto",          "trzynasto",
          "czternasto",        "pi\304\231tnasto",
          "szesnasto",         "siedemnasto",
          "osiemnasto",        "dziewi\304\231tnasto",
     };

     static constexpr char const* const tab2[]{
          "dwudziesto",
          "trzydziesto",
          "czterdziesto",
          "pi\304\231\304\207dziesi\304\231cio",
          "sze\305\233\304\207dziesi\304\231cio",
          "siedemdziesi\304\231cio",
          "osiemdziesi\304\231cio",
          "dziewi\304\231\304\207dziesi\304\231cio",
     };

     static constexpr char const* const tab3[]{
          "stu",
          "dwustu",
          "trzystu",
          "czterystu",
          "pi\304\231\304\207set",
          "sze\305\233\304\207set",
          "siedemset",
          "osiemset",
          "dziewi\304\231\304\207set",
     };

     static constexpr char const* const tab4[]{
          "",
          "tysi\304\205c",
     };

     static void do_tab0(std::string const& p, Vecstring& v);
     static void do_tab1(std::string const& p, Vecstring& v);
     static void do_tab2(std::string const& p, Vecstring& v);
     static void do_tab3(std::string const& p, Vecstring& v);
     static void do_tab4(std::string const& p, Vecstring& v);
};

Numeral_prefixes::Vecstring Numeral_prefixes::generate() {
     Vecstring v;
     for (auto const& s : tab4)
          do_tab4(s, v);
     return v;
}

void Numeral_prefixes::do_tab0(std::string const& p, Vecstring& v) {
     for (auto const& s : tab0)
          v.push_back(p + s);
}

void Numeral_prefixes::do_tab1(std::string const& p, Vecstring& v) {
     for (auto const& s : tab1)
          v.push_back(p + s);
}

void Numeral_prefixes::do_tab2(std::string const& p, Vecstring& v) {
     for (auto const& s : tab2) {
          v.push_back(p + s);
          do_tab0(p + s, v);
     }
}

void Numeral_prefixes::do_tab3(std::string const& p, Vecstring& v) {
     for (auto const& s : tab3) {
          v.push_back(p + s);
          do_tab0(p + s, v);
          do_tab1(p + s, v);
          do_tab2(p + s, v);
     }
}

void Numeral_prefixes::do_tab4(std::string const& p, Vecstring& v) {
     if (!p.empty())
          v.push_back(p);
     do_tab0(p, v);
     do_tab1(p, v);
     do_tab2(p, v);
     do_tab3(p, v);
}

std::vector<std::string> generate_numeral_prefixes() {
     return Numeral_prefixes::generate();
}

}  // namespace TESTS
