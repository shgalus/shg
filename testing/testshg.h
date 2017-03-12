/* testshg.h: testing SHG library */

/**
 * \file testing/testshg.h
 * Testing SHG library.
 * \date Created on 20 August 2009.
 */

#ifndef TESTSHG_H
#define TESTSHG_H

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
void test_drbnwtsn();
void test_except();
void test_gsgts();
void test_hmm();
void test_laplace();
void test_mathprog();
void test_matrix();
void test_mstat();
void test_mzt();
void test_normmix();
void test_ols();
void test_opdts();
void test_permentr();
void test_permut();
void test_runs();
void test_smc();
void test_specfunc();
void test_stock();
void test_strdlr();
void test_unicode();
void test_utils();
void test_vector();

}       // namespace Testing
}       // namespace SHG

#endif
