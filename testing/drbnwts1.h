#ifndef DRBNWTS1_H
#define DRBNWTS1_H

namespace SHG {
namespace Testing {

struct Durbin_Watson_test_data {
     const double pdl;
     const double pdu;
};

/**
 * Data for testing cumulative distribution function of the
 * Durbin-Watson statistic. Data in tab_nag come from the function
 * SHG::dwcdf and was checked to be equal to the results of the
 * function nag-prob-durbin-watson (g01epc) from the NAG C library,
 * mark 7 (2001).
 */
extern const Durbin_Watson_test_data tab_nag[3465];

}       // namespace Testing
}       // namespace SHG

#endif
