#include <shg/term.h>
#include <shg/utils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(term_test)

BOOST_AUTO_TEST_CASE(basic_test) {
     using SHG::to_string;
     using SHG::ALGEBRA::Element;
     using SHG::ALGEBRA::Term;
     using SHG::ALGEBRA::Monomial;
     using SHG::ALGEBRA::Ring_Z;
     using SHG::ALGEBRA::Field_Q;

     Ring_Z const z;
     Term const t1;
     Term const t2(&z);
     Element const a(&z);
     Monomial const m{1, 2, 3};
     Term const t3(a, m);

     BOOST_CHECK(!t1.a().is_valid());
     BOOST_CHECK(t1.m().dim() == 0);
     BOOST_CHECK(t1.as() == nullptr);
     BOOST_CHECK_THROW(to_string(t1), std::runtime_error);
     BOOST_CHECK_THROW(to_string(t1.a()), std::runtime_error);
     BOOST_CHECK(to_string(t1.m()) == "0");

     BOOST_CHECK(t2.a().is_valid());
     BOOST_CHECK(t2.m().dim() == 0);
     BOOST_CHECK(t2.as() == &z);
     BOOST_CHECK(to_string(t2) == "0 0");
     BOOST_CHECK(to_string(t2.a()) == "0");
     BOOST_CHECK(to_string(t2.m()) == "0");

     BOOST_CHECK(t3.a().is_valid());
     BOOST_CHECK(t3.m().dim() == 3);
     BOOST_CHECK(t3.as() == &z);
     BOOST_CHECK(to_string(t3) == "0 3 1 2 3");
     BOOST_CHECK(to_string(t3.a()) == "0");
     BOOST_CHECK(to_string(t3.m()) == "3 1 2 3");

     bool b;
     BOOST_CHECK_THROW((b = t1 == t1), std::invalid_argument);
     BOOST_CHECK_THROW((b = t1 != t2), std::invalid_argument);
     BOOST_CHECK_THROW((b = t1 != t3), std::invalid_argument);
     BOOST_CHECK_THROW((b = t2 != t1), std::invalid_argument);
     BOOST_CHECK(b = t2 == t2);
     BOOST_CHECK(b = t2 != t3);
     BOOST_CHECK_THROW((b = t3 != t1), std::invalid_argument);
     BOOST_CHECK(b = t3 != t2);
     BOOST_CHECK(b = t3 == t3);

     Field_Q const Q;
     Term const t4(&Q);
     BOOST_CHECK(to_string(t4) == "0 0");

     Term const t5{Q.element(-3, 4), Monomial{1, 2, 1}};
     BOOST_CHECK(to_string(t5) == "-3/4 3 1 2 1");
     BOOST_CHECK(t4 != t5);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
