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

bool is_prime(int n) {
     if (n <= 3)
          return n > 1;
     if (n % 2 == 0 || n % 3 == 0)
          return false;
     for (int i = 5; i * i <= n; i += 6)
          if (n % i == 0 || n % (i + 2) == 0)
               return false;
     return true;
}

Vecchar wfread(const char* filename) {
     using std::ifstream;
     using std::ios;
     ifstream f(filename, ios::in | ios::binary);
     if (f && f.seekg(0, ios::end)) {
          const ifstream::pos_type n = f.tellg();
          // Here either f.fail() != false or n >= 0. See
          // \cite ansi-cpp-2012, 27.7.2.3 (40), page 1035.
          if (f && f.seekg(0, ios::beg)) {
               // ifstream::pos_type may be long long (64 bit), size_t
               // may be unsigned (32 bit), streamsize may be int (32
               // bit)
               const auto nn = static_cast<decltype(
                    std::numeric_limits<Vecchar::size_type>::max())>(
                    n);
               if (nn > std::numeric_limits<
                             Vecchar::size_type>::max() ||
                   n > std::numeric_limits<std::streamsize>::max())
                    throw std::runtime_error(__func__);
               SHG::Vecchar buf(static_cast<Vecchar::size_type>(n));
               const std::streamsize ns =
                    static_cast<std::streamsize>(n);
               if (f.read(buf.c_vec(), ns) && f.gcount() == ns)
                    return buf;
          }
     }
     throw File_error(filename);
}

const char* const white_space = " \f\n\r\t\v";

string& ltrim(string& s, const string& trimchars) {
     return s.erase(0, s.find_first_not_of(trimchars));
}

string& rtrim(string& s, const string& trimchars) {
     return s.erase(s.find_last_not_of(trimchars) + 1);
}

string& trim(string& s, const string& trimchars) {
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

string& clean_string(string& s, const string& trimchars,
                     char replace_char) {
     string::size_type low = s.find_first_not_of(trimchars);
     if (low == string::npos)
          return s.erase();
     const string::size_type high = s.find_last_not_of(trimchars);
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

std::vector<string> split_string(const string& s, const string& sep) {
     std::vector<string> v;
     string::size_type p1, p2 = 0;
     while ((p1 = s.find_first_not_of(sep, p2)) != string::npos) {
          p2 = s.find_first_of(sep, p1);
          if (p2 == string::npos)
               p2 = s.size();
          v.push_back(s.substr(p1, p2 - p1));
     }
     return v;
}

char* strrtok(char* s, const char* delim, char** next) {
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

char* strdup(const char* s) {
     std::size_t n = strlen(s) + 1;
     char* t = new (std::nothrow) char[n];
     return t == nullptr
                 ? nullptr
                 : reinterpret_cast<char*>(std::memcpy(t, s, n));
}

Comblex::Comblex(int n, int k)
     : k(k), n1(n - 1), k1(k - 1), j(k < n ? k - 1 : -1), a(k) {
     if (k <= 0 || n < k)
          throw std::invalid_argument("invalid argument in Comblex");
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

bool isdegenerate(const std::gslice& g) {
     using std::begin;
     using std::end;
     using std::inner_product;
     using std::size_t;
     using std::valarray;

     const valarray<size_t>& l = g.size();
     const valarray<size_t>& d = g.stride();
     const size_t n = l.size();
     if (n < 1 || n != d.size())
          return true;
     if (l.min() < 1 || d.min() < 1)
          return true;
     valarray<bool> addr(
          inner_product(begin(l), end(l), begin(d), 0) - d.sum() + 1);
     valarray<size_t> i(n);
     for (;;) {
          size_t kk = inner_product(begin(i), end(i), begin(d), 0);
          SHG_ASSERT(kk < addr.size());
          if (addr[kk])
               return true;
          addr[kk] = true;
          for (size_t k = n - 1; ++i[k] >= l[k]; k--) {
               if (k == 0)
                    return false;
               i[k] = 0;
          }
     }
}

bool isvalid(size_t n, const std::gslice& g) {
     using std::begin;
     using std::end;
     using std::inner_product;
     using std::size_t;
     using std::valarray;

     if (isdegenerate(g))
          return false;
     const valarray<size_t>& l = g.size();
     const valarray<size_t>& d = g.stride();
     const size_t maxaddr =
          inner_product(begin(l), end(l), begin(d), g.start()) -
          d.sum();
     return maxaddr < n;
}

}  // namespace SHG
