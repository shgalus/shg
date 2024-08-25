/**
 * \file include/shg/term.h
 * The class Term.
 */

#ifndef SHG_TERM_H
#define SHG_TERM_H

#include <shg/algebra.h>
#include <shg/monomial.h>

namespace SHG::ALGEBRA {

/**
 * \addtogroup algebra
 *
 * \{
 */

struct Term {
     Term() = default;
     Term(Element const& a, Monomial const& m) : a_(a), m_(m) {}
     explicit Term(AS const* as) : a_(Element(as)) {}
     Element const& a() const { return a_; }
     Monomial const& m() const { return m_; }
     AS const* as() const { return a_.as(); }

private:
     Element a_{};
     Monomial m_{};
};

bool operator==(Term const& x, Term const& y);
bool operator!=(Term const& x, Term const& y);
std::ostream& operator<<(std::ostream& stream, Term const& x);
std::istream& operator>>(std::istream& stream, Term& x);

/** \} */ /* end of group algebra */

}  // namespace SHG::ALGEBRA

#endif
