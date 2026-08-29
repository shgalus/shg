#include <shg/experiments.h>
#include <boost/multiprecision/integer.hpp>
#include <shg/utils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(experiments_test)

struct S {
     S() { default_constructor++; }
     S(const S& other) : x(other.x), y(other.y) {
          copy_constructor++;
     }
     S(S&& other) noexcept
          : x(std::move(other.x)), y(std::move(other.y)) {
          move_constructor++;
     }

     S& operator=(const S& other) {
          x = other.x;
          y = other.y;
          copy_assignment++;
          return *this;
     }
     S& operator=(S&& other) noexcept {
          x = std::move(other.x);
          y = std::move(other.y);
          move_assignment++;
          return *this;
     }

     int x{};
     double y{};

     static int default_constructor;
     static int copy_constructor;
     static int move_constructor;
     static int copy_assignment;
     static int move_assignment;
};

int S::default_constructor = 0;
int S::copy_constructor = 0;
int S::move_constructor = 0;
int S::copy_assignment = 0;
int S::move_assignment = 0;

S fun() {
     S s;
     s.x = 1;
     s.y = 2.0;
     return s;
}

BOOST_AUTO_TEST_CASE(move_test) {
     S t;
     BOOST_CHECK(S::default_constructor == 1);
     t = fun();
     BOOST_CHECK(S::default_constructor == 2);
     BOOST_CHECK(S::move_assignment == 1);
     auto s = fun();
     BOOST_CHECK(s.x == 1);

     BOOST_CHECK(S::default_constructor == 3);
     BOOST_CHECK(S::copy_constructor == 0);
     BOOST_CHECK(S::move_constructor == 0);
     BOOST_CHECK(S::copy_assignment == 0);
     BOOST_CHECK(S::move_assignment == 1);
}

/*
 * The great philosophers were told that they would be given two
 * natural numbers greater than 1 and less than 100. Socrates only
 * knew the sum of these numbers, while Plato only knew their product.
 *
 * The philosophers then engaged in the following dialogue:
 *
 * PLATO: I don't know what these numbers are.
 * SOCRATES: I knew you wouldn't know, but I don't know either.
 * PLATO: Well, now I know what these numbers are.
 * SOCRATES: Well, now I know what these numbers are.
 *
 * What are these numbers?
 *
 * https://alfa.im.pwr.edu.pl/~kisielewicz/Najtrudniejsze%20
 * zagadki.pdf
 *
 * 30 December 2025
 */

// unordered pair of integers
struct Pair {
     Pair() = default;
     Pair(int u, int v) : u(u), v(v) {}
     int u{0};
     int v{0};
};

using Decomposition = std::vector<Pair>;

// Returns all factorizations of n into two factors.
Decomposition factorize(int n) {
     using boost::multiprecision::sqrt;
     assert(4 <= n && n <= 9801);
     Decomposition d;
     int rest;
     int const root = sqrt(n, rest);
     assert(rest >= 0);
     for (int u = 2; u <= root; u++)
          if (n % u == 0) {
               assert(n / u >= 2);
               if (n / u <= 99)
                    d.push_back(Pair(u, n / u));
          }
     return d;
}

// Returns all partitions of n into two summands.
Decomposition partition(int n) {
     assert(4 <= n && n <= 198);
     Decomposition d;
     for (int u = 2; u <= n / 2; u++) {
          assert(n - u >= 2);
          if (n - u <= 99)
               d.push_back(Pair(u, n - u));
     }
     return d;
}

BOOST_AUTO_TEST_CASE(plato_and_socrates_test) {
     std::vector<int> S;
     for (int s = 4; s <= 198; s++) {
          auto const D = partition(s);
          if (D.size() > 1) {  // ...but I don't know either.
               bool ok = true;
               for (auto const& d : D) {
                    if (factorize(d.u * d.v).size() < 2) {
                         ok = false;  // I knew you wouldn't know...
                         break;
                    }
               }
               if (ok)
                    S.push_back(s);
          }
     }
     // Here S contains all sums with ambiguous factorization.
     BOOST_CHECK(S == std::vector<int>(
                           {11, 17, 23, 27, 29, 35, 37, 41, 47, 53}));
     for (auto s : S) {
          auto const D = partition(s);
          int n = 0;
          for (auto const& d : D) {
               auto const P = factorize(d.u * d.v);
               int k = 0;
               for (auto const& p : P)
                    if (SHG::contains(S, p.u + p.v))
                         k++;
               if (k == 1)
                    n++;
          }
          if (n == 1) {  // Plato knows because it is the only case
               BOOST_CHECK(s == 17);
               for (auto const& d : D) {
                    auto const P = factorize(d.u * d.v);
                    int k = 0;
                    for (auto const& p : P)
                         if (SHG::contains(S, p.u + p.v))
                              k++;
                    if (k == 1)
                         BOOST_CHECK(d.u == 4 && d.v == 13);
               }
          }
     }
}

BOOST_AUTO_TEST_CASE(basic_test) {
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
