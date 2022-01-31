#include <shg/neuralnet.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(neuralnet_test)

BOOST_AUTO_TEST_CASE(mnn_basic_test) {
     using SHG::Neural_networks::MNN;
     std::vector<int> const p{3, 3, 1};
     MNN mnn(5, 1, p);
     BOOST_CHECK(mnn.n() == 5);
     BOOST_CHECK(mnn.m() == 1);
     BOOST_CHECK(mnn.k() == 3);
     BOOST_CHECK(mnn.p() == p);
}

BOOST_AUTO_TEST_CASE(basic_test) {
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
