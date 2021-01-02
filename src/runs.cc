/**
 * \file src/runs.cc
 * Wald-Wolfowitz runs test.
 * \date Created on 28 September 2013.
 */

#include <shg/runs.h>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>
#include <shg/except.h>

namespace SHG {

using std::size_t;

void runs(size_t n1, size_t n2, size_t k, bool exact, double& p1,
          double& p2) {
     using std::numeric_limits;
     using std::range_error;
     using std::sqrt;
     using std::swap;
     if (n1 > 0 && n2 > 0) {
          if (n2 < n1)
               swap(n1, n2);
          if (k < 2) {
               // k is smaller than the smallest possible number of
               // runs.
               p1 = 0.0;
               p2 = 1.0;
          } else if (k > (n1 != n2 ? 2 * n1 + 1 : n1 + n2)) {
               // k is greater than the greatest possible number of
               // runs.
               p1 = 1.0;
               p2 = 0.0;
          } else if (exact) {
               double z = 1.0;
               for (size_t i = 1; i <= n1; i++)
                    z *= static_cast<double>(i) /
                         static_cast<double>(n2 + i);
               if (z < 4.0 * numeric_limits<double>::min())
                    throw range_error(__func__);
               p1 = 0.0;
               const size_t imax = k / 2;
               const bool odd = k % 2 == 1;
               double c1 = 1.0, c2 = 1.0, p = 0.0;
               for (size_t i = 1; i <= imax; i++) {
                    p = 2.0 * c1 * c2;
                    p1 += p;
                    if (i < imax || odd) {
                         const double d1 = c1;
                         const double d2 = c2;
                         c1 *= static_cast<double>(n1 - i) /
                               static_cast<double>(i);
                         c2 *= static_cast<double>(n2 - i) /
                               static_cast<double>(i);
                         p = d1 * c2 + c1 * d2;
                         p1 += p;
                    }
               }
               p *= z;   // p = P(K = k)
               p1 *= z;  // p1 = P(K <= k)
               if (p1 > 1.0)
                    p1 = 1.0;
               p2 = 1.0 - p1 + p;  // p2 = P(K >= k)
               SHG_ASSERT(p1 >= 0.0 && p1 <= 1.0);
               SHG_ASSERT(p2 >= 0.0 && p2 <= 1.0);
          } else {
               const double en = n1 + n2;
               // m is the expected value, s is the standard deviation
               // times sqrt(2.0)
               const double m = 2.0 * n1 * n2 / en + 1.0;
               const double s =
                    sqrt(2.0 * (m - 1.0) * (m - 2.0) / (en - 1.0));
               const double z = (k - m) / s;
               p1 = 0.5 + 0.5 * erf(z);
               p2 = 0.5 * erfc(z);
          }
     } else if (n1 == 0 && n2 == 0) {
          if (k > 0) {
               p1 = 1.0;
               p2 = 0.0;
          } else {
               p1 = p2 = 1.0;
          }
     } else {
          // (n1 == 0 && n2 > 0) || (n1 > 0 && n2 == 0)
          if (k < 1) {
               p1 = 0.0;
               p2 = 1.0;
          } else if (k > 1) {
               p1 = 1.0;
               p2 = 0.0;
          } else {
               p1 = p2 = 1.0;
          }
     }
}

}  // namespace SHG
