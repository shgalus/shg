/* specfunc.cc: generates tables for testing special functions */

#include "nr/nr3.h"
#include "nr/erf.h"
#include "nr/gamma.h"
#include "nr/incgammabeta.h"

const char* const indent = "     ";
const char* const head = R"(namespace SHG {
namespace Testing {

)";
const char* const tail = R"(
}       // namespace Testing
}       // namespace SHG
)";

void ppnd7() {
     ofstream f("ppnd7.cc");
     f << "#include \"ppnd7.h\"\n\n";
     f << head;
     f << "// ppnd7(x) for x in 0.005(0.005)0.995.\n";
     f << "extern const double tab_ppnd7[199] = {\n";
     f << scientific;
     Normaldist normaldist;
     for (int i = 0; i < 199; i++)
          f << indent << setprecision(14) << setw(21) <<
               normaldist.invcdf(0.005 * (i + 1)) << ",\n";
     f << "};\n";
     f << tail;
}

void gammad() {
     ofstream f("gammad.cc");
     f << "#include \"gammad.h\"\n\n";
     f << head;
     f << "// gammad(x, p) for x in 0.0(0.1)10.0, p in 1.0(0.1)10.0.\n";
     f << "extern const double tab_gammad[10100] = {\n";
     f << scientific;
     Gamma gamma;
     for (int ix = 0; ix <= 100; ix++) {
          const double x = ix * 0.1;
          for (int p = 1; p <= 100; p++)
               f << indent << setprecision(14) << setw(21)
                 << gamma.gammp(p * 0.1, x) << ",\n";
     }
     f << "};\n";
     f << tail;
}

void probst() {
     ofstream f("probst.cc");
     f << "#include \"probst.h\"\n\n";
     f << head;
     f << "//  probst(t, df) for t in 0.0(0.1)8.0, df in 1(1)18.\n";
     f << "extern const double tab_probst[1458] = {\n";
     f << scientific;
     for (int df = 1; df <= 18; df++) {
          Studenttdist studenttdist(df);
          for (int i = 0; i <= 80; i++)
               f << indent << setprecision(14) << setw(21)
                 << studenttdist.cdf(i * 0.1) << ",\n";
     }
     f << "};\n";
     f << tail;
}

void betain() {
     ofstream f("betain.cc");
     f << "#include \"betain.h\"\n\n";
     f << head;
     f << "// betain(x, p, q) for x in 0.1(0.1)0.9, "
          "p in 1.0(1.0)10.0, q in\n";
     f << "// 1.0(1.0)10.0.\n";
     f << "extern const double tab_betain[900] = {\n";
     f << scientific;
     Beta beta;
     for (int i = 1; i <= 9; i++) {
          const double x = i * 0.1;
          for (int p = 1; p <= 10; p++)
               for (int q = 1; q <= 10; q ++)
                    f << indent << setprecision(14) << setw(21)
                      << beta.betai(p, q, x) << ",\n";
     }
     f << "};\n";
     f << tail;
}

void cdffdist() {
     ofstream f("cdffdist.cc");
     f << "#include \"cdffdist.h\"\n\n";
     f << head;
     f << "// cdffdist(m, n, x) for m in 1(1)10, n in 1(1)10, x in\n";
     f << "// 1.0(1.0)10.0.\n";
     f << "extern const double tab_cdffdist[1000] = {\n";
     f << scientific;
     for (int m = 1; m <= 10; m++)
          for (int n = 1; n <= 10; n++) {
               Fdist fdist(m, n);
               for (int x = 1; x <= 10; x++)
                    f << indent << setprecision(14) << setw(21)
                      << fdist.cdf(x) << ",\n";
          }
     f << "};\n";
     f << tail;
}

int main() {
     ppnd7();
     gammad();
     probst();
     betain();
     cdffdist();
}
