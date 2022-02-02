#include <shg/neuralnet.h>
#include <cmath>
#include <sstream>
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

BOOST_AUTO_TEST_CASE(xor_test) {
     using SHG::Neural_networks::MNN;
     using SHG::Neural_networks::Activation_function;

     // There will be one hidden layer with two neurons.
     std::vector<int> const p{2};
     MNN mnn(2, 1, p);
     BOOST_CHECK(mnn.n() == 2);
     BOOST_CHECK(mnn.m() == 1);
     BOOST_CHECK(mnn.k() == 1);
     BOOST_CHECK(mnn.p() == p);
     mnn.set_activation_function(1, Activation_function::sigmoid);
     mnn.set_activation_function(2, Activation_function::identity);
     mnn.set_random_weights();
     std::ostringstream oss(binout);
     mnn.write(oss);
     BOOST_REQUIRE(oss.good());
     MNN mnn1;
     std::istringstream iss(bininp);
     iss.str(oss.str());
     mnn1.read(iss);
     BOOST_REQUIRE(iss.good());
     BOOST_CHECK(fcmp(mnn, mnn1, 1e-10));
     auto const y = mnn.y({0.0, 0.0});
     BOOST_REQUIRE(y.size() == 1);
     BOOST_CHECK(facmp(y[0], 0.0732425, 5e-8) == 0);
}

BOOST_AUTO_TEST_CASE(basic_test) {
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
