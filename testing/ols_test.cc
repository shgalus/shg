#include "shg/ols.h"
#include <cstring>
#include <sstream>
#include "testing.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26444)  // NO_UNNAMED_RAII_OBJECTS
#endif

namespace SHG::BTesting {

BOOST_AUTO_TEST_SUITE(ols_test)

BOOST_AUTO_TEST_CASE(basic_test) {
     for (int k = 0; k <= 10; k++) {
          const int k1 = k + 1;
          const int n = 10 * k + 100;
          Vecdouble y(n);
          Matdouble X(n, k1);
          Vecdouble b(k1);
          for (int i = 0; i < k1; i++)
               b[i] = i + 1;
          for (int i = 0; i < n; i++) {
               const double x = i + 1;
               double xp = 1.0, p = b(0);
               X(i, 0) = 1.0;
               for (int j = 1; j < k1; j++) {
                    xp *= x;
                    X(i, j) = xp;
                    p += b(j) * xp;
               }
               y(i) = p;
          }
          BOOST_CHECK_NO_THROW(OLS(X, y, false));
     }
}

BOOST_AUTO_TEST_CASE(sing_cov_matrix_throws_test) {
     const int k = 2;
     const int n = 5;
     Vecdouble y(n);
     Matdouble X(n, k);
     for (int i = 0; i < n; i++) {
          y(i) = i * i * i;
          X(i, 0) = i;
          X(i, 1) = i;
     }
     BOOST_CHECK_THROW(OLS(X, y, true),
                       OLS::Singular_covariance_matrix);
}

BOOST_AUTO_TEST_CASE(zero_degrees_of_freedom_test) {
     const int n = 5;
     const int k = 4;
     Vecdouble y(n);
     Matdouble X(n, k, 0.0);
     for (int i = 0; i < n; i++) {
          y(i) = i;
          if (i < k)
               X(i, i) = 1.0;
     }
     BOOST_CHECK_NO_THROW(OLS(X, y, true));
     y = 0;
     BOOST_CHECK_NO_THROW(OLS(X, y, true));
}

BOOST_AUTO_TEST_CASE(constant_y_test) {
     const int n = 3;
     const int k = 1;
     Vecdouble y(n);
     Matdouble X(n, k, 0.0);
     for (int i = 0; i < k; i++)
          X(i, i) = 1.0;

     y = 0.0;
     BOOST_CHECK_NO_THROW(OLS(X, y, true));
     BOOST_CHECK_NO_THROW(OLS(X, y, false));
     y = 5.0;
     BOOST_CHECK_NO_THROW(OLS(X, y, true));
     BOOST_CHECK_NO_THROW(OLS(X, y, false));
}

BOOST_AUTO_TEST_CASE(too_many_regressors_test) {
     Vecdouble y(4);
     Matdouble X(4, 4);
     BOOST_CHECK_THROW(OLS(X, y), OLS::Invalid_argument);
}

namespace {

constexpr const char* const goldberger_test_results =
     R"(
Ordinary least squares estimation results
Problem: Goldberger, p.213, 226, 238.
Number of observations:              6
Number of explanatory variables:     3
Number of degrees of freedom:        3
The first parameter is an intercept.

Param.   Estimate    Std. error   t statistic       p-value
 1.    5.4054e-01    1.8231e-01    2.9650e+00    5.9306e-02 *
 2.    2.0270e-01    2.0089e-01    1.0090e+00    3.8729e-01
 3.    1.3514e+00    2.7562e-01    4.9029e+00    1.6241e-02 **

Standard error of regression:                4.1914e-01
Coefficient of determination:                9.2287e-01
Adjusted coefficient of determination:       8.7146e-01
Variance of residuals:                       1.7568e-01
Residual sum of squares:                     5.2703e-01
Explained sum of squares:                    6.3063e+00
Total sum of squares:                        6.8333e+00
Mean of dependent variable:                  8.3333e-01
Standard deviation of dependent variable:    1.0672e+00
Variation of dependent variable:          *************
F statistic:                                 1.7949e+01
p-value:                                     2.1419e-02 **
Durbin-Watson d statistic:                *************
p-value for positive autocorrelation:     *************
p-value for negative autocorrelation:     *************
)";

}  // anonymous namespace

BOOST_AUTO_TEST_CASE(goldberger_book_test) {
     const Vecdouble y{0, 2, 1, 2, -1, 1};
     const Matdouble X(6, 2, {-1, 0, 0, 1, 1, 0, 2, 1, 0, -1, 0, 0});
     OLS ols(X, y);
     ols.set_problem_name("Goldberger, p.213, 226, 238.");
     ols.dw();
     std::ostringstream oss;
     oss << '\n';
     ols.print(oss);
     BOOST_CHECK(std::strcmp(oss.str().c_str(),
                             goldberger_test_results) == 0);
}

