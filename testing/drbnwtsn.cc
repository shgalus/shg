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
#include "shg/drbnwtsn.h"
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
using SHG::dwcdf;
using SHG::ppdw;
using SHG::swtbl;

namespace {

/**
 * Compares the results of calls to dwcdf with those read from the
 * file drbnwts1.txt. If the difference is too big, an SHG_ASSERT(0)
 * is executed. The results in the input file come from the function
 * dwcdf itself and were checked to be equal to the results of the
 * function nag-prob-durbin-watson (g01epc) from the NAG C library,
 * mark 7 (2001).
 */
void cdftst() {
     static const int n[20] =  {
          6, 7, 8, 9, 10, 15, 20, 25, 30, 35, 40,
          45, 50, 60, 70, 80, 90, 100, 150, 200
     };

     ifstream f(datapath("drbnwts1.txt").c_str());
     SHG_ASSERT(f);
     {
          string s;
          getline(f, s);
          SHG_ASSERT(f);
     }

     for (int i = 0; i < 20; i++) {
          const int kk = min(10, n[i] - 5);
          for (int k = 1; k <= kk; k++)
               for (int j = 0; j <= 20; j++) {
                    const double x = j / 5.0;
                    const double
                         pdl = dwcdf(n[i], k, x, true, 1e-7, 17);
                    const double
                         pdu = dwcdf(n[i], k, x, false, 1e-7, 17);
                    double pdl0, pdu0;
                    f >> pdl0 >> pdu0;
                    SHG_ASSERT(f);
                    SHG_ASSERT(abs(pdl - pdl0) < 5e-5);
                    SHG_ASSERT(abs(pdu - pdu0) < 5e-5);
               }
     }
}

/**
 * The function compares the results of calls to ppdw with those read
 * from the file drbnwts2.txt. This file contains a portion of the
 * table from \cite savin-white-1977. If the difference is too big, an
 * SHG_ASSERT(0) is executed.
 *
 * \warning In the table in drbnwts2.txt, for n = 6, 7, 8, 9, the
 * inadequate positions were filled with 0.000 instead of dashes to
 * keep one input format.
 */
void ppdtst() {
     double xl[5], xu[5], xl0, xu0, tol;
     int n;

     ifstream f(datapath("drbnwts2.txt").c_str());
     SHG_ASSERT(f);
     {
          string s;
          for (int i = 0; i < 5; i++)
               getline(f, s);
          SHG_ASSERT(f);
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

void swtbltst() {
     string s, t;
     stringstream ss;
     swtbl(ss);
     ifstream f(datapath("drbnwts3.txt").c_str());
     SHG_ASSERT(f);
     while (getline(ss, s)) {
          getline(f, t);
          SHG_ASSERT(s == t);
     }
}

}       // anonymous namespace

void test_drbnwtsn() {
     cdftst();
     ppdtst();
     swtbltst();
}

}       // namespace Testing
}       // namespace SHG
