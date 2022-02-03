/**
 * \file src/neuralnet.cc
 * Multilayer neural networks.
 */

//#define NEURALNET_DEBUG

#include <shg/neuralnet.h>
#include <cassert>
#include <algorithm>
#include <limits>
#include <numeric>
#include <fstream>
#include <ios>
#include <iomanip>
#include <type_traits>
#ifdef NEURALNET_DEBUG
#include <iostream>
#include <boost/numeric/ublas/io.hpp>
#endif
#include <shg/fcmp.h>
#include <shg/mzt.h>

namespace SHG::Neural_networks {

Invalid_number::Invalid_number()
     : std::runtime_error("Invalid number") {}

double identity(double x) {
     check(x);
     return x;
}

double sign(double x) {
     check(x);
     return x < 0.0 ? -1.0 : (x > 0.0 ? 1.0 : 0.0);
}

double sigmoid(double x) {
     check(x);
     double const y = 1.0 / (1.0 + std::exp(-x));
     check(y);
     return y;
}

double tgh(double x) {
     check(x);
     double const y = std::tanh(x);
     check(y);
     return y;
}

double relu(double x) {
     check(x);
     return x < 0.0 ? 0.0 : x;
}

double hardtanh(double x) {
     check(x);
     if (x < -1.0)
          return -1.0;
     if (x > 1.0)
          return 1.0;
     return x;
}

std::vector<double> softmax(std::vector<double> const& x) {
     std::for_each(x.cbegin(), x.cend(), check);
     auto const it = std::max_element(x.cbegin(), x.cend());
     if (it == x.cend())
          throw std::invalid_argument("invalid argument in softmax");
     double const s = std::accumulate(
          x.cbegin(), x.cend(), 0.0,
          [it](double a, double b) { return a + std::exp(b - *it); });
     double const logs = std::log(s);
     std::vector<double> y(x.size());
     for (std::vector<double>::size_type i = 0; i < x.size(); i++) {
          y[i] = std::exp(x[i] - *it - logs);
          check(y[i]);
     }
     return y;
}

MNN::MNN(int n, int m, std::vector<int> const& p) {
     init(n, m, p);
}

void MNN::init(int n, int m, std::vector<int> const& p) {
     if (n < 1 || m < 1 || p.size() < 1 ||
         std::any_of(p.cbegin(), p.cend(),
                     [](int pi) { return pi < 1; }))
          throw std::invalid_argument(
               "invalid argument in MNN::init");
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
     phi_.clear();
     phi_.resize(k_ + 1);
}

void MNN::set_activation_function(int i, Activation_function f,
                                  double x0, double s) {
     if (i < 1 || i > k_ + 1)
          throw std::invalid_argument("invalid layer");
     if (s < std::numeric_limits<double>::min())
          throw std::invalid_argument("invalid scale");
     Activation a;
     switch (f) {
     case Activation_function::identity:
          a.f = identity;
          break;
     case Activation_function::sign:
          a.f = sign;
          break;
     case Activation_function::sigmoid:
          a.f = sigmoid;
          break;
     case Activation_function::tgh:
          a.f = tgh;
          break;
     case Activation_function::relu:
          a.f = relu;
          break;
     case Activation_function::hardtanh:
          a.f = hardtanh;
          break;
     case Activation_function::softmax:
          a.f2 = softmax;
          break;
     default:
          throw std::invalid_argument("invalid activation function");
     }
     a.af = f;
     a.x0 = x0;
     a.s = s;
     phi_.at(--i) = a;
}

void MNN::set_random_weights() {
     MZT mzt;
     for (auto& wi : w_) {
          for (Matrix::size_type i = 0; i < wi.size1(); i++)
               for (Matrix::size_type j = 0; j < wi.size2(); j++)
                    wi(i, j) = 2.0 * mzt() - 1.0;
     }
}

std::vector<double> MNN::y(std::vector<double> const& x) const {
     if (x.size() != static_cast<std::vector<double>::size_type>(n_))
          throw std::invalid_argument("bad dimension");
     std::vector<double> u;
     std::vector<double> h(x);

     for (std::vector<Matrix>::size_type k = 0; k <= k_; k++) {
          auto const& w = w_[k];
#ifdef NEURALNET_DEBUG
          std::cout << w << std::endl;
#endif
          // u = w_[k] * h
          u.resize(w.size1());
          for (std::vector<double>::size_type i = 0; i < u.size();
               i++) {
               double s = 0.0;
               for (Matrix::size_type j = 0; j < w.size2(); j++)
                    s += w(i, j) * h.at(j);
               u[i] = s;
          }
          // h = phi_[k](u)
          phi(k, u, h);
     }
     return h;
}

void MNN::write(std::ostream& f) const {
     f << std::scientific
       << std::setprecision(std::numeric_limits<double>::digits10 +
                            1);
     f << n_ << '\n' << m_ << '\n' << k_ << '\n';
     for (auto p : p_)
          f << p << '\n';
     for (auto const& w : w_)
          for (Matrix::size_type i = 0; i < w.size1(); i++)
               for (Matrix::size_type j = 0; j < w.size2(); j++)
                    f << w(i, j) << '\n';
     for (auto const& a : phi_) {
          f << static_cast<
                    std::underlying_type<Activation_function>::type>(
                    a.af)
            << '\n'
            << a.x0 << '\n'
            << a.s << '\n';
     }
}

bool MNN::write(char const* fname) const {
     std::ofstream f(fname,
                     std::ios_base::out | std::ios_base::binary);
     write(f);
     return f.good();
}

void MNN::read(std::istream& f) {
     int n, m, k;
     std::vector<int> p;
     if (f.fail())
          return;
     f >> n >> m >> k;
     if (f.fail())
          return;
     if (k <= 0) {
          f.setstate(std::ios::failbit);
          return;
     }
     p.resize(k);
     for (std::vector<int>::size_type i = 0; i < p.size(); i++)
          f >> p[i];
     if (f.fail())
          return;
     try {
          init(n, m, p);
     } catch (std::invalid_argument const&) {
          f.setstate(std::ios::failbit);
          return;
     }
     for (auto& w : w_)
          for (Matrix::size_type i = 0; i < w.size1(); i++)
               for (Matrix::size_type j = 0; j < w.size2(); j++)
                    f >> w(i, j);
     if (f.fail())
          return;
     for (std::vector<Activation>::size_type i = 0; i < phi_.size();
          i++) {
          std::underlying_type<Activation_function>::type u;
          double x0, s;
          f >> u >> x0 >> s;
          if (f.fail())
               return;
          try {
               set_activation_function(
                    i + 1, static_cast<Activation_function>(u), x0,
                    s);
          } catch (std::invalid_argument const&) {
               f.setstate(std::ios::failbit);
               return;
          }
     }
}

bool MNN::read(char const* fname) {
     std::ifstream f(fname,
                     std::ios_base::in | std::ios_base::binary);
     read(f);
     return f.good();
}

void MNN::phi(std::vector<Matrix>::size_type k,
              std::vector<double> const& u,
              std::vector<double>& h) const {
     Activation const& a = phi_[k];
     if (a.f2 != nullptr)
          h = a.f2(u);
     else {
          h.resize(u.size());
          for (std::vector<double>::size_type i = 0; i < u.size();
               i++) {
               h[i] = a.f((u.at(i) - a.x0) / a.s);
          }
     }
}

bool fcmp(MNN const& lhs, MNN const& rhs, double eps) {
     using Matrix = MNN::Matrix;
     using Activation = MNN::Activation;

     if (lhs.n_ != rhs.n_)
          return false;
     if (lhs.m_ != rhs.m_)
          return false;
     if (lhs.k_ != rhs.k_)
          return false;
     assert(lhs.p_.size() == rhs.p_.size());
     if (lhs.p_ != rhs.p_)
          return false;
     assert(lhs.w_.size() == rhs.w_.size());
     for (std::vector<Matrix>::size_type k = 0; k < lhs.w_.size();
          k++) {
          Matrix const& lw = lhs.w_[k];
          Matrix const& rw = rhs.w_[k];
          assert(lw.size1() == rw.size1());
          assert(lw.size2() == rw.size2());
          for (Matrix::size_type i = 0; i < lw.size1(); i++)
               for (Matrix::size_type j = 0; j < lw.size2(); j++)
                    if (facmp(lw(i, j), rw(i, j), eps) != 0)
                         return false;
     }
     assert(lhs.phi_.size() == rhs.phi_.size());
     for (std::vector<Activation>::size_type i = 0;
          i < lhs.phi_.size(); i++) {
          Activation const& la = lhs.phi_[i];
          Activation const& ra = rhs.phi_[i];
          if (la.af != ra.af)
               return false;
          if (la.f != ra.f)
               return false;
          if (la.f2 != ra.f2)
               return false;
          if (facmp(la.x0, ra.x0, eps) != 0)
               return false;
          if (facmp(la.s, ra.s, eps) != 0)
               return false;
     }
     return true;
}

}  // namespace SHG::Neural_networks
