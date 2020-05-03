#include <cmath>
#include <cstring>
#include <ios>
#include <limits>
#include <memory>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include "shg/utils.h"

namespace SHG::BTesting {

BOOST_AUTO_TEST_SUITE(utils_test)

BOOST_AUTO_TEST_CASE(narrow_cast_test) {
     constexpr auto max = std::numeric_limits<signed char>::max();
     constexpr auto min = std::numeric_limits<signed char>::min();
     constexpr double maxd = max;
     constexpr double mind = min;

     BOOST_CHECK(narrow_cast<signed char>(maxd) == max);
     BOOST_CHECK(narrow_cast<signed char>(mind) == min);
     BOOST_CHECK_THROW(narrow_cast<signed char>(maxd + 1.0),
                       std::runtime_error);
     BOOST_CHECK_THROW(narrow_cast<signed char>(mind - 1.0),
                       std::runtime_error);
}

BOOST_AUTO_TEST_CASE(sqr_test) {
     for (int i = -20; i <= 20; i++)
          BOOST_CHECK(sqr(i) == i * i);
     const signed char x = -4, y = -3, z = 49;
     BOOST_CHECK(sqr(x + y) == z);
}

BOOST_AUTO_TEST_CASE(cube_test) {
     for (int i = -20; i <= 20; i++)
          BOOST_CHECK(cube(i) == i * i * i);
     const signed char x = -4, y = 7, z = 27;
     BOOST_CHECK(cube(x + y) == z);
}

BOOST_AUTO_TEST_CASE(sgn_test) {
     for (int i = -20; i <= 20; i++)
          BOOST_CHECK(sgn(i) == (i < 0 ? -1 : (i > 0 ? 1 : 0)));
}

BOOST_AUTO_TEST_CASE(ifloor_test) {
     for (int i = -100; i <= 100; i++) {
          const double x = i;
          const int n = ifloor(x);
          BOOST_CHECK(n == i);
          BOOST_CHECK(n == -iceil(-x));
          BOOST_CHECK(i - 1 < n && n <= i);
     }
     for (int i = -100; i <= 99; i++)
          for (int j = 0; j <= 100; j++) {
               const double x = i + (j / 100.0);
               const int n = ifloor(x);
               if (j != 100)
                    BOOST_CHECK(n == i);
               else
                    BOOST_CHECK(n == i + 1);
               BOOST_CHECK(n == -iceil(-x));
               BOOST_CHECK(x - 1 < n && n <= x);
          }
     constexpr auto max = std::numeric_limits<signed char>::max();
     constexpr auto min = std::numeric_limits<signed char>::min();
     BOOST_CHECK_THROW((ifloor<signed char, double>(max)),
                       std::overflow_error);
     BOOST_CHECK_THROW((ifloor<signed char, double>(min)),
                       std::overflow_error);
     for (signed char i = min + 1; i < max; i++) {
          const double x = i;
          BOOST_CHECK((ifloor<signed char, double>(x)) == i);
     }
}

BOOST_AUTO_TEST_CASE(iceil_test) {
     for (int i = -100; i <= 100; i++) {
          const double x = i;
          const int n = iceil(x);
          BOOST_CHECK(n == i);
          BOOST_CHECK(n == -ifloor(-x));
          BOOST_CHECK(i <= n && n < i + 1);
     }
     for (int i = -100; i <= 99; i++)
          for (int j = 0; j <= 100; j++) {
               const double x = i + (j / 100.0);
               const int n = iceil(x);
               if (j != 0)
                    BOOST_CHECK(n == i + 1);
               else
                    BOOST_CHECK(n == i);
               BOOST_CHECK(n == -ifloor(-x));
               BOOST_CHECK(x <= n && n < x + 1);
          }
     constexpr auto max = std::numeric_limits<signed char>::max();
     constexpr auto min = std::numeric_limits<signed char>::min();
     BOOST_CHECK_THROW((iceil<signed char, double>(max)),
                       std::overflow_error);
     BOOST_CHECK_THROW((iceil<signed char, double>(min)),
                       std::overflow_error);
     for (signed char i = min + 1; i < max; i++) {
          const double x = i;
          BOOST_CHECK((iceil<signed char, double>(x)) == i);
     }
}

using test_types =
     boost::mpl::list<signed char, int, long int, long long int>;

BOOST_AUTO_TEST_CASE_TEMPLATE(integer_division_test, T, test_types) {
     using Idiv = Integer_division<T>;
     for (T a = static_cast<T>(-100); a <= static_cast<T>(100); a++)
          for (T b = static_cast<T>(-100); b <= static_cast<T>(100);
               b++) {
               try {
                    const Idiv d(a, b);
                    BOOST_CHECK(b != 0);
                    BOOST_CHECK(d.r >= 0 && d.r < std::abs(b));
                    BOOST_CHECK(a == d.q * b + d.r);
                    BOOST_CHECK(Idiv::quotient(a, b) == d.q);
                    BOOST_CHECK(Idiv::remainder(a, b) == d.r);
               } catch (const std::invalid_argument&) {
                    BOOST_CHECK(b == 0);
               }
          }
}

BOOST_AUTO_TEST_CASE(round_test) {
     using std::abs;
     constexpr double eps = 4.0 * std::numeric_limits<double>::min();
     BOOST_CHECK(abs(round(112.495, 2) - 112.5) < eps);
     BOOST_CHECK(abs(round(-112.495, 2) + 112.5) < eps);
     BOOST_CHECK(abs(round(112.495, 0) - 112.0) < eps);
     BOOST_CHECK(abs(round(-112.495, 0) + 112.0) < eps);
     BOOST_CHECK(abs(round(112.495, -2) - 100.0) < eps);
     BOOST_CHECK(abs(round(-112.495, -2) + 100.0) < eps);
}

BOOST_AUTO_TEST_CASE(write_binary_test) {
     using std::ios_base;
     using std::strcmp;
     using std::stringstream;

     struct S {
          char c[10];
          int i;
          double d;
     };

     const S s{"123456789", 13, 0.1};
     S t;
     stringstream ss(ios_base::in | ios_base::out | ios_base::binary);
     write_binary(s, ss);
     ss.read(reinterpret_cast<char*>(&t), sizeof t);
     BOOST_CHECK(strcmp(s.c, t.c) == 0);
     BOOST_CHECK(s.i == t.i);
     BOOST_CHECK(s.d == t.d);
}

BOOST_AUTO_TEST_CASE(read_binary_test) {
     using std::ios_base;
     using std::strcmp;
     using std::stringstream;

     struct S {
          char c[10];
          int i;
          double d;
     };

     const S s{"123456789", 13, 0.1};
     S t;
     stringstream ss(ios_base::in | ios_base::out | ios_base::binary);
     ss.write(reinterpret_cast<const char*>(&s), sizeof s);
     read_binary(t, ss);
     BOOST_CHECK(strcmp(s.c, t.c) == 0);
     BOOST_CHECK(s.i == t.i);
     BOOST_CHECK(s.d == t.d);
}

BOOST_AUTO_TEST_CASE(wfread_test) {
     BOOST_TEST_MESSAGE("wfread is not tested");
}

BOOST_AUTO_TEST_CASE(ltrim_test) {
     std::string s;
     s = "";
     BOOST_CHECK(ltrim(s) == "");
     s = " ";
     BOOST_CHECK(ltrim(s) == "");
     s = "   ";
     BOOST_CHECK(ltrim(s) == "");
     s = "a";
     BOOST_CHECK(ltrim(s) == "a");
     s = " a";
     BOOST_CHECK(ltrim(s) == "a");
     s = "  a";
     BOOST_CHECK(ltrim(s) == "a");
     s = "ab";
     BOOST_CHECK(ltrim(s) == "ab");
     s = " ab";
     BOOST_CHECK(ltrim(s) == "ab");
     s = "  ab";
     BOOST_CHECK(ltrim(s) == "ab");
     s = "abc";
     BOOST_CHECK(ltrim(s) == "abc");
     s = " abc";
     BOOST_CHECK(ltrim(s) == "abc");
     s = "  abc";
     BOOST_CHECK(ltrim(s) == "abc");

     s = "a ";
     BOOST_CHECK(ltrim(s) == "a ");
     s = " a ";
     BOOST_CHECK(ltrim(s) == "a ");
     s = "  a ";
     BOOST_CHECK(ltrim(s) == "a ");
     s = "ab ";
     BOOST_CHECK(ltrim(s) == "ab ");
     s = " ab ";
     BOOST_CHECK(ltrim(s) == "ab ");
     s = "  ab ";
     BOOST_CHECK(ltrim(s) == "ab ");
     s = "abc ";
     BOOST_CHECK(ltrim(s) == "abc ");
     s = " abc ";
     BOOST_CHECK(ltrim(s) == "abc ");
     s = "  abc ";
     BOOST_CHECK(ltrim(s) == "abc ");
}

BOOST_AUTO_TEST_CASE(rtrim_test) {
     std::string s;
     s = "";
     BOOST_CHECK(rtrim(s) == "");
     s = " ";
     BOOST_CHECK(rtrim(s) == "");
     s = "   ";
     BOOST_CHECK(rtrim(s) == "");
     s = "a";
     BOOST_CHECK(rtrim(s) == "a");
     s = "a ";
     BOOST_CHECK(rtrim(s) == "a");
     s = "a  ";
     BOOST_CHECK(rtrim(s) == "a");
     s = "ab";
     BOOST_CHECK(rtrim(s) == "ab");
     s = "ab ";
     BOOST_CHECK(rtrim(s) == "ab");
     s = "ab  ";
     BOOST_CHECK(rtrim(s) == "ab");
     s = "abc";
     BOOST_CHECK(rtrim(s) == "abc");
     s = "abc ";
     BOOST_CHECK(rtrim(s) == "abc");
     s = "abc  ";
     BOOST_CHECK(rtrim(s) == "abc");

     s = " a";
     BOOST_CHECK(rtrim(s) == " a");
     s = " a ";
     BOOST_CHECK(rtrim(s) == " a");
     s = " a  ";
     BOOST_CHECK(rtrim(s) == " a");
     s = " ab";
     BOOST_CHECK(rtrim(s) == " ab");
     s = " ab ";
     BOOST_CHECK(rtrim(s) == " ab");
     s = " ab  ";
     BOOST_CHECK(rtrim(s) == " ab");
     s = " abc";
     BOOST_CHECK(rtrim(s) == " abc");
     s = " abc ";
     BOOST_CHECK(rtrim(s) == " abc");
     s = " abc  ";
     BOOST_CHECK(rtrim(s) == " abc");
}

BOOST_AUTO_TEST_CASE(trim_test) {
     std::string s;
     s = "";
     BOOST_CHECK(trim(s) == "");
     s = " ";
     BOOST_CHECK(trim(s) == "");
     s = "  ";
     BOOST_CHECK(trim(s) == "");
     s = "a";
     BOOST_CHECK(trim(s) == "a");
     s = " a ";
     BOOST_CHECK(trim(s) == "a");
     s = "  a  ";
     BOOST_CHECK(trim(s) == "a");
     s = "ab";
     BOOST_CHECK(trim(s) == "ab");
     s = " ab ";
     BOOST_CHECK(trim(s) == "ab");
     s = "  ab  ";
     BOOST_CHECK(trim(s) == "ab");
     s = "abc";
     BOOST_CHECK(trim(s) == "abc");
     s = " abc ";
     BOOST_CHECK(trim(s) == "abc");
     s = "  abc  ";
     BOOST_CHECK(trim(s) == "abc");
}

namespace {

struct Case {
     const char* const input;
     const char* const output;
};

std::ostream& operator<<(std::ostream& stream, const Case& c) {
     stream.width(0);
     stream << c.input << "   " << c.output << '\n';
     return stream;
}

const std::vector<Case> vc{
     {"", ""},
     {" ", ""},
     {"  ", ""},
     {"a", "a"},
     {" a ", "a"},
     {"  a  ", "a"},
     {"ab", "ab"},
     {" ab ", "ab"},
     {"  ab  ", "ab"},
     {"a b", "a b"},
     {" a b ", "a b"},
     {"  a  b  ", "a b"},
     {"abc", "abc"},
     {" abc ", "abc"},
     {"  abc  ", "abc"},
     {" a bc ", "a bc"},
     {" a  bc ", "a bc"},
     {" a  b c ", "a b c"},
     {" a  b  c ", "a b c"},
};

}  // anonymous namespace

namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(strtrim_test, bdata::make(vc), c) {
     using std::strcmp;
     using std::strcpy;
     char s[10];
     BOOST_CHECK(!strcmp(strtrim(strcpy(s, c.input)), c.output));
}

BOOST_DATA_TEST_CASE(clean_string_test, bdata::make(vc), c) {
     std::string s;
     s = c.input;
     BOOST_CHECK(clean_string(s) == c.output);
}

BOOST_AUTO_TEST_CASE(split_string_test) {
     std::vector<std::string> v;

     v = split_string("");
     BOOST_CHECK(v.size() == 0);
     v = split_string(" \n\v \t");
     BOOST_CHECK(v.size() == 0);
     v = split_string("abc");
     BOOST_CHECK(v.size() == 1 && v[0] == "abc");
     v = split_string(" abc");
     BOOST_CHECK(v.size() == 1 && v[0] == "abc");
     v = split_string("abc ");
     BOOST_CHECK(v.size() == 1 && v[0] == "abc");
     v = split_string(" abc ");
     BOOST_CHECK(v.size() == 1 && v[0] == "abc");
     v = split_string("a b c");
     BOOST_CHECK(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                 v[2] == "c");
     v = split_string(" a b c");
     BOOST_CHECK(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                 v[2] == "c");
     v = split_string("a b c ");
     BOOST_CHECK(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                 v[2] == "c");
     v = split_string(" a b c ");
     BOOST_CHECK(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                 v[2] == "c");
     v = split_string("", "");
     BOOST_CHECK(v.size() == 0);
     v = split_string(" ", "");
     BOOST_CHECK(v.size() == 1 && v[0] == " ");
}

BOOST_AUTO_TEST_CASE(indirect_sort_test) {
     const std::vector<double> w{3.0, 4.0, 1.0, 5.0, 2.0};
     const auto v = indirect_sort(w);
     for (std::size_t i = 1; i < w.size(); i++)
          BOOST_CHECK(w[v[i - 1]] <= w[v[i]]);
}

BOOST_AUTO_TEST_CASE(strrtok_test) {
     using std::strcpy;
     using std::strtok;
     char s[100], s1[100], *last, *t, *t1;
     const char* const delim = " ";

     strcpy(s, "a ab abc abcd ");
     strcpy(s1, "a ab abc abcd ");
     t = strrtok(s, delim, &last);
     t1 = strtok(s1, delim);

     for (;;) {
          if (t == nullptr) {
               BOOST_CHECK(t1 == nullptr);
               break;
          }
          BOOST_CHECK(t1 != nullptr);
          BOOST_CHECK(strcmp(t, t1) == 0);
          t = strrtok(nullptr, delim, &last);
          t1 = strtok(nullptr, delim);
     }
}

BOOST_AUTO_TEST_CASE(strdup_test) {
     std::unique_ptr<char> s(strdup("abcdefgh"));
     if (s.get() != nullptr)
          BOOST_CHECK(!std::strcmp(s.get(), "abcdefgh"));
}

BOOST_AUTO_TEST_CASE(alloc_c_matrix_test) {
     BOOST_TEST_MESSAGE("alloc_c_matrix is not tested");
}

BOOST_AUTO_TEST_CASE(free_c_matrix_test) {
     BOOST_TEST_MESSAGE("free_c_matrix is not tested");
}

BOOST_AUTO_TEST_CASE(timer_test) {
     Timer t;
     const double t1 = t.elapsed();
     BOOST_CHECK(t1 >= 0.0);
     const double t2 = t.elapsed();
     BOOST_CHECK(t2 >= t1);
     t.restart();
     BOOST_CHECK(t.elapsed() >= 0.0);
}

BOOST_DATA_TEST_CASE(comblex_test,
                     bdata::xrange(9) * bdata::xrange(9), xr1, xr2) {
     using std::size_t;
     const int n = xr1 + 1;
     const int k = xr2 + 1;

     if (k > n) {
          BOOST_CHECK(true);
          return;
     }

     // Check if combinations are lexicographically ordered and if the
     // number of combinations agrees with Newton formula.

     auto fact = [](int n) {
          long f = 1;
          for (int i = 2; i <= n; i++)
               f *= i;
          return f;
     };
     const size_t kk = k;
     Comblex c(n, k);
     std::vector<int> curr, prev;
     int nr = 0;
     do {
          curr = c.get();
          BOOST_CHECK(curr.size() == kk);
          for (size_t i = 1; i < kk; i++)
               BOOST_CHECK(curr[i - 1] < curr[i]);
          if (nr > 0) {
               size_t i = 0;
               while (i < kk && prev[i] == curr[i])
                    i++;
               BOOST_CHECK(i < kk);
               BOOST_CHECK(prev[i] < curr[i]);
          }
          prev = curr;
          nr++;
     } while (c.next());
     const long tnr = fact(n) / fact(k) / fact(n - k);
     BOOST_CHECK(nr == tnr);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting
