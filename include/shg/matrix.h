/**
 * \file include/shg/matrix.h
 *  Matrix class and functions.
 * \date Created on 9 November 2013.
 */

#ifndef SHG_MATRIX_H
#define SHG_MATRIX_H

#include <cstdlib>
#include <ios>
#include <limits>
#include <utility>
#include <shg/vector.h>

namespace SHG {

/** \addtogroup vector_and_matrix */
/** \{ */

/**
 * %Matrix class.
 */
template <class T>
class Matrix {
public:
     typedef T value_type;
     typedef std::size_t size_type;

     /** \name Construct/destroy. */
     /** \{ */

     /** Constructs an empty matrix. */
     inline Matrix();

     /**
      * Constructs an <em>m &times; n</em> matrix with
      * value-initialized elements.
      */
     Matrix(std::size_t m, std::size_t n);

     /**
      * Constructs an <em>m &times; n</em> matrix with all elements
      * equal to \a a.
      */
     inline Matrix(std::size_t m, std::size_t n, const T& a);

     /**
      * Constructs from C memory block.
      */
     inline Matrix(std::size_t m, std::size_t n, const T* a);

     /**
      * Constructs from C two-dimensional array.
      */
     Matrix(std::size_t m, std::size_t n, const T* const* a);

     /**
      * Constructs from Vector.
      */
     Matrix(std::size_t m, std::size_t n, const Vector<T>& v);

     /**
      * Move constructor from Vector.
      */
     Matrix(std::size_t m, std::size_t n, Vector<T>&& v);

     /**
      * Constructs a matrix from an initializer list. Matrix elements
      * are initialized by rows. If the length of the list is less
      * than \a m &times; \a n, given elements are used cyclically. If
      * the list is empty, nothing happens.
      */
     Matrix(std::size_t m, std::size_t n,
            std::initializer_list<T> il);

     /**
      * Copy constructor.
      */
     Matrix(const Matrix& a);

     /**
      * Move constructor.
      */
     inline Matrix(Matrix&& a) noexcept;

     /**
      * Destructor.
      */
     virtual ~Matrix();

     /** \} */
     /** \name Assignment. */
     /** \{ */

     /**
      * Copy assignment.
      */
     Matrix& operator=(const Matrix& a);

     /**
      * Move assignment.
      */
     inline Matrix& operator=(Matrix&& a) noexcept(
          noexcept(std::is_nothrow_move_constructible<T>::value&&
                        std::is_nothrow_move_assignable<T>::value));

     /**
      * Assigns \a a to all elements
      */
     inline Matrix& operator=(const T& a);

     /**
      * Assigns an initializer list. Matrix elements are assigned by
      * rows. If the length of the list is less than \a m &times; \a
      * n, given elements are used cyclically. If the list is empty,
      * nothing happens.
      */
     Matrix& operator=(std::initializer_list<T> il);

     /** \} */
     /** \name Element access. */
     /** \{ */

     /**
      * Returns a pointer to the \a i-th row.
      */
     inline T* operator[](std::size_t i);

     /**
      * Returns a constant pointer to the \a i-th row.
      */
     inline const T* operator[](std::size_t i) const;

     /**
      * Returns element in row \a i and column \a j.
      */
     inline T& operator()(std::size_t i, std::size_t j);

     /**
      * Returns constant element in row \a i and column \a j.
      */
     inline const T& operator()(std::size_t i, std::size_t j) const;

     /**
      * Returns a reference to the element <em>(i, j)</em> with range
      * checking.
      *
      * \exception std::out_of_range if !(i < nrows() && j < ncols())
      */
     T& at(std::size_t i, std::size_t j);

     /**
      * Returns a constant reference to the element <em>(i, j)</em>
      * with range checking.
      *
      * \exception std::out_of_range if !(i < nrows() && j < ncols())
      */
     const T& at(std::size_t i, std::size_t j) const;

     /** \} */
     /** \name Member functions. */
     /** \{ */

     /**
      * Returns the number of rows in the matrix.
      */
     inline std::size_t nrows() const;

     /**
      * Returns the number of columns in the matrix.
      */
     inline std::size_t ncols() const;

     /**
      * Changes the dimensions of the matrix to \a m by \a n. The
      * elements are value-initialized.
      */
     void resize(std::size_t m, std::size_t n);

