#include <shg/utils.h>
#include <cstring>
#include <ios>
#include <limits>
#include <memory>
#include <sstream>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(utils_test)

namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_CASE(ignore_unused_variable_test) {
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

BOOST_DATA_TEST_CASE(is_prime_test, bdata::xrange(1000), xr) {
     const int n = xr;
     bool is_prime1 = n > 1;
     if (is_prime1) {
          for (int i = 2; i * i <= n; i++)
               if (n % i == 0) {
                    is_prime1 = false;
                    break;
               }
     }
     BOOST_CHECK(is_prime1 == is_prime(n));
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

/**
 * Basic test. See \cite knuth-2002b, section 4.5.2, page 367.
 */
BOOST_AUTO_TEST_CASE(extended_gcd_basic_test) {
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

BOOST_AUTO_TEST_CASE(pow_test) {
     for (int x = -10; x <= 10; x++) {
          for (int n = 0; n <= 5; n++) {
               const int p = pow(x, n);
               const int p2 = ipow(x, n);
               BOOST_CHECK(p == p2);
          }
     }
     for (int x = -3; x <= 3; x++) {
          for (int n = 0; n <= 18; n++) {
               const int p = pow(x, n);
               const int p2 = ipow(x, n);
               BOOST_CHECK(p == p2);
          }
     }
     for (int x = -2; x <= 2; x++) {
          for (int n = 0; n <= 31; n++) {
               const int p = pow(x, n);
               const int p2 = ipow(x, n);
               BOOST_CHECK(p == p2);
          }
     }
     const double x = 1.001;
     for (int n = 0; n <= 1000; n++) {
          const double p = pow(x, n);
          const double p2 = std::pow(x, n);
          BOOST_CHECK(faeq(p, p2, 8e-14));
     }
}

struct Mod_case {
     const double x;
     const double y;
     const double z;
};

std::ostream& operator<<(std::ostream& stream, const Mod_case& c) {
     stream.width(0);
     stream << c.x << "   " << c.y << "   " << c.z << '\n';
     return stream;
}

const std::vector<Mod_case> mod1_case{
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
     BOOST_CHECK(faeq(mod1(c.x, c.y), c.z, tolerance));
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
     std::unique_ptr<char[]> s(strdup("abcdefgh"));
     if (s.get() != nullptr)
          BOOST_CHECK(!std::strcmp(s.get(), "abcdefgh"));
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

BOOST_AUTO_TEST_CASE(comblex_2_test) {
     using std::getline;

     std::stringstream ss;
     const int n = 4, k = 3;
     Comblex c(n, k);
     do {
          const std::vector<int>& v = c.get();
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

BOOST_AUTO_TEST_CASE(output_operator_for_vectors_test) {
     using SHG::operator<<;
     const std::vector<int> v1{1, 2, 3};
     const std::vector<int> v2{};
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
          const std::vector<int> v{1, 2, 3};
          const std::vector<int> w{2, 3, 1};
          BOOST_CHECK(have_equal_content(v, w));
     }
     {
          const std::vector<int> v{1, 2, 3};
          const std::vector<int> w{1, 2};
          BOOST_CHECK(!have_equal_content(v, w));
     }
     {
          const std::vector<int> v{};
          const std::vector<int> w{};
          BOOST_CHECK(have_equal_content(v, w));
     }
}

BOOST_AUTO_TEST_CASE(contains_test) {
     using SHG::have_equal_content;
     {
          const std::vector<int> v{1, 2, 3};
          BOOST_CHECK(contains(v, 1));
          BOOST_CHECK(contains(v, 2));
          BOOST_CHECK(contains(v, 3));
          BOOST_CHECK(!contains(v, 4));
     }
     {
          const std::vector<int> v{};
          BOOST_CHECK(!contains(v, 0));
     }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
