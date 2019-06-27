/* bdstest.cc: testing BDS test */

/**
 * \file testing/bdstest.cc
 * Testing BDS test.
 * \date Created on 12 October 2013.
 */

#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "shg/bdstest.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::abs;
using std::invalid_argument;
using std::sqrt;
using std::vector;

namespace {

/**
 * Test data from \cite lebaron-1991.
 *
 * Cf. also \cite brock-hsieh-lebaron-1991 and
 * \cite brock-dechert-scheinkman-lebaron-1996.
 */
const vector<double> input {
     -208, -19, 70, 271, -87, -199, 76, 69, 187, -66,
     125, -85, -374, 234, -29, -172, -170, 266, 385, 78,
     8, 84, 77, -16, 165, 220, 141, 102, 74, -350,
     -161, 364, -35, -132, -10, -115, 37, 41, -300, -85,
     38, -29, 38, 62, -24, -102, 326, 178, -157, -232,
     -323, 58, 6, -174, 155, 20, -274, -155, 11, 169,
     -68, -281, 335, -144, 9, -241, 45, -317, -337, 377,
     -199, -182, 56, 252, -66, 2, -133, -422, 239, -80,
     -106, -29, -491, -126, -227, 78, -40, 75, 407, 222,
     -394, -89, 24, 198, 359, 189, 77, -335, -73, -18,
     -294, -62, -219, -46, 130, 148, -168, 259, -2, -227,
     -136, -262, -312, 89, 296, 242, -146, -114, 189, 439,
     -89, 132, -31, -43, 248, 167, 120, 53, -173, 69,
     -99, 114, -596, -757, 405, -377, -165, -24, 87, -36,
     216, 188, -192, 26, 18, -54, -302, 3, -357, 8,
     -70, 23, -57, -70, 231, -180, -115, -39, 258, -32,
     -310, -140, -91, -6, 90, -119, 92, -138, -475, -2,
     10, 197, -380, -270, -259, -305, 381, 30, 41, -5,
     -110, 45, 486, 16, 337, -58, 321, -60, 267, -32,
     54, -32, -115, 102, -141, -322, 3, -67, -71, 185,
     -144, -338, -356, -52, -197, -151, 124, 184, 29, 170,
     -138, 100, -25, 353, -180, -177, -200, -97, 100, 326,
     1, 63, -39, -233, -171, -38, -15, 54, 264, -269,
     99, -320, 236, -265, -243, 178, -137, 2, 146, 450,
     -100, 255, 148, 40, 226, -123, -14, 29, -270, 346,
     -141, -317, -145, 175, 123, 100, -54, -231, -409, 238,
     88, -132, 362, -297, 121, 110, -239, 245, 98, -19,
     204, 43, 303, -29, -229, 258, 301, 318, 197, -76,
     -163, 88, -375, 270, -75, -141, 133, 90, -211, -30,
     199, 89, 1, 262, 16, -554, -242, 105, -2, -38,
     -311, 190, 88, 40, -214, 159, -52, -100, -296, -13,
     352, -18, -90, 387, 215, 41, -354, -186, -332, -133,
     -145, -227, 268, 277, 193, -310, 150, 313, -97, 297,
     -176, -31, 115, 227, 145, -12, 12, 107, 204, -118,
     232, 62, 388, -108, 148, -312, -239, 186, 380, 169,
     39, -47, 269, 267, -176, -22, -250, -299, 243, 45,
     51, -449, -257, 79, 318, 230, 129, -61, 202, -134,
     124, 51, 215, 14, 278, -57, 160, 59, 386, -54,
     -160, 242, -42, -82, 34, -182, 157, -150, 313, -73,
     242, 33, 209, -86, -133, 95, -139, 97, 56, -225,
     210, -64, -36, 35, 20, -125, 285, 210, -167, 79,
     286, 54, 66, -165, -164, -12, 370, 173, -216, 122,
     45, 56, 36, -66, -200, 26, -87, -63, -25, 131,
     46, -240, 54, 135, 329, -226, -10, 138, -127, 171,
     -110, 295, -244, 239, 291, -179, -55, 344, 372, 357,
     -126, -297, -92, 161, -52, -163, 273, 141, 43, -119,
     30, 6, -143, -212, 15, -36, -255, -46, -113, -218,
     -9, 320, -346, -213, 4, -372, 17, 100, -305, 47,
     17, -60, -260, 331, 131, 122, 97, -152, -139, -219,
     127, -2, 159, 150, -162, 311, 88, -108, 279, -248,
};

/**
 * Borosh-Niederreiter random number generator. See \cite knuth-2002b,
 * p. 113.
 */
double bn() {
     static unsigned long int x = 1ul;
     x = (1812433253ul * x) & 0xfffffffful;
     return x / 4294967296.0;
}

void test_case1() {
     const vector<SHG::BDS_test::Result> res {
          { 0.00000000, 0.00000000},
          { 0.00000000, 0.00000000},
          {-0.26335571, 0.39613821},
          {-1.49073535, 0.06801550},
          {-1.39766924, 0.08110621},
          {-0.73963905, 0.22975952}
     };
     const vector<double> eps {100.0};
     SHG::BDS_test b(input, 5, eps);
     SHG_ASSERT(b.res().size() == 1);
     SHG_ASSERT(b.res()[0].size() == 6);
     for (int m = 0; m <= 5; m++) {
          SHG_ASSERT(abs(b.res()[0][m].stat - res[m].stat) < 5e-9);
          SHG_ASSERT(abs(b.res()[0][m].pval - res[m].pval) < 5e-9);
     }
}

void test_case2() {
     const vector<double> u(1000, 13.0);
     const vector<double> epsu {1.0};
     SHG::BDS_test b1(u, 6, epsu);
     SHG_ASSERT(b1.res().size() == 1);
     SHG_ASSERT(b1.res()[0].size() == 7);
     SHG_ASSERT(abs(b1.res()[0][0].stat) < 5e-12);
     SHG_ASSERT(abs(b1.res()[0][0].pval) < 5e-12);
     SHG_ASSERT(abs(b1.res()[0][1].stat) < 5e-12);
     SHG_ASSERT(abs(b1.res()[0][1].pval) < 5e-12);
     for (int m = 2; m <= 6; m++) {
          SHG_ASSERT(abs(b1.res()[0][m].stat) < 5e-12);
          SHG_ASSERT(abs(b1.res()[0][m].pval - 0.5) < 5e-12);
     }
}

void test_case3() {
     const vector<SHG::BDS_test::Result> res1 {
          {    0.00000000, 0.00000000},
          {    0.00000000, 0.00000000},
          { 3956.79991436, 0.00000000},
          { 5315.32144067, 0.00000000},
          { 7421.54466769, 0.00000000},
          {10878.81568895, 0.00000000},
          {16628.13935776, 0.00000000},
          {26309.31184862, 0.00000000}
     };
     const vector<SHG::BDS_test::Result> res2 {
          {    0.00000000, 0.00000000},
          {    0.00000000, 0.00000000},
          { 3956.79991436, 0.00000000},
          { 5315.32144067, 0.00000000},
          { 7421.54466769, 0.00000000},
          {10878.81568895, 0.00000000},
          {16628.13935776, 0.00000000},
          {26309.31184862, 0.00000000}
     };
     const vector<SHG::BDS_test::Result> res3 {
          {    0.00000000, 0.00000000},
          {    0.00000000, 0.00000000},
          {    0.00000000, 0.50000000},
          {    0.00000000, 0.50000000},
          {    0.00000000, 0.50000000},
          {    0.00000000, 0.50000000},
          {    0.00000000, 0.50000000},
          {    0.00000000, 0.50000000}
     };

     vector<double> u(1000, 13.0);
     for (vector<double>::size_type i = 1; i < u.size(); i += 2)
          u[i] = 14.0;
     const vector<double> eps {0.5, 1.0, 1.0001};
     const int maxm = 7;
     SHG::BDS_test b(u, maxm, eps);
     SHG_ASSERT(b.res().size() == eps.size());
     for (size_t i = 0; i < eps.size(); i++) {
          SHG_ASSERT(b.res()[i].size() ==
                     static_cast<size_t>(maxm) + 1);
          const vector<SHG::BDS_test::Result>* p = nullptr;
          if (i == 0)
               p = &res1;
          else if (i == 1)
               p = &res2;
          else
               p = &res3;
          for (size_t j = 0; j < b.res()[i].size(); j++) {
               SHG_ASSERT(abs(b.res()[i][j].stat - (*p)[j].stat) <
                          6e-8);
               SHG_ASSERT(abs(b.res()[i][j].pval - (*p)[j].pval) <
                          5e-9);
          }
     }
}

void test_case4() {
     vector<double> u, eps;
     int maxm;
     bool ok;
     {
          u.clear();
          maxm = 1;
          eps = {1.0};
          ok = false;
          try {
               SHG::BDS_test b(u, maxm, eps);
          } catch (const invalid_argument&) {
               ok = true;
          }
          SHG_ASSERT(ok);
     }
     {
          u = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
          maxm = 1;
          eps = {1.0};
          ok = false;
          try {
               SHG::BDS_test b(u, maxm, eps);
          } catch (const invalid_argument&) {
               ok = true;
          }
          SHG_ASSERT(ok);
     }
     {
          u = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
          maxm = 9;
          eps = {1.0};
          ok = true;
          try {
               SHG::BDS_test b(u, maxm, eps);
          } catch (const invalid_argument&) {
               ok = false;
          }
          SHG_ASSERT(ok);
     }
     {
          u = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
          maxm = 10;
          eps = {1.0};
          ok = false;
          try {
               SHG::BDS_test b(u, maxm, eps);
          } catch (const invalid_argument&) {
               ok = true;
          }
          SHG_ASSERT(ok);
     }
     {
          u = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
          maxm = 9;
          eps = {};
          ok = false;
          try {
               SHG::BDS_test b(u, maxm, eps);
          } catch (const invalid_argument&) {
               ok = true;
          }
          SHG_ASSERT(ok);
     }
}

void test_case5() {
     const char* const res = {
          "0.28868 2 0.27392 0.39207\n"
          "0.28868 3 0.26732 0.39461\n"
          "0.28868 4 -0.33474 0.36891\n"
          "\n"
          "0.52042 2 -3.96242 0.00004\n"
          "0.52042 3 0.39043 0.34811\n"
          "0.52042 4 -0.07102 0.47169\n"
     };

     vector<double> u(1000);
     std::stringstream ss;
     ss << std::fixed << std::setprecision(5);

     for (vector<double>::size_type i = 0; i < u.size(); i++)
          u[i] = bn();
     // eps is the standard deviation of U(0, 1).
     ss << BDS_test(u, 4, {sqrt(1.0 / 12.0)}) << '\n';

     for (vector<double>::size_type i = 0; i < u.size(); i++)
          u[i] = i % 2 ? 2.0 * bn() : bn();
     // eps is the standard deviation of the mixture of U(0, 1) and
     // U(0, 2) with mixing weights 0.5.
     ss << BDS_test(u, 4, {sqrt(13.0 / 48.0)});

     SHG_ASSERT(std::strcmp(ss.str().c_str(), res) == 0);
}

}       // anonymous namespace

void test_bdstest() {
     test_case1();
     test_case2();
     test_case3();
     test_case4();
     test_case5();
}

}       // namespace Testing
}       // namespace SHG
