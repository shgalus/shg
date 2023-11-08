/**
 * \file src/ninv.cc
 * Number of inversions in a sequence.
 * Created on  3 March 2016.
 */

#include <shg/ninv.h>
#include <stdexcept>

namespace SHG {

double nninv(std::size_t i, std::size_t n) {
     if (n < 2)
          throw std::invalid_argument(__func__);
     double const ni = static_cast<double>(i) /
                       static_cast<double>(n) /
                       static_cast<double>(n - 1) * 2.0;
     if (ni > 1.0)
          throw std::invalid_argument(__func__);
     return ni;
}

}  // namespace SHG
