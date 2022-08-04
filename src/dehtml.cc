/**
 * \file src/dehtml.cc
 * Implementation of dehtml.
 */

#include <shg/utils.h>
#include <cstring>
#include <cctype>

namespace SHG {

namespace {

class Error : public std::runtime_error {
public:
     Error();
};

Error::Error() : std::runtime_error("dehtml error") {}

class Dehtml {
public:
     Dehtml(std::istream& f, std::ostream& g) : f_(f), g_(g) {}
     void dehtml();

private:
     void getc();
     void putw();
     /** output white space until the given character */
     void whiten(char b);
     void markup_declaration();
     void processing_instruction();
     void tag();
     void reference();

     constexpr static struct {
          char const* const name;
          char const c;
     } const reftab_[] = {
          {"amp", '&'},   {"gt", '>'},   {"lt", '<'},  {"nbsp", ' '},
          {"quot", '\"'}, {"#38", '&'},  {"#62", '>'}, {"#60", '<'},
          {"#160", ' '},  {"#34", '\"'},
     };
     /**
      * Maximum length of character or entity reference, should be at
      * least as big as length of the longest reftab_[].name.
      */
     constexpr static unsigned const reflen_ = 4;
     char s_[reflen_ + 1];
     std::istream& f_;
     std::ostream& g_;
     char c_{};
};

void Dehtml::dehtml() {
     if (!f_.good())
          return;
     if (!g_.good())
          return;

     while (f_.get(c_)) {
          // std::cout << c_;
          if (c_ == '<') {
               putw();
               getc();
               putw();
               if (c_ == '!')
                    markup_declaration();
               else if (c_ == '?')
                    processing_instruction();
               else if (c_ != '>')
                    tag();
          } else if (c_ == '&') {
               reference();
          } else {
               if (!g_.put(c_))
                    throw Error();
          }
     }
}

void Dehtml::getc() {
     if (!f_.get(c_))
          throw Error();
}

void Dehtml::putw() {
     if (!g_.put(std::isspace(c_) ? c_ : ' '))
          throw Error();
}

void Dehtml::whiten(char b) {
     do {
          getc();
          putw();
     } while (c_ != b);
}

void Dehtml::markup_declaration() {
     getc();
     putw();
     if (c_ == '[')  // marked section
     {
          int n = 3;  // "]]>" is marked section end
          while (n > 0) {
               getc();
               putw();
               if (c_ == ']' && n > 1)
                    n--;
               else if (c_ == '>' && n == 1)
                    n = 0;
               else
                    n = 3;
          }
     } else {
          int m = 0;  // not in comment
          for (;;) {
               if (m == 0 && c_ == '>')
                    break;
               if (m == 0) {
                    if (c_ == '\"' || c_ == '\'') {
                         whiten(c_);
                    } else if (c_ == '-')
                         m = 1;
               } else if (m == 1)
                    m = (c_ == '-') ? 2 : 0;
               else if (m == 2) {
                    if (c_ == '-')
                         m = 3;
               } else if (m == 3)
                    m = (c_ == '-') ? 0 : 2;
               getc();
               putw();
          }
     }
}

void Dehtml::processing_instruction() {
     whiten('>');
}

void Dehtml::tag() {
     int n = 1;
     while (n > 0) {
          getc();
          putw();
          if (c_ == '<')  // another TAG
               n++;
          else if (c_ == '>')
               n--;
          else if (c_ == '\"' || c_ == '\'')
               whiten(c_);
     }
}

void Dehtml::reference() {
     int n = 1;
     unsigned i = 0;

     getc();
     n++;
     if (c_ == '#') {
          // character reference
          s_[i++] = c_;
          getc();
          n++;
     }
     while (c_ != ';' && c_ != '\r' && c_ != '\n' && c_ != ' ') {
          // reference end, see [1, p. 84]
          if (i < reflen_)
               s_[i++] = c_;
          getc();
          n++;
     }
     s_[i] = '\0';
     for (i = 0; i < std::size(reftab_); i++)
          if (std::strcmp(s_, reftab_[i].name) == 0)
               break;
     if (i < std::size(reftab_)) {
          if (!g_.put(reftab_[i].c))
               throw Error();
     } else {
          while (n-- > 0)
               if (!g_.put(' '))
                    throw Error();
     }
}

}  // anonymous namespace

bool dehtml(std::istream& f, std::ostream& g) {
     Dehtml d(f, g);
     try {
          d.dehtml();
          return f.eof() && !f.bad() && g.good();
     } catch (Error const&) {
          return false;
     }
}

}  // namespace SHG
