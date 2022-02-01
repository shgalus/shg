#include <shg/neuralnet.h>
#include <cmath>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(neuralnet_test)

std::vector<double> softmax(std::vector<double> const& x) {
     std::vector<double> y(x.size());
     double s = 0.0;
     for (std::vector<double>::size_type i = 0; i < x.size(); i++) {
          y[i] = std::exp(x[i]);
          s += y[i];
     }
     for (auto& yi : y)
          yi /= s;
     return y;
}

BOOST_AUTO_TEST_CASE(softmax_test) {
     std::vector<double> x{-2.0, -1.0, 0.0, 1.0, 2.0};
     auto const y0 = softmax(x);
     auto const y1 = SHG::Neural_networks::softmax(x);
     for (std::vector<double>::size_type i = 0; i < x.size(); i++)
          BOOST_CHECK(std::abs(y1[i] - y0[i]) < 2.0e-18);
}

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
