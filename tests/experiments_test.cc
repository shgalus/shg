#include <shg/experiments.h>
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

BOOST_AUTO_TEST_CASE(basic_test) {
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
