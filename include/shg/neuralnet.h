/**
 * \file include/shg/neuralnet.h
 * Multilayer neural networks.
 */

#ifndef SHG_NEURALNET_H
#define SHG_NEURALNET_H

#include <cmath>
#include <stdexcept>
#include <vector>
#include <istream>
#include <ostream>
#include <boost/numeric/ublas/matrix.hpp>

/**
 * Neural networks.
 */
namespace SHG::Neural_networks {

/**
 * %Exception class for invalid floating-point numbers. Thrown if a
 * function argument or function result is not a finite floating point
 * number as checked by std::isfinite();
 */
class Invalid_number : public std::runtime_error {
public:
     Invalid_number();
};

/** Throws Invalid_number if !%std::isfinite(x). */
void check(double x);

/** Identity function. Returns \f$x\f$. */
double identity(double x);

/** Sign function. Returns
 * \f$\begin{cases}
 *  -1 & \text{if $x < 0$}, \\
 *   0 & \text{if $x = 0$}, \\
 *   1 & \text{if $x > 0$}.
 * \end{cases}\f$
 */
double sign(double x);

/** Sigmoid function. Returns \f$1 / (1 + e^{-x})\f$. */
double sigmoid(double x);

/** Hyperbolic tangent. Returns \f$ \frac{e^x - e^{-x}}{e^x + e^{-x}}
    \f$. */
double tgh(double x);

/** Rectified linear unit (ReLU). Returns
 * \f$\begin{cases}
 *  0 & \text{if $x \leq 0$}, \\
 *  x & \text{if $x > 0$}.
 * \end{cases}\f$
 */
double relu(double x);

/** Hard hyperbolic tangent. Returns
 * \f$\begin{cases}
 *  -1 & \text{if $x < -1$}, \\
 *   x & \text{if $-1 \leq x \leq 1$}, \\
 *   1 & \text{if $x > 1$}.
 * \end{cases}\f$
 */
double hardtanh(double x);

/** Softmax. Returns \f$ e^{x_i} / (e^{x_1} + \ldots + e^{x_n})\f$ for
 *  \f$i = 1, \ldots, n\f$, where \f$n\f$ is the number of
 *  variables.
 */
std::vector<double> softmax(std::vector<double> const& x);

std::vector<double> identity(std::vector<double> const& x,
                             std::vector<double> const& a,
                             std::vector<double> const& b);

std::vector<double> sign(std::vector<double> const& x,
                         std::vector<double> const& a,
                         std::vector<double> const& b);

std::vector<double> sigmoid(std::vector<double> const& x,
                            std::vector<double> const& a,
                            std::vector<double> const& b);

std::vector<double> tgh(std::vector<double> const& x,
                        std::vector<double> const& a,
                        std::vector<double> const& b);

std::vector<double> relu(std::vector<double> const& x,
                         std::vector<double> const& a,
                         std::vector<double> const& b);

std::vector<double> hardtanh(std::vector<double> const& x,
                             std::vector<double> const& a,
                             std::vector<double> const& b);

std::vector<double> softmax(std::vector<double> const& x,
                            std::vector<double> const& a,
                            std::vector<double> const& b);

using Act_func = std::vector<double>(std::vector<double> const&,
                                     std::vector<double> const&,
                                     std::vector<double> const&);

extern std::vector<Act_func*> const activation_functions;

enum class Activation_function {
     identity,
     sign,
     sigmoid,
     tgh,
     relu,
     hardtanh,
     softmax
};

/** Quadratic loss. Returns \f$\sum_{i = 1}^m (t_i - y_i)^2\f$. */
double quadratic_loss(std::vector<double> const& t,
                      std::vector<double> const& y);

/** Hinge loss. Returns \f$\max\{0, 1 - ty\}\f$, \f$t\f$ and \f$y\f$
    must be vectors of size 1. */
double hinge_loss(std::vector<double> const& t,
                  std::vector<double> const& y);

using Loss_function = double(std::vector<double> const&,
                             std::vector<double> const&);

/**
 * A table of loss functions. The first argument should be the true
 * value, the second argument should be the approximation.
 * Contents:
 *   - 0: quadratic loss
 *   - 1: hinge loss
 */
extern std::vector<Loss_function*> const loss_functions;

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
 * An example of a 2-layer (\f$k = 2\f$) network is shown below.
 *
\verbatim
+---+              +---+              +---+              +---+
|   |\Phi_1\Sigma_1|   |\Phi_2\Sigma_2|   |\Phi_3\Sigma_3|   |
| x |------------->|h_1|------------->|h_2|------------->| y |
|   |              |   |              |   |              |   |
+---+              +---+              +---+              +---+
 R^n              R^{p_1}            R^{p_2}              R^m
\endverbatim
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
     MNN() = default;
     MNN(int n, int m, std::vector<int> const& p);
     /**
      * Initializes a network. \f$n \geq 1\f$ is a dimension of the
      * input layer. \f$m \geq 1\f$ is a dimension of the output
      * layer. The number of hidden layers \f$k \geq 1\f$ will be
      * equal to \f$\var{p.size()}\f$.
      */
     void init(int n, int m, std::vector<int> const& p);
     /**
      * Sets the activation function \f$f\f$ for the \f$i\f$-th layer.
      * There must be \f$1 \leq i \leq k + 1\f$. If \f$i = k + 1\f$,
      * activation function for the output layer is set. The parameter
      * \f$x_0\f$ is a threshold and the parameter \f$s > 0\f$ is a
      * scale. \f$x_0\f$ and \f$s\f$ are used only for activation
      * functions of one variable: instead of \f$f(x)\f$, \f$f((x -
      * x_0) / s)\f$ will be called.
      */
     void set_activation_function(int i, Activation_function f,
                                  double x0 = 0.0, double s = 1.0);
     void set_random_weights();
     void set_loss_function(int loss);

     int n() const;
     int m() const;
     int k() const;
     std::vector<int> const& p() const;

     /** Returns value \f$y = f(x)\f$. */
     std::vector<double> y(std::vector<double> const& x) const;

     /** Writes this MNN to the stream. f.good() indicates success. */
     void write(std::ostream& f) const;
     /** Writes this MNN to the file. Returns true on success. */
     bool write(char const* fname) const;

     /** Reads this MNN from the stream. f.good() indicates success.
      */
     void read(std::istream& f);
     /** Reads this MNN from the file. Returns true on success. */
     bool read(char const* fname);

private:
     using Matrix = boost::numeric::ublas::matrix<double>;

     /** Calculates \f$h = \Phi_k(u)\f$. */
     void phi(std::vector<Matrix>::size_type k,
              std::vector<double> const& u,
              std::vector<double>& h) const;

     /// \todo When write(), check f and f2 if all activations are
     /// valid.
     struct Activation {
          using F = double(double);
          using F2 = std::vector<double>(std::vector<double> const&);
          Activation_function af{};
          F* f{nullptr};
          F2* f2{nullptr};
          double x0{0.0};  // threshold
          double s{1.0};   // scale
     };

     friend bool fcmp(MNN const& lhs, MNN const& rhs, double eps);

     int n_{};
     int m_{};
     int k_{};
     std::vector<int> p_{};
     std::vector<Matrix> w_{};
     std::vector<Activation> phi_{};
     int loss_{-1};
};

inline void check(double x) {
     if (!std::isfinite(x))
          throw Invalid_number();
}

/**
 * Compares two MNNs. \f$\epsilon > 0\f$ is used to absolutely compare
 * weights, thresholds and scales.
 */
bool fcmp(MNN const& lhs, MNN const& rhs, double eps);

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
