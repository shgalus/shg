#include <shg/safeint.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(safeint_test)

using test_types = boost::mpl::list<unsigned char, signed char>;

BOOST_AUTO_TEST_CASE_TEMPLATE(basic_test, T, test_types) {
     static_assert(std::is_same<T, unsigned char>::value ||
                   std::is_same<T, signed char>::value);
     T const m = std::numeric_limits<T>::min();
     T const M = std::numeric_limits<T>::max();

     for (T x = m;; x++) {
          int const i = x;
          for (T y = m;; y++) {
               int const j = y;
               int k = i + j;
               if (SHG::is_add_safe(x, y))
                    BOOST_CHECK(x + y == k);
               else
                    BOOST_CHECK(k < m || k > M);
               k = i - j;
               if (SHG::is_sub_safe(x, y))
                    BOOST_CHECK(x - y == k);
               else
                    BOOST_CHECK(k < m || k > M);
               k = i * j;
               if (SHG::is_mul_safe(x, y))
                    BOOST_CHECK(x * y == k);
               else
                    BOOST_CHECK(k < m || k > M);
               if (y == 0) {
                    BOOST_CHECK(!SHG::is_div_safe(x, y));
               } else {
                    k = i / j;
                    if (SHG::is_div_safe(x, y))
                         BOOST_CHECK(x / y == k);
                    else
                         BOOST_CHECK(k < m || k > M);
               }
               if (y == M)
                    break;
          }
          if (x == M)
               break;
     }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
