#include <shg/ninv.h>
#include <numeric>
#include <shg/vector.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(ninv_test)

/**
 * Counting the number of inversions in a sequence. This is a
 * \f$O(n^2)\f$ algorithm.
 */
template <class T>
std::size_t ninvn2(const T* a, std::size_t n) {
     std::size_t ninv = 0;
     for (std::size_t i = 0; i < n; i++) {
          const T ai = a[i];
          for (std::size_t j = i + 1; j < n; j++)
               if (ai > a[j])
                    ninv++;
     }
     return ninv;
}

namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(ninv_permutations, bdata::xrange(8), xr) {
     const std::size_t n = xr;
     Vector<int> a(n);
     std::iota(a.begin(), a.end(), 0);
     do {
          const std::size_t n1 = ninvn2(a.c_vec(), n);
          const std::size_t n2 = ninv(a.c_vec(), n);
          BOOST_CHECK(n1 == n2);
     } while (std::next_permutation(a.begin(), a.end()));
}

// Test sequences with repeated elements.
BOOST_DATA_TEST_CASE(ninv_sequences, bdata::xrange(10000), xr) {
     const std::size_t n = xr;
     const std::string s = std::to_string(n);
     const std::size_t n1 = ninvn2(s.c_str(), s.size());
     const std::size_t n2 = ninv(s.c_str(), s.size());
     BOOST_CHECK(n1 == n2);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
