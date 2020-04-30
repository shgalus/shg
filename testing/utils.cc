/* utils.cc: testing miscellaneous utilities */

/**
 * \file testing/utils.cc
 * Testing miscellaneous utilities.
 * \date Created on 4 April 2010.
 */

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <limits>
#include "shg/except.h"
#include "shg/vector.h"
#include "shg/utils.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::size_t;
using std::string;
using std::vector;
using SHG::sqr;
using SHG::clean_string;
using SHG::iceil;
using SHG::ifloor;
using SHG::indirect_sort;
using SHG::ltrim;
using SHG::rtrim;
using SHG::split_string;
using SHG::strrtok;
using SHG::strtrim;
using SHG::trim;
using SHG::vbsearch;
using SHG::white_space;

namespace {

void test_sqr() {
     for (int i = -20; i <= 20; i++)
          SHG_ASSERT(sqr(i) == i * i);
     signed char x = -4, y = -3, z = 49;
     SHG_ASSERT(sqr(x + y) == z);
}

void test_utils1() {
     int i, j, m, n;
     double x;

     for (i = -100; i <= 100; ++i) {
          m = ifloor(i);
          n = iceil(i);
          SHG_ASSERT(m == -iceil(-i));
          SHG_ASSERT(n == -ifloor(-i));
          SHG_ASSERT(i - 1 < m && m <= i && i <= n && n < i + 1);
     }
     for (i = -100; i <= 99; ++i)
          for (j = 0; j <= 100; ++j) {
               x = i + (j / 100.0);
               m = ifloor(x);
               n = iceil(x);
               SHG_ASSERT(m == -iceil(-x));
               SHG_ASSERT(n == -ifloor(-x));
               // There is an error here with -Ofast.
               SHG_ASSERT(x - 1 < m && m <= x && x <= n && n < x + 1);
          }
}

void test_trim() {
     string s;
#define TEST(s1, s2)                                    \
     s = s1; rtrim(s, white_space); SHG_ASSERT(s == s2)
     TEST("",        "");
     TEST(" ",       "");
     TEST("  ",      "");
     TEST("a",       "a");
     TEST("a ",      "a");
     TEST("a  ",     "a");
     TEST("ab",      "ab");
     TEST("ab ",     "ab");
     TEST("ab  ",    "ab");
     TEST("abc",     "abc");
     TEST("abc ",    "abc");
     TEST("abc  ",   "abc");
#undef TEST
#define TEST(s1, s2)                                    \
     s = s1; ltrim(s, white_space); SHG_ASSERT(s == s2)
     TEST("",        "");
     TEST(" ",       "");
     TEST("  ",      "");
     TEST("a",       "a");
     TEST(" a",      "a");
     TEST("  a",     "a");
     TEST("ab",      "ab");
     TEST(" ab",     "ab");
     TEST("  ab",    "ab");
     TEST("abc",     "abc");
     TEST(" abc",    "abc");
     TEST("  abc",   "abc");
#undef TEST
#define TEST(s1, s2)                                    \
     s = s1; trim(s, white_space); SHG_ASSERT(s == s2)
     TEST("",        "");
     TEST(" ",       "");
     TEST("  ",      "");
     TEST("a",       "a");
     TEST(" a ",     "a");
     TEST("  a  ",   "a");
     TEST("ab",      "ab");
     TEST(" ab ",    "ab");
     TEST("  ab  ",  "ab");
     TEST("abc",     "abc");
     TEST(" abc ",   "abc");
     TEST("  abc  ", "abc");
#undef TEST
}

// Test cases are the same as in test_strtrim.
void test_clean_string() {
     string s;
#define TEST(s1, s2) s = s1; SHG_ASSERT(clean_string(s) == s2)
     TEST("",         "");
     TEST(" ",        "");
     TEST("  ",       "");
     TEST("   ",      "");
     TEST("a",        "a");
     TEST(" a",       "a");
     TEST("  a",      "a");
     TEST("a ",       "a");
     TEST("a  ",      "a");
     TEST(" a ",      "a");
     TEST("  a  ",    "a");
     TEST("ab",       "ab");
     TEST(" ab",      "ab");
     TEST("  ab",     "ab");
     TEST("ab ",      "ab");
     TEST(" ab  ",    "ab");
     TEST("  ab   ",  "ab");
     TEST("ab    c",  "ab c");
     TEST(" a   bc",  "a bc");
     TEST("  abc  ",  "abc");
     TEST("  a  bc ", "a bc");
     TEST(" abc  ",   "abc");
     TEST("  abc   ", "abc");
#undef TEST
}

void test_strsplit() {
     vector<string> v;
     v = split_string(string(""));
     SHG_ASSERT(v.size() == 0);
     v = split_string(string(" \n\v \t"));
     SHG_ASSERT(v.size() == 0);
     v = split_string(string("abc"));
     SHG_ASSERT(v.size() == 1 && v[0] == "abc");
     v = split_string(string(" abc"));
     SHG_ASSERT(v.size() == 1 && v[0] == "abc");
     v = split_string(string("abc "));
     SHG_ASSERT(v.size() == 1 && v[0] == "abc");
     v = split_string(string(" abc "));
     SHG_ASSERT(v.size() == 1 && v[0] == "abc");
     v = split_string(string("a b c"));
     SHG_ASSERT(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                v[2] == "c");
     v = split_string(string(" a b c"));
     SHG_ASSERT(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                v[2] == "c");
     v = split_string(string("a b c "));
     SHG_ASSERT(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                v[2] == "c");
     v = split_string(string(" a b c "));
     SHG_ASSERT(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                v[2] == "c");
     v = split_string(string(""), string(""));
     SHG_ASSERT(v.size() == 0);
     v = split_string(string(" "), string(""));
     SHG_ASSERT(v.size() == 1 && v[0] == " ");
}

template <class T>
void test_vbsearch() {
     auto comp = [](typename T::value_type x, typename T::value_type y)
          {return x < y;};
     T v {1, 3, 5, 7, 9};
     typename T::size_type l, u, i;
     typename T::value_type k;
     bool found;

     for (l = 0; l < v.size(); l++)
          for (u = 0; u <= v.size(); u++)
               for (k = 0; k <= 10; k++) {
                    found = vbsearch(k, v, l, u, i);
                    if (l >= u)
                         SHG_ASSERT(!found);
                    else if (k % 2 && v[l] <= k && k <= v[u - 1])
                         SHG_ASSERT(found && v[i] == k);
                    else
                         SHG_ASSERT(!found);
                    found = vbsearch(k, v, l, u, i, comp);
                    if (l >= u)
                         SHG_ASSERT(!found);
                    else if (k % 2 && v[l] <= k && k <= v[u - 1])
                         SHG_ASSERT(found && v[i] == k);
                    else
                         SHG_ASSERT(!found);
               }

     for (k = 0; k <= 10; k++) {
          found = vbsearch(k, v, i);
          if (k % 2)
               SHG_ASSERT(found && v[i] == k);
          else
               SHG_ASSERT(!found);
          found = vbsearch(k, v, i, comp);
          if (k % 2)
               SHG_ASSERT(found && v[i] == k);
          else
               SHG_ASSERT(!found);
     }
}

void test_strtrim(void) {
     char s[100];
#define TEST(s1, s2) SHG_ASSERT(!strcmp(strtrim(strcpy(s, s1)), s2))
     TEST("",         "");
     TEST(" ",        "");
     TEST("  ",       "");
     TEST("   ",      "");
     TEST("a",        "a");
     TEST(" a",       "a");
     TEST("  a",      "a");
     TEST("a ",       "a");
     TEST("a  ",      "a");
     TEST(" a ",      "a");
     TEST("  a  ",    "a");
     TEST("ab",       "ab");
     TEST(" ab",      "ab");
     TEST("  ab",     "ab");
     TEST("ab ",      "ab");
     TEST(" ab  ",    "ab");
     TEST("  ab   ",  "ab");
     TEST("ab    c",  "ab c");
     TEST(" a   bc",  "a bc");
     TEST("  abc  ",  "abc");
     TEST("  a  bc ", "a bc");
     TEST(" abc  ",   "abc");
     TEST("  abc   ", "abc");
#undef TEST
}

void test_strrtok(void) {
     char s[100], s1[100], *last, *t, *t1;
     int r;
     const char *delim = " ";

     strcpy(s, "a ab abc abcd ");
     strcpy(s1, "a ab abc abcd ");
     t = strrtok(s, delim, &last);
     t1 = strtok(s1, delim);

     for (;;) {
          if (t == 0)  {
               SHG_ASSERT(t1 == 0);
               break;
          }
          SHG_ASSERT(t1 != 0);
          r = strcmp(t, t1);
          SHG_ASSERT(r == 0);
          t = strrtok(0, delim, &last);
          t1 = strtok(0, delim);
     }
}

void test_indirect_sort() {
     typedef vector<double>::size_type dst;
     vector<double> w(5);
     w[0] = 3.0; w[1] = 4.0; w[2] = 1.0; w[3] = 5.0; w[4] = 2.0;
     vector<dst> v = indirect_sort(w);
     for (size_t i = 1; i < w.size(); i++)
          SHG_ASSERT(w[v[i - 1]] <= w[v[i]]);
}

void test_gcd() {
     using SHG::gcd;
     {
          signed char m = 12, n = 24;
          SHG_ASSERT(gcd(m, n) == 12);
     } {
          int m = 12, n = 24;
          SHG_ASSERT(gcd(m, n) == 12);
     } {                        // Testing specialisation for unsigned.
          unsigned m = 12, n = 24;
          SHG_ASSERT(gcd(m, n) == 12);
     } {
          long m = 1, n = 1;
          for (int i = 0; i < 30; i++)
               m *= 2;
          n = m / 2;
          // m = 2^30, n = 2^29.
          SHG_ASSERT(gcd(m, n) == n);
     }
     SHG_ASSERT(gcd(0, 124) == 124);
     SHG_ASSERT(gcd(0, -124) == 124);
     SHG_ASSERT(gcd(124, 0) == 124);
     SHG_ASSERT(gcd(-124, 0) == 124);
}

template <class T>
void test_int_div() {
     using SHG::Integer_division;
     for (T a = static_cast<T>(-100); a <= static_cast<T>(100); a++)
          for (T b = static_cast<T>(-100);
               b <= static_cast<T>(100); b++) {
               try {
                    const Integer_division<T> d(a, b);
                    SHG_ASSERT(b != 0);
                    SHG_ASSERT(
                         d.r >= 0 && d.r < std::abs(b) &&
                         a == d.q * b + d.r &&
                         Integer_division<T>::quotient(a, b) == d.q &&
                         Integer_division<T>::remainder(a, b) == d.r);
               } catch (const std::invalid_argument&) {
                    SHG_ASSERT(b == 0);
               }
          }
}

void test_integer_division() {
     test_int_div<int>();
     test_int_div<long int>();
     test_int_div<long long int>();
}

void test_round() {
     const double eps = 4.0 * std::numeric_limits<double>::min();
     SHG_ASSERT(std::abs(round(112.495, 2) - 112.5) < eps);
     SHG_ASSERT(std::abs(round(-112.495, 2) + 112.5) < eps);
     SHG_ASSERT(std::abs(round(112.495, 0) - 112.0) < eps);
     SHG_ASSERT(std::abs(round(-112.495, 0) + 112.0) < eps);
     SHG_ASSERT(std::abs(round(112.495, -2) - 100.0) < eps);
     SHG_ASSERT(std::abs(round(-112.495, -2) + 100.0) < eps);
}

// Factorial for small integers.
long fact(int n) {
     long f = 1;
     for (int i = 2; i <= n; i++)
          f *= i;
     return f;
}

// Check if combinations are lexicographically ordered and if the
// number of combinations agrees with Newton formula.
void test(int n, int k) {
     const size_t kk = k;
     SHG::Comblex c(n, k);
     vector<int> curr, prev;
     int nr = 0;
     do {
          curr = c.get();
          SHG_ASSERT(curr.size() == kk);
          for (size_t i = 1; i < kk; i++)
               SHG_ASSERT(curr[i - 1] < curr[i]);
          if (nr > 0) {
               size_t i = 0;
               while (i < kk && prev[i] == curr[i])
                    i++;
               SHG_ASSERT(i < kk);
               SHG_ASSERT(prev[i] < curr[i]);
          }
          prev = curr;
          nr++;
     } while (c.next());
     const long tnr = fact(n) / fact(k) / fact(n - k);
     SHG_ASSERT(nr == tnr);
}

void test_comblex() {
     for (int n = 1; n <= 10; n++)
          for (int k = 1; k <= n; k++)
               test(n, k);
}

}       // anonymous namespace

void test_utils() {
     test_sqr();
     test_utils1();
     test_trim();
     test_clean_string();
     test_strsplit();
     test_vbsearch<std::vector<int>>();
     test_vbsearch<SHG::Vector<int>>();
     test_strtrim();
     test_strrtok();
     test_indirect_sort();
     test_gcd();
     test_integer_division();
     test_round();
     test_comblex();
}

}       // namespace Testing
}       // namespace SHG
