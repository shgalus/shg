/* bdstest.h: BDS test for independence */

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
     BDS_test(const std::vector<double>& u,
              int maxm,
              const std::vector<double>& eps);
     explicit BDS_test(const std::vector<double>& u);
     inline int maxm() const;
     inline const std::vector<double>& eps() const;
     inline const std::vector<std::vector<Result>>& res() const;
private:
     typedef std::vector<double>::size_type vdst;
     static std::vector<double> default_eps(
          const std::vector<double>& u);
     static const double isqrt2;
     static const vdst maxn_;
     const int maxm_;
     const std::vector<double> eps_;
     std::vector<std::vector<Result>> res_;
};

std::ostream& operator<<(std::ostream& stream, const BDS_test& b);

/** \} */ /* end of group mathematical_statistics */

int BDS_test::maxm() const {
     return maxm_;
}

const std::vector<double>& BDS_test::eps() const {
     return eps_;
}

const std::vector<std::vector<BDS_test::Result>>&
BDS_test::res() const {
     return res_;
}

}       // namespace SHG

#endif