namespace {

// GRETL 1.7.5 sample data, Greene 7.8.

const Vecdouble greene7_8_y{
     129.7, 131.3, 137.1, 141.6, 148.8, 155.9, 164.9, 171.0, 183.4,
     195.8, 207.4, 218.3, 226.8, 237.9, 225.8, 232.4, 241.7, 249.2,
     261.3, 248.9, 226.8, 225.6, 228.8, 239.6, 244.7, 245.8, 269.4,
     276.8, 279.9, 284.1, 282.0, 271.8, 280.2, 286.7, 290.2, 297.8};

// clang-format off
const Matdouble greene7_8_X(36, 10, {
     // obs, Pg,    Y,   Pnc,   Puc,   Ppt,    Pd,    Pn,    Ps,   Pop
     60, 0.925,  6036, 1.045, 0.836, 0.810, 0.444, 0.331, 0.302, 180.7,
     61, 0.914,  6113, 1.045, 0.869, 0.846, 0.448, 0.335, 0.307, 183.7,
     62, 0.919,  6271, 1.041, 0.948, 0.874, 0.457, 0.338, 0.314, 186.5,
     63, 0.918,  6378, 1.035, 0.960, 0.885, 0.463, 0.343, 0.320, 189.2,
     64, 0.914,  6727, 1.032, 1.001, 0.901, 0.470, 0.347, 0.325, 191.9,
     65, 0.949,  7027, 1.009, 0.994, 0.919, 0.471, 0.353, 0.332, 194.3,
     66, 0.970,  7280, 0.991, 0.970, 0.952, 0.475, 0.366, 0.342, 196.6,
     67, 1.000,  7513, 1.000, 1.000, 1.000, 0.483, 0.375, 0.353, 198.7,
     68, 1.014,  7728, 1.028, 1.028, 1.046, 0.501, 0.390, 0.368, 200.7,
     69, 1.047,  7891, 1.044, 1.031, 1.127, 0.514, 0.409, 0.386, 202.7,
     70, 1.056,  8134, 1.076, 1.043, 1.285, 0.527, 0.427, 0.407, 205.1,
     71, 1.063,  8322, 1.120, 1.102, 1.377, 0.547, 0.442, 0.431, 207.7,
     72, 1.076,  8562, 1.110, 1.105, 1.434, 0.555, 0.458, 0.451, 209.9,
     73, 1.181,  9042, 1.111, 1.176, 1.448, 0.566, 0.497, 0.474, 211.9,
     74, 1.599,  8867, 1.175, 1.226, 1.480, 0.604, 0.572, 0.513, 213.9,
     75, 1.708,  8944, 1.276, 1.464, 1.586, 0.659, 0.615, 0.556, 216.0,
     76, 1.779,  9175, 1.357, 1.679, 1.742, 0.695, 0.638, 0.598, 218.0,
     77, 1.882,  9381, 1.429, 1.828, 1.824, 0.727, 0.671, 0.648, 220.2,
     78, 1.963,  9735, 1.538, 1.865, 1.878, 0.769, 0.719, 0.698, 222.6,
     79, 2.656,  9829, 1.660, 2.010, 2.003, 0.821, 0.800, 0.756, 225.1,
     80, 3.691,  9722, 1.793, 2.081, 2.516, 0.892, 0.894, 0.839, 227.7,
     81, 4.109,  9769, 1.902, 2.569, 3.120, 0.957, 0.969, 0.926, 230.0,
     82, 3.894,  9725, 1.976, 2.964, 3.460, 1.000, 1.000, 1.000, 232.2,
     83, 3.764,  9930, 2.026, 3.297, 3.626, 1.041, 1.021, 1.062, 234.3,
     84, 3.707, 10421, 2.085, 3.757, 3.852, 1.038, 1.050, 1.117, 236.3,
     85, 3.738, 10563, 2.152, 3.797, 4.028, 1.045, 1.075, 1.173, 238.5,
     86, 2.921, 10780, 2.240, 3.632, 4.264, 1.053, 1.069, 1.224, 240.7,
     87, 3.038, 10859, 2.321, 3.776, 4.413, 1.085, 1.111, 1.271, 242.8,
     88, 3.065, 11186, 2.368, 3.939, 4.494, 1.105, 1.152, 1.336, 245.0,
     89, 3.353, 11300, 2.414, 4.019, 4.719, 1.129, 1.213, 1.408, 247.3,
     90, 3.834, 11389, 2.451, 3.926, 5.197, 1.144, 1.285, 1.482, 249.9,
     91, 3.766, 11272, 2.538, 3.942, 5.427, 1.167, 1.332, 1.557, 252.6,
     92, 3.751, 11466, 2.528, 4.113, 5.518, 1.184, 1.358, 1.625, 255.4,
     93, 3.713, 11476, 2.663, 4.470, 6.086, 1.200, 1.379, 1.684, 258.1,
     94, 3.732, 11636, 2.754, 4.730, 6.268, 1.225, 1.396, 1.734, 260.7,
     95, 3.789, 11934, 2.815, 5.224, 6.410, 1.239, 1.419, 1.786, 263.2
});
// clang-format on

constexpr const char* const greene_test_results =
     R"(
Ordinary least squares estimation results
Problem: Greene7_8, gasoline price and consumption.
Number of observations:             36
Number of explanatory variables:    11
Number of degrees of freedom:       25
The first parameter is an intercept.

Param.   Estimate    Std. error   t statistic       p-value
 1.   -3.4466e+04    1.8032e+04   -1.9113e+00    6.7491e-02 *
 2.    1.8049e+01    9.5133e+00    1.8972e+00    6.9407e-02 *
 3.   -1.5898e+01    9.2380e+00   -1.7210e+00    9.7612e-02 *
 4.    1.3602e-02    9.1411e-03    1.4880e+00    1.4927e-01
 5.    3.1210e+01    2.9009e+01    1.0759e+00    2.9225e-01
 6.   -8.4312e+00    6.3390e+00   -1.3301e+00    1.9551e-01
 7.    3.4130e+00    9.6922e+00    3.5213e-01    7.2769e-01
 8.   -1.5266e+01    8.2206e+01   -1.8570e-01    8.5418e-01
 9.   -1.7425e+01    1.3606e+02   -1.2807e-01    8.9912e-01
10.   -8.1342e+01    9.9725e+01   -8.1566e-01    4.2240e-01
11.   -4.6688e+00    3.1024e+00   -1.5049e+00    1.4488e-01

Standard error of regression:                4.6697e+00
Coefficient of determination:                9.9391e-01
Adjusted coefficient of determination:       9.9148e-01
Variance of residuals:                       2.1806e+01
Residual sum of squares:                     5.4516e+02
Explained sum of squares:                    8.9038e+04
Total sum of squares:                        8.9584e+04
Mean of dependent variable:                  2.2609e+02
Standard deviation of dependent variable:    4.9884e+01
Variation of dependent variable:             2.2063e-01
F statistic:                                 4.0832e+02
p-value:                                     0.0000e+00 ***
Durbin-Watson d statistic:                   1.1317e+00
p-value for positive autocorrelation:        2.6701e-01
p-value for negative autocorrelation:        1.0000e+00
)";

}  // anonymous namespace

