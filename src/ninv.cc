/* permut.cc: permutations */

/**
 * \file src/permut.cc
 * Permutations.
 * Created on  3 March 2016.
 */

#include <stdexcept>
#include "shg/permut.h"

namespace SHG {
namespace Permutations {

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

}       // namespace Permutations
}       // namespace SHG
