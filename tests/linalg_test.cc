#include <shg/linalg.h>
#include <numeric>
#include <vector>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(linalg_test)

BOOST_AUTO_TEST_CASE(basic_test) {
     using SHG::Linalg::Matint;
     using SHG::Linalg::Vecint;
     using SHG::Linalg::Row_vecint;

     std::vector<int> v0(12);
     std::iota(v0.begin(), v0.end(), 1);
     Matint const a = Eigen::Map<Matint const>(v0.data(), 3, 4);
     Matint b(3, 4);
     b << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12;
     BOOST_CHECK(b == a);
     BOOST_CHECK(a.rows() == 3);
     BOOST_CHECK(a.cols() == 4);

     Vecint v(3);
     v << 1, 3, 6;
     BOOST_CHECK(v.size() == 3);
     BOOST_CHECK(v.rows() == 3);
     BOOST_CHECK(v.cols() == 1);
     BOOST_CHECK(v[0] == 1);
     BOOST_CHECK(v(1) == 3);
     BOOST_CHECK(v(2) == 6);

     Row_vecint w(3);
     w << 1, 3, 6;
     BOOST_CHECK(w.size() == 3);
     BOOST_CHECK(w.rows() == 1);
     BOOST_CHECK(w.cols() == 3);
     BOOST_CHECK(w[0] == 1);
     BOOST_CHECK(w(1) == 3);
     BOOST_CHECK(w(2) == 6);
}

BOOST_AUTO_TEST_CASE(block_operations_test) {
     using SHG::Linalg::Matint;
     Matint a0(4, 4);
     a0 << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;
     Matint a(4, 4);
     a << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;
     BOOST_CHECK(a == a0);
     Matint b = a.block(1, 1, 2, 2);
     b(0, 0) = 33;
     BOOST_CHECK(a == a0);
     BOOST_CHECK(b.rows() == 2 && b.cols() == 2);
     BOOST_CHECK(b(0, 0) == 33);
     BOOST_CHECK(b(0, 1) == 7);
     BOOST_CHECK(b(1, 0) == 10);
     BOOST_CHECK(b(1, 1) == 11);
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
