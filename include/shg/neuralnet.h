/**
 * \file include/shg/neuralnet.h
 * Multilayer neural networks.
 */

#ifndef SHG_NEURALNET_H
#define SHG_NEURALNET_H

#include <vector>
#include <boost/numeric/ublas/matrix.hpp>

/**
 * Neural networks.
 */
namespace SHG::Neural_networks {

/** Sign function. Returns
 * \f$\begin{cases}
 *  -1 & \text{if $x < 0$}, \\
 *   0 & \text{if $x = 0$}, \\
 *   1 & \text{if $x \geq 0$}.
 * \end{cases}\f$
 */
double sign(double x);

/** Sigmoid function. Returns \f$1 / (1 + e^{-x})\f$. */
double sigmoid(double x);

/**
 * Multilayer neural network.
 *
 * Let \f$k \in \naturaln\f$, \f$p_0, p_1, \ldots, p_{k + 1} \in
 * \naturaln\f$, \f$n = p_0\f$, \f$m = p_{k + 1}\f$.
 * Let, for \f$i = 1, \ldots, k + 1\f$, \f$\Sigma_i \colon
 * \realn^{p_{i - 1}} \rightarrow \realn^{p_i}\f$, \f$\Phi_i \colon
 * \realn^{p_i} \rightarrow \realn^{p_i}\f$.
 * Let \f$x \in \realn^n\f$.
 * Denote \f{align*}{
 * h_1 &= \Phi_1(\Sigma_1(x)) \\
 * h_{i + 1} &= \Phi_{i + 1}(\Sigma_{i + 1}(h_i)), \quad i = 1,
 * \ldots, k - 1, \\
 * y &= \Phi_{k + 1}(\Sigma_{k + 1}(h_k)). \f}
 * The function \f$f \colon \realn^n \rightarrow \realn^m\f$, \f$f(x)
 * = y\f$ is called a \f$k\f$-layer neural network.
 *
 * This definition follows \cite aggarwal-2018, section 1.2.2.
 *
 * \f$x\f$ is an input layer, \f$h_1, \ldots, h_k\f$ are hidden
 * layers, \f$y\f$ is an output layer. \f$n\f$ is the dimensionality
 * of the input layer, the vector \f$p = [p_1, \ldots, p_k]\f$
 * contains dimensionalities of the hidden layers, \f$m\f$ is the
 * dimensionality of the output layer.
 *
 * For \f$i = 1, \ldots, k + 1\f$, \f$\Sigma_i(u) = W_i \times u\f$,
 * where \f$W_i\f$ is an \f$p_i \times p_{i - 1}\f$ matrix of weights
 * and \f$u = [u_1 \ldots u_{p_{i - 1}}]^T\f$.
 *
 * For \f$i = 1, \ldots, k + 1\f$, \f$\Phi_i(v) = \left( \Phi_{i1}(v),
 * \ldots, \Phi_{ip_i}(v) \right)\f$, where \f$\Phi_{ij} \colon
 * \realn^{p_i} \rightarrow \realn\f$, \f$j = 1, \ldots, p_i\f$, and
 * \f$v = (v_1, \ldots, v_{p_i})\f$.
 *
 */
class MNN {
public:
     using Matrix = boost::numeric::ublas::matrix<double>;

     MNN() = default;
     MNN(int n, int m, std::vector<int> const& p);
     void init(int n, int m, std::vector<int> const& p);
     int n() const;
     int m() const;
     int k() const;
     std::vector<int> const& p() const;

     /** Returns value \f$y = f(x)\f$. */
     std::vector<double> y(std::vector<double> const& x) const;

private:
     int n_{};
     int m_{};
     int k_{};
     std::vector<int> p_{};
     std::vector<Matrix> w_{};
};

inline int MNN::n() const {
     return n_;
}
inline int MNN::m() const {
     return m_;
}
inline int MNN::k() const {
     return k_;
}
inline std::vector<int> const& MNN::p() const {
     return p_;
}

}  // namespace SHG::Neural_networks

#endif
