/**
 * \file src/neuralnet.cc
 * Multilayer neural networks.
 */

#include <shg/neuralnet.h>
#include <shg/except.h>
#include <shg/utils.h>

namespace SHG {

MNN::MNN(int n, int m, std::vector<int> const& p)
     : n_(n), m_(m), k_(p.size()), p_(p) {
     SHG_ASSERT(n_ > 0);
     SHG_ASSERT(m_ > 0);
     SHG_ASSERT(k_ > 0);
     for (auto p : p_)
          SHG_ASSERT(p > 0);
}

MNN::~MNN() {}

std::vector<double> MNN::y(std::vector<double> const& x) const {
     return do_y(x);
}

Linear_MNN::Linear_MNN(int n, int m, std::vector<int> const& p)
     : MNN(n, m, p) {}

std::vector<double> Linear_MNN::do_y(
     std::vector<double> const& x) const {
     ignore_unused_variable(x);
     std::vector<double> y;
     return y;
}

}  // namespace SHG
