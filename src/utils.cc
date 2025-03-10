/**
 * \file src/utils.cc
 * Miscellaneous utilities.
 * \date Created on 3 June 2009.
 */

#include <shg/utils.h>
#include <cctype>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <numeric>
#include <shg/except.h>

namespace SHG {

using std::string;

template <>
unsigned gcd<unsigned>(unsigned u, unsigned v) {
     unsigned r;
     while (v) {
          r = u % v;
          u = v;
          v = r;
     }
     return u;
}

Vecchar wfread(char const* filename) {
     using std::ifstream;
     using std::ios;
     ifstream f(filename, ios::in | ios::binary);
     if (f && f.seekg(0, ios::end)) {
          ifstream::pos_type const n = f.tellg();
          // Here either f.fail() != false or n >= 0. See
          // \cite ansi-cpp-2012, 27.7.2.3 (40), page 1035.
          if (f && f.seekg(0, ios::beg)) {
               // ifstream::pos_type may be long long (64 bit), size_t
               // may be unsigned (32 bit), streamsize may be int (32
               // bit)
               auto const nn =
                    static_cast<decltype(std::numeric_limits<
                                         Vecchar::size_type>::max())>(
                         n);
               if (nn > std::numeric_limits<
                             Vecchar::size_type>::max() ||
                   n > std::numeric_limits<std::streamsize>::max())
                    throw std::runtime_error(__func__);
               SHG::Vecchar buf(static_cast<Vecchar::size_type>(n));
               std::streamsize const ns =
                    static_cast<std::streamsize>(n);
               if (f.read(buf.c_vec(), ns) && f.gcount() == ns)
                    return buf;
          }
     }
     throw Exception(std::string("error reading file: ") + filename);
}

template <>
void read_binary(std::string& a, std::istream& f) {
     std::string::size_type size;
     read_binary(size, f);
     char* t = new (std::nothrow) char[size];
     if (t == nullptr) {
          f.setstate(std::ios::failbit);
     } else {
          f.read(t, size);
          a.assign(t, size);
          delete[] t;
     }
}

char const* const white_space = " \f\n\r\t\v";

string& ltrim(string& s, string const& trimchars) {
     return s.erase(0, s.find_first_not_of(trimchars));
}

string& rtrim(string& s, string const& trimchars) {
     auto const pos = s.find_last_not_of(trimchars);
     return pos == std::string::npos ? s.erase() : s.erase(pos + 1);
}

string& trim(string& s, string const& trimchars) {
     return ltrim(rtrim(s, trimchars), trimchars);
}

char* strtrim(char* s) {
     char *t = s, *u = s;

     while (std::isspace(*u))
          u++;
     while ((*t++ = *u++))
          if (std::isspace(*u)) {
               while (std::isspace(*++u)) { /* VOID */
               }
               if (*u) {
                    *t++ = ' ';
               } else {
                    *t = '\0';
                    break;
               }
          }
     return s;
}

string& clean_string(string& s, string const& trimchars,
                     char replace_char) {
     string::size_type low = s.find_first_not_of(trimchars);
     if (low == string::npos)
          return s.erase();
     string::size_type const high = s.find_last_not_of(trimchars);
     for (string::size_type i = 0;; i++) {
          s[i] = s[low];
          if (low == high)
               return s.erase(i + 1);
          if (trimchars.find(s[++low]) != string::npos) {
               while (trimchars.find(s[++low]) != string::npos) {
                    /* VOID */
               }
               s[++i] = replace_char;
          }
     }
}

char* strrtok(char* s, char const* delim, char** next) {
     char* tok;

     if (s == 0 && (s = *next) == 0)
          return 0;
     s += std::strspn(s, delim);
     if (!*s) {
          *next = 0;
          return 0;
     }
     tok = s;
     s = std::strpbrk(tok, delim);
     if (s == 0) {
          *next = 0;
     } else {
          *s = '\0';
          *next = s + 1;
     }
     return tok;
}

char* strdup(char const* s) {
     std::size_t n = strlen(s) + 1;
     char* t = new (std::nothrow) char[n];
     return t == nullptr
                 ? nullptr
                 : reinterpret_cast<char*>(std::memcpy(t, s, n));
}

int tmcmp(std::tm const& lhs, std::tm const& rhs) {
     if (lhs.tm_year < rhs.tm_year)
          return -1;
     if (lhs.tm_year > rhs.tm_year)
          return 1;
     if (lhs.tm_mon < rhs.tm_mon)
          return -1;
     if (lhs.tm_mon > rhs.tm_mon)
          return 1;
     if (lhs.tm_mday < rhs.tm_mday)
          return -1;
     if (lhs.tm_mday > rhs.tm_mday)
          return 1;
     if (lhs.tm_hour < rhs.tm_hour)
          return -1;
     if (lhs.tm_hour > rhs.tm_hour)
          return 1;
     if (lhs.tm_min < rhs.tm_min)
          return -1;
     if (lhs.tm_min > rhs.tm_min)
          return 1;
     if (lhs.tm_sec < rhs.tm_sec)
          return -1;
     if (lhs.tm_sec > rhs.tm_sec)
          return 1;
     return 0;
}

Comblex::Comblex(int n, int k)
     : k(k), n1(n - 1), k1(k - 1), j(k < n ? k - 1 : -1), a(k) {
     if (k <= 0 || n < k)
          throw std::invalid_argument(
               "invalid argument in Comblex()");
     for (int i = 0; i < k; i++)
          a[i] = i;
}

bool Comblex::next() {
     if (j < 0)
          return false;
     a[j]++;
     for (int i = j + 1; i < k; i++)
          a[i] = a[i - 1] + 1;
     if (a[k1] < n1)
          j = k1;
     else
          j--;
     return true;
}

Varlex::Varlex(int n, int k) : n_(n), n1_(n - 1), k_(k), a_() {
     if (n <= 0 || k <= 0)
          throw std::invalid_argument("invalid argument in Varlex()");
     a_.resize(k_);
     reset();
}

void Varlex::reset() {
     std::fill(a_.begin(), a_.end(), 0);
}

bool Varlex::next() {
     for (int i = 0; i < k_; i++)
          if (a_[i]++ < n1_)
               return true;
          else
               a_[i] = 0;
     return false;
}

std::string to_octal(std::string const& s) {
     std::ostringstream oss;
     unsigned char u;
     for (auto c : s) {
          u = reinterpret_cast<unsigned char&>(c);
          if (u < 0x80)
               oss << c;
          else
               oss << '\\' << std::oct << +u;
     }
     return oss.str();
}

}  // namespace SHG
