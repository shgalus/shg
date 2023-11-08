/**
 * \file tools/genbuchb.cc
 * Generates data for testing the Buchberger algorithm.
 */

#include <cassert>
#include <fstream>
#include <CoCoA/library.H>

using namespace std;

namespace CoCoA {

struct Test_data {
     int dim;  // number of variables x, y, ...
     char const* const description;
     vector<char const*> const p;  // input polynomials
};

Test_data const test_data[] = {
     {2,
      "\\\\cite cox-little-oshea-2007, example 1, pages 89-92.",
      {"x^3-2*x*y", "x^2*y-2*y^2+x"}},
     {2,
      "\\\\cite cox-little-oshea-2007, exercise 2a, page 94.",
      {"x^2*y-1", "x^2*y-x"}},
     {2,
      "\\\\cite cox-little-oshea-2007, exercise 2b, page 94.",
      {"x^2+y", "x^4+2*x^2*y+y^2+3"}},
     {3,
      "\\\\cite cox-little-oshea-2007, exercise 2c, page 94.",
      {"x-z^4", "y-z^5"}},
     {3,
      "\\\\cite cox-little-oshea-2007, example 2, page 96.",
      {"x^2+y^2+z^2-1", "x^2+z^2-y", "x-z"}},
     {4,
      "\\\\cite cox-little-oshea-2007, example 3, pages 97-98.",
      {"3*x^2+2*y*z-2*x*w", "2*x*z-2*y*w", "2*x*y-2*z-2*z*w",
       "x^2+y^2+z^2-1"}},
     {3,
      "\\\\cite cox-little-oshea-2007, exercise 13b, page 114.",
      {"x^5+y^4+z^3-1", "x^3+y^3+z^2-1"}},
     {2, "Test with zero polynomial.", {"0", "x*y-x"}},
     {2, "Test with two zero polynomials.", {"0", "0"}},
     {3,
      "\\\\cite buchberger-1985, example 6.6, page 198.",
      {"x^3*y*z-x*z^2", "x*y^2*z-x*y*z", "x^2*y^2-z^2"}},
};

// Changes integer rational literal to rational literal: 25 --> 25/1.
// This is required for boost/rational.hpp.
string adjust_coeff(RingElem const& e) {
     ostringstream oss;
     oss << e;
     string s = oss.str();
     if (s.find('/') == string::npos)
          s.append("/1");
     return s;
}

// Returns polynomial in format required by SHG::ALGEBRA::Polynomial.
string poly(int dim, RingElem const& e) {
     ostringstream oss;
     oss << '\"' << dim << ' ' << NumTerms(e);
     for (SparsePolyIter jt = BeginIter(e); !IsEnded(jt); ++jt) {
          oss << ' ' << adjust_coeff(coeff(jt)) << ' ' << dim;
          ConstRefPPMonoidElem pp = PP(jt);
          for (int i = 0; i < dim; i++)
               oss << ' ' << exponent(pp, i);
     }
     oss << "\",\n";
     return oss.str();
}

// Returns monomial order string literal.
string order(PPOrdering const& ord) {
     ostringstream oss;
     oss << ord;
     string s = oss.str();
     if (s.find("PPOrderingLex") != string::npos)
          return "Lex_less";
     else if (s.find("PPOrderingStdDegLex") != string::npos)
          return "Grlex_less";
     else if (s.find("PPOrderingStdDegRevLex") != string::npos)
          return "Grevlex_less";
     assert(0);
}

// Generates one entry in buchberger_test_data.
void generate(Test_data const& data, PPOrdering const ord,
              ostream& os) {
     char const* const syms[] = {"x", "x,y", "x,y,z", "w,x,y,z"};
     assert(data.dim >= 1 && data.dim <= 4);
     PolyRing P =
          NewPolyRing(RingQQ(), symbols(syms[data.dim - 1]), ord);
     vector<RingElem> v;
     for (vector<char const*>::size_type i = 0; i < data.p.size();
          i++)
          v.push_back(RingElem(P, data.p[i]));
     ideal I = ideal(v);
     vector<RingElem> const b = GBasis(I);
     os << "{\n";
     os << "\"" << data.description << "\",\n";
     os << "\"" << order(ord) << "\",\n";
     os << "{\n";
     for (auto it = v.cbegin(); it != v.cend(); ++it)
          os << poly(data.dim, *it);
     os << "},\n";
     os << "{\n";
     for (auto it = b.cbegin(); it != b.cend(); ++it)
          os << poly(data.dim, *it);
     os << "},\n";
     os << "},\n";
}

// Generates three entries in buchberger_test_data, one for each
// monomial order.
void generate(ostream& os) {
     for (size_t i = 0; i < std::size(test_data); i++) {
          int const dim = test_data[i].dim;
          generate(test_data[i], lex(dim), os);
          generate(test_data[i], StdDegLex(dim), os);
          generate(test_data[i], StdDegRevLex(dim), os);
     }
}

void generate() {
     std::ofstream os("../tests/buchbdat.cc");
     os << R"(#include "buchbdat.h"

namespace TESTS {

std::vector<Buchberger_test_case> const buchberger_test_data {
)";

     for (size_t i = 0; i < std::size(test_data); i++) {
          int const dim = test_data[i].dim;
          generate(test_data[i], lex(dim), os);
          generate(test_data[i], StdDegLex(dim), os);
          generate(test_data[i], StdDegRevLex(dim), os);
     }
     os << "};\n\n}  // namespace SHG::Testing";
}

void program() {
     GlobalManager CoCoAFoundations;
     generate();
}

}  // namespace CoCoA

int main() {
     try {
          CoCoA::program();
          return 0;
     } catch (CoCoA::ErrorInfo const& err) {
          cerr << "***ERROR***  UNCAUGHT CoCoA error";
          ANNOUNCE(cerr, err);
     } catch (std::exception const& exc) {
          cerr << "***ERROR***  UNCAUGHT std::exception: "
               << exc.what() << endl;
     } catch (...) {
          cerr << "***ERROR***  UNCAUGHT UNKNOWN EXCEPTION" << endl;
     }

     CoCoA::BuildInfo::PrintAll(cerr);
     return 1;
}
