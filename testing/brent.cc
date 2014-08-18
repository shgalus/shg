/* brent.cc: testing Brent method */

/**
 * \file testing/brent.cc
 * Testing Brent method.
 * \date Created on 28 December 2013.
 */

#include "shg/brent.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

double fun1(double x) {
     return x * x - 2.0;
}

struct Fun2 {
     double operator()(double x) {
          return x * x - 4.0;
     }
};

void test1() {
     double x = SHG::brent(fun1, 1.0, 2.0, 1e-12);
     SHG_ASSERT(std::abs(x - sqrt(2.0)) < 1e-12);
     x = SHG::brent(fun1, -2.0, -1.0, 1e-12);
     SHG_ASSERT(std::abs(x + sqrt(2.0)) < 1e-12);
     Fun2 fun2;
     x = SHG::brent(fun2, 1.0, 2.0, 1e-12);
     SHG_ASSERT(std::abs(x - 2.0) < 1e-12);
     x = SHG::brent(fun2, 2.0, 3.0, 1e-12);
     SHG_ASSERT(std::abs(x - 2.0) < 1e-12);
     x = SHG::brent(fun2, -2.0, 2.0, 1e-12);
     SHG_ASSERT(std::abs(x - 2.0) < 1e-12);
}

}       // anonymous namespace

void test_brent() {
     test1();
}

}       // namespace Testing
}       // namespace SHG
