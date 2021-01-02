#ifndef TESTING_H
#define TESTING_H

#include <limits>
#include <boost/mpl/list.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <shg/fcmp.h>

/**
 * \namespace SHG::Testing %Testing %SHG library.
 */
namespace SHG::Testing {

constexpr float min_flt = std::numeric_limits<float>::min();
constexpr float max_flt = std::numeric_limits<float>::max();
constexpr double min_dbl = std::numeric_limits<double>::min();
constexpr double max_dbl = std::numeric_limits<double>::max();
constexpr long double min_ldbl =
     std::numeric_limits<long double>::min();
constexpr long double max_ldbl =
     std::numeric_limits<long double>::max();

}  // namespace SHG::Testing

#endif
