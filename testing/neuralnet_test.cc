#include <shg/neuralnet.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(neuralnet_test)

BOOST_AUTO_TEST_CASE(linear_mnn_basic_test) {
     using SHG::Linear_MNN;
     std::vector<int> const p{3, 3, 1};
     Linear_MNN lmnn(5, 1, p);
     BOOST_CHECK(lmnn.n() == 5);
     BOOST_CHECK(lmnn.m() == 1);
     BOOST_CHECK(lmnn.k() == 3);
     BOOST_CHECK(lmnn.p() == p);
}

BOOST_AUTO_TEST_CASE(basic_test) {
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
