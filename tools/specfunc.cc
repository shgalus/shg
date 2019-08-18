/* specfunc.cc: generates tables for testing special functions */

#include <cassert>
#include "nr/nr3.h"
#include "nr/erf.h"
#include "nr/gamma.h"
#include "nr/incgammabeta.h"

int main() {
     cout << "# specfunc.txt: tables for testing special functions\n";
     cout << "# See tools/specfunc.cc and testing/specfunc.cc.\n\n";

     // loggamma(x) for x in 1.0(0.01)2.0 and 0.25(0.25)100.0.
     cout << scientific;
     for (int i = 0; i <= 100; i++)
          cout << setprecision(14) << setw(21)
               << lgamma(1.0 + i * 0.01) << '\n';
     for (int i = 0; i < 400; i++)
          cout << setprecision(14) << setw(21)
               << lgamma((i + 1) * 0.25) << '\n';

     // alnorm(x) for x in -10.00(0.05)10.00.
     cout << fixed;
     {
          Normaldist nd;
          for (int i = 0; i <= 400; i++) {
               cout << setprecision(15) << setw(17) <<
                    nd.cdf(-10.0 + i * 0.05) << '\n';
          }
     }

     // ppnd7(x) for x in 0.005(0.005)0.995.
     cout << scientific;
     {
          Normaldist nd;
          for (int i = 0; i < 199; i++)
               cout << setprecision(14) << setw(21) <<
                    nd.invcdf(0.005 * (i + 1)) << '\n';
     }

     // gammad(x, p) for x in 0.0(0.1)10.0, p in 1.0(0.1)10.0.
     cout << scientific;
     {
          Gamma gam;
          for (int ix = 0; ix <= 100; ix++) {
               const double x = ix * 0.1;
               for (int p = 1; p <= 100; p++)
                    cout << setprecision(14) << setw(21)
                         << gam.gammp(p * 0.1, x) << '\n';
          }
     }

     // probst(t, df) for t in 0.0(0.1)8.0, df in 1(1)18.
     cout << scientific;
     for (int df = 1; df <= 18; df++) {
          Studenttdist s(df);
          for (int i = 0; i <= 80; i++)
               cout << setprecision(14) << setw(21)
                    << s.cdf(i * 0.1) << '\n';
     }

     // betain(x, p, q) for x in 0.1(0.1)0.9, p in 1.0(1.0)10.0, q in
     // 1.0(1.0)10.0.
     cout << scientific;
     {
          Beta b;
          for (int i = 1; i <= 9; i++) {
               const double x = i * 0.1;
               for (int p = 1; p <= 10; p++)
                    for (int q = 1; q <= 10; q ++)
                         cout << setprecision(14) << setw(21)
                              << b.betai(p, q, x) << '\n';
          }
     }

     // cdffdist(m, n, x) for m in 1(1)10, n in 1(1)10, x in
     // 1.0(1.0)10.0.
     cout << scientific;
     for (int m = 1; m <= 10; m++)
          for (int n = 1; n <= 10; n++) {
               Fdist f(m, n);
               for (int x = 1; x <= 10; x++)
                    cout << setprecision(14) << setw(21)
                         << f.cdf(x) << '\n';
          }
}
