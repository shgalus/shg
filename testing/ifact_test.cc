#include <shg/ifact.h>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <cmath>
#include <set>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/integer/common_factor.hpp>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(ifact_test)

using test_unsigned_type = unsigned;

static_assert(std::numeric_limits<test_unsigned_type>::max() >=
              4294967295);

test_unsigned_type result[] = {
     2,   3,   5,   7,   11,  13,  17,  19,  23,  25,  29,  31,  35,
     37,  41,  43,  47,  49,  53,  55,  59,  61,  65,  67,  71,  73,
     77,  79,  83,  85,  89,  91,  95,  97,  101, 103, 107, 109, 113,
     115, 119, 121, 125, 127, 131, 133, 137, 139, 143, 145, 149, 151,
     155, 157, 161, 163, 167, 169, 173, 175, 179, 181, 185, 187, 191,
     193, 197, 199, 203, 205, 209, 211, 215, 217, 221, 223, 227, 229,
     233, 235, 239, 241, 245, 247, 251, 253, 257, 259, 263, 265, 269,
     271, 275, 277, 281, 283, 287, 289, 293, 295, 299,
};

BOOST_AUTO_TEST_CASE(d_test) {
     using SHG::Divisors;
     Divisors<test_unsigned_type> d;
     for (std::size_t i = 0; i < std::size(result); i++)
          BOOST_CHECK(d() == result[i]);
}

BOOST_AUTO_TEST_CASE(factorize_test) {
     using SHG::factorize;
     std::vector<test_unsigned_type> v;

     factorize<test_unsigned_type>(0, v);
     BOOST_CHECK(v.size() == 0);

     factorize<test_unsigned_type>(1, v);
     BOOST_CHECK(v.size() == 0);

     for (test_unsigned_type n = 2; n <= 1000000; n++) {
          factorize(n, v);
          test_unsigned_type n1 = 1;
          for (std::vector<test_unsigned_type>::size_type i = 0;
               i < v.size(); i++) {
               if (i > 0)
                    BOOST_CHECK(v[i] >= v[i - 1]);
               n1 *= v[i];
          }
          BOOST_CHECK(n1 == n);
     }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
