#ifndef TESTS_H
#define TESTS_H

#include <limits>
#include <ios>
#include <boost/mpl/list.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <shg/fcmp.h>

/**
 * \namespace TESTS Testing %SHG library.
 */
namespace TESTS {

namespace bdata = boost::unit_test::data;

constexpr float min_flt = std::numeric_limits<float>::min();
constexpr float max_flt = std::numeric_limits<float>::max();
constexpr double min_dbl = std::numeric_limits<double>::min();
constexpr double max_dbl = std::numeric_limits<double>::max();
constexpr long double min_ldbl =
     std::numeric_limits<long double>::min();
constexpr long double max_ldbl =
     std::numeric_limits<long double>::max();

constexpr std::ios_base::openmode const bininp =
     std::ios_base::in | std::ios_base::binary;
constexpr std::ios_base::openmode const binout =
     std::ios_base::out | std::ios_base::binary;
constexpr std::ios_base::openmode const bininpout =
     std::ios_base::in | std::ios_base::out | std::ios_base::binary;

extern const char* const datadir;

}  // namespace TESTS

#endif
