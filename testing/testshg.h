/* testshg.h: testing SHG library */

/**
 * \file testing/testshg.h
 * Testing SHG library.
 * \date Created on 20 August 2009.
 */

#ifndef TESTSHG_H
#define TESTSHG_H

#define NELEMS(a) (sizeof(a) / sizeof((a)[0]))

#include <fstream>
#include <string>
#include "shg/except.h"

namespace SHG {
namespace Testing {

extern std::ofstream logfile;
std::string datapath(const char* fname);
void header(const std::string& s);
void footer(const std::string& s);

// Module tests.
void test_bdstest();
void test_brent();
void test_date();
void test_except();
void test_gsgts();
void test_hmm();
void test_laplace();
void test_mstat();
void test_normmix();
void test_ols();
void test_opdts();
void test_permentr();
void test_runs();
void test_smc();
void test_strdlr();

}       // namespace Testing
}       // namespace SHG

#endif