BOOST_AUTO_TEST_CASE(greene_book_test) {
     Matdouble X(greene7_8_X);
     for (size_t i = 0; i < X.nrows(); i++)
          X(i, 0) += 1900.0;
     OLS ols(X, greene7_8_y);
     ols.set_problem_name(
          "Greene7_8, gasoline price and consumption.");
     ols.dw();
     std::ostringstream oss;
     oss << '\n';
     ols.print(oss);
     BOOST_CHECK(
          std::strcmp(oss.str().c_str(), greene_test_results) == 0);
}

namespace {

/**
 * Returns common logarithm of the relative error of approximation q
 * of the correct value c. If c == 0.0, returns common logarithm of
 * the absolute error.
 *
 * See \cite mccullough-1998 and \cite mccullough-1999, which at the
 * time of writing could be found at
 * http://www.pages.drexel.edu/~bdm25/publications.html.
 */
double log_error(double q, double c) {
     if (std::isnan(q) || std::isnan(c))
          return 0.0;
     if (q == c)
          return std::numeric_limits<double>::digits10;
     if (std::isinf(c) || std::isinf(q))
          return 0.0;
     const double le =
          c == 0.0 ? -std::log10(std::abs(q))
                   : -std::log10(std::abs(q - c) / std::abs(c));
     SHG_ASSERT(!std::isnan(le));
     return le;
}

// ncsd
// number_of_correct_significant_digits
int ncsd(double q, double c) {
     const double le = log_error(q, c);
     int nd;
     if (le < 1.0)
          nd = 0;
     else if (le > std::numeric_limits<double>::digits10)
          nd = std::numeric_limits<double>::digits10;
     else
          nd = static_cast<int>(le);
     return nd;
}

}  // anonymous namespace

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_norris_test) {
     const Matdouble X{
          36, 1, {0.2,   337.4, 118.2, 884.6, 10.1,  226.5,
                  666.3, 996.3, 448.6, 777.0, 558.2, 0.4,
                  0.6,   775.5, 666.9, 338.0, 447.5, 11.6,
                  556.0, 228.1, 995.8, 887.6, 120.2, 0.3,
                  0.3,   556.8, 339.1, 887.2, 999.0, 779.0,
                  11.1,  118.3, 229.2, 669.1, 448.9, 0.5}};
     const Vecdouble y{0.1,   338.8, 118.1, 888.0, 9.2,   228.1,
                       668.5, 998.5, 449.1, 778.9, 559.2, 0.3,
                       0.1,   778.1, 668.8, 339.3, 448.9, 10.8,
                       557.7, 228.3, 998.0, 888.8, 119.6, 0.3,
                       0.6,   557.6, 339.3, 888.0, 998.5, 778.9,
                       10.2,  117.6, 228.9, 668.4, 449.2, 0.2};
     BOOST_CHECK(X.nrows() == y.size());
     int nd;
     OLS ols(X, y, true);

     nd = ncsd(ols.beta()(0), -0.262323073774029);
     BOOST_CHECK(nd >= 13);
     nd = ncsd(ols.standard_err()(0), 0.232818234301152);
     BOOST_CHECK(nd >= 13);
     nd = ncsd(ols.beta()(1), 1.00211681802045);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.standard_err()(1), 0.429796848199937E-03);
     BOOST_CHECK(nd >= 13);
     nd = ncsd(ols.ser(), 0.884796396144373);
     BOOST_CHECK(nd >= 13);
     nd = ncsd(ols.r2(), 0.999993745883712);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess(), 4255954.13232369);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess() / X.ncols(), 4255954.13232369);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.fstat(), 5436385.54079785);
     BOOST_CHECK(nd >= 13);
     BOOST_CHECK(ols.dof() == 34);
     nd = ncsd(ols.rss(), 26.6173985294224);
     BOOST_CHECK(nd >= 13);
     nd = ncsd(ols.rss() / ols.dof(), 0.782864662630069);
     BOOST_CHECK(nd >= 13);
}

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_pontius_test) {
     const Matdouble X{
          40, 1, {150000,  300000,  450000,  600000,  750000,
                  900000,  1050000, 1200000, 1350000, 1500000,
                  1650000, 1800000, 1950000, 2100000, 2250000,
                  2400000, 2550000, 2700000, 2850000, 3000000,
                  150000,  300000,  450000,  600000,  750000,
                  900000,  1050000, 1200000, 1350000, 1500000,
                  1650000, 1800000, 1950000, 2100000, 2250000,
                  2400000, 2550000, 2700000, 2850000, 3000000}};
     const Vecdouble y{.11019,  .21956,  .32949,  .43899,  .54803,
                       .65694,  .76562,  .87487,  .98292,  1.09146,
                       1.20001, 1.30822, 1.41599, 1.52399, 1.63194,
                       1.73947, 1.84646, 1.95392, 2.06128, 2.16844,
                       .11052,  .22018,  .32939,  .43886,  .54798,
                       .65739,  .76596,  .87474,  .98300,  1.09150,
                       1.20004, 1.30818, 1.41613, 1.52408, 1.63159,
                       1.73965, 1.84696, 1.95445, 2.06177, 2.16829};
     BOOST_CHECK(X.nrows() == y.size());
     int nd;
     Matdouble X1(X.nrows(), X.ncols() + 1);
     for (std::size_t i = 0; i < X1.nrows(); i++) {
          X1(i, 0) = X(i, 0);
          X1(i, 1) = sqr(X(i, 0));
     }
     OLS ols(X1, y, true);

     nd = ncsd(ols.beta()(0), 0.673565789473684E-03);
     BOOST_CHECK(nd >= 11);
     nd = ncsd(ols.standard_err()(0), 0.107938612033077E-03);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.beta()(1), 0.732059160401003E-06);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.standard_err()(1), 0.157817399981659E-09);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.beta()(2), -0.316081871345029E-14);
     BOOST_CHECK(nd >= 12);
     nd = ncsd(ols.standard_err()(2), 0.486652849992036E-16);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.ser(), 0.205177424076185E-03);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.r2(), 0.999999900178537);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess(), 15.6040343244198);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess() / X1.ncols(), 7.80201716220991);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.fstat(), 185330865.995752);
     BOOST_CHECK(nd >= 14);
     BOOST_CHECK(ols.dof() == 37);
     nd = ncsd(ols.rss(), 0.155761768796992E-05);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.rss() / ols.dof(), 0.420977753505385E-07);
     BOOST_CHECK(nd >= 14);
}

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_noint1_test) {
     const Matdouble X{
          11, 1, {60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70}};
     const Vecdouble y{130, 131, 132, 133, 134, 135,
                       136, 137, 138, 139, 140};
     BOOST_CHECK(X.nrows() == y.size());
     int nd;
     OLS ols(X, y, false);

     nd = ncsd(ols.beta()(0), 2.07438016528926);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.standard_err()(0), 0.165289256198347E-01);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ser(), 3.56753034006338);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.r2(), 0.999365492298663);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess(), 200457.727272727);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.ess() / X.ncols(), 200457.727272727);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.fstat(), 15750.2500000000);
     BOOST_CHECK(nd >= 15);
     BOOST_CHECK(ols.dof() == 10);
     nd = ncsd(ols.rss(), 127.272727272727);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.rss() / ols.dof(), 12.7272727272727);
     BOOST_CHECK(nd >= 14);
}

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_noint2_test) {
     const Matdouble X{3, 1, {4, 5, 6}};
     const Vecdouble y{3, 4, 4};
     BOOST_CHECK(X.nrows() == y.size());
     int nd;
     OLS ols(X, y, false);

     nd = ncsd(ols.beta()(0), 0.727272727272727);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.standard_err()(0), 0.420827318078432E-01);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.ser(), 0.369274472937998);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.r2(), 0.993348115299335);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess(), 40.7272727272727);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess() / X.ncols(), 40.7272727272727);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.fstat(), 298.6666666666667);
     BOOST_CHECK(nd >= 15);
     BOOST_CHECK(ols.dof() == 2);
     nd = ncsd(ols.rss(), 0.272727272727273);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.rss() / ols.dof(), 0.136363636363636);
     BOOST_CHECK(nd >= 14);
}

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_filip_test) {
     const Matdouble X{
          82,
          1,
          {-6.860120914, -4.324130045, -4.358625055, -4.358426747,
           -6.955852379, -6.661145254, -6.355462942, -6.118102026,
           -7.115148017, -6.815308569, -6.519993057, -6.204119983,
           -5.853871964, -6.109523091, -5.79832982,  -5.482672118,
           -5.171791386, -4.851705903, -4.517126416, -4.143573228,
           -3.709075441, -3.499489089, -6.300769497, -5.953504836,
           -5.642065153, -5.031376979, -4.680685696, -4.329846955,
           -3.928486195, -8.56735134,  -8.363211311, -8.107682739,
           -7.823908741, -7.522878745, -7.218819279, -6.920818754,
           -6.628932138, -6.323946875, -5.991399828, -8.781464495,
           -8.663140179, -8.473531488, -8.247337057, -7.971428747,
           -7.676129393, -7.352812702, -7.072065318, -6.774174009,
           -6.478861916, -6.159517513, -6.835647144, -6.53165267,
           -6.224098421, -5.910094889, -5.598599459, -5.290645224,
           -4.974284616, -4.64454848,  -4.290560426, -3.885055584,
           -3.408378962, -3.13200249,  -8.726767166, -8.66695597,
           -8.511026475, -8.165388579, -7.886056648, -7.588043762,
           -7.283412422, -6.995678626, -6.691862621, -6.392544977,
           -6.067374056, -6.684029655, -6.378719832, -6.065855188,
           -5.752272167, -5.132414673, -4.811352704, -4.098269308,
           -3.66174277,  -3.2644011}};
     const Vecdouble y{
          0.8116, 0.9072, 0.9052, 0.9039, 0.8053, 0.8377, 0.8667,
          0.8809, 0.7975, 0.8162, 0.8515, 0.8766, 0.8885, 0.8859,
          0.8959, 0.8913, 0.8959, 0.8971, 0.9021, 0.909,  0.9139,
          0.9199, 0.8692, 0.8872, 0.89,   0.891,  0.8977, 0.9035,
          0.9078, 0.7675, 0.7705, 0.7713, 0.7736, 0.7775, 0.7841,
          0.7971, 0.8329, 0.8641, 0.8804, 0.7668, 0.7633, 0.7678,
          0.7697, 0.77,   0.7749, 0.7796, 0.7897, 0.8131, 0.8498,
          0.8741, 0.8061, 0.846,  0.8751, 0.8856, 0.8919, 0.8934,
          0.894,  0.8957, 0.9047, 0.9129, 0.9209, 0.9219, 0.7739,
          0.7681, 0.7665, 0.7703, 0.7702, 0.7761, 0.7809, 0.7961,
          0.8253, 0.8602, 0.8809, 0.8301, 0.8664, 0.8834, 0.8898,
          0.8964, 0.8963, 0.9074, 0.9119, 0.9228};
     BOOST_CHECK(X.nrows() == y.size());
     Matdouble X1(X.nrows(), 10);
     for (std::size_t i = 0; i < X1.nrows(); i++) {
          const double xi = X(i, 0);
          double t = xi;
          for (std::size_t j = 0; j < X1.ncols(); j++) {
               X1(i, j) = t;
               t *= xi;
          }
     }

     BOOST_CHECK_THROW(OLS(X1, y, true),
                       OLS::Singular_covariance_matrix);
}

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_longley_test) {
     const Matdouble X{
          16,
          6,
          {83.0,   234289, 2356,   1590,   107608, 1947,   88.5,
           259426, 2325,   1456,   108632, 1948,   88.2,   258054,
           3682,   1616,   109773, 1949,   89.5,   284599, 3351,
           1650,   110929, 1950,   96.2,   328975, 2099,   3099,
           112075, 1951,   98.1,   346999, 1932,   3594,   113270,
           1952,   99.0,   365385, 1870,   3547,   115094, 1953,
           100.0,  363112, 3578,   3350,   116219, 1954,   101.2,
           397469, 2904,   3048,   117388, 1955,   104.6,  419180,
           2822,   2857,   118734, 1956,   108.4,  442769, 2936,
           2798,   120445, 1957,   110.8,  444546, 4681,   2637,
           121950, 1958,   112.6,  482704, 3813,   2552,   123366,
           1959,   114.2,  502601, 3931,   2514,   125368, 1960,
           115.7,  518173, 4806,   2572,   127852, 1961,   116.9,
           554894, 4007,   2827,   130081, 1962}};
     const Vecdouble y{60323, 61122, 60171, 61187, 63221, 63639,
                       64989, 63761, 66019, 67857, 68169, 66513,
                       68655, 69564, 69331, 70551};
     BOOST_CHECK(X.nrows() == y.size());
     int nd;
     OLS ols(X, y, true);

     nd = ncsd(ols.beta()(0), -3482258.63459582);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.standard_err()(0), 890420.383607373);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(1), 15.0618722713733);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.standard_err()(1), 84.9149257747669);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(2), -0.358191792925910E-01);
     BOOST_CHECK(nd >= 8);
     nd = ncsd(ols.standard_err()(2), 0.334910077722432E-01);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.beta()(3), -2.02022980381683);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.standard_err()(3), 0.488399681651699);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.beta()(4), -1.03322686717359);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.standard_err()(4), 0.214274163161675);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(5), -0.511041056535807E-01);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.standard_err()(5), 0.226073200069370);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(6), 1829.15146461355);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.standard_err()(6), 455.478499142212);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.ser(), 304.854073561965);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.r2(), 0.995479004577296);
     BOOST_CHECK(nd >= 12);
     nd = ncsd(ols.ess(), 184172401.944494);
     BOOST_CHECK(nd >= 12);
     nd = ncsd(ols.ess() / X.ncols(), 30695400.3240823);
     BOOST_CHECK(nd >= 12);
     nd = ncsd(ols.fstat(), 330.285339234588);
     BOOST_CHECK(nd >= 9);
     BOOST_CHECK(ols.dof() == 9);
     nd = ncsd(ols.rss(), 836424.055505915);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.rss() / ols.dof(), 92936.0061673238);
     BOOST_CHECK(nd >= 9);
}

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_wampler1_test) {
     const Matdouble X{
          21, 1, {0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                  11, 12, 13, 14, 15, 16, 17, 18, 19, 20}};
     const Vecdouble y{1,      6,       63,      364,     1365,
                       3906,   9331,    19608,   37449,   66430,
                       111111, 177156,  271453,  402234,  579195,
                       813616, 1118481, 1508598, 2000719, 2613660,
                       3368421};
     BOOST_CHECK(X.nrows() == y.size());
     Matdouble X1(X.nrows(), 5);
     for (std::size_t i = 0; i < X1.nrows(); i++) {
          const double xi = X(i, 0);
          double t = xi;
          for (std::size_t j = 0; j < X1.ncols(); j++) {
               X1(i, j) = t;
               t *= xi;
          }
     }
     int nd;
     OLS ols(X1, y, true);

     nd = ncsd(ols.beta()(0), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(0), 0.000000000000000);
     BOOST_CHECK(nd >= 4);
     nd = ncsd(ols.beta()(1), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(1), 0.000000000000000);
     BOOST_CHECK(nd >= 4);
     nd = ncsd(ols.beta()(2), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(2), 0.000000000000000);
     BOOST_CHECK(nd >= 5);
     nd = ncsd(ols.beta()(3), 1.00000000000000);
     BOOST_CHECK(nd >= 7);
     nd = ncsd(ols.standard_err()(3), 0.000000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.beta()(4), 1.00000000000000);
     BOOST_CHECK(nd >= 8);
     nd = ncsd(ols.standard_err()(4), 0.000000000000000);
     BOOST_CHECK(nd >= 7);
     nd = ncsd(ols.beta()(5), 1.00000000000000);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.standard_err()(5), 0.000000000000000);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.ser(), 0.000000000000000);
     BOOST_CHECK(nd >= 4);
     nd = ncsd(ols.r2(), 1.00000000000000);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess(), 18814317208116.7);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.ess() / X1.ncols(), 3762863441623.33);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.fstat(), std::numeric_limits<double>::infinity());
     BOOST_CHECK(nd >= 0);
     BOOST_CHECK(ols.dof() == 15);
     nd = ncsd(ols.rss(), 0.000000000000000);
     BOOST_CHECK(nd >= 8);
     nd = ncsd(ols.rss() / ols.dof(), 0.000000000000000);
     BOOST_CHECK(nd >= 9);
}

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_wampler2_test) {
     const Matdouble X{
          21, 1, {0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                  11, 12, 13, 14, 15, 16, 17, 18, 19, 20}};
     const Vecdouble y{
          1.00000,  1.11111,  1.24992,  1.42753,  1.65984,  1.96875,
          2.38336,  2.94117,  3.68928,  4.68559,  6.00000,  7.71561,
          9.92992,  12.75603, 16.32384, 20.78125, 26.29536, 33.05367,
          41.26528, 51.16209, 63.00000};
     BOOST_CHECK(X.nrows() == y.size());
     Matdouble X1(X.nrows(), 5);
     for (std::size_t i = 0; i < X1.nrows(); i++) {
          const double xi = X(i, 0);
          double t = xi;
          for (std::size_t j = 0; j < X1.ncols(); j++) {
               X1(i, j) = t;
               t *= xi;
          }
     }
     int nd;
     OLS ols(X1, y, true);

     nd = ncsd(ols.beta()(0), 1.00000000000000);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.standard_err()(0), 0.000000000000000);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.beta()(1), 0.100000000000000);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.standard_err()(1), 0.000000000000000);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.beta()(2), 0.100000000000000E-01);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.standard_err()(2), 0.000000000000000);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(3), 0.100000000000000E-02);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.standard_err()(3), 0.000000000000000);
     BOOST_CHECK(nd >= 11);
     nd = ncsd(ols.beta()(4), 0.100000000000000E-03);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.standard_err()(4), 0.000000000000000);
     BOOST_CHECK(nd >= 12);
     nd = ncsd(ols.beta()(5), 0.100000000000000E-04);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.standard_err()(5), 0.000000000000000);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.ser(), 0.000000000000000);
     BOOST_CHECK(nd >= 9);
     nd = ncsd(ols.r2(), 1.00000000000000);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess(), 6602.91858365167);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess() / X1.ncols(), 1320.58371673033);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.fstat(), std::numeric_limits<double>::infinity());
     BOOST_CHECK(nd >= 0);
     BOOST_CHECK(ols.dof() == 15);
     nd = ncsd(ols.rss(), 0.000000000000000);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.rss() / ols.dof(), 0.000000000000000);
     BOOST_CHECK(nd >= 15);
}

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_wampler3_test) {
     const Matdouble X{
          21, 1, {0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                  11, 12, 13, 14, 15, 16, 17, 18, 19, 20}};
     const Vecdouble y{
          760.,     -2042.,   2111.,   -1684.,  3888.,    1858.,
          11379.,   17560.,   39287.,  64382.,  113159.,  175108.,
          273291.,  400186.,  581243., 811568., 1121004., 1506550.,
          2002767., 2611612., 3369180.};
     BOOST_CHECK(X.nrows() == y.size());
     Matdouble X1(X.nrows(), 5);
     for (std::size_t i = 0; i < X1.nrows(); i++) {
          const double xi = X(i, 0);
          double t = xi;
          for (std::size_t j = 0; j < X1.ncols(); j++) {
               X1(i, j) = t;
               t *= xi;
          }
     }
     int nd;
     OLS ols(X1, y, true);

     nd = ncsd(ols.beta()(0), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(0), 2152.32624678170);
     BOOST_CHECK(nd >= 11);
     nd = ncsd(ols.beta()(1), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(1), 2363.55173469681);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(2), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(2), 779.343524331583);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(3), 1.00000000000000);
     BOOST_CHECK(nd >= 7);
     nd = ncsd(ols.standard_err()(3), 101.475507550350);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(4), 1.00000000000000);
     BOOST_CHECK(nd >= 8);
     nd = ncsd(ols.standard_err()(4), 5.64566512170752);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(5), 1.00000000000000);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.standard_err()(5), 0.112324854679312);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.ser(), 2360.14502379268);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.r2(), 0.999995559025820);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess(), 18814317208116.7);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.ess() / X1.ncols(), 3762863441623.33);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.fstat(), 675524.458240122);
     BOOST_CHECK(nd >= 15);
     BOOST_CHECK(ols.dof() == 15);
     nd = ncsd(ols.rss(), 83554268.0000000);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.rss() / ols.dof(), 5570284.53333333);
     BOOST_CHECK(nd >= 15);
}

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_wampler4_test) {
     const Matdouble X{
          21, 1, {0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                  11, 12, 13, 14, 15, 16, 17, 18, 19, 20}};
     const Vecdouble y{
          75901,   -204794, 204863,  -204436, 253665,  -200894,
          214131,  -185192, 221249,  -138370, 315911,  -27644,
          455253,  197434,  783995,  608816,  1370781, 1303798,
          2205519, 2408860, 3444321,

     };
     BOOST_CHECK(X.nrows() == y.size());
     Matdouble X1(X.nrows(), 5);
     for (std::size_t i = 0; i < X1.nrows(); i++) {
          const double xi = X(i, 0);
          double t = xi;
          for (std::size_t j = 0; j < X1.ncols(); j++) {
               X1(i, j) = t;
               t *= xi;
          }
     }
     int nd;
     OLS ols(X1, y, true);

     nd = ncsd(ols.beta()(0), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(0), 215232.624678170);
     BOOST_CHECK(nd >= 11);
     nd = ncsd(ols.beta()(1), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(1), 236355.173469681);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(2), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(2), 77934.3524331583);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(3), 1.00000000000000);
     BOOST_CHECK(nd >= 7);
     nd = ncsd(ols.standard_err()(3), 10147.5507550350);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(4), 1.00000000000000);
     BOOST_CHECK(nd >= 8);
     nd = ncsd(ols.standard_err()(4), 564.566512170752);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(5), 1.00000000000000);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.standard_err()(5), 11.2324854679312);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.ser(), 236014.502379268);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.r2(), 0.957478440825662);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.ess(), 18814317208116.7);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.ess() / X1.ncols(), 3762863441623.33);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.fstat(), 67.5524458240122);
     BOOST_CHECK(nd >= 15);
     BOOST_CHECK(ols.dof() == 15);
     nd = ncsd(ols.rss(), 835542680000.000);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.rss() / ols.dof(), 55702845333.3333);
     BOOST_CHECK(nd >= 15);
}

