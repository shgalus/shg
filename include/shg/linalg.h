/**
 * \file include/shg/linalg.h
 * Matrix class and functions with Eigen.
 */

#ifndef SHG_LINALG_H
#define SHG_LINALG_H

#include <cstddef>
#include <type_traits>
#include <Eigen/Dense>

namespace SHG::Linalg {

/**
 * \defgroup linalg Matrix class and functions with Eigen
 *
 * %Matrix class and functions with Eigen.
 *
 * \{
 */

#if defined(_MSC_VER)
// packages22\Eigen3.3.3.9\lib\native\include\Eigen\src\Core\util\Macros.h
static_assert(EIGEN_WORLD_VERSION == 3);
static_assert(EIGEN_MAJOR_VERSION == 3);
static_assert(EIGEN_MINOR_VERSION == 9);
#else
// /usr/local/include/eigen3/Eigen/src/Core/util/Macros.h
static_assert(EIGEN_WORLD_VERSION == 3);
static_assert(EIGEN_MAJOR_VERSION == 4);
static_assert(EIGEN_MINOR_VERSION == 0);
#endif

static_assert(std::is_same<Eigen::Index, std::ptrdiff_t>::value);

using Index = Eigen::Index;

template <typename T>
using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic,
                             Eigen::RowMajor>;

#if defined(_MSC_VER)
template <typename T>
using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
template <typename T>
using Row_vector = Eigen::Matrix<T, 1, Eigen::Dynamic>;
#else
template <typename T>
using Vector = Eigen::Vector<T, Eigen::Dynamic>;
template <typename T>
using Row_vector = Eigen::RowVector<T, Eigen::Dynamic>;
#endif

using Matint = Matrix<int>;
using Matlong = Matrix<long>;
using Matdouble = Matrix<double>;

using Vecint = Vector<int>;
using Veclong = Vector<long>;
using Vecdouble = Vector<double>;

using Row_vecint = Row_vector<int>;
using Row_veclong = Row_vector<long>;
using Row_vecdouble = Row_vector<double>;

/** \} */

}  // namespace SHG::Linalg

#endif
