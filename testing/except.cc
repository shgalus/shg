/* except.cc: testing exception class and auxiliary objects */

/**
 * \file testing/except.cc
 * Testing exception class and auxiliary objects.
 * \date Created on 22 July 2010.
 */

#include <cstring>
#include <sstream>
#include "shg/except.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::string;
using std::strncmp;
using SHG::Exception;
using SHG::Assertion;

namespace {

void test1() {
     using std::ostringstream;

     struct E1 : public virtual Exception {
          E1() : Exception(string("E1")) {}
     };
     struct E2 : public virtual Exception {
          E2() : Exception(string("E2")) {}
     };
     struct E3 : public virtual E1, public virtual E2 {
          E3() : Exception(string("E3")) {}
     };

     try {
          throw E3();
     }
     catch (const Exception& e) {
          ostringstream s;
          e.print("test2", s);
          SHG_ASSERT(s.str() == string("test2: E3\n"));
     }
}

void test2(bool b) {
     if (b)
          SHG_ASSERT(b);
     else
          SHG_ASSERT(!b);
     try {
          SHG_ASSERT(!b);
     }
     catch (const SHG::Exception) {}
}

// Test copy constructor and assignment operator.
void test3() {
     Exception a("error"), b(a), c;
     c = a;
     SHG_ASSERT(strcmp(a.what(), b.what()) == 0);
     SHG_ASSERT(strcmp(a.what(), c.what()) == 0);
}

void test_except1() {
     using SHG::Exception;
     using std::strcmp;
     using std::stringstream;
     const std::string s0 = "SHG::Exception";
     const std::string s1 = "alpha";
     const char* const progname = "progname";
     stringstream ss;

     Exception e1, e2(s1), e3(s1.c_str()), e4(nullptr), e5(e3), e6;
     e6 = e5;
     SHG_ASSERT(!strcmp(e1.what(), s0.c_str()));
     SHG_ASSERT(!strcmp(e2.what(), s1.c_str()));
     SHG_ASSERT(!strcmp(e3.what(), s1.c_str()));
     SHG_ASSERT(!strcmp(e4.what(), ""));
     SHG_ASSERT(!strcmp(e5.what(), e3.what()));
     SHG_ASSERT(!strcmp(e6.what(), e5.what()));

     e1.print(nullptr, ss);
     SHG_ASSERT(ss.str() == s0 + '\n');
     ss.str("");
     e1.print(progname, ss);
     SHG_ASSERT(ss.str() == progname + (": " + s0) + '\n');
     ss.str("");
     e2.print(nullptr, ss);
     SHG_ASSERT(ss.str() == s1 + '\n');
     ss.str("");
     e2.print(progname, ss);
     SHG_ASSERT(ss.str() == progname + (": " + s1) + '\n');
}

void test_assertion() {
     using SHG::Assertion;
     using std::stringstream;
     const char* const f = "assertion failed";
     const char* const file = "file";
     const int line = 100;
     Assertion a1(file, line), a2(a1), a3(nullptr, 0);
     SHG_ASSERT(!strcmp(a1.what(), f) &&
                !strcmp(a1.file(), file) && a1.line() == line);
     SHG_ASSERT(!strcmp(a1.what(), a2.what()) &&
                !strcmp(a1.file(), a2.file()) &&
                a1.line() == a2.line());

     a3 = a1;
     SHG_ASSERT(!strcmp(a1.what(), a3.what()) &&
                !strcmp(a1.file(), a3.file()) &&
                a1.line() == a3.line());

     stringstream ss;

     a1.print(nullptr, ss);

     SHG_ASSERT(ss.str() ==
                "assertion failed in file file, line 100\n");
     ss.str("");
     a1.print("program_name", ss);

     SHG_ASSERT(ss.str() ==
                "program_name: assertion failed in file file,"
                " line 100\n");

     Assertion a(nullptr, 100);
     ss.str("");
     a.print(nullptr, ss);
     SHG_ASSERT(ss.str() == "assertion failed\n");
     ss.str("");
     a.print("program_name", ss);
     SHG_ASSERT(ss.str() == "program_name: assertion failed\n");
     try {
          SHG_ASSERT(false);
          std::abort();
     } catch (const SHG::Assertion&) {}
}

void test_invalid_argument(double x) {
     SHG_VALIDATE(x > 0.0);
}

}       // anonymous namespace

void test_except() {
     test1();
     test2(true);
     test3();
     test_except1();
     test_assertion();
     test_invalid_argument(3.0);
     bool ok = false;
     try {
          test_invalid_argument(-3.0);
     }
     catch (SHG::Invalid_argument) {
          ok = true;
     }
     SHG_ASSERT(ok);
}

}       // namespace Testing
}       // namespace SHG
