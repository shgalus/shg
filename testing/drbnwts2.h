#ifndef DRBNWTS2_H
#define DRBNWTS2_H

namespace SHG {
namespace Testing {

/**
 * Data for testing percentage points of the Durbin-Watson statistic.
 * Data in savin_white_table1 contains a portion of the table from
 * \cite savin-white-1977. For n = 6, 7, 8, 9, the inadequate
 * positions were filled with 0.000 instead of dashes to keep one
 * input format.
 */
extern const char* const savin_white_table1;

}       // namespace Testing
}       // namespace SHG

#endif
