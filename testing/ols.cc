/* ols.cc: testing ordinary least squares */

/**
 * \file testing/ols.cc
 * Testing ordinary least squares.
 * \date Created on 28 November 2010.
 */

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <limits>
#include <string>
#include "shg/strdlr.h"
#include "shg/ols.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::numeric_limits;
using std::ofstream;
using std::size_t;
using std::string;

namespace {

void test1() {
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
          try {
               OLS o(X, y, false);
          }
          catch (OLS::Singular_covariance_matrix) {
               SHG_ASSERT(0);
          }
     }
}

void test2() {
     const int k = 2;
     const int n = 5;
     Vecdouble y(n);
     Matdouble X(n, k);
     for (int i = 0; i < n; i++) {
          y(i) = i * i * i;
          X(i, 0) = i;
          X(i, 1) = i;
     }
     try {
          OLS o(X, y, true);
          SHG_ASSERT(0);
     }
     catch (OLS::Singular_covariance_matrix) {}
}

void test3() {                  // Test for zero degrees of freedom.
     const int n = 5;
     const int k = 4;
     Vecdouble y(n);
     Matdouble X(n, k, 0.0);
     for (int i = 0; i < n; i++) {
          y(i) = i;
          if (i < k)
               X(i, i) = 1.0;
     }
     OLS o(X, y, true);
     // The same with y = 0.
     y = 0;
     OLS o1(X, y, true);
}

void test4() {                  // A test with constant y.
     const int n = 3;
     const int k = 1;
     Vecdouble y(n);
     Matdouble X(n, k, 0.0);
     for (int i = 0; i < k; i++)
          X(i, i) = 1.0;

     y = 0.0;
     OLS o1(X, y, true);
     OLS o2(X, y, false);

     y = 5.0;
     OLS o3(X, y, true);
     OLS o4(X, y, false);
}

void test5() {                  // A test with too many regressors.
     Vecdouble y(4);
     Matdouble X(4, 4);
     try {
          OLS ols(X, y);
          SHG_ASSERT(false);
     }
     catch (OLS::Invalid_argument) {}
}

void test6(ofstream& f) {       // Data from the book of Goldberger.
     const Vecdouble y {0, 2, 1, 2, -1, 1};
     const Matdouble X(6, 2, {-1, 0, 0, 1, 1, 0, 2, 1, 0, -1, 0, 0});
     OLS ols(X, y);
     ols.set_problem_name("Goldberger, p.213, 226, 238.");
     ols.dw();
     ols.print(f);
     f << '\n';
}

const Vecdouble greene7_8_y {
     129.7, 131.3, 137.1, 141.6, 148.8, 155.9, 164.9, 171.0, 183.4,
     195.8, 207.4, 218.3, 226.8, 237.9, 225.8, 232.4, 241.7, 249.2,
     261.3, 248.9, 226.8, 225.6, 228.8, 239.6, 244.7, 245.8, 269.4,
     276.8, 279.9, 284.1, 282.0, 271.8, 280.2, 286.7, 290.2, 297.8
};

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

void test7(ofstream& f) {       // GRETL 1.7.5 sample data, Greene 7.8.
     Matdouble X(greene7_8_X);
     for (size_t i = 0; i < X.nrows(); i++)
          X(i, 0) += 1900.0;
     OLS ols(X, greene7_8_y);
     ols.set_problem_name("Greene7_8, gasoline price and consumption.");
     ols.dw();
     ols.print(f);
     f << '\n';
}

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
          return numeric_limits<double>::digits10;
     if (std::isinf(c) || std::isinf(q))
          return 0.0;
     const double le = c == 0.0 ?
          -std::log10(std::abs(q)) :
          -std::log10(std::abs(q - c) / std::abs(c));
     SHG_ASSERT(!std::isnan(le));
     return le;
}

void test_nist(ofstream& f, StRDLR& d, size_t n) {
     double minle;
     int mincsd;
     auto update = [&minle, &mincsd](double q, double c) {
          const double le = log_error(q, c);
          int nd;          // number of correct significant digits
          if (le < 1.0)
               nd = 0;
          else if (le > numeric_limits<double>::digits10)
               nd = numeric_limits<double>::digits10;
          else
               nd = static_cast<int>(le);
          if (le < minle)
               minle = le;
          if (nd < mincsd)
               mincsd = nd;
     };

     d.active(n);
     try {
          const bool intercept = n != d.noint1 && n != d.noint2;
          const int p = intercept ? d.k() + 1 : d.k();
          OLS ols(d.X(), d.y(), intercept);
          minle = numeric_limits<double>::max();
          mincsd = numeric_limits<int>::max();

          // Compare values calculated by OLS with certified values.
          for (int i = 0; i < p; i++) {
               update(ols.beta()(i), d.b()(i));
               update(ols.stderr()(i), d.sdevb()(i));
          }
          update(ols.ser(), d.sdevres());
          update(ols.r2(), d.r2());
          SHG_ASSERT(d.k() == static_cast<size_t>(d.dfreg()));
          update(ols.ess(), d.ssreg());
          update(ols.ess() / d.k(), d.msreg());
          update(ols.fstat(), d.fstat());
          SHG_ASSERT(ols.dof() == d.dfres());
          update(ols.rss(), d.ssres());
          update(ols.rss() / ols.dof(), d.msres());

          f << std::left << std::setw(10) << d.name() + string(": ")
            << std::right << std::fixed << std::setprecision(4)
            << "results correct to at least " << std::setw(2)
            << mincsd << " digits (LRE = " << minle << ")\n";
          if (n == d.wampler1 || n == d.wampler2) {
               f << std::scientific << std::setprecision(12)
                 << "rss = " << ols.rss()
                 << ", orig rss = " << d.ssres()
                 << "\nfstat = " << ols.fstat()
                 << ", orig fstat =  " << d.fstat() << '\n';
          }
     } catch (OLS::Singular_covariance_matrix) {
          f << std::left << std::setw(10) << d.name() + string(": ")
            << std::right << "could not invert matrix" << '\n';
     }
}

void test8(ofstream& f) {
     StRDLR d;
     for (size_t n = 0; n < d.nsets(); n++) {
          test_nist(f, d, n);
     }
}

}       // anonymous namespace

void test_ols() {
     static_assert(numeric_limits<double>::has_infinity,
                   "double must have infinity");
     header("test_ols()");
     test1();
     test2();
     test3();
     test4();
     test5();
     test6(logfile);
     test7(logfile);
     test8(logfile);
     footer("test_ols()");
}

}       // namespace Testing
}       // namespace SHG
