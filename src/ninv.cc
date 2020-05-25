/* ninv.h: number of inversions in a sequence */

/**
 * \file src/ninv.cc
 * Number of inversions in a sequence.
 * Created on  3 March 2016.
 */

#include <stdexcept>
#include "shg/ninv.h"

namespace SHG {

double nninv(std::size_t i, std::size_t n) {
     if (n < 2)
          throw std::invalid_argument(__func__);
     const double ni =
          static_cast<double>(i) / static_cast<double>(n)
          / static_cast<double>(n - 1) * 2.0;
     if (ni > 1.0)
          throw std::invalid_argument(__func__);
     return ni;
}

}       // namespace SHG
