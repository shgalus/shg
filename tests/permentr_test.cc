#include <shg/permentr.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(permentr_test)

using SHG::permutation_entropy;
using SHG::faeq;
using std::vector;

BOOST_AUTO_TEST_CASE(basic_test) {
     vector<int> const test_data{18, 54, 32, 16, 16, 43, 16, 12};
     vector<double> const test_result{
          9.852281e-01, 1.918296e+00, 1.921928e+00, 2.000000e+00,
          1.584963e+00, 1.000000e+00, 0.000000e+00};

     for (vector<int>::size_type L = 2; L <= test_data.size(); L++)
          BOOST_CHECK(faeq(permutation_entropy(test_data, L),
                           test_result[L - 2], 5e-6));
}

// Example from \cite bandt-pompe-2002, page 2.
BOOST_AUTO_TEST_CASE(bandt_pompe_test) {
     vector<int> const test_data{4, 7, 9, 10, 6, 11, 3};
     vector<double> const test_result{9.182958e-01, 1.521928e+00,
                                      2.000000e+00, 1.584963e+00,
                                      1.000000e+00, 0.000000e+00};

     for (vector<int>::size_type L = 2; L <= test_data.size(); L++)
          BOOST_CHECK(faeq(permutation_entropy(test_data, L),
                           test_result[L - 2], 5e-6));
}

BOOST_AUTO_TEST_CASE(const_test) {
     vector<int> const test_data(1000, 1);
     for (vector<int>::size_type L = 2; L <= 8; L++)
          BOOST_CHECK(
               faeq(permutation_entropy(test_data, L), 0.0, 5e-6));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
