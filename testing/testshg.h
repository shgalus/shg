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
void test_brent();
void test_except();

}       // namespace Testing
}       // namespace SHG

#endif
