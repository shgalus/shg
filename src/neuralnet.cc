/**
 * \file src/neuralnet.cc
 * Multilayer neural networks.
 */

#include <shg/neuralnet.h>
#include <cmath>
#include <numeric>
#include <limits>
#include <fstream>
#include <ios>
#include <iomanip>
#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <shg/fcmp.h>
#include <shg/utils.h>
#include <shg/mzt.h>

namespace SHG::Neural_networks {

Error::Error() : Error("neural network error") {}
Error::Error(std::string const& what) : Error(what.c_str()) {}
Error::Error(char const* what) : std::runtime_error(what) {}

Vecreal identity(Vecreal const& x) {
     return x;
}

Vecreal sign(Vecreal const& x) {
     auto const f = [](Real x) {
          return x < 0.0 ? -1.0 : (x > 0.0 ? 1.0 : 0.0);
     };
     Vecreal y(x.size());
     std::transform(x.begin(), x.end(), y.begin(), f);
     return y;
}

Vecreal sigmoid(Vecreal const& x) {
     auto const f = [](Real x) { return 1.0 / (1.0 + std::exp(-x)); };
     Vecreal y(x.size());
     std::transform(x.begin(), x.end(), y.begin(), f);
     return y;
}

Vecreal tgh(Vecreal const& x) {
     auto const f = [](Real x) { return std::tanh(x); };
     Vecreal y(x.size());
     std::transform(x.begin(), x.end(), y.begin(), f);
     return y;
}

Vecreal relu(Vecreal const& x) {
     auto const f = [](Real x) { return x < 0.0 ? 0.0 : x; };
     Vecreal y(x.size());
     std::transform(x.begin(), x.end(), y.begin(), f);
     return y;
}

Vecreal hardtanh(Vecreal const& x) {
     auto const f = [](Real x) {
          return x < -1.0 ? -1.0 : (x > 1.0 ? 1.0 : x);
     };
     Vecreal y(x.size());
     std::transform(x.begin(), x.end(), y.begin(), f);
     return y;
}

Vecreal softmax(Vecreal const& x) {
     auto const it = std::max_element(x.cbegin(), x.cend());
     assert(it != x.cend());
     Real const s = std::accumulate(
          x.cbegin(), x.cend(), 0.0,
          [it](double a, double b) { return a + std::exp(b - *it); });
     Real const logs = std::log(s);
     Vecreal y(x.size());
     std::transform(
          x.begin(), x.end(), y.begin(),
          [it, logs](Real x) { return std::exp(x - *it - logs); });
     return y;
}

Matreal didentity(Vecreal const& x,
                  [[maybe_unused]] Vecreal const& f) {
     assert(x.size() == f.size());
     return boost::numeric::ublas::identity_matrix<Real>(x.size());
}

Matreal dsign(Vecreal const& x, [[maybe_unused]] Vecreal const& f) {
     assert(x.size() == f.size());
     if (std::any_of(x.cbegin(), x.cend(),
                     [](Real x) { return x == 0.0; }))
          throw Error("no derivative in dsign");
     return boost::numeric::ublas::zero_matrix<Real>(x.size(),
                                                     x.size());
}

Matreal dsigmoid(Vecreal const& x, Vecreal const& f) {
     assert(x.size() == f.size());
     boost::numeric::ublas::banded_matrix<Real> df(x.size(),
                                                   x.size());
     for (Vecreal::size_type i = 0; i < x.size(); i++)
          df(i, i) = f(i) * (1.0 - f(i));
     return df;
}

Matreal dtgh(Vecreal const& x, [[maybe_unused]] Vecreal const& f) {
     assert(x.size() == f.size());
     boost::numeric::ublas::banded_matrix<Real> df(x.size(),
                                                   x.size());
     for (Vecreal::size_type i = 0; i < x.size(); i++)
          df(i, i) = 1.0 - sqr(f(i));
     return df;
}

Matreal drelu(Vecreal const& x, [[maybe_unused]] Vecreal const& f) {
     assert(x.size() == f.size());
     boost::numeric::ublas::banded_matrix<Real> df(x.size(),
                                                   x.size());
     for (Vecreal::size_type i = 0; i < x.size(); i++)
          if (x(i) > 0.0)
               df(i, i) = 1.0;
          else if (x(i) < 0.0)
               df(i, i) = 0.0;
          else
               throw Error("no derivative in drelu");
     return df;
}

Matreal dhardtanh(Vecreal const& x,
                  [[maybe_unused]] Vecreal const& f) {
     assert(x.size() == f.size());
     boost::numeric::ublas::banded_matrix<Real> df(x.size(),
                                                   x.size());
     for (Vecreal::size_type i = 0; i < x.size(); i++)
          if (x(i) > 1.0 || x(i) < -1.0)
               df(i, i) = 0.0;
          else if (x(i) > -1.0 && x(i) < 1.0)
               df(i, i) = 1.0;
          else
               throw Error("no derivative in drelu");
     return df;
}

Matreal dsoftmax(Vecreal const& x, Vecreal const& f) {
     using boost::numeric::ublas::upper;
     using Symrealmat =
          boost::numeric::ublas::symmetric_matrix<Real, upper>;
     assert(x.size() == f.size());
     Symrealmat df(x.size(), x.size());

     for (Symrealmat::size_type i = 0; i < df.size1(); i++) {
          df(i, i) = f(i) * (1.0 - f(i));
          for (Symrealmat::size_type j = i + 1; j < df.size2(); j++)
               df(i, j) = -f(i) * f(j);
     }
     return df;
}

Real quadratic(Vecreal const& aL, Vecreal const& y) {
     assert(aL.size() == y.size());
     Real s{0.0};
     for (Vecreal::size_type i = 0; i < aL.size(); i++)
          s += sqr(aL(i) - y(i));
     return 0.5 * s;
}

Real cross_entropy(Vecreal const& aL, Vecreal const& y) {
     using boost::numeric::ublas::inner_prod;
     assert(aL.size() == y.size());
     Real const s = inner_prod(aL, y);
     return -std::log(s);
}

Vecreal dquadratic(Vecreal const& aL, Vecreal const& y) {
     assert(aL.size() == y.size());
     return aL - y;
}

Vecreal dcross_entropy(Vecreal const& aL, Vecreal const& y) {
     using boost::numeric::ublas::zero_vector;
     assert(aL.size() == y.size());
     Vecreal v = zero_vector<Real>(aL.size());
     for (Vecreal::size_type i = 0; i < aL.size(); i++)
          if (y(i) > 0.0) {
               v(i) = -1.0 / aL(i);
               if (!std::isfinite(v(i)))
                    throw Error(
                         "error calculating derivative of cross "
                         "entropy");
               return v;
          }
     throw std::invalid_argument(
          "invalid argument in dcross_entropy");
}

MNN::MNN() {
     Vecuint n(2);
     n(0) = n(1) = 1;
     init(n);
}

MNN::MNN(Vecuint const& n) {
     init(n);
}

void MNN::init(Vecuint const& n) {
     if (n.size() < 2 || std::any_of(n.cbegin(), n.cend(),
                                     [](Uint u) { return u < 1; }))
          throw std::invalid_argument(
               "invalid argument in MNN::init");
     n_ = n;
     eta_ = 0.1;
     W_.resize(n_.size());
     b_.resize(n_.size());
     a_.resize(n_.size());
     z_.resize(n_.size());
     phi_.resize(n_.size());
     phi_ptr_.resize(n_.size());
     dphi_ptr_.resize(n_.size());
     // W_(0), b_(0), ... are not used
     MZT mzt;
     for (Uint l = 1; l < W_.size(); l++) {
          Matreal& w = W_(l);
          w.resize(n_(l), n_(l - 1));
          for (auto it1 = w.begin1(); it1 != w.end1(); ++it1)
               for (auto it2 = it1.begin(); it2 != it1.end(); ++it2)
                    *it2 = 2.0 * mzt() - 1.0;
          Vecreal& b = b_(l);
          b.resize(n_(l));
          for (auto it = b.begin(); it != b.end(); ++it)
               *it = 2.0 * mzt() - 1.0;
          phi_(l) = Activation_function::sigmoid;
          phi_ptr_(l) = sigmoid;
          dphi_ptr_(l) = dsigmoid;
     }
     C_ = Cost_function::quadratic;
     C_ptr_ = quadratic;
     dC_ptr_ = dquadratic;
}

void MNN::eta(Real e) {
     if (e <= 0.0)
          throw std::invalid_argument("bad learning rate");
     eta_ = e;
}

Activation_function MNN::phi(Uint l) const {
     if (l < 1 || l >= phi_.size())
          throw std::invalid_argument("bad layer in phi()");
     return phi_(l);
}

void MNN::phi(Activation_function f, Uint l) {
     if (l < 1 || l >= phi_.size())
          throw std::invalid_argument("bad layer in phi()");
     switch (f) {
     case Activation_function::identity:
          phi_ptr_(l) = identity;
          dphi_ptr_(l) = didentity;
          break;
     case Activation_function::sign:
          phi_ptr_(l) = sign;
          dphi_ptr_(l) = dsign;
          break;
     case Activation_function::sigmoid:
          phi_ptr_(l) = sigmoid;
          dphi_ptr_(l) = dsigmoid;
          break;
     case Activation_function::tgh:
          phi_ptr_(l) = tgh;
          dphi_ptr_(l) = dtgh;
          break;
     case Activation_function::relu:
          phi_ptr_(l) = relu;
          dphi_ptr_(l) = drelu;
          break;
     case Activation_function::hardtanh:
          phi_ptr_(l) = hardtanh;
          dphi_ptr_(l) = dhardtanh;
          break;
     case Activation_function::softmax:
          phi_ptr_(l) = softmax;
          dphi_ptr_(l) = dsoftmax;
          break;
     default:
          throw std::invalid_argument("bad activation function");
     }
     phi_(l) = f;
}

void MNN::C(Cost_function f) {
     switch (f) {
     case Cost_function::quadratic:
          C_ptr_ = quadratic;
          dC_ptr_ = dquadratic;
          break;
     case Cost_function::cross_entropy:
          C_ptr_ = cross_entropy;
          dC_ptr_ = dcross_entropy;
          break;
     default:
          throw std::invalid_argument("bad cost function");
     }
     C_ = f;
}

Vecreal MNN::aL(Vecreal const& x) const {
     using boost::numeric::ublas::prod;
     if (x.size() != n_(0))
          throw std::invalid_argument("bad dimension in aL");
     Vecreal a{x};
     for (Uint l = 1; l < W_.size(); l++) {
          Vecreal const z = prod(W_(l), a) + b_(l);
          a = phi_ptr_(l)(z);
     }
     return a;
}

void MNN::train(Vecreal const& x, Vecreal const& y) {
     using boost::numeric::ublas::prod;
     if (x.size() != n_(0) || y.size() != n_(n_.size() - 1))
          throw std::invalid_argument("bad dimension in train");
     Uint l;
     a_(0) = x;
     for (l = 1; l < W_.size(); l++) {
          z_(l) = prod(W_(l), a_(l - 1)) + b_(l);
          a_(l) = phi_ptr_(l)(z_(l));
     }
     assert(l == W_.size());
     l--;
     Vecreal delta = dC_ptr_(a_(l), y);
     Matreal dphi = dphi_ptr_(l)(z_(l), a_(l));
     Vecreal w = prod(delta, dphi);
     delta = w;
     for (;;) {
          Matreal Delta(n_(l), n_(l - 1));
          for (Matreal::size_type j = 0; j < Delta.size2(); j++) {
               Real const q = a_(l - 1)(j);
               for (Matreal::size_type i = 0; i < Delta.size1(); i++)
                    Delta(i, j) = delta(i) * q;
          }
          W_(l) -= eta_ * Delta;
          b_(l) -= eta_ * delta;
          if (l == 1)
               break;
          l--;
          w = prod(delta, W_(l + 1));
          dphi = dphi_ptr_(l)(z_(l), a_(l));
          delta = prod(w, dphi);
     }
}

Real MNN::cost(Vecreal const& x, Vecreal const& y) const {
     return C_ptr_(aL(x), y);
}

bool MNN::is_hit(Vecreal const& x, Vecreal const& y, Real eps) const {
     Vecreal const aL = this->aL(x);
     Real max = -1.0;
     Uint k = 0;
     for (Uint i = 0; i < aL.size(); i++)
          if (aL(i) > max) {
               max = aL(i);
               k = i;
          }
     return SHG::facmp(y(k), 1.0, eps) == 0;
}

void MNN::write(std::ostream& f) const {
     f << std::scientific
       << std::setprecision(std::numeric_limits<double>::digits10 +
                            1);
     f << n_.size() << '\n';
     for (auto n : n_)
          f << n << '\n';
     f << eta_ << '\n';
     for (Uint l = 1; l < W_.size(); l++) {
          Matreal const& w = W_(l);
          for (auto it1 = w.cbegin1(); it1 != w.cend1(); ++it1)
               for (auto it2 = it1.cbegin(); it2 != it1.cend(); ++it2)
                    f << *it2 << '\n';
          Vecreal const& b = b_(l);
          for (auto it = b.cbegin(); it != b.cend(); ++it)
               f << *it << '\n';
          f << static_cast<Activation_function_ut>(phi_(l)) << '\n';
     }
     f << static_cast<Cost_function_ut>(C_) << '\n';
}

bool MNN::write(char const* fname) const {
     std::ofstream f(fname,
                     std::ios_base::out | std::ios_base::binary);
     write(f);
     return f.good();
}

void MNN::read(std::istream& f) {
     if (f.fail())
          return;
     Vecuint::size_type size;
     f >> size;
     if (f.fail())
          return;
     Vecuint n(size);
     for (Vecuint::size_type i = 0; i < size; i++)
          f >> n(i);
     if (f.fail())
          return;
     MNN mnn;
     try {
          mnn.init(n);
          Real e;
          f >> e;
          mnn.eta(e);
     } catch (std::invalid_argument const&) {
          f.setstate(std::ios::failbit);
          return;
     }
     for (Uint l = 1; l < mnn.W_.size(); l++) {
          Matreal& w = mnn.W_(l);
          for (auto it1 = w.begin1(); it1 != w.end1(); ++it1)
               for (auto it2 = it1.begin(); it2 != it1.end(); ++it2)
                    f >> *it2;
          if (f.fail())
               return;
          Vecreal& b = mnn.b_(l);
          for (auto it = b.begin(); it != b.end(); ++it)
               f >> *it;
          if (f.fail())
               return;
          Activation_function_ut au;
          f >> au;
          if (f.fail())
               return;
          try {
               mnn.phi(static_cast<Activation_function>(au), l);
          } catch (std::invalid_argument const&) {
               f.setstate(std::ios::failbit);
               return;
          }
     }
     Cost_function_ut cu;
     f >> cu;
     if (f.fail())
          return;
     try {
          mnn.C(static_cast<Cost_function>(cu));
     } catch (std::invalid_argument const&) {
          f.setstate(std::ios::failbit);
          return;
     }
     *this = mnn;
}

bool MNN::read(char const* fname) {
     std::ifstream f(fname,
                     std::ios_base::in | std::ios_base::binary);
     read(f);
     return f.good();
}

bool facmp(MNN const& lhs, MNN const& rhs, double eps) {
     if (lhs.n().size() != rhs.n().size())
          return false;
     for (Vecuint::size_type l = 0; l < lhs.n().size(); l++)
          if (lhs.n()(l) != rhs.n()(l))
               return false;
     if (SHG::facmp(lhs.eta(), rhs.eta(), eps) != 0)
          return false;
     for (Uint l = 1; l < lhs.W().size(); l++) {
          if (!facmp(lhs.W()(l), rhs.W()(l), eps))
               return false;
          if (!facmp(lhs.b()(l), rhs.b()(l), eps))
               return false;
          if (lhs.phi(l) != rhs.phi(l))
               return false;
     }
     if (lhs.C() != rhs.C())
          return false;
     return true;
}

bool facmp(Vecreal const& lhs, Vecreal const& rhs, Real eps) {
     if (lhs.size() != rhs.size())
          return false;
     for (Vecreal::size_type i = 0; i < lhs.size(); i++)
          if (SHG::facmp(lhs(i), rhs(i), eps) != 0)
               return false;
     return true;
}

bool facmp(Matreal const& lhs, Matreal const& rhs, Real eps) {
     if (lhs.size1() != rhs.size1())
          return false;
     if (lhs.size2() != rhs.size2())
          return false;
     for (Matreal::size_type i = 0; i < lhs.size1(); i++)
          for (Matreal::size_type j = 0; j < lhs.size2(); j++)
               if (SHG::facmp(lhs(i, j), rhs(i, j), eps) != 0)
                    return false;
     return true;
}

bool is_standard_basis_vector(Vecreal const& v, Real eps) {
     bool found{false};
     for (Vecreal::size_type i = 0; i < v.size(); i++)
          if (SHG::facmp(v(i), 0.0, eps) != 0) {
               if (found)
                    return false;
               if (SHG::facmp(v(i), 1.0, eps) != 0)
                    return false;
               found = true;
          }
     return found;
}

}  // namespace SHG::Neural_networks