     /**
      * Changes the dimensions of the matrix to \a m by \a n and then
      * assigns to each element the value \a a.
      */
     void assign(std::size_t m, std::size_t n, const T& a);

     /**
      * Returns a pointer to the memory block where the data of matrix
      * are stored.
      */
     inline T* c_vec();

     /**
      * Returns a constant pointer to the memory block where the data
      * of matrix are stored.
      */
     inline const T* c_vec() const;

     /**
      * Returns this matrix as a C-style matrix.
      */
     inline T* const* c_mat();

     /**
      * Returns this matrix as a constant C-style matrix.
      */
     inline T const* const* c_mat() const;

     /**
      * Returns this matrix as a Vector.
      */
     inline Vector<T>& vector();

     /**
      * Returns this matrix as a constant Vector.
      */
     inline const Vector<T>& vector() const;

     /**
      * Exchanges values of \a *this and \a a.
      */
     void swap(Matrix& a) noexcept(
          noexcept(std::is_nothrow_move_constructible<T>::value&&
                        std::is_nothrow_move_assignable<T>::value));

     /** \} */

protected:
     Vector<T> v_;   /**< data */
     Vector<T*> p_;  /**< pointers to rows */
     std::size_t m_; /**< the number of rows */
     std::size_t n_; /**< the number of columns */
};

/** \name Matrix typedefs for some fundamental types. */
/** \{ */

typedef Matrix<bool> Matbool;
typedef Matrix<char> Matchar;
typedef Matrix<short> Matshort;
typedef Matrix<int> Matint;
typedef Matrix<long> Matlong;
typedef Matrix<long long> Matlonglong;
typedef Matrix<float> Matfloat;
typedef Matrix<double> Matdouble;
typedef Matrix<long double> Matlongdouble;

/** \} */ /* matrix typedefs for some fundamental types */

/** \name Matrix non-member functions. */
/** \{ */

/**
 * Compares two matrices.
 *
 * \returns true if the two matrices have the same dimensions and a(i,
 * j) == b(i, j) for all i, j, false otherwise.
 */
template <class T>
bool equal(const Matrix<T>& a, const Matrix<T>& b);

/**
 * \copydoc equal(const Matrix<T>& a, const Matrix<T>& b)
 */
template <class T>
inline bool operator==(const Matrix<T>& a, const Matrix<T>& b);

/**
 * Returns the sum of all the elements of the matrix. Operator \a +=
 * must be defined for the type \a T. If the matrix is empty, the
 * behaviour is undefined.
 */
template <class T>
inline T sum(const Matrix<T>& a);

/**
 * Returns the minimum value contained in \a *this. The value for an
 * empty matrix is undefined.
 */
template <class T>
inline T min(const Matrix<T>& a);

/**
 * Returns the maximum value contained in \a *this. The value for an
 * empty matrix is undefined.
 */
template <class T>
inline T max(const Matrix<T>& a);

/**
 * Returns the minimum and maximum values contained in \a *this. The
 * value for an empty matrix is undefined.
 */
template <class T>
inline std::pair<T, T> minmax(const Matrix<T>& a);

/**
 * Returns index of the smallest element. Uses operator< to compare
 * values. If the matrix is empty, 0 is returned.
 */
template <class T>
inline std::pair<std::size_t, std::size_t> minloc(const Matrix<T>& a);

/**
 * Returns index of the greatest element. Uses operator< to compare
 * values. If the matrix is empty, 0 is returned.
 */
template <class T>
inline std::pair<std::size_t, std::size_t> maxloc(const Matrix<T>& a);

/**
 * Returns indexes of the smallest and the greatest element. Uses
 * operator< to compare values. If the matrix is empty, 0 is returned.
 */
template <class T>
inline std::pair<std::pair<std::size_t, std::size_t>,
                 std::pair<std::size_t, std::size_t>>
minmaxloc(const Matrix<T>& a);

/**
 * Changes the dimensions of the matrix to 0 by 0.
 */
template <class T>
inline void clear(Matrix<T>& a);

/**
 * Exchanges values of \a *this and \a a.
 */
template <class T>
inline void swap(Matrix<T>& a, Matrix<T>& b) noexcept(
     noexcept(std::is_nothrow_move_constructible<T>::value&&
                   std::is_nothrow_move_assignable<T>::value));

/**
 * Outputs matix to a text stream. First, the dimensions of the matrix
 * are output with width zero followed by a newline character. Then
 * all elements are printed with each row on a line with the width set
 * just before a call to this function. stream.fail() should be
 * checked after return.
 */
template <class T>
std::ostream& operator<<(std::ostream& stream, const Matrix<T>& a);

/**
 * Inputs matrix from a text stream. The input should be as output
 * from std::ostream& operator<<(std::ostream&, const Matrix<T>&).
 * stream.fail() should be checked after return. If the operation
 * failed, previous content of \a a is untouched.
 */
template <class T>
std::istream& operator>>(std::istream& stream, Matrix<T>& a);

/**
 * Outputs matrix to a text stream in the form of initializer list.
 * stream.fail() should be checked after return.
 */
template <class T>
void print(const Matrix<T>& a, std::ostream& stream);

/**
 * Writes this matrix to a binary stream. Check f.fail() to check if
 * the operation was successful.
 */
template <class T>
void write(const Matrix<T>& a, std::ostream& f);

/**
 * Reads this matrix from a binary stream. Check f.fail() to check if
 * the operation was successful. If the operation fails, the matrix
 * remains unchanged.
 */
template <class T>
void read(Matrix<T>& a, std::istream& f);

/**
 * Returns maximum norm distance between two matrices. The norm is
 * defined as \f$ \max \{|a_{ij} - b_{ij}| \colon 0 \leq i < m, 0 \leq
 * j < n \} \f$, where \f$m\f$ and \f$n\f$ are the common dimensions
 * of the matrices. If the matrices are empty, 0 is returned.
 *
 * \exception std::invalid_argument if !(a.nrows() == b.nrows() &&
 * a.ncols() == b.ncols())
 */
template <class T>
T maximum_norm_distance(const Matrix<T>& a, const Matrix<T>& b);

/**
 * Returns diagonal matrix with \a c on the main diagonal.
 */
template <class T>
Matrix<T> diagonal_matrix(std::size_t n, const T& c = 1);

/**
 * Returns matrix transposed to \a a.
 */
template <class T>
Matrix<T> transpose(const Matrix<T>& a);

/**
 * Transposes the square matrix \a a in situ.
 * \returns reference to \a a
 * \exception std::invalid_argument if (a.nrows() != a.ncols())
 */
template <class T>
Matrix<T>& transpose_in_situ(Matrix<T>& a);

/**
 * Returns the multiplication of \a a and \a b.
 * \exception std::invalid_argument if a.ncols() != b.nrows()
 */
template <class T>
Matrix<T> multiply(const Matrix<T>& a, const Matrix<T>& b);

/**
 * Performs operation \f$ a \leftarrow a \times b \f$. \f$b\f$ must be
 * a square matrix with the number of rows equal to the number of
 * columns in \f$a\f$. The matrices must be different.
 *
 * \exception std::invalid_argument if &a == &b || a.ncols() !=
 * b.nrows() || b.nrows() != b.ncols()
 */
template <class T>
void right_multiply_and_assign(Matrix<T>& a, const Matrix<T>& b);

/**
 * Returns the matrix \f$a^Ta\f$.
 */
template <class T>
Matrix<T> left_multiply_by_transposition(const Matrix<T>& a);

/**
 * Inverts matrix by Cholesky decomposition. Calculates in situ the
 * inverse matrix for a symmetric and positive definite matrix by
 * Cholesky decomposition. The function does not check if the matrix
 * is symmetric and uses only the upper-right triangle of the matrix.
 *
 * \tparam T floating-point type
 * \param [inout] a matrix to invert
 * \param [in] eps if a number is less than or equal to \a eps, it is
 * treated as 0
 * \exception std::invalid_argument if a.nrows() != a.ncols() || eps <
 * 0 \exception std::range_error if the matrix is not positive
 * definite
 *
 * \note In case of exception elements of the matrix are undefined.
 *
 * \implementation See \cite marciniak-gregulec-kaczmarek-1991, p.
 * 90-92.
 */
template <class T>
void cholesky(Matrix<T>& a,
              T eps = 4 * std::numeric_limits<T>::epsilon());

/**
 * Returns an <em>n &times; n</em> Hilbert matrix. The Hilbert matrix
 * is defined by \f$a_{ij} = 1 / (i + j + 1) \; i, j = 0, \ldots, n -
 * 1\f$.
 */
template <class T>
Matrix<T> hilbert_matrix(std::size_t n);

/**
 * Returns the vector \f$av\f$.
 * \exception std::invalid_argument if a.ncols() != v.size()
 */
template <class T>
Vector<T> multiply(const Matrix<T>& a, const Vector<T>& v);

/**
 * Returns the vector \f$a^Tv\f$.
 * \exception std::invalid_argument if a.nrows() != v.size()
 */
template <class T>
Vector<T> multiply_transposed(const Matrix<T>& a, const Vector<T>& v);

/** \} */ /* end of matrix non-member functions */

/** \} */  // end of group vector_and_matrix

template <class T>
Matrix<T>::Matrix() : v_(), p_(), m_(0), n_(0) {}

template <class T>
Matrix<T>::Matrix(std::size_t m, std::size_t n)
     : v_(m * n), p_(n > 0 ? m : 0), m_(m), n_(n) {
     if (m_ > 0 && n_ > 0) {
          T* p = p_[0] = &(v_[0]);
          std::generate(p_.begin() + 1, p_.end(),
                        [&p, this]() { return p += this->n_; });
     } else {
          m_ = n_ = 0;
     }
}

template <class T>
Matrix<T>::Matrix(std::size_t m, std::size_t n, const T& a)
     : Matrix(m, n) {
     v_ = a;
}

template <class T>
Matrix<T>::Matrix(std::size_t m, std::size_t n, const T* a)
     : Matrix(m, n) {
     std::copy_n(a, m_ * n_, v_.begin());
}

template <class T>
Matrix<T>::Matrix(std::size_t m, std::size_t n, const T* const* a)
     : Matrix(m, n) {
     for (std::size_t i = 0; i < m; i++)
          for (std::size_t j = 0; j < n; j++)
               p_[i][j] = a[i][j];
}

template <class T>
Matrix<T>::Matrix(std::size_t m, std::size_t n, const Vector<T>& v)
     : Matrix(m, n) {
     if (v.size() != m * n)
          throw std::invalid_argument(__func__);
     v_ = v;
}

template <class T>
Matrix<T>::Matrix(std::size_t m, std::size_t n, Vector<T>&& v)
     : Matrix() {
     if (m > 0 && n > 0) {
          if (v.size() != m * n)
               throw std::invalid_argument(__func__);
          v_ = std::move(v);
          p_.resize(m);
          m_ = m;
          n_ = n;
          T* p = p_[0] = &(v_[0]);
          std::generate(p_.begin() + 1, p_.end(),
                        [&p, this]() { return p += this->n_; });
     } else {
          m_ = n_ = 0;
     }
}

template <class T>
Matrix<T>::Matrix(std::size_t m, std::size_t n,
                  std::initializer_list<T> il)
     : Matrix(m, n) {
     typename std::initializer_list<T>::const_iterator it =
          il.begin();
     if (it != il.end()) {
          for (std::size_t i = 0; i < v_.size(); i++) {
               v_[i] = *it;
               if (++it == il.end())
                    it = il.begin();
          }
     }
}

template <class T>
Matrix<T>::Matrix(const Matrix& a)
     : v_(a.v_), p_(a.p_.size()), m_(a.m_), n_(a.n_) {
     if (m_ > 0 && n_ > 0) {
          T* p = p_[0] = &(v_[0]);
          std::generate(p_.begin() + 1, p_.end(),
                        [&p, this]() { return p += this->n_; });
     }
}

template <class T>
Matrix<T>::Matrix(Matrix&& a) noexcept : Matrix() {
     swap(a);
}

template <class T>
Matrix<T>::~Matrix() {}

template <class T>
Matrix<T>& Matrix<T>::operator=(const Matrix& a) {
     if (this != &a) {
          resize(a.m_, a.n_);
          v_ = a.v_;
     }
     return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator=(Matrix&& a) noexcept(
     noexcept(std::is_nothrow_move_constructible<T>::value&&
                   std::is_nothrow_move_assignable<T>::value)) {
     swap(a);
     return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator=(const T& a) {
     v_ = a;
     return *this;
}

template <class T>
Matrix<T>& Matrix<T>::operator=(const std::initializer_list<T> il) {
     typename std::initializer_list<T>::const_iterator it =
          il.begin();
     if (it != il.end()) {
          for (std::size_t i = 0; i < v_.size(); i++) {
               v_[i] = *it;
               if (++it == il.end())
                    it = il.begin();
          }
     }
     return *this;
}

template <class T>
T* Matrix<T>::operator[](std::size_t i) {
     return p_[i];
}

template <class T>
const T* Matrix<T>::operator[](std::size_t i) const {
     return p_[i];
}

template <class T>
T& Matrix<T>::operator()(std::size_t i, std::size_t j) {
     return *(p_[i] + j);
}

template <class T>
const T& Matrix<T>::operator()(std::size_t i, std::size_t j) const {
     return *(p_[i] + j);
}

template <class T>
T& Matrix<T>::at(std::size_t i, std::size_t j) {
     if (i < nrows() && j < ncols())
          return *(p_[i] + j);
     throw std::out_of_range(__func__);
}

template <class T>
const T& Matrix<T>::at(std::size_t i, std::size_t j) const {
     if (i < nrows() && j < ncols())
          return *(p_[i] + j);
     throw std::out_of_range(__func__);
}

template <class T>
std::size_t Matrix<T>::nrows() const {
     return m_;
}

template <class T>
std::size_t Matrix<T>::ncols() const {
     return n_;
}

template <class T>
void Matrix<T>::resize(std::size_t m, std::size_t n) {
     if (m != m_ || n != n_) {
          v_.resize(0);
          p_.resize(0);
          m_ = 0;
          n_ = 0;
          Matrix<T> a(m, n);
          swap(a);
     }
}

template <class T>
void Matrix<T>::assign(std::size_t m, std::size_t n, const T& a) {
     resize(m, n);
     v_ = a;
}

template <class T>
T* Matrix<T>::c_vec() {
     return v_.c_vec();
}

template <class T>
const T* Matrix<T>::c_vec() const {
     return v_.c_vec();
}

template <class T>
T* const* Matrix<T>::c_mat() {
     return p_.c_vec();
}

template <class T>
T const* const* Matrix<T>::c_mat() const {
     return p_.c_vec();
}

template <class T>
Vector<T>& Matrix<T>::vector() {
     return v_;
}

template <class T>
const Vector<T>& Matrix<T>::vector() const {
     return v_;
}

template <class T>
void Matrix<T>::swap(Matrix& a) noexcept(
     noexcept(std::is_nothrow_move_constructible<T>::value&&
                   std::is_nothrow_move_assignable<T>::value)) {
     v_.swap(a.v_);
     p_.swap(a.p_);
     std::swap(m_, a.m_);
     std::swap(n_, a.n_);
}

template <class T>
bool equal(const Matrix<T>& a, const Matrix<T>& b) {
     return (a.nrows() == b.nrows()) && (a.ncols() == b.ncols()) &&
            equal(a.vector(), b.vector());
}

template <class T>
bool operator==(const Matrix<T>& a, const Matrix<T>& b) {
     return equal(a, b);
}

template <class T>
T sum(const Matrix<T>& a) {
     return sum(a.vector());
}

template <class T>
T min(const Matrix<T>& a) {
     return min(a.vector());
}

template <class T>
T max(const Matrix<T>& a) {
     return max(a.vector());
}

template <class T>
std::pair<T, T> minmax(const Matrix<T>& a) {
     return minmax(a.vector());
}

template <class T>
std::pair<std::size_t, std::size_t> minloc(const Matrix<T>& a) {
     const auto d = std::div(
          static_cast<std::ptrdiff_t>(minloc(a.vector())), a.ncols());
     return std::make_pair(d.quot, d.rem);
}

template <class T>
std::pair<std::size_t, std::size_t> maxloc(const Matrix<T>& a) {
     const auto d = std::div(
          static_cast<std::ptrdiff_t>(maxloc(a.vector())), a.ncols());
     return std::make_pair(d.quot, d.rem);
}

template <class T>
std::pair<std::pair<std::size_t, std::size_t>,
          std::pair<std::size_t, std::size_t>>
minmaxloc(const Matrix<T>& a) {
     const std::pair<std::size_t, std::size_t> p =
          minmaxloc(a.vector());

     const auto d1 =
          std::div(static_cast<std::ptrdiff_t>(p.first), a.ncols());
     const auto d2 =
          std::div(static_cast<std::ptrdiff_t>(p.second), a.ncols());
     return std::make_pair(std::make_pair(d1.quot, d1.rem),
                           std::make_pair(d2.quot, d2.rem));
}

template <class T>
void clear(Matrix<T>& a) {
     a.resize(0, 0);
}

template <class T>
void swap(Matrix<T>& a, Matrix<T>& b) noexcept(
     noexcept(std::is_nothrow_move_constructible<T>::value&&
                   std::is_nothrow_move_assignable<T>::value)) {
     a.swap(b);
}

template <class T>
std::ostream& operator<<(std::ostream& stream, const Matrix<T>& a) {
     const std::streamsize w = stream.width(0);
     stream << a.nrows() << ' ' << a.ncols() << '\n';
     for (std::size_t i = 0; i < a.nrows(); i++) {
          if (a.ncols() > 0) {
               stream.width(w);
               stream << a[i][0];
               for (std::size_t j = 1; j < a.ncols(); j++) {
                    stream << ' ';
                    stream.width(w);
                    stream << a[i][j];
               }
          }
          stream << '\n';
     }
     return stream;
}

template <class T>
std::istream& operator>>(std::istream& stream, Matrix<T>& a) {
     std::size_t m, n;
     stream >> m >> n;
     if (stream.fail())
          return stream;
     Matrix<T> b(m, n);
     for (std::size_t i = 0; i < b.nrows(); i++)
          for (std::size_t j = 0; j < b.ncols(); j++) {
               stream >> b(i, j);
               if (stream.fail())
                    return stream;
          }
     a.swap(b);
     return stream;
}

template <class T>
void print(const Matrix<T>& a, std::ostream& stream) {
     print(a.vector(), stream);
}

template <class T>
void write(const Matrix<T>& a, std::ostream& f) {
     const std::size_t m = a.nrows();
     const std::size_t n = a.ncols();
     f.write(reinterpret_cast<const char*>(&m), sizeof m);
     f.write(reinterpret_cast<const char*>(&n), sizeof n);
     write(a.vector(), f);
}

template <class T>
void read(Matrix<T>& a, std::istream& f) {
     std::size_t m, n;
     f.read(reinterpret_cast<char*>(&m), sizeof m);
     f.read(reinterpret_cast<char*>(&n), sizeof n);
     if (f.fail())
          return;
     Vector<T> v;
     read(v, f);
     if (f.fail())
          return;
     Matrix<T> b(m, n, std::move(v));
     a.swap(b);
}

template <class T>
T maximum_norm_distance(const Matrix<T>& a, const Matrix<T>& b) {
     if (a.nrows() == b.nrows() && a.ncols() == b.ncols())
          return maximum_norm_distance(a.vector(), b.vector());
     throw std::invalid_argument(__func__);
}

template <class T>
Matrix<T> diagonal_matrix(std::size_t n, const T& c) {
     Matrix<T> a(n, n, static_cast<T>(0));
     for (std::size_t i = 0; i < n; i++)
          a[i][i] = c;
     return a;
}

template <class T>
Matrix<T> transpose(const Matrix<T>& a) {
     Matrix<T> b(a.ncols(), a.nrows());
     for (std::size_t i = 0; i < a.nrows(); i++)
          for (std::size_t j = 0; j < a.ncols(); j++)
               b(j, i) = a(i, j);
     return b;
}

template <class T>
Matrix<T>& transpose_in_situ(Matrix<T>& a) {
     if (a.nrows() != a.ncols())
          throw std::invalid_argument(__func__);
     for (std::size_t i = 0; i < a.nrows(); i++)
          for (std::size_t j = i + 1; j < a.ncols(); j++)
               std::swap(a(i, j), a(j, i));
     return a;
}

template <class T>
Matrix<T> multiply(const Matrix<T>& a, const Matrix<T>& b) {
     if (a.ncols() != b.nrows())
          throw std::invalid_argument(__func__);
     Matrix<T> c(a.nrows(), b.ncols());
     for (std::size_t i = 0; i < a.nrows(); i++) {
          const T* const p = a[i];
          T* const q = c[i];
          for (std::size_t j = 0; j < b.ncols(); j++) {
               T s = 0;
               for (std::size_t k = 0; k < a.ncols(); k++)
                    s += p[k] * b[k][j];
               q[j] = s;
          }
     }
     return c;
}

template <class T>
void right_multiply_and_assign(Matrix<T>& a, const Matrix<T>& b) {
     if (&a == &b || a.ncols() != b.nrows() || b.nrows() != b.ncols())
          throw std::invalid_argument(__func__);
     Vector<T> z(a.ncols());
     std::size_t i, j, k;
     T s, *p;
     for (i = 0; i < a.nrows(); i++) {
          p = a[i];
          for (j = 0; j < a.ncols(); j++)
               z[j] = p[j];
          for (j = 0; j < a.ncols(); j++) {
               s = 0;
               for (k = 0; k < a.ncols(); k++)
                    s += z[k] * b[k][j];
               p[j] = s;
          }
     }
}

template <class T>
Matrix<T> left_multiply_by_transposition(const Matrix<T>& a) {
     Matrix<T> b(a.ncols(), a.ncols());
     std::size_t i, j, k;
     T s;
     for (i = 0; i < a.ncols(); i++) {
          for (j = 0; j < i; j++) {
               s = 0;
               for (k = 0; k < a.nrows(); k++)
                    s += a(k, i) * a(k, j);
               b(i, j) = b(j, i) = s;
          }
          s = 0;
          for (k = 0; k < a.nrows(); k++)
               s += [](T x) { return x * x; }(a(k, i));
          b(i, i) = s;
     }
     return b;
}

template <class T>
void cholesky(Matrix<T>& a, T eps) {
     static_assert(std::is_floating_point<T>::value,
                   "cholesky requires floating-point matrix");
     if (a.nrows() != a.ncols() || eps < 0)
          throw std::invalid_argument(__func__);
     const std::size_t n = a.nrows();
     std::size_t i, j, k;
     T x, z;

     // Find the matrix L such that LL^T is equal to the given matrix
     // and the upper-right triangle of L contains only zeros. Put it
     // in the lower-left triangle of a, inverting the diagonal
     // elements of L.

     for (i = 0; i < n; i++) {
          z = a[i][i];
          for (k = 0; k < i; k++)
               z -= [](T x) { return x * x; }(a[i][k]);
          if (z <= eps)
               throw std::range_error(__func__);
          a[i][i] = z = 1.0 / std::sqrt(z);
          if (!std::isfinite(z))
               throw std::range_error(__func__);
          for (j = i + 1; j < n; j++) {
               x = a[i][j];
               for (k = 0; k < i; k++)
                    x -= a[j][k] * a[i][k];
               a[j][i] = x * z;
          }
     }

     // Calculate L^{-1} and put it in the lower-left triangle of a.

     for (i = 1; i < n; i++)
          for (j = 0; j < i; j++) {
               x = 0.0;
               for (k = j; k < i; k++)
                    x -= a[i][k] * a[k][j];
               a[i][j] = x * a[i][i];
          }

     // Calculate (L^{-1})^T L^{-1} and put it in the whole array a.

     for (i = 0; i < n; i++)
          for (j = i; j < n; j++) {
               x = 0.0;
               for (k = j; k < n; k++)
                    x += a[k][i] * a[k][j];
               a[i][j] = a[j][i] = x;
          }
}

template <class T>
Matrix<T> hilbert_matrix(std::size_t n) {
     static_assert(std::is_floating_point<T>::value,
                   "hilbert_matrix requires floating-point type");
     Matrix<T> h(n, n);
     const std::size_t n2 = 2 * n;
     for (std::size_t k = 1, k1 = 0; k < n2; k++, k1++) {
          const T z = static_cast<T>(1) / k;
          const std::size_t min = k < n ? k : n;
          for (std::size_t i = k - min; i < min; i++)
               h(i, k1 - i) = z;
     }
     return h;
}

template <class T>
Vector<T> multiply(const Matrix<T>& a, const Vector<T>& v) {
     if (a.ncols() != v.size())
          throw std::invalid_argument(__func__);
     Vector<T> w(a.nrows());
     for (std::size_t i = 0; i < w.size(); i++) {
          const T* const p = a[i];
          T s = 0;
          for (std::size_t j = 0; j < v.size(); j++)
               s += p[j] * v[j];
          w[i] = s;
     }
     return w;
}

template <class T>
Vector<T> multiply_transposed(const Matrix<T>& a,
                              const Vector<T>& v) {
     if (a.nrows() != v.size())
          throw std::invalid_argument(__func__);
     Vector<T> w(a.ncols());
     for (std::size_t i = 0; i < w.size(); i++) {
          T s = 0;
          for (std::size_t j = 0; j < v.size(); j++)
               s += a[j][i] * v[j];
          w[i] = s;
     }
     return w;
}

}  // namespace SHG

#endif
