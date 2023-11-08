#include <shg/ninv.h>
#include <numeric>
#include <shg/vector.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(ninv_test)

using SHG::ninv;
using SHG::Vector;
using std::size_t;

/**
 * Counting the number of inversions in a sequence. This is a
 * \f$O(n^2)\f$ algorithm.
 */
template <class T>
size_t ninvn2(T const* a, size_t n) {
     size_t ninv = 0;
     for (size_t i = 0; i < n; i++) {
          T const ai = a[i];
          for (size_t j = i + 1; j < n; j++)
               if (ai > a[j])
                    ninv++;
     }
     return ninv;
}

BOOST_DATA_TEST_CASE(ninv_permutations, bdata::xrange(8), xr) {
     size_t const n = xr;
     Vector<int> a(n);
     std::iota(a.begin(), a.end(), 0);
     do {
          size_t const n1 = ninvn2(a.c_vec(), n);
          size_t const n2 = ninv(a.c_vec(), n);
          BOOST_CHECK(n1 == n2);
     } while (std::next_permutation(a.begin(), a.end()));
}

// Test sequences with repeated elements.
BOOST_DATA_TEST_CASE(ninv_sequences, bdata::xrange(10000), xr) {
     size_t const n = xr;
     std::string const s = std::to_string(n);
     size_t const n1 = ninvn2(s.c_str(), s.size());
     size_t const n2 = ninv(s.c_str(), s.size());
     BOOST_CHECK(n1 == n2);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
