/**
 * \file include/shg/bdstest.h
 * BDS test for independence.
 * \date Created on 12 October 2013.
 */

#ifndef SHG_BDSTEST_H
#define SHG_BDSTEST_H

#include <cmath>
#include <limits>
#include <ostream>
#include <vector>

namespace SHG {

/**
 * \addtogroup mathematical_statistics
 *
 * \{
 */

/**
 * BDS test for independence. For a description see <a
 * href="shg.pdf">BDS test for independence.</a>
 */
class BDS_test {
public:
     struct Result {
          double stat;
          double pval;
     };
     BDS_test(std::vector<double> const& u, int maxm,
              std::vector<double> const& eps);
     explicit BDS_test(std::vector<double> const& u);
     int maxm() const;
     std::vector<double> const& eps() const;
     std::vector<std::vector<Result>> const& res() const;

private:
     typedef std::vector<double>::size_type vdst;
     static std::vector<double> default_eps(
          std::vector<double> const& u);
     static double const isqrt2;
     static vdst const maxn_;
     int const maxm_;
     std::vector<double> const eps_;
     std::vector<std::vector<Result>> res_;
};

std::ostream& operator<<(std::ostream& stream, BDS_test const& b);

/** \} */ /* end of group mathematical_statistics */

inline int BDS_test::maxm() const {
     return maxm_;
}

inline std::vector<double> const& BDS_test::eps() const {
     return eps_;
}

inline std::vector<std::vector<BDS_test::Result>> const&
BDS_test::res() const {
     return res_;
}

}  // namespace SHG

#endif
