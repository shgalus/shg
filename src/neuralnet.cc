/**
 * \file src/neuralnet.cc
 * Multilayer neural networks.
 */

#include <shg/neuralnet.h>
#include <shg/except.h>

namespace SHG {

MNN::MNN(int n, int m, std::vector<int> const& dim)
     : n_(n), m_(m), k_(dim.size()), p_(dim) {
     SHG_ASSERT(n_ > 0);
     SHG_ASSERT(m_ > 0);
     SHG_ASSERT(k_ > 0);
     for (auto p : p_)
          SHG_ASSERT(p > 0);
}

MNN::~MNN() {}

}  // namespace SHG
