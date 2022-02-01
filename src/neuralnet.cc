/**
 * \file src/neuralnet.cc
 * Multilayer neural networks.
 */

#include <shg/neuralnet.h>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <boost/numeric/ublas/io.hpp>  // \todo Really necessary?
#include <shg/except.h>
#include <shg/utils.h>  // unused_variables

namespace SHG::Neural_networks {

double identity(double x) {
     SHG_ASSERT(std::isfinite(x));
     return x;
}

double sign(double x) {
     SHG_ASSERT(std::isfinite(x));
     return x < 0.0 ? -1.0 : (x > 0.0 ? 1.0 : 0.0);
}

double sigmoid(double x) {
     SHG_ASSERT(std::isfinite(x));
     double const y = 1.0 / (1.0 + std::exp(-x));
     SHG_ASSERT(std::isfinite(y));
     return y;
}

double tgh(double x) {
     SHG_ASSERT(std::isfinite(x));
     double const y = std::tanh(x);
     SHG_ASSERT(std::isfinite(y));
     return y;
}

double relu(double x) {
     SHG_ASSERT(std::isfinite(x));
     return x < 0.0 ? 0.0 : x;
}

double hardtanh(double x) {
     SHG_ASSERT(std::isfinite(x));
     if (x < -1.0)
          return -1.0;
     if (x > 1.0)
          return 1.0;
     return x;
}

std::vector<double> softmax(std::vector<double> const& x) {
     SHG_ASSERT(
          std::all_of(x.cbegin(), x.cend(), std::isfinite<double>));
     auto const it = std::max_element(x.cbegin(), x.cend());
     SHG_ASSERT(it != x.cend());
     double const s = std::accumulate(
          x.cbegin(), x.cend(), 0.0,
          [it](double a, double b) { return a + std::exp(b - *it); });
     double const logs = std::log(s);
     std::vector<double> y(x.size());
     for (std::vector<double>::size_type i = 0; i < x.size(); i++) {
          y[i] = std::exp(x[i] - *it - logs);
          SHG_ASSERT(std::isfinite(y[i]));
     }
     return y;
}

MNN::MNN(int n, int m, std::vector<int> const& p) {
     init(n, m, p);
}

void MNN::init(int n, int m, std::vector<int> const& p) {
     SHG_ASSERT(n > 0);
     SHG_ASSERT(m > 0);
     SHG_ASSERT(p.size() > 0);
     SHG_ASSERT(std::all_of(p.cbegin(), p.cend(),
                            [](int pi) { return pi > 0; }));
     n_ = n;
     m_ = m;
     k_ = p.size();
     p_ = p;
     w_.clear();
     w_.resize(k_ + 1);
     w_[0].resize(p_.at(0), n_);
     for (std::vector<Matrix>::size_type i = 1; i < k_; i++)
          w_[i].resize(p_.at(i), p_.at(i - 1));
     w_[k_].resize(m_, p_.at(k_ - 1));
}

std::vector<double> MNN::y(std::vector<double> const& x) const {
     using namespace boost::numeric::ublas;
     ignore_unused_variable(x);
     matrix<double> m(3, 3);
     for (unsigned i = 0; i < m.size1(); ++i)
          for (unsigned j = 0; j < m.size2(); ++j)
               m(i, j) = 3 * i + j;
     std::cout << m << std::endl;
     std::vector<double> y;
     return y;
}

}  // namespace SHG::Neural_networks
