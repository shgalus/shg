/* gstrdlrd.cc: generates data for strdlr.cc */

/*
 * This program generates the constant StRDLR::v_ in src/strdlr.cc.
 * This constant is differently formatted in src/strdlr.cc, so use
 * diff -w for comparing.
 */

#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include "shg/shg.h"

using std::cout;
using std::ifstream;
using std::ostream;
using std::setw;
using std::size_t;
using std::string;
using SHG::Matrix;
using SHG::Vector;

class NISTStRD {
public:
     explicit NISTStRD(int dsn);
     void print(ostream& os);

private:
     enum Dataset {norris, pontius, noint1, noint2, filip, longley,
                   wampler1, wampler2, wampler3, wampler4, wampler5};
     struct Data {
          const char* name;     // file name
          int n;                // number of observations
          int k;                // number of columns in X
     };
     static const Data data[11];

     int ds;
     string dsname;             // dataset name
     int n;                     // number of observations
     int k;                     // number of columns in X
     int p;                     // number of parameters
     Matrix<string> X;          // predictor variables data
     Vector<string> y;          // response variable data

     // certified values
     Vector<string> b;          // estimates
     Vector<string> sdevb;      // standard deviations of estimates
     string sdevres;            // residual standard deviation
     string r2;                 // R-squared
     string dfreg;              // regression degrees of freedom
     string ssreg;              // regression sum of squares
     string msreg;              // regression mean squares
     string fstat;              // F statistic
     string dfres;              // residual degrees of freedom
     string ssres;              // residual sum of squares
     string msres;              // residual mean squares
};

NISTStRD::NISTStRD(int dsn)
     : ds(dsn), dsname(), n(), k(), p(), X(), y(), b(), sdevb(),
       sdevres(), r2(), dfreg(), ssreg(), msreg(), fstat(), dfres(),
       ssres(), msres() {
     SHG_ASSERT(ds >= norris && ds <= wampler5);
     dsname = data[ds].name;
     n = data[ds].n;
     k = data[ds].k;
     p = (ds == noint1 || ds == noint2) ? k :   // no intercept
          k + 1;
     if (ds == longley)
          X.resize(n, 6);
     else
          X.resize(n, 1);
     y.resize(n);
     b.resize(p);
     sdevb.resize(p);

     const string fn = "strdlr/" + dsname + ".dat";
     ifstream f(fn.c_str());
     SHG_ASSERT(f);
     int line = 0;
     string s;
     while (getline(f, s) && ++line < 30) {/* VOID */}
     for (int i = 0; i < p; i++)
          f >> s >> b(i) >> sdevb(i);
     f >> s >> s >> s >> sdevres >> s >> r2;
     while (f >> s && s != "Regression") {/* VOID */}
     f >> dfreg >>  ssreg >>  msreg >> s;
     if (s == "Infinity") {
          SHG_ASSERT(ds == wampler1 || ds == wampler2);
          fstat = "std::numeric_limits<double>::infinity()";
     } else {
          SHG_ASSERT(ds != wampler1 && ds != wampler2);
          fstat = s;
     }
     f >> s >> dfres >> ssres >> msres >> s;
     getline(f, s);
     for (int i = 0; i < n; i++) {
          f >> y(i);
          if (ds == longley) {  // six predictor variables
               for (int j = 0; j < k; j++) {
                    f >> X(i, j);
               }
          } else {              // one predictor variable
               f >> X(i, 0);
          }
     }
     f >> s;
     SHG_ASSERT(f.eof() && !f.bad());
     f.close();
}

const NISTStRD::Data NISTStRD::data[11] = {
     {"Norris",   36,  1},
     {"Pontius",  40,  2},
     {"NoInt1",   11,  1},
     {"NoInt2",    3,  1},
     {"Filip",    82, 10},
     {"Longley",  16,  6},
     {"Wampler1", 21,  5},
     {"Wampler2", 21,  5},
     {"Wampler3", 21,  5},
     {"Wampler4", 21,  5},
     {"Wampler5", 21,  5},
};

void NISTStRD::print(ostream& os) {
     const int k0 = (ds == longley) ? 6 : 1;
     os << "     {\n";
     os << "          ";
     os << "\"" << dsname << "\", // i = " << ds << "\n";
     os << "          " << n << ", // number of observations n\n";
     os << "          " << k << ", // number of columns in X k\n";
     os << "          {" << n << ", " << k0 << ",\n";
     os << "           {                    // predictor variables X\n";
     for (size_t i = 0; i < X.nrows(); i++)
          for (size_t j = 0; j < X.ncols(); j++)
               os << "                " << X(i, j) << ",\n";
     os << "           }},\n";
     os << "          {                     // response variable y\n";
     for (size_t i = 0; i < y.size(); i++)
          os << "               " << y(i) << ",\n";
     os << "          },\n";
     os << "          {                     // estimates b\n";
     for (size_t i = 0; i < b.size(); i++)
          os << "               " << b(i) << ",\n";
     os << "          },\n";
     os << "          {                     // standard"
          " deviations of estimates\n";
     for (size_t i = 0; i < sdevb.size(); i++)
          os << "               " << sdevb(i) << ",\n";
     os << "          },\n";
     os << "          " << sdevres << ", // residual standard"
          " deviation sdevres\n";
     os << "          " << r2 <<      ", // R-squared r2\n";
     os << "          " << dfreg <<   ", // regression degrees"
          " of freedom dfreg\n";
     os << "          " << ssreg <<   ", // regression sum of"
          " squares ssreg\n";
     os << "          " << msreg << ", // regression mean"
          " squares msreg\n";
     os << "          " << fstat << ", // F statistic fstat\n";
     os << "          " << dfres << ", // residual degrees of"
          " freedom dfres\n";
     os << "          " << ssres << ", // residual sum of"
          " squares ssres\n";
     os << "          " << msres << ", // residual mean"
          " squares msres\n";
     os << "     },\n";
}

int main() {
     try {
          cout << "const std::vector<StRDLR::Data>"
               " StRDLR::v_ = {\n";
          for (int i = 0; i < 11; i++)
               NISTStRD(i).print(cout);
          cout << "};\n";
     } catch (const SHG::Exception& e) {
          e.print();
     } catch (const std::exception& e) {
          SHG::error(e.what());
     }
}
