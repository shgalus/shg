/**
 * \file src/neuralnet.cc
 * Multilayer neural networks.
 */

#include <shg/neuralnet.h>
#include <algorithm>
#include <shg/except.h>
#include <shg/utils.h>

namespace SHG {

MNN::MNN(int n, int m, std::vector<int> const& p) {
     init(n, m, p);
}

MNN::~MNN() {}

void MNN::init(int n, int m, std::vector<int> const& p) {
     SHG_ASSERT(n > 0);
     SHG_ASSERT(m > 0);
     SHG_ASSERT(p.size() > 0);
     SHG_ASSERT(!std::any_of(p.cbegin(), p.cend(),
                             [](int pi) { return pi < 1; }));
     n_ = n;
     m_ = m;
     k_ = p.size();
     p_ = p;
}

std::vector<double> MNN::y(std::vector<double> const& x) const {
     return do_y(x);
}

Linear_MNN::Linear_MNN(int n, int m, std::vector<int> const& p) {
     init(n, m, p);
}

void Linear_MNN::init(int n, int m, std::vector<int> const& p) {
     MNN::init(n, m, p);
}

Linear_MNN::~Linear_MNN() {}

std::vector<double> Linear_MNN::do_y(
     std::vector<double> const& x) const {
     ignore_unused_variable(x);
     std::vector<double> y;
     return y;
}

}  // namespace SHG
