/* gmconsts.cc: generates mathematical constants */

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <gmpxx.h>
#include <nr/nr3.h>
#include <nr/fourier.h>
#include <nr/mparith.h>

void generate(const int prec, stringstream& ss) {
     const string spi = MParith().mppi(1000);
     const mpf_class pi(spi, prec);

     ss.str("");
     ss.clear();
     ss << fixed << setprecision(41);
     ss << "ipi      = " << 1.0 / pi << "; // 1 / pi\n";
     ss << "isqrt2pi = " << 1.0 / sqrt(2.0 * pi)
        << "; // 1 / sqrt(2 * pi)\n";
     ss << "sqrt2pi  = " << sqrt(2.0 * pi) << "; // sqrt(2 * pi)\n";
     {
          const mpf_class j(1.0, prec);
          mpf_class s(0.0, prec);
          mpf_class p(2.0, prec);
          mpf_class n(1.0, prec);
          mpf_class r(0.0, prec);
          for (int i = 0; i < 200; i++) {
               s += j / (n * p);
               r = 1.0 / s;
               n++;
               p = 2 * p;
          }
          ss << "iln2     = " << r << "; // 1 / ln 2\n";
     }
     {
          const mpf_class one(1.0, prec);
          const mpf_class two(2.0, prec);
          const mpf_class isqrt2 = one / sqrt(two);
          ss << "isqrt2   = " << isqrt2 << "; // 1 / sqrt(2)\n";
     }
     ss << "sqrpi8   = " << pi * pi / 8.0 << "; // pi * pi / 8\n";
}

int main() {
     stringstream ss;
     generate(2000, ss);
     assert(ss);
     stringstream ss1;
     generate(4000, ss1);
     assert(ss1);
     assert(ss.str() == ss1.str());
     cout << ss.str();
}
