/**
 * \file src/ieee.cc
 * IEEE exception handling.
 * \date Created on 20 July 2014.
 */

#if defined SHG_IEEE_EXCEPTIONS && defined __GNUG__
#include <shg/ieee.h>
#include <fenv.h>
#include <signal.h>
#include <iomanip>
#include <iostream>

namespace SHG {

namespace {

void fatal_ieee_exception(int sig) {
     std::cout.flush();
     std::cerr << "Floating point exception.\nProgram aborted.\n";
     signal(sig, SIG_DFL);
     raise(sig);
}

}  // anonymous namespace

void set_ieee_exception_handler() {
     signal(SIGFPE, fatal_ieee_exception);
}

void enable_ieee_exceptions() {
     feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
}

void disable_ieee_exceptions() {
     fedisableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
}

}  // namespace SHG

#endif
