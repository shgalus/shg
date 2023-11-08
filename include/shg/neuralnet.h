/**
 * \file include/shg/neuralnet.h
 * Multilayer neural networks.
 */

#ifndef SHG_NEURALNET_H
#define SHG_NEURALNET_H

#include <cassert>
#include <cstddef>
#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <istream>
#include <ostream>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

/**
 * Neural networks.
 */
namespace SHG::Neural_networks {

/**
 * \defgroup neural_networks Neural networks
 *
 * Neural networks.
 *
 * \{
 */

class Error : public std::runtime_error {
public:
     Error();
     explicit Error(std::string const& what);
     explicit Error(char const* what);
};

using Uint = unsigned int;
using Real = double;
template <class T>
using Vector = boost::numeric::ublas::vector<T>;
template <class T>
using Matrix = boost::numeric::ublas::matrix<T>;
using Vecuint = Vector<Uint>;
using Vecreal = Vector<Real>;
using Matreal = Matrix<Real>;
using Vecvecreal = Vector<Vecreal>;
using Vecmatreal = Vector<Matreal>;

/// \name Activation functions.
/// \{

Vecreal identity(Vecreal const& x);
Vecreal sign(Vecreal const& x);
Vecreal sigmoid(Vecreal const& x);
Vecreal tgh(Vecreal const& x);
Vecreal relu(Vecreal const& x);
Vecreal hardtanh(Vecreal const& x);
Vecreal softmax(Vecreal const& x);

/// \}

enum class Activation_function {
     identity,
     sign,
     sigmoid,
     tgh,
     relu,
     hardtanh,
     softmax,
};

/// \name Derivatives of activation functions.
/// \{

Matreal didentity(Vecreal const& x, Vecreal const& f);
Matreal dsign(Vecreal const& x, Vecreal const& f);
Matreal dsigmoid(Vecreal const& x, Vecreal const& f);
Matreal dtgh(Vecreal const& x, Vecreal const& f);
Matreal drelu(Vecreal const& x, Vecreal const& f);
Matreal dhardtanh(Vecreal const& x, Vecreal const& f);
Matreal dsoftmax(Vecreal const& x, Vecreal const& f);

/// \}

/// \name Cost functions.
/// \{

Real quadratic(Vecreal const& aL, Vecreal const& y);
Real cross_entropy(Vecreal const& aL, Vecreal const& y);

/// \}

enum class Cost_function {
     quadratic,
     cross_entropy,
};

/// \name Derivatives of cost functions.
/// \{

Vecreal dquadratic(Vecreal const& aL, Vecreal const& y);
Vecreal dcross_entropy(Vecreal const& aL, Vecreal const& y);

/// \}

class MNN {
public:
     MNN();
     explicit MNN(Vecuint const& n);
     void init(Vecuint const& n);

     Uint L() const;
     Vecuint const& n() const;
     Real eta() const;
     void eta(Real e);
     Vecmatreal const& W() const;
     Vecmatreal& W();
     Vecvecreal const& b() const;
     Vecvecreal& b();
     /** Returns Activation function of the l-th layer, 1 <= l < L. */
     Activation_function phi(Uint l) const;
     /** Sets activation function of the l-th layer, 1 <= l < L. */
     void phi(Activation_function f, Uint l);
     Cost_function C() const;
     void C(Cost_function f);

     Vecreal aL(Vecreal const& x) const;
     void train(Vecreal const& x, Vecreal const& y);
     /** Returns value of the cost function. */
     Real cost(Vecreal const& x, Vecreal const& y) const;
     /** For classification returns true if the class is correctly
      * identified. */
     bool is_hit(Vecreal const& x, Vecreal const& y, Real eps) const;

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
     using Activation_function_ptr = Vecreal (*)(Vecreal const&);
     using Activation_function_derivative_ptr =
          Matreal (*)(Vecreal const&, Vecreal const&);
     using Cost_function_ptr = Real (*)(Vecreal const&,
                                        Vecreal const&);
     using Cost_function_derivative_ptr = Vecreal (*)(Vecreal const&,
                                                      Vecreal const&);
     using Activation_function_ut =
          std::underlying_type<Activation_function>::type;
     using Cost_function_ut =
          std::underlying_type<Cost_function>::type;

     Vecuint n_{};
     Real eta_{};
     Vecmatreal W_{};
     Vecvecreal b_{};
     Vecvecreal a_{};
     Vecvecreal z_{};
     Vector<Activation_function> phi_{};
     Vector<Activation_function_ptr> phi_ptr_{};
     Vector<Activation_function_derivative_ptr> dphi_ptr_{};
     Cost_function C_{};
     Cost_function_ptr C_ptr_{};
     Cost_function_derivative_ptr dC_ptr_{};
};

/**
 * Compares two MNNs. \f$\epsilon > 0\f$ is used to absolutely compare
 * weights, biases and learning rate.
 */
bool facmp(MNN const& lhs, MNN const& rhs, double eps);

struct Mnistdhd_example {
     Vecreal image{784};
     Vecreal label{10};
};

using Mnistdhd = Vector<Mnistdhd_example>;

/**
 * Loads training or test data from the MNIST database of handwritten
 * digits. See \ref mnist_database.
 *
 * \param[in]  path  Directory of database files.
 * \param[in]  kind  Which set to load: \c train or \c t10k.
 * \throws std::invalid_argument if \c kind has illegal value
 * \throws std::runtime_error if another error occurs
 *
 * \implementation Written according to specification in \ref
 * mnist_database and \cite raschka-2016, pages 351-356.
 */
Mnistdhd mnistdhd(char const* path, char const* kind);

template <typename T>
Vector<T> make_vector(std::initializer_list<T> il);

template <typename T>
Matrix<T> make_matrix(std::size_t size1, std::size_t size2,
                      std::initializer_list<T> il);

bool facmp(Vecreal const& lhs, Vecreal const& rhs, Real eps);
bool facmp(Matreal const& lhs, Matreal const& rhs, Real eps);

/**
 * Checks if \c v is a vector of standard basis. \c v is a vector of
 * standard basis if all its components are equal to 0 except one
 * which is 1.
 *
 * \param[in] v a vector to check
 * \param[in] eps tolerance, should be greater than 0
 * \returns true if \c is a vector of standard basis, otherwise false
 */
bool is_standard_basis_vector(Vecreal const& v, Real eps);

inline Uint MNN::L() const {
     return n_.size();
}

inline Vecuint const& MNN::n() const {
     return n_;
}

inline Real MNN::eta() const {
     return eta_;
}

inline Vecmatreal const& MNN::W() const {
     return W_;
}

inline Vecmatreal& MNN::W() {
     return W_;
}

inline Vecvecreal const& MNN::b() const {
     return b_;
}

inline Vecvecreal& MNN::b() {
     return b_;
}

inline Cost_function MNN::C() const {
     return C_;
}

template <typename T>
Vector<T> make_vector(std::initializer_list<T> il) {
     Vector<T> v(il.size());
     std::copy(il.begin(), il.end(), v.begin());
     return v;
}

template <typename T>
Matrix<T> make_matrix(std::size_t size1, std::size_t size2,
                      std::initializer_list<T> il) {
     assert(size1 * size2 == il.size());
     Matrix<T> a(size1, size2);
     auto it = il.begin();
     for (auto it1 = a.begin1(); it1 != a.end1(); ++it1)
          for (auto it2 = it1.begin(); it2 != it1.end(); ++it2)
               *it2 = *(it++);
     return a;
}

/** \} */

}  // namespace SHG::Neural_networks

#endif
