#include <shg/utils.h>
#include <cstring>
#include <memory>
#include <sstream>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(utils_test)

BOOST_AUTO_TEST_CASE(ignore_unused_variable_test) {
     using SHG::ignore_unused_variable;
     int i;
     double c;
     char const* s = "xxx";
     char t[100];

     ignore_unused_variable();
     ignore_unused_variable(&i);
     ignore_unused_variable(i, &c, s, t);
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(narrow_cast_test) {
     using SHG::narrow_cast;
     constexpr auto max = std::numeric_limits<signed char>::max();
     constexpr auto min = std::numeric_limits<signed char>::min();
     constexpr double maxd = max;
     constexpr double mind = min;

     BOOST_CHECK(narrow_cast<signed char>(maxd) == max);
     BOOST_CHECK(narrow_cast<signed char>(mind) == min);

     /* These two checks are not performed as they cause errors when
        running executable compiled with -fsanitize=undefined.
     BOOST_CHECK_THROW(narrow_cast<signed char>(maxd + 1.0),
                       std::runtime_error);
     BOOST_CHECK_THROW(narrow_cast<signed char>(mind - 1.0),
                       std::runtime_error);
     */
}

BOOST_AUTO_TEST_CASE(sqr_test) {
     using SHG::sqr;
     for (int i = -20; i <= 20; i++)
          BOOST_CHECK(sqr(i) == i * i);
     signed char const x = -4, y = -3, z = 49;
     BOOST_CHECK(sqr(x + y) == z);
}

BOOST_AUTO_TEST_CASE(cube_test) {
     using SHG::cube;
     for (int i = -20; i <= 20; i++)
          BOOST_CHECK(cube(i) == i * i * i);
     signed char const x = -4, y = 7, z = 27;
     BOOST_CHECK(cube(x + y) == z);
}

BOOST_AUTO_TEST_CASE(sgn_test) {
     using SHG::sgn;
     for (int i = -20; i <= 20; i++)
          BOOST_CHECK(sgn(i) == (i < 0 ? -1 : (i > 0 ? 1 : 0)));
}

BOOST_AUTO_TEST_CASE(ifloor_test) {
     using SHG::ifloor;
     using SHG::iceil;
     for (int i = -100; i <= 100; i++) {
          double const x = i;
          int const n = ifloor(x);
          BOOST_CHECK(n == i);
          BOOST_CHECK(n == -iceil(-x));
          BOOST_CHECK(i - 1 < n && n <= i);
     }
     for (int i = -100; i <= 99; i++)
          for (int j = 0; j <= 100; j++) {
               double const x = i + (j / 100.0);
               int const n = ifloor(x);
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
          double const x = i;
          BOOST_CHECK((ifloor<signed char, double>(x)) == i);
     }
}

BOOST_AUTO_TEST_CASE(iceil_test) {
     using SHG::ifloor;
     using SHG::iceil;
     for (int i = -100; i <= 100; i++) {
          double const x = i;
          int const n = iceil(x);
          BOOST_CHECK(n == i);
          BOOST_CHECK(n == -ifloor(-x));
          BOOST_CHECK(i <= n && n < i + 1);
     }
     for (int i = -100; i <= 99; i++)
          for (int j = 0; j <= 100; j++) {
               double const x = i + (j / 100.0);
               int const n = iceil(x);
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
          double const x = i;
          BOOST_CHECK((iceil<signed char, double>(x)) == i);
     }
}

using test_types =
     boost::mpl::list<signed char, short int, int, long int,
                      long long int, boost::multiprecision::cpp_int>;

BOOST_AUTO_TEST_CASE_TEMPLATE(divide_test, T, test_types) {
     using SHG::divide;
     using SHG::divides;
     int const n1 = std::numeric_limits<signed char>::min();
     int const n2 = std::numeric_limits<signed char>::max();
     T q, r;
     for (int ia = n1; ia <= n2; ia++) {
          T const a = static_cast<T>(ia);
          for (int ib = n1; ib <= n2; ib++) {
               T const b = static_cast<T>(ib);
               try {
                    divide(a, b, q, r);
                    BOOST_CHECK(r >= 0 && r < abs(b));
                    BOOST_CHECK(a == q * b + r);
                    BOOST_CHECK(divides(b, a) == (r == 0));
               } catch (std::invalid_argument const&) {
                    BOOST_CHECK(b == 0);
               } catch (std::out_of_range const&) {
                    BOOST_CHECK(
                         (std::is_same<T, signed char>::value));
                    BOOST_CHECK(a == n1 && b == -1);
               }
          }
     }
}

BOOST_AUTO_TEST_CASE(gcd_test) {
     using SHG::gcd;
     using boost::multiprecision::cpp_int;
     using boost::multiprecision::pow;
     {
          signed char m = 12, n = 24;
          BOOST_CHECK(gcd(m, n) == 12);
     }
     {
          int m = 12, n = 24;
          BOOST_CHECK(gcd(m, n) == 12);
     }
     {  // Testing specialization for unsigned.
          unsigned m = 12, n = 24;
          BOOST_CHECK(gcd(m, n) == 12);
     }
     {
          long m = 1, n = 1;
          for (int i = 0; i < 30; i++)
               m *= 2;
          n = m / 2;
          // m = 2^30, n = 2^29.
          BOOST_CHECK(gcd(m, n) == n);
     }
     {
          cpp_int const m = pow(cpp_int(2), 30);
          cpp_int const n = pow(cpp_int(2), 29);
          BOOST_CHECK(gcd(m, n) == n);
     }
     BOOST_CHECK(gcd(0, 124) == 124);
     BOOST_CHECK(gcd(0, -124) == 124);
     BOOST_CHECK(gcd(124, 0) == 124);
     BOOST_CHECK(gcd(-124, 0) == 124);
}

/**
 * Basic test. See \cite knuth-2002b, section 4.5.2, page 367.
 */
BOOST_AUTO_TEST_CASE(extended_gcd_basic_test) {
     using SHG::Extended_gcd;
     {
          Extended_gcd<int> xgcd;
          xgcd.calculate(40902, 24140);
          BOOST_CHECK(xgcd.u1 == 337 && xgcd.u2 == -571 &&
                      xgcd.u3 == 34);
     }
     {
          Extended_gcd xgcd(40902, 24140);
          BOOST_CHECK(xgcd.u1 == 337 && xgcd.u2 == -571 &&
                      xgcd.u3 == 34);
     }
}

BOOST_DATA_TEST_CASE(extended_gcd_extended_test,
                     bdata::xrange(20) * bdata::xrange(20), xr1,
                     xr2) {
     using SHG::Extended_gcd;
     using std::gcd;
     Extended_gcd<int> xgcd(xr1, xr2);
     BOOST_CHECK(xr1 * xgcd.u1 + xr2 * xgcd.u2 == xgcd.u3);
     BOOST_CHECK(xgcd.u3 == gcd(xr1, xr2));
}

template <typename T>
T ipow(T x, int n) {
     T y = T(1);
     for (int i = 1; i <= n; i++)
          y *= x;
     return y;
}

BOOST_AUTO_TEST_CASE(ipower_test) {
     using SHG::ipower;
     for (int x = -10; x <= 10; x++) {
          for (int n = 0; n <= 5; n++) {
               int const p = ipower(x, n);
               int const p2 = ipow(x, n);
               BOOST_CHECK(p == p2);
          }
     }
     for (int x = -3; x <= 3; x++) {
          for (int n = 0; n <= 18; n++) {
               int const p = ipower(x, n);
               int const p2 = ipow(x, n);
               BOOST_CHECK(p == p2);
          }
     }
     for (int x = -2; x <= 2; x++) {
          for (int n = 0; n <= 30; n++) {
               int const p = ipower(x, n);
               int const p2 = ipow(x, n);
               BOOST_CHECK(p == p2);
          }
     }
     double const x = 1.001;
     for (int n = 0; n <= 1000; n++) {
          double const p = ipower(x, n);
          double const p2 = std::pow(x, n);
          BOOST_CHECK(SHG::faeq(p, p2, 8e-14));
     }
}

struct Mod_case {
     double const x;
     double const y;
     double const z;
};

std::ostream& operator<<(std::ostream& stream, Mod_case const& c) {
     stream.width(0);
     stream << c.x << "   " << c.y << "   " << c.z << '\n';
     return stream;
}

std::vector<Mod_case> const mod1_case{
     {-2.0, -2.0, 0.0},    {-1.99, -2.0, -1.99}, {-1.0, -2.0, -1.0},
     {-0.01, -2.0, -0.01}, {0.0, -2.0, 0.0},     {0.01, -2.0, -1.99},
     {1.0, -2.0, -1.0},    {1.99, -2.0, -0.01},  {2.0, -2.0, 0.0},

     {-2.0, 0.0, -2.0},    {-1.99, 0.0, -1.99},  {-1.0, 0.0, -1.0},
     {-0.01, 0.0, -0.01},  {0.0, 0.0, 0.0},      {0.01, 0.0, 0.01},
     {1.0, 0.0, 1.0},      {1.99, 0.0, 1.99},    {2.0, 0.0, 2.0},

     {-2.0, 2.0, 0.0},     {-1.99, 2.0, 0.01},   {-1.0, 2.0, 1.0},
     {-0.01, 2.0, 1.99},   {0.0, 2.0, 0.0},      {0.01, 2.0, 0.01},
     {1.0, 2.0, 1.0},      {1.99, 2.0, 1.99},    {2.0, 2.0, 0.0}};

BOOST_DATA_TEST_CASE(mod1_test, bdata::make(mod1_case), c) {
     using SHG::tolerance;
     BOOST_CHECK(SHG::faeq(SHG::mod1(c.x, c.y), c.z, tolerance));
}

BOOST_AUTO_TEST_CASE(round_test) {
     using SHG::round;
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
     using std::strcmp;
     using std::stringstream;

     struct S {
          char c[10];
          int i;
          double d;
     };

     S const s{"123456789", 13, 0.1};
     S t;
     stringstream ss(bininpout);
     SHG::write_binary(s, ss);
     ss.read(reinterpret_cast<char*>(&t), sizeof t);
     BOOST_CHECK(strcmp(s.c, t.c) == 0);
     BOOST_CHECK(s.i == t.i);
     BOOST_CHECK(s.d == t.d);
}

BOOST_AUTO_TEST_CASE(read_binary_test) {
     using std::strcmp;
     using std::stringstream;

     struct S {
          char c[10];
          int i;
          double d;
     };

     S const s{"123456789", 13, 0.1};
     S t;
     stringstream ss(bininpout);
     ss.write(reinterpret_cast<char const*>(&s), sizeof s);
     SHG::read_binary(t, ss);
     BOOST_CHECK(strcmp(s.c, t.c) == 0);
     BOOST_CHECK(s.i == t.i);
     BOOST_CHECK(s.d == t.d);
}

BOOST_AUTO_TEST_CASE(write_read_string_binary_test) {
     using std::stringstream;
     std::string const s0{"A\0B\0C\0", 6};
     BOOST_REQUIRE(s0.size() == 6);
     stringstream ss(bininpout);
     SHG::write_binary(s0, ss);
     BOOST_CHECK(ss.good());
     std::string s1;
     SHG::read_binary(s1, ss);
     BOOST_CHECK(ss.good());
     BOOST_CHECK(s1 == s0);
}

BOOST_AUTO_TEST_CASE(ltrim_test) {
     using SHG::ltrim;
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
     using SHG::rtrim;
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
     using SHG::trim;
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

struct Case {
     char const* const input;
     char const* const output;
};

std::ostream& operator<<(std::ostream& stream, Case const& c) {
     stream.width(0);
     stream << c.input << "   " << c.output << '\n';
     return stream;
}

std::vector<Case> const vc{
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

BOOST_DATA_TEST_CASE(strtrim_test, bdata::make(vc), c) {
     using SHG::strtrim;
     using std::strcmp;
     using std::strcpy;
     char s[10];
     BOOST_CHECK(!strcmp(strtrim(strcpy(s, c.input)), c.output));
}

BOOST_DATA_TEST_CASE(clean_string_test, bdata::make(vc), c) {
     std::string s;
     s = c.input;
     BOOST_CHECK(SHG::clean_string(s) == c.output);
}

std::vector<std::string> split_string1(
     std::string const& s,
     std::string const& sep = SHG::white_space) {
     return SHG::split(s, sep);
}

BOOST_AUTO_TEST_CASE(split_test) {
     using SHG::split_string;
     std::vector<std::string> v;

     v = split_string1("");
     BOOST_CHECK(v.size() == 0);
     v = split_string1(" \n\v \t");
     BOOST_CHECK(v.size() == 0);
     v = split_string1("abc");
     BOOST_CHECK(v.size() == 1 && v[0] == "abc");
     v = split_string1(" abc");
     BOOST_CHECK(v.size() == 1 && v[0] == "abc");
     v = split_string1("abc ");
     BOOST_CHECK(v.size() == 1 && v[0] == "abc");
     v = split_string1(" abc ");
     BOOST_CHECK(v.size() == 1 && v[0] == "abc");
     v = split_string1("a b c");
     BOOST_CHECK(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                 v[2] == "c");
     v = split_string1(" a b c");
     BOOST_CHECK(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                 v[2] == "c");
     v = split_string1("a b c ");
     BOOST_CHECK(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                 v[2] == "c");
     v = split_string1(" a b c ");
     BOOST_CHECK(v.size() == 3 && v[0] == "a" && v[1] == "b" &&
                 v[2] == "c");
     v = split_string1("", "");
     BOOST_CHECK(v.size() == 0);
     v = split_string1(" ", "");
     BOOST_CHECK(v.size() == 1 && v[0] == " ");
}

std::vector<std::string> split_string2(std::string const& s,
                                       std::string const& sep) {
     return SHG::split_string(s, sep);
}

BOOST_AUTO_TEST_CASE(split_string_test) {
     std::vector<std::string> v;
     v = split_string2("", "");
     BOOST_CHECK(v.size() == 1 && v[0] == "");
     v = split_string2("abc", "");
     BOOST_CHECK(v.size() == 1 && v[0] == "abc");
     v = split_string2("", "%%");
     BOOST_CHECK(v.size() == 1 && v[0] == "");
     v = split_string2("abc", "%%");
     BOOST_CHECK(v.size() == 1 && v[0] == "abc");
     v = split_string2("abc%%cde", "%%");
     BOOST_CHECK(v.size() == 2 && v[0] == "abc" && v[1] == "cde");
     v = split_string2("%%abc%%cde", "%%");
     BOOST_CHECK(v.size() == 3 && v[0] == "" && v[1] == "abc" &&
                 v[2] == "cde");
     v = split_string2("%%%%abc%%%%cde", "%%");
     BOOST_CHECK(v.size() == 5 && v[0] == "" && v[1] == "" &&
                 v[2] == "abc" && v[3] == "" && v[4] == "cde");
     v = split_string2("%%%%", "%%");
     BOOST_CHECK(v.size() == 3 && v[0] == "" && v[1] == "" &&
                 v[2] == "");
     v = split_string2("%%%", "%%");
     BOOST_CHECK(v.size() == 2 && v[0] == "" && v[1] == "%");
}

BOOST_AUTO_TEST_CASE(indirect_sort_test) {
     std::vector<double> const w{3.0, 4.0, 1.0, 5.0, 2.0};
     auto const v = SHG::indirect_sort(w);
     for (std::size_t i = 1; i < w.size(); i++)
          BOOST_CHECK(w[v[i - 1]] <= w[v[i]]);
}

BOOST_AUTO_TEST_CASE(strrtok_test) {
     using std::strcpy;
     using std::strtok;
     char s[100], s1[100], *last, *t, *t1;
     char const* const delim = " ";

     strcpy(s, "a ab abc abcd ");
     strcpy(s1, "a ab abc abcd ");
     t = SHG::strrtok(s, delim, &last);
     t1 = strtok(s1, delim);

     for (;;) {
          if (t == nullptr) {
               BOOST_CHECK(t1 == nullptr);
               break;
          }
          BOOST_CHECK(t1 != nullptr);
          BOOST_CHECK(strcmp(t, t1) == 0);
          t = SHG::strrtok(nullptr, delim, &last);
          t1 = strtok(nullptr, delim);
     }
}

BOOST_AUTO_TEST_CASE(strdup_test) {
     std::unique_ptr<char[]> s(SHG::strdup("abcdefgh"));
     if (s.get() != nullptr)
          BOOST_CHECK(!std::strcmp(s.get(), "abcdefgh"));
}

BOOST_AUTO_TEST_CASE(timer_test) {
     SHG::Timer t;
     double const t1 = t.elapsed();
     BOOST_CHECK(t1 >= 0.0);
     double const t2 = t.elapsed();
     BOOST_CHECK(t2 >= t1);
     t.restart();
     BOOST_CHECK(t.elapsed() >= 0.0);
}

BOOST_DATA_TEST_CASE(comblex_test,
                     bdata::xrange(9) * bdata::xrange(9), xr1, xr2) {
     using std::size_t;
     int const n = xr1 + 1;
     int const k = xr2 + 1;

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
     size_t const kk = k;
     SHG::Comblex c(n, k);
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
     long const tnr = fact(n) / fact(k) / fact(n - k);
     BOOST_CHECK(nr == tnr);
}

BOOST_AUTO_TEST_CASE(comblex_2_test) {
     using std::getline;

     std::stringstream ss;
     int const n = 4, k = 3;
     SHG::Comblex c(n, k);
     do {
          std::vector<int> const& v = c.get();
          for (int i = 0; i < k; i++)
               ss << v[i];
          ss << '\n';
     } while (c.next());
     std::string s;
     getline(ss, s);
     BOOST_CHECK(s == "012");
     getline(ss, s);
     BOOST_CHECK(s == "013");
     getline(ss, s);
     BOOST_CHECK(s == "023");
     getline(ss, s);
     BOOST_CHECK(s == "123");
     BOOST_CHECK(!getline(ss, s));
}

std::vector<std::vector<int>> const varlex_expected{
     {0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1},
     {2, 1}, {0, 2}, {1, 2}, {2, 2}};

BOOST_AUTO_TEST_CASE(varlex_test) {
     using SHG::Varlex;
     BOOST_CHECK_THROW(Varlex(0, 1), std::invalid_argument);
     BOOST_CHECK_THROW(Varlex(1, 0), std::invalid_argument);
     Varlex g1(1, 1);
     BOOST_REQUIRE(g1.get().size() == 1);
     BOOST_CHECK(g1.get()[0] == 0);
     BOOST_CHECK(!g1.next());

     Varlex g2(3, 2);
     unsigned k = 0;
     do {
          BOOST_REQUIRE(k < varlex_expected.size());
          BOOST_CHECK(g2.get() == varlex_expected[k]);
          k++;
     } while (g2.next());
     BOOST_CHECK(g2.get() == varlex_expected[0]);
}

BOOST_AUTO_TEST_CASE(output_operator_for_vectors_test) {
     using SHG::operator<<;
     std::vector<int> const v1{1, 2, 3};
     std::vector<int> const v2{};
     std::ostringstream buf;
     buf << v1;
     BOOST_CHECK(buf.str() == "0 1\n1 2\n2 3\n");
     buf.str("");
     buf << v2;
     BOOST_CHECK(buf.str() == "");
}

BOOST_AUTO_TEST_CASE(have_equal_content_test) {
     using SHG::have_equal_content;
     {
          std::vector<int> const v{1, 2, 3};
          std::vector<int> const w{2, 3, 1};
          BOOST_CHECK(have_equal_content(v, w));
     }
     {
          std::vector<int> const v{1, 2, 3};
          std::vector<int> const w{1, 2};
          BOOST_CHECK(!have_equal_content(v, w));
     }
     {
          std::vector<int> const v{};
          std::vector<int> const w{};
          BOOST_CHECK(have_equal_content(v, w));
     }
}

BOOST_AUTO_TEST_CASE(contains_test) {
     using SHG::contains;
     {
          std::vector<int> const v{1, 2, 3};
          BOOST_CHECK(contains(v, 1));
          BOOST_CHECK(contains(v, 2));
          BOOST_CHECK(contains(v, 3));
          BOOST_CHECK(!contains(v, 4));
     }
     {
          std::vector<int> const v{};
          BOOST_CHECK(!contains(v, 0));
     }
}

struct insert_test_case {
     std::vector<int> const before;
     std::vector<int>::size_type const pos;
     std::vector<int> const after;
};

insert_test_case const insert_test_data[] = {
     {{1, 2, 3}, 0, {4, 1, 2, 3}},
     {{1, 2, 3}, 1, {1, 4, 2, 3}},
     {{1, 2, 3}, 2, {1, 2, 4, 3}},
     {{1, 2, 3}, 3, {1, 2, 3, 4}},
     {{1, 2}, 0, {4, 1, 2}},
     {{1, 2}, 1, {1, 4, 2}},
     {{1, 2}, 2, {1, 2, 4}},
     {{1}, 0, {4, 1}},
     {{1}, 1, {1, 4}},
     {{}, 0, {4}}};

BOOST_AUTO_TEST_CASE(insert_test) {
     using SHG::insert;
     for (std::size_t i = 0; i < std::size(insert_test_data); i++) {
          auto const& d = insert_test_data[i];
          auto b = d.before;
          insert(b, d.pos, 4);
          BOOST_CHECK(b == d.after);
     }
     std::vector<int> a{1, 2, 3};
     BOOST_CHECK_THROW(insert(a, 4, 4), std::out_of_range);
     a = {};
     BOOST_CHECK_THROW(insert(a, 1, 4), std::out_of_range);
}

struct Rd_test_struct {
     std::string s;
     int i;
};

bool operator==(Rd_test_struct const& a, Rd_test_struct const& b) {
     return a.s == b.s && a.i == b.i;
}

BOOST_AUTO_TEST_CASE(remove_duplicates_test) {
     std::vector<Rd_test_struct> v1 = {
          {"a", 1},
          {"b", 2},
          {"a", 1},
     };
     std::vector<Rd_test_struct> const v2 = {
          {"a", 1},
          {"b", 2},
     };
     SHG::remove_duplicates(v1);
     BOOST_CHECK(v1 == v2);
}

constexpr char const* const to_octal_test_res0 =
     "\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"
     "\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036"
     "\037 "
     "!\"#$%&'()*+,-./"
     "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
     "abcdefghijklmnopqrstuvwxyz{|}~"
     "\177\\200\\201\\202\\203\\204\\205\\206\\207\\210\\211\\212\\21"
     "3\\214\\215\\216\\217\\220\\221\\222\\223\\224\\225\\226\\227\\"
     "230\\231\\232\\233\\234\\235\\236\\237\\240\\241\\242\\243\\244"
     "\\245\\246\\247\\250\\251\\252\\253\\254\\255\\256\\257\\260\\2"
     "61\\262\\263\\264\\265\\266\\267\\270\\271\\272\\273\\274\\275"
     "\\276\\277\\300\\301\\302\\303\\304\\305\\306\\307\\310\\311\\3"
     "12\\313\\314\\315\\316\\317\\320\\321\\322\\323\\324\\325\\326"
     "\\327\\330\\331\\332\\333\\334\\335\\336\\337\\340\\341\\342\\3"
     "43\\344\\345\\346\\347\\350\\351\\352\\353\\354\\355\\356\\357"
     "\\360\\361\\362\\363\\364\\365\\366\\367\\370\\371\\372\\373\\3"
     "74\\375\\376\\377";

BOOST_AUTO_TEST_CASE(to_octal_test) {
     std::string res{to_octal_test_res0};
     res.insert(0, 1, '\0');
     BOOST_REQUIRE(res.size() == 640);

     std::string s;
     for (int i = 0; i < 256; i++)
          s += static_cast<char>(i);
     auto t = SHG::to_octal(s);
     BOOST_CHECK(t == res);
}

constexpr char const* const html = R"(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,
initial-scale=1.0">
<title>Title of the page</title>
<link rel="stylesheet" type="text/css" href="default.css">
</head>
<body>
<h1>Title of the page</h1>
<address>
<img src="image.jpg" width="128" height="256"
alt="A photograph" title="Title of the image"><br>
</address>
<h2>Subtitle 1</h2>
<ul>
<li>The<!-- comment -->first position&nbsp;&quot;&amp;&quot;
<li><a href="http://xxx.com" target="_blank">
The second <![CDATA[<x>y</x>]]> position</a>
</ul>
<h2>Subtitle 2</h2>
<p>The paragraph.</p>
<hr>
<p class="footer">This is the footer<br></p>
</body>
</html>
)";

constexpr char const* const html_result =
     "\n"
     "               \n"
     "                \n"
     "      \n"
     "                      \n"
     "                                                  \n"
     "                   \n"
     "       Title of the page        \n"
     "                                                          \n"
     "       \n"
     "      \n"
     "    Title of the page     \n"
     "         \n"
     "                                             \n"
     "                                                  \n"
     "          \n"
     "    Subtitle 1     \n"
     "    \n"
     "    The                first position \"&\"\n"
     "                                             \n"
     "The second                      position    \n"
     "     \n"
     "    Subtitle 2     \n"
     "   The paragraph.    \n"
     "    \n"
     "                  This is the footer        \n"
     "       \n"
     "       \n";

BOOST_AUTO_TEST_CASE(dehtml_test) {
     std::istringstream iss(bininp);
     std::ostringstream oss(binout);
     iss.str(html);
     BOOST_CHECK(SHG::dehtml(iss, oss));
     BOOST_CHECK(std::strcmp(oss.str().data(), html_result) == 0);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
