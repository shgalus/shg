/* testshg.cc: testing SHG library */

/**
 * \file testing/testshg.cc
 * Testing SHG library.
 * \date Created on 20 August 2009.
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#ifdef MCHECK
extern "C" {
#include <mcheck.h>
}
#endif
#include "shg/ieee.h"
#include "shg/utils.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

const char* const prgname {"testshg"};
std::ofstream logfile;

namespace {

std::string center(const std::string& s, std::size_t n) {
     std::string t(s);
     const std::string::size_type l = SHG::clean_string(t).size();
     if (n <= l)
          return t;
     return std::string((n - l) / 2, ' ') + t;
}

}       // anonymous namespace

void header(const std::string& s) {
     logfile << std::string(72, '=') << '\n';
     logfile << center(s, 72) << '\n';
     logfile << std::string(72, '=') << '\n';
}

void footer(const std::string& s) {
     logfile << std::string(72, '=') << '\n';
     logfile << center("end of " + s, 72) << '\n';
     logfile << std::string(72, '=') << '\n';
}

class Test_shg {
public:
     Test_shg();
     void run(int argc, char* argv[]);
private:
     typedef void (*const Test_function)();
     struct Module {
          Module(Test_function testfun, const std::string& name);
          const Test_function testfun;
          const std::string name;
          bool do_test;
     };
     std::vector<Module> mm;
     void register_test(Test_function testfun,
                        const std::string& name);
     void do_testing();
};

}       // namespace Testing
}       // namespace SHG

int main(int argc, char* argv[]) {
#ifdef MCHECK
     mtrace();
#endif
     SHG_SET_IEEE_EXCEPTION_HANDLER();
     SHG_ENABLE_IEEE_EXCEPTIONS();
     try {
          SHG::Testing::Test_shg().run(argc, argv);
          return EXIT_SUCCESS;
     }
     catch (const SHG::Exception& e) {
          e.print(SHG::Testing::prgname);
     }
     catch (const std::exception& e) {
          SHG::error(e.what(), SHG::Testing::prgname);
     }
     if (SHG::Testing::logfile.is_open())
          SHG::Testing::logfile.close();
     return EXIT_FAILURE;
}

namespace SHG {
namespace Testing {

void Test_shg::run(int argc, char* argv[]) {
     using std::cout;
     using std::fixed;
     using std::flush;
     using std::ofstream;
     using std::setprecision;
     using std::setw;
     typedef std::vector<Module>::size_type vmst;
     if (argc <= 1) {
          for (vmst i = 0; i < mm.size(); i++)
               mm[i].do_test = true;
     } else {
          for (int i = 1; i < argc; i++) {
               const char* s = argv[i];
               const std::vector<Module>::iterator it =
                    std::find_if(
                         mm.begin(), mm.end(),
                         [&s](const Module& m) {return m.name == s;});
               SHG_ASSERT(it != mm.end());
               SHG_ASSERT(!it->do_test);
               it->do_test = true;
          }
     }
     std::string::size_type maxlen = 0;
     for (vmst i = 0; i < mm.size(); i++)
          if (mm[i].name.size() > maxlen)
               maxlen = mm[i].name.size();
     unsigned total = 0, passed = 0;
     double elapsed, total_elapsed = 0.0;
     SHG::Timer t;
     logfile.exceptions(ofstream::failbit | ofstream::badbit);
     logfile.open("testshg.log");
     const std::ios_base::fmtflags flags = logfile.flags();
     {
          std::time_t rawtime;
          std::time(&rawtime);
          cout << "Testing SHG on "
               << std::asctime(std::localtime(&rawtime)) << flush;
     }
     for (vmst i = 0; i < mm.size(); i++)
          if (mm[i].do_test) {
               total++;
               const std::string& s = mm[i].name;
               cout << "Testing " << s << ' '
                    << std::string(maxlen - s.size() + 3, '.')
                    << ' ' << flush;
               logfile.flags(flags);
               try {
                    t.restart();
                    mm[i].testfun();
                    elapsed = t.elapsed();
                    total_elapsed += elapsed;
                    passed++;
                    cout << "passed. ("
                         << fixed << setprecision(3)
                         << setw(6) << elapsed << " seconds)\n";
               }
               catch (const SHG::Exception& e) {
                    cout << "failed.\n";
                    e.print(prgname, cout);
               }
               catch (const std::exception& e) {
                    cout << "failed.\n";
                    SHG::error(e.what(), prgname, cout);
               }
          }
     logfile.close();
     cout << "Total number of tests:  " << setw(3) << total
          << ". (" << fixed << setprecision(3) << setw(6)
          << total_elapsed << " seconds)\n"
          << "Number of tests passed: " << setw(3) << passed << ".\n"
          << "Number of tests failed: " << setw(3)
          << total - passed << ".\n"
          << "Testing finished.\n";
}

Test_shg::Test_shg() : mm() {
     register_test(test_bdstest, "bdstest");
     register_test(test_brent, "brent");
     register_test(test_date, "date");
     register_test(test_durbin_watson_statistic, "drbnwtsn");
     register_test(test_encoding, "encoding");
     register_test(test_except, "except");
     register_test(test_gsgts, "gsgts");
     register_test(test_hmm, "hmm");
     register_test(test_laplace, "laplace");
     register_test(test_mathprog, "mathprog");
     register_test(test_matrix, "matrix");
     register_test(test_mstat, "mstat");
     register_test(test_mzt, "mzt");
     register_test(test_normmix, "normmix");
     register_test(test_numalg, "numalg");
     register_test(test_ols, "ols");
     register_test(test_opdts, "opdts");
     register_test(test_permentr, "permentr");
     register_test(test_permut, "permut");
     register_test(test_runs, "runs");
     register_test(test_smc, "smc");
     register_test(test_specfunc, "specfunc");
     register_test(test_stock, "stock");
     register_test(test_strdlr, "strdlr");
     register_test(test_utils, "utils");
     register_test(test_vector, "vector");

     SHG_ASSERT
          (std::is_sorted(
               mm.begin(), mm.end(),
               [](const Module& m1, const Module& m2) {
                    return m1.name < m2.name;
               }));
}

Test_shg::Module::Module(Test_function testfun,
                         const std::string& name)
     : testfun(testfun), name(name), do_test() {}

void Test_shg::register_test(Test_function testfun,
                             const std::string& name) {
     mm.push_back(Module(testfun, name));
}

}       // namespace Testing
}       // namespace SHG
