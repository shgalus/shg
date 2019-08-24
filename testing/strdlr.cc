/* date.cc: testing NIST StRD Linear Regression */

/**
 * \file testing/strdlr.cc
 * Testing NIST StRD Linear Regression.
 * \date Created on 12 August 2014.
 */

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include "shg/strdlr.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

/**
 * F statistics copied from NIST data files. See page \ref strdlr.
 */
const char* const fstat[] = {
     "5436385.54079785",
     "185330865.995752",
     "15750.2500000000",
     "298.6666666666667",
     "2162.43954511489",
     "330.285339234588",
     "Infinity",
     "Infinity",
     "675524.458240122",
     "67.5524458240122",
     "6.7552445824012241E-03",
};

}       // anonymous namespace

void test_strdlr() {
     StRDLR d;
     double maxdiff = -1.0;
     for (std::size_t i = 0; i < d.nsets(); i++) {
          d.active(i);
          if (std::isinf(d.fstat())) {
               SHG_ASSERT(std::strcmp(fstat[i], "Infinity") == 0);
          } else {
               const double w =
                    std::abs(d.fstat() - std::atof(fstat[i]));
               if (w > maxdiff)
                    maxdiff = w;
          }
     }
     SHG_ASSERT(maxdiff < std::numeric_limits<double>::min());
     try {
          d.active(d.nsets());
          SHG_ASSERT(false);
     } catch (const std::invalid_argument&) {}
}

}       // namespace Testing
}       // namespace SHG
