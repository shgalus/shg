/**
 * \file src/textutils.cc
 * Text processing utility functions.
 */

#include <shg/textutils.h>

namespace SHG {

Logical_line::Logical_line(std::istream& input, char delim)
     : input(input), delim(delim) {};

bool Logical_line::getline(std::string& str) {
     if (empty) {
          Logical_line::getline(input, str, delim);
          if (input.fail()) {
               return false;
          }
     } else {
          str = nextline;
          empty = true;
     }
     for (;;) {
          Logical_line::getline(input, nextline, delim);
          if (input.fail()) {
               return true;
          }
          if (nextline.starts_with(' ')) {
               SHG::ltrim(nextline);
               str += ' ';
               str += nextline;
          } else {
               empty = false;
               return true;
          }
     }
}

std::istream& Logical_line::getline(std::istream& input,
                                    std::string& str, char delim) {
     std::string s;
     std::string::size_type p;
     std::string::size_type last;   // last backslash
     std::string::size_type first;  // first backslash
     std::string::size_type n;      // number of backslashes

     str.clear();
     for (;;) {
          if (!std::getline(input, s, delim))
               break;
          rtrim(s);
          p = s.find_last_not_of(white_space);
          if (p == std::string::npos)
               s.erase();
          else
               s.erase(p + 1);
          if (!s.ends_with('\\')) {
               str += s;
               break;
          }
          last = s.size() - 1;
          first = s.find_last_not_of('\\', last);
          n = first == std::string::npos ? s.size() : last - first;
          if (!(n % 2)) {
               str += s;
               break;
          }
          s.erase(last);
          str += s;
          // Two lines below are important for cases when the last
          // line ends with unescaped backslash without delim
          // character.
          if (input.eof())
               break;
     }
     return input;
}

bool string_to_fields(std::string const& str, char delim,
                      std::vector<std::string>& v) {
     std::string s;
     v.clear();
     for (std::string::size_type i = 0; i < str.size(); i++) {
          if (str[i] == '\\') {
               if (i == str.size() - 1)
                    return false;
               else if (str[i + 1] == delim) {
                    s += delim;
                    i++;
               } else {
                    char c;
                    switch (str[i + 1]) {
                    case '\'':
                         c = '\'';
                         break;
                    case '\"':
                         c = '\"';
                         break;
                    case '\?':
                         c = '\?';
                         break;
                    case '\\':
                         c = '\\';
                         break;
                    case 'a':
                         c = '\a';
                         break;
                    case 'b':
                         c = '\b';
                         break;
                    case 'f':
                         c = '\f';
                         break;
                    case 'n':
                         c = '\n';
                         break;
                    case 'r':
                         c = '\r';
                         break;
                    case 't':
                         c = '\t';
                         break;
                    case 'v':
                         c = '\v';
                         break;
                    default:
                         return false;
                         break;
                    }
                    s += c;
                    i++;
               }
          } else if (str[i] == delim) {
               v.push_back(s);
               s.clear();
          } else {
               s += str[i];
          }
     }
     v.push_back(s);
     return true;
}

}  // namespace SHG
