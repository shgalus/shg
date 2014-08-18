/* permentr.cc: testing permutation entropy */

/**
 * \file testing/permentr.cc
 * Testing permutation entropy.
 * \date Created on 5 May 2014.
 */

#include <cmath>
#include "shg/permentr.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

const std::vector<int> test {18, 54, 32, 16, 16, 43, 16, 12};
// Example from \cite bandt-pompe-2002, page 2.
const std::vector<int> bandt_pompe {4, 7, 9, 10, 6, 11, 3};
const std::vector<int> cnst(1000, 1);

const std::vector<double> test_res {9.852281e-01,
          1.918296e+00,
          1.921928e+00,
          2.000000e+00,
          1.584963e+00,
          1.000000e+00,
          0.000000e+00
          };

const std::vector<double> bandt_pompe_res {9.182958e-01,
          1.521928e+00,
          2.000000e+00,
          1.584963e+00,
          1.000000e+00,
          0.000000e+00
          };

}       // anonymous namespace

void test_permentr() {
     using std::abs;
     using SHG::permutation_entropy;

     for (std::size_t L = 2; L <= test.size(); L++)
          SHG_ASSERT(abs(permutation_entropy(test, L) -
                         test_res[L - 2]) < 5e-6);

     for (std::size_t L = 2; L <= bandt_pompe.size(); L++)
          SHG_ASSERT(abs(permutation_entropy(bandt_pompe, L) -
                         bandt_pompe_res[L - 2]) < 5e-6);

     for (std::size_t L = 2; L <= 8; L++)
          SHG_ASSERT(permutation_entropy(cnst, L) < 5e-6);
}

}       // namespace Testing
}       // namespace SHG
