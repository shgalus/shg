/**
 * \file src/term.cc
 * The class Term.
 */

#include <shg/term.h>

namespace SHG::ALGEBRA {

bool operator==(Term const& x, Term const& y) {
     return x.a() == y.a() && x.m() == y.m();
}

bool operator!=(Term const& x, Term const& y) {
     return !(x == y);
}

std::ostream& operator<<(std::ostream& stream, Term const& x) {
     stream << x.a() << ' ' << x.m();
     return stream;
}

std::istream& operator>>(std::istream& stream, Term& x) {
     bool fail = true;
     if (x.as() != nullptr) {
          Element z(x.as());
          Monomial m;
          if ((stream >> z) && (stream >> m)) {
               x = Term(z, m);
               fail = false;
          }
     }
     if (fail)
          stream.setstate(std::ios::failbit);
     return stream;
}

}  // namespace SHG::ALGEBRA
