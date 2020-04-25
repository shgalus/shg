#include <cassert>
#include <locale>
#include <iostream>
#include <iomanip>
#include <string>
#include <codecvt>

// codecvt_iso88592_utf32

class codecvt_utf32_iso88592 :
     public std::codecvt<char32_t, char, std::mbstate_t> {
     
};



void dosw() {
     codecvt_utf32_iso88592 C;
     std::wstring_convert<codecvt_utf32_iso88592> wc;
     // wstring_convert<codecvt_utf8<char32_t>, char32_t> ucs2conv;
     // u32string t = ucs2conv.from_bytes(s);
     //return t;
}

/*
template <class Char1, class Char2>
class Convert {
public:
     enum result {ok, error};
     virtual std::basic_string<Char1> convert(const std::basic_string<Char2>& s) = 0;
     virtual Char1 convert(Char2 c) = 0;
     virtual std::basic_string<Char2> convert(const std::basic_string<Char1>& s) = 0;
     virtual Char2 convert(Char1 c) = 0;
     result get_result() {
          return res_;
     }
     virtual ~Convert() {}
private:
     result res_;
};

class UTF8_UTF32_converter : public Convert<char, char32_t> {
public:
     std::basic_string<char> convert(const std::basic_string<char32_t>& s) {
          return std::basic_string<char>();
     }
     char convert(char32_t c) {
          return 0;
     }
     
};
*/

using namespace std;

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};


u32string utf8_to_utf32(const string& s) {
     wstring_convert<deletable_facet<codecvt<char32_t, char, std::mbstate_t>>, char32_t> ucs2conv;     
     // wstring_convert<codecvt_utf8<char32_t>, char32_t> ucs2conv;
     u32string t = ucs2conv.from_bytes(s);
     return t;
}

string utf32_to_utf8(const u32string& s) {
     wstring_convert<codecvt_utf8<char32_t>, char32_t> ucs2conv;
     string t = ucs2conv.to_bytes(s);
     return t;
}

int main() {
     string s = "Ala";
     u32string t = utf8_to_utf32(s);
     string u = utf32_to_utf8(t);
     assert(s == u);

     const string pla8 = u8"a\u0105bc\u0107de\u0119fghijkl\u0142mn"
          "\u0144o\u00F3pqrs\u015Btuvwxyz\u017A\u017C\n"
          "A\u0104BC\u0106DE\u0118FGHIJKL\u0141MN"
          "\u0143O\u00D3PQRS\u015ATUVWXYZ\u0179\u017B\n";
     const u32string pla32 = U"a\u0105bc\u0107de\u0119fghijkl\u0142mn"
          "\u0144o\u00F3pqrs\u015Btuvwxyz\u017A\u017C\n"
          "A\u0104BC\u0106DE\u0118FGHIJKL\u0141MN"
          "\u0143O\u00D3PQRS\u015ATUVWXYZ\u0179\u017B\n";
     assert(pla8 == utf32_to_utf8(pla32));
     assert(pla32 == utf8_to_utf32(pla8));


}
