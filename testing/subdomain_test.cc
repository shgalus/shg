#include <shg/subdomain.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(subdomain_test)

BOOST_AUTO_TEST_CASE(basic_test) {
     int n;
     n = Positive<int>(1);
     BOOST_CHECK(n == 1);
     BOOST_CHECK_THROW(n = Positive<int>(-1), Subdomain_error<int>);
     BOOST_CHECK_THROW(n = Positive<int>(0), Subdomain_error<int>);
     n = Nonnegative<int>(1);
     BOOST_CHECK(n == 1);
     n = Nonnegative<int>(0);
     BOOST_CHECK(n == 0);
     BOOST_CHECK_THROW(n = Nonnegative<int>(-1),
                       Subdomain_error<int>);
     n = Prime<int>(2);
     n = Prime<int>(3);
     n = Prime<int>(5);
     BOOST_CHECK_THROW(n = Prime<int>(-1), Subdomain_error<int>);
     BOOST_CHECK_THROW(n = Prime<int>(0), Subdomain_error<int>);
     BOOST_CHECK_THROW(n = Prime<int>(1), Subdomain_error<int>);
     BOOST_CHECK_THROW(n = Prime<int>(4), Subdomain_error<int>);
     BOOST_CHECK_THROW(n = Prime<int>(6), Subdomain_error<int>);
}

BOOST_AUTO_TEST_CASE(function_test) {
     auto fun1 = [](Positive<int> x) { BOOST_CHECK(x > 0); };
     auto fun2 = [](Nonnegative<double> x) { BOOST_CHECK(x >= 0); };
     fun1(1);
     BOOST_CHECK_THROW(fun1(0), Subdomain_error<int>);
     BOOST_CHECK_THROW(fun1(-1), Subdomain_error<int>);
     fun2(1.0);
     fun2(0.0);
     BOOST_CHECK_THROW(fun2(-1.0), Subdomain_error<double>);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
