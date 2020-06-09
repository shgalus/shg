#include "shg/except.h"
#include <cstring>
#include <sstream>
#include <string>
#include <boost/test/unit_test.hpp>

namespace SHG::BTesting {

BOOST_AUTO_TEST_SUITE(except_test)

BOOST_AUTO_TEST_CASE(multiple_bases_test) {
     struct E1 : public virtual Exception {
          E1() : Exception("E1") {}
     };
     struct E2 : public virtual Exception {
          E2() : Exception("E2") {}
     };
     struct E3 : public virtual E1, public virtual E2 {
          E3() : Exception("E3") {}
     };

     std::ostringstream s;

     try {
          throw E3();
     } catch (const Exception& e) {
          e.print("test2", s);
     }
     BOOST_CHECK(s.str() == "test2: E3\n");
}

BOOST_AUTO_TEST_CASE(shg_assert_test) {
     BOOST_CHECK_NO_THROW(SHG_ASSERT(true));
     BOOST_CHECK_THROW(SHG_ASSERT(false), Assertion);
}

BOOST_AUTO_TEST_CASE(copy_assignment_test) {
     Exception a("error"), b(a), c;
     c = a;
     BOOST_CHECK(std::strcmp(a.what(), b.what()) == 0);
     BOOST_CHECK(std::strcmp(a.what(), c.what()) == 0);
}

BOOST_AUTO_TEST_CASE(output_test) {
     const std::string s0 = "SHG::Exception";
     const std::string s1 = "alpha";
     const char* const progname = "progname";
     std::stringstream ss;
     Exception e1, e2(s1), e3(s1.c_str()), e4(nullptr), e5(e3), e6;
     e6 = e5;

     BOOST_CHECK(!std::strcmp(e1.what(), s0.c_str()));
     BOOST_CHECK(!std::strcmp(e2.what(), s1.c_str()));
     BOOST_CHECK(!std::strcmp(e3.what(), s1.c_str()));
     BOOST_CHECK(!std::strcmp(e4.what(), ""));
     BOOST_CHECK(!std::strcmp(e5.what(), e3.what()));
     BOOST_CHECK(!std::strcmp(e6.what(), e5.what()));

     e1.print(nullptr, ss);
     BOOST_CHECK(ss.str() == s0 + '\n');
     ss.str("");
     e1.print(progname, ss);
     BOOST_CHECK(ss.str() == progname + (": " + s0) + '\n');
     ss.str("");
     e2.print(nullptr, ss);
     BOOST_CHECK(ss.str() == s1 + '\n');
     ss.str("");
     e2.print(progname, ss);
     BOOST_CHECK(ss.str() == progname + (": " + s1) + '\n');
}

BOOST_AUTO_TEST_CASE(assertion_test) {
     const char* const f = "assertion failed";
     const char* const file = "file";
     const int line = 100;
     Assertion a1(file, line), a2(a1), a3(nullptr, 0);
     BOOST_CHECK(!std::strcmp(a1.what(), f) &&
                 !std::strcmp(a1.file(), file) && a1.line() == line);
     BOOST_CHECK(!std::strcmp(a1.what(), a2.what()) &&
                 !std::strcmp(a1.file(), a2.file()) &&
                 a1.line() == a2.line());

     a3 = a1;
     BOOST_CHECK(!std::strcmp(a1.what(), a3.what()) &&
                 !std::strcmp(a1.file(), a3.file()) &&
                 a1.line() == a3.line());

     std::stringstream ss;

     a1.print(nullptr, ss);
     BOOST_CHECK(ss.str() ==
                 "assertion failed in file file, line 100\n");

     ss.str("");
     a1.print("program_name", ss);
     BOOST_CHECK(ss.str() ==
                 "program_name: assertion failed in file file,"
                 " line 100\n");

     Assertion a(nullptr, 100);
     ss.str("");
     a.print(nullptr, ss);
     BOOST_CHECK(ss.str() == "assertion failed\n");

     ss.str("");
     a.print("program_name", ss);
     BOOST_CHECK(ss.str() == "program_name: assertion failed\n");
}

BOOST_AUTO_TEST_CASE(invalid_argument_test) {
     BOOST_CHECK_NO_THROW(SHG_VALIDATE(1 > 0));
     BOOST_CHECK_THROW(SHG_VALIDATE(1 < 0), Invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting
