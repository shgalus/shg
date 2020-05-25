/* date.cc: testing number of inversions in a sequence */

/**
 * \file testing/ninv.cc
 * Testing number of inversions in a sequence.
 * Created on  3 March 2016.
 */

#include <numeric>      // iota
#include "shg/ninv.h"
#include "shg/vector.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

/**
 * \f$O(n^2)\f$ algorithm to count the number of inversions.
 */
template <class T>
std::size_t ninv(const T* a, std::size_t n) {
     std::size_t ninv = 0;
     for (std::size_t i = 0; i < n; i++) {
          const T ai = a[i];
          for (std::size_t j = i + 1; j < n; j++)
               if (ai > a[j])
                    ninv++;
     }
     return ninv;
}

}

void test_ninv() {
     for (std::size_t n = 0; n <= 7; n++) {
          SHG::Vector<int> a(n);
          std::iota(a.begin(), a.end(), 0);
          do {
               const std::size_t n1 = ninv(a.c_vec(), n);
               const std::size_t n2 =
                    SHG::ninv(a.c_vec(), n);
               SHG_ASSERT(n1 == n2);
          } while (std::next_permutation(a.begin(), a.end()));
     }
     // Test also sequences with repeated elements. This is important
     // for merging algorithm in ninv.
     for (std::size_t n = 0; n <= 99999; n++) {
          std::string s = std::to_string(n);
          const std::size_t n1 = ninv(s.c_str(), s.size());
          const std::size_t n2 =
               SHG::ninv(s.c_str(), s.size());
          SHG_ASSERT(n1 == n2);
     }
}

}       // namespace Testing
}       // namespace SHG
