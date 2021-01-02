#include <shg/brent.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(brent_test)

double fun(double x) {
     return x * x - 2.0;
}

BOOST_AUTO_TEST_CASE(function_test) {
     double x = brent(fun, 1.0, 2.0, 1e-12);
     BOOST_CHECK(faeq(x, std::sqrt(2.0), 1e-12));
     x = brent(fun, -2.0, -1.0, 1e-12);
     BOOST_CHECK(faeq(x, -std::sqrt(2.0), 1e-12));
}

struct Fun {
     double operator()(double x) { return x * x - 4.0; }
};

BOOST_AUTO_TEST_CASE(function_object_test) {
     Fun fun;
     double x = brent(fun, 1.0, 2.0, 1e-12);
     BOOST_CHECK(faeq(x, 2.0, 1e-12));
     x = brent(fun, 2.0, 3.0, 1e-12);
     BOOST_CHECK(faeq(x, 2.0, 1e-12));
     x = brent(fun, -2.0, 2.0, 1e-12);
     BOOST_CHECK(faeq(x, 2.0, 1e-12));
}

BOOST_AUTO_TEST_CASE(lambda_test) {
     auto fun = [](double x) { return x * x - 4.0; };
     double x = brent(fun, 1.0, 2.0, 1e-12);
     BOOST_CHECK(faeq(x, 2.0, 1e-12));
     x = brent(fun, 2.0, 3.0, 1e-12);
     BOOST_CHECK(faeq(x, 2.0, 1e-12));
     x = brent(fun, -2.0, 2.0, 1e-12);
     BOOST_CHECK(faeq(x, 2.0, 1e-12));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
