/* drbnwtsn.cc: testing Durbin-Watson statistic */

/**
 * \file testing/drbnwtsn.cc
 * Testing Durbin-Watson statistic.
 * \date Created on 12 July 2010.
 */

#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include "shg/utils.h"
#include "shg/drbnwtsn.h"
#include "drbnwts1.h"
#include "drbnwts2.h"
#include "drbnwts3.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::abs;
using std::min;
using std::string;
using std::getline;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::istringstream;
using SHG::dwcdf;
using SHG::ppdw;
using SHG::swtbl;
using SHG::rtrim;

namespace {

void test_cumulative_distribution_function() {
     static const int n[20] =  {
          6, 7, 8, 9, 10, 15, 20, 25, 30, 35, 40,
          45, 50, 60, 70, 80, 90, 100, 150, 200
     };

     int c = 0;
     for (int i = 0; i < 20; i++) {
          const int maxk = min(10, n[i] - 5);
          for (int k = 1; k <= maxk; k++)
               for (int j = 0; j <= 20; j++, c++) {
                    const double x = j / 5.0;
                    const double
                         pdl = dwcdf(n[i], k, x, true, 1e-7, 17);
                    const double
                         pdu = dwcdf(n[i], k, x, false, 1e-7, 17);
                    SHG_ASSERT(abs(pdl - tab_nag[c].pdl) < 5e-5);
                    SHG_ASSERT(abs(pdu - tab_nag[c].pdu) < 5e-5);
               }
     }
     SHG_ASSERT(c == NELEMS(tab_nag));
}

void test_percentage_points() {
     double xl[5], xu[5], xl0, xu0, tol;
     int n;

     istringstream f(savin_white_table1);
     SHG_ASSERT(static_cast<bool>(f));
     {
          string s;
          for (int i = 0; i < 5; i++)
               getline(f, s);
          SHG_ASSERT(static_cast<bool>(f));
     }

     for (int i = 0; i < 49; i++) {
          f >> n
            >> xl[0] >> xu[0]
            >> xl[1] >> xu[1]
            >> xl[2] >> xu[2]
            >> xl[3] >> xu[3]
            >> xl[4] >> xu[4];
          const int kk = min(5, n - 5);
          for (int k = 1; k <= kk; k++) {
               ppdw(n, k, 1e-2, &xl0, &xu0, 4.9e-5);
               tol = 0.5e-3;
               // These are differences between our function and the
               // tables of Savin and White greater than 0.5e-3. There
               // are 6 differences for d(l):
               if (n ==  10 && k == 1) tol = 0.6e-3;
               if (n ==  16 && k == 1) tol = 0.6e-3;
               if (n ==  18 && k == 2) tol = 0.6e-3;
               if (n ==  40 && k == 5) tol = 1.1e-3;
               if (n == 200 && k == 2) tol = 0.8e-3;
               if (n == 200 && k == 3) tol = 0.8e-3;
               SHG_ASSERT(abs(xl0 - xl[k - 1]) < tol);
               tol = 0.5e-3;
               // and 9 differences for d(u):
               if (n ==  10 && k == 2) tol = 0.6e-3;
               if (n ==  11 && k == 4) tol = 0.6e-3;
               if (n ==  18 && k == 1) tol = 0.6e-3;
               if (n ==  20 && k == 2) tol = 0.6e-3;
               if (n ==  23 && k == 4) tol = 0.6e-3;
               if (n ==  29 && k == 4) tol = 0.6e-3;
               if (n ==  31 && k == 2) tol = 0.6e-3;
               if (n == 200 && k == 2) tol = 0.8e-3;
               if (n == 200 && k == 4) tol = 0.8e-3;
               SHG_ASSERT(abs(xu0 - xu[k - 1]) < tol);
          }
     }
}

void test_table() {
     string s, t;
     stringstream ss;
     swtbl(ss);
     istringstream f(savin_white_table2);
     SHG_ASSERT(static_cast<bool>(f));
     getline(f, t);
     SHG_ASSERT(static_cast<bool>(f));
     while (getline(ss, s)) {
          rtrim(s);
          getline(f, t);
          SHG_ASSERT(s == t);
     }
}

}       // anonymous namespace

void test_durbin_watson_statistic() {
     test_cumulative_distribution_function();
     test_percentage_points();
     test_table();
}

}       // namespace Testing
}       // namespace SHG