/// See \ref strdlr.
BOOST_AUTO_TEST_CASE(strdlr_wampler5_test) {
     const Matdouble X{
          21, 1, {0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                  11, 12, 13, 14, 15, 16, 17, 18, 19, 20}};
     const Vecdouble y{7590001,  -20479994, 20480063, -20479636,
                       25231365, -20476094, 20489331, -20460392,
                       18417449, -20413570, 20591111, -20302844,
                       18651453, -20077766, 21059195, -19666384,
                       26348481, -18971402, 22480719, -17866340,
                       10958421};
     BOOST_CHECK(X.nrows() == y.size());
     Matdouble X1(X.nrows(), 5);
     for (std::size_t i = 0; i < X1.nrows(); i++) {
          const double xi = X(i, 0);
          double t = xi;
          for (std::size_t j = 0; j < X1.ncols(); j++) {
               X1(i, j) = t;
               t *= xi;
          }
     }
     int nd;
     OLS ols(X1, y, true);

     nd = ncsd(ols.beta()(0), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(0), 21523262.4678170);
     BOOST_CHECK(nd >= 11);
     nd = ncsd(ols.beta()(1), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(1), 23635517.3469681);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(2), 1.00000000000000);
     BOOST_CHECK(nd >= 6);
     nd = ncsd(ols.standard_err()(2), 7793435.24331583);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(3), 1.00000000000000);
     BOOST_CHECK(nd >= 7);
     nd = ncsd(ols.standard_err()(3), 1014755.07550350);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(4), 1.00000000000000);
     BOOST_CHECK(nd >= 8);
     nd = ncsd(ols.standard_err()(4), 56456.6512170752);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.beta()(5), 1.00000000000000);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.standard_err()(5), 1123.24854679312);
     BOOST_CHECK(nd >= 10);
     nd = ncsd(ols.ser(), 23601450.2379268);
     BOOST_CHECK(nd >= 14);
     nd = ncsd(ols.r2(), 0.224668921574940E-02);
     BOOST_CHECK(nd >= 13);
     nd = ncsd(ols.ess(), 18814317208116.7);
     BOOST_CHECK(nd >= 13);
     nd = ncsd(ols.ess() / X1.ncols(), 3762863441623.33);
     BOOST_CHECK(nd >= 13);
     nd = ncsd(ols.fstat(), 6.7552445824012241E-03);
     BOOST_CHECK(nd >= 13);
     BOOST_CHECK(ols.dof() == 15);
     nd = ncsd(ols.rss(), 0.835542680000000E+16);
     BOOST_CHECK(nd >= 15);
     nd = ncsd(ols.rss() / ols.dof(), 557028453333333.);
     BOOST_CHECK(nd >= 15);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting

#ifdef _MSC_VER
#pragma warning(pop)
#endif
