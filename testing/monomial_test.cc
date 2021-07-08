#include <shg/monomial.h>
#include <numeric>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(monomial_test)

using namespace SHG::ALGEBRA;
using std::invalid_argument;

BOOST_AUTO_TEST_CASE(monomial_test) {
     BOOST_CHECK_THROW(Monomial m(-1), invalid_argument);
     BOOST_CHECK_NO_THROW(Monomial m(0));

     for (int n = 1; n <= 5; n++) {
          Monomial m(n);
          BOOST_CHECK(m.dim() == n);
          BOOST_CHECK(m.deg() == 0);
          for (int i = 0; i < n; i++) {
               BOOST_CHECK(m[i] == 0);
               BOOST_CHECK(m.deg(i) == 0);
          }
          Monomial m1{m};
          BOOST_CHECK(m == m1);
          BOOST_CHECK(!(m != m1));
     }

     for (int n = 1; n <= 5; n++) {
          std::vector<int> d;
          for (int i = 0; i < n; i++)
               d.push_back(i + 1);
          const int td = std::accumulate(d.cbegin(), d.cend(), 0);
          Monomial m{d};
          BOOST_CHECK(m.dim() == n);
          for (int i = 0; i < m.dim(); i++) {
               BOOST_CHECK(m[i] == d[i]);
               BOOST_CHECK(m.deg(i) == d[i]);
          }
          BOOST_CHECK(m.deg() == td);
          Monomial m1{m};
          BOOST_CHECK(m == m1);
          BOOST_CHECK(!(m != m1));
     }

     BOOST_CHECK_THROW(Monomial({-1}), invalid_argument);
     BOOST_CHECK_THROW(Monomial({0, -1}), invalid_argument);
     BOOST_CHECK_THROW(Monomial({-1, 0}), invalid_argument);

     {
          Monomial m(2);
          m = Monomial{0, 1, 2};
          BOOST_CHECK(m.dim() == 3);
          for (int i = 0; i < m.dim(); i++) {
               BOOST_CHECK(m[i] == i);
               BOOST_CHECK(m.deg(i) == i);
          }
     }

     {
          BOOST_CHECK((Monomial{1, 2, 3}.divides(Monomial{2, 3, 4})));
          BOOST_CHECK((Monomial{1, 2, 3}.divides(Monomial{1, 2, 3})));
          BOOST_CHECK(
               (!Monomial{1, 2, 3}.divides(Monomial{1, 2, 1})));
          BOOST_CHECK_THROW(
               (Monomial{1, 2, 3}.divides(Monomial{2, 3})),
               invalid_argument);
          BOOST_CHECK_THROW((Monomial{}.divides(Monomial{2, 3})),
                            invalid_argument);
          BOOST_CHECK_THROW((Monomial{1, 2, 3}.divides(Monomial{})),
                            invalid_argument);
          BOOST_CHECK_THROW((Monomial{}.divides(Monomial{})),
                            invalid_argument);
          BOOST_CHECK_THROW((Monomial().divides(Monomial())),
                            invalid_argument);
     }

     {
          Monomial m1{1, 2, 3};
          Monomial m2{0, 1, 2};
          Monomial m3{1, 3, 5};
          BOOST_CHECK(m1 * m2 == m3);
          BOOST_CHECK(m3 / m1 == m2);
          BOOST_CHECK(m3 / m2 == m1);
          BOOST_CHECK(m2 * m1 == m3);
          BOOST_CHECK(m3 / m1 == m2);
          BOOST_CHECK(m3 / m2 == m1);
          BOOST_CHECK_THROW(m2 / m1, invalid_argument);
          Monomial m4{1, 2};
          BOOST_CHECK_THROW(m1 * m4, invalid_argument);
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-comparison"
#endif
          BOOST_CHECK_NO_THROW(m1 != m4);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
     }

     {
          BOOST_CHECK(
               gcd(Monomial({0, 1, 0, 1}), Monomial({1, 0, 1, 0})) ==
               Monomial({0, 0, 0, 0}));
          BOOST_CHECK(gcd(Monomial({1, 2, 3}), Monomial({3, 1, 4})) ==
                      Monomial({1, 1, 3}));
          BOOST_CHECK(gcd(Monomial({3, 1, 4}), Monomial({1, 2, 3})) ==
                      Monomial({1, 1, 3}));
          BOOST_CHECK_THROW(gcd(Monomial(), Monomial()),
                            invalid_argument);
          BOOST_CHECK_THROW(gcd(Monomial(), Monomial({1, 2, 3})),
                            invalid_argument);
          BOOST_CHECK_THROW(
               gcd(Monomial({1, 2}), Monomial({1, 2, 3})),
               invalid_argument);
     }

     {
          BOOST_CHECK(
               lcm(Monomial({0, 1, 0, 1}), Monomial({1, 0, 1, 0})) ==
               Monomial({1, 1, 1, 1}));
          BOOST_CHECK(lcm(Monomial({1, 2, 3}), Monomial({3, 1, 4})) ==
                      Monomial({3, 2, 4}));
          BOOST_CHECK(lcm(Monomial({3, 1, 4}), Monomial({1, 2, 3})) ==
                      Monomial({3, 2, 4}));
          BOOST_CHECK_THROW(lcm(Monomial(), Monomial()),
                            invalid_argument);
          BOOST_CHECK_THROW(lcm(Monomial(), Monomial({1, 2, 3})),
                            invalid_argument);
          BOOST_CHECK_THROW(
               lcm(Monomial({1, 2}), Monomial({1, 2, 3})),
               invalid_argument);
     }
}

constexpr const char* const result = R"(
 000
 001 010 100
 002 011 020 101 110 200
 003 012 021 030 102 111 120 201 210 300
 004 013 022 031 040 103 112 121 130 202 211 220 301 310 400
)";

BOOST_AUTO_TEST_CASE(recursive_monomial_generator_test) {
     std::ostringstream buf;
     auto fun = [&buf](const std::vector<int>& a) {
          buf << ' ';
          for (std::vector<int>::size_type i = 0; i < a.size(); i++)
               buf << a[i];
     };
     const int n = 3;

     buf << '\n';
     for (int k = 0; k < 5; k++) {
          Recursive_monomial_generator g(n, k, fun);
          g.generate();
          buf << '\n';
     }
     BOOST_CHECK(buf.str() == result);
}

BOOST_AUTO_TEST_CASE(iterative_monomial_generator_test) {
     BOOST_CHECK_THROW(Iterative_monomial_generator g(0, 2),
                       Subdomain_error<int>);
     BOOST_CHECK_THROW(Iterative_monomial_generator g(3, -1),
                       Subdomain_error<int>);

     std::ostringstream buf;
     const int n = 3;

     buf << '\n';
     for (int k = 0; k < 5; k++) {
          Iterative_monomial_generator g(n, k);
          do {
               buf << ' ';
               const std::vector<int>& a = g.get();
               for (std::vector<int>::size_type i = 0; i < a.size();
                    i++)
                    buf << a[i];
          } while (g.next());
          buf << '\n';
     }
     BOOST_CHECK(buf.str() == result);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
