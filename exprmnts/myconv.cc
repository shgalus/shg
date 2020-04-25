/* cc.cc: description */

#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#ifdef MCHECK
extern "C" {
#include <mcheck.h>
}
#endif

constexpr bool is_surrogate(char32_t c) {
     return c >= 0xd800u && c < 0xe000u;
}

template <class CharT1, class CharT2>
class Convert {
public:
     enum result {ok, error};
     typedef CharT1 char1_type;
     typedef CharT2 char2_type;
     typedef std::basic_string<char1_type> string1_type;
     typedef std::basic_string<char2_type> string2_type;

     string2_type convert12(const string1_type& s) {return do_convert12(s);}
     char2_type convert12(char1_type c) {return do_convert12(c);}
     string1_type convert21(const string2_type& s) {return do_convert21(s);}
     char1_type convert21(char2_type c) {return do_convert21(c);}
     
     result get_result() {
          return res_;
     }
protected:
     virtual string2_type do_convert12(const string1_type& s) = 0;
     virtual char2_type do_convert12(char1_type c) = 0;
     virtual string1_type do_convert21(const string2_type& s) = 0;
     virtual char1_type do_convert21(char2_type c) = 0;
     virtual ~Convert() {}
private:
     result res_ {ok};
};

using UTF8char        = char;
using UTF32char       = char32_t;

using UTF8string        = std::basic_string<UTF8char>;
using UTF32string       = std::basic_string<UTF32char>;

class UTF8_UTF32_converter : public Convert<UTF8char, UTF32char> {
protected:
     string2_type do_convert12(const string1_type& s) {
          string2_type t {s.begin(), s.end()};
          return t;
     }
     char2_type do_convert12(char1_type c) {
          return c;
     }
     string1_type do_convert21(const string2_type& s) {
          string1_type t;
          for (auto c : s)
               t += u32tou8(c);
          return t;
     }
     char1_type do_convert21(char2_type c) {
          return c;
     }
private:
     std::string u32tou8(char32_t c) {
          std::string::size_type nchars;
          unsigned char w[4];

          if (c < 0x80u) {
               w[0] = c;
               nchars = 1;
          } else if (c < 0x0800u) {
               w[0] = 0xc0u + c / 0x40u;
               w[1] = 0x80u + c % 0x40u;
               nchars = 2;
          } else if (c < 0x10000u) {
               if (is_surrogate(c))
                    goto error;
               w[0] = 0xe0u + c / 0x1000u;
               c %= 0x1000u;
               w[1] = 0x80u + c / 0x40u;
               w[2] = 0x80u + c % 0x40u;
               nchars = 3;
          } else if (c <= 0x10ffffu) {
               w[0] = 0xf0u + c / 0x40000u;
               c %= 0x40000u;
               w[1] = 0x80u + c / 0x1000u;
               c %= 0x1000u;
               w[2] = 0x80u + c / 0x40u;
               w[3] = 0x80u + c % 0x40u;
               nchars = 4;
          } else {
               goto error;
          }
          return std::string(reinterpret_cast<char*>(w), nchars);
     error:
          throw std::invalid_argument(__func__);
     }
};


using namespace std;

void test1() {
     const UTF8string pla8 = u8"a\u0105bc\u0107de\u0119fghijkl\u0142mn"
          "\u0144o\u00F3pqrs\u015Btuvwxyz\u017A\u017C\n"
          "A\u0104BC\u0106DE\u0118FGHIJKL\u0141MN"
          "\u0143O\u00D3PQRS\u015ATUVWXYZ\u0179\u017B\n";
     const UTF32string pla32 = U"a\u0105bc\u0107de\u0119fghijkl\u0142mn"
          "\u0144o\u00F3pqrs\u015Btuvwxyz\u017A\u017C\n"
          "A\u0104BC\u0106DE\u0118FGHIJKL\u0141MN"
          "\u0143O\u00D3PQRS\u015ATUVWXYZ\u0179\u017B\n";
     
     UTF8_UTF32_converter c;
     const UTF32string pla32res = c.convert12(pla8);
     const UTF8string pla8res = c.convert21(pla32);
     cout << pla8res << '\n';
     // assert(pla32res == pla32);     
     // assert(pla8res == pla8);
}

int main() {
#ifdef MCHECK
     mtrace();
#endif
     test1();
}
