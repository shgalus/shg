/**
 * \file include/shg/alggeom.h
 * Algebraic geometry.
 */

#ifndef SHG_ALGGEOM_H
#define SHG_ALGGEOM_H

#include <map>
#include <shg/algebra.h>
#include <shg/polynomial.h>
#include <shg/term.h>

/**
 * Algebraic geometry.
 */
namespace SHG::ALGEBRA::ALGGEOM {

/**
 * \addtogroup algebra
 *
 * \{
 */

/**
 * \defgroup alggeom Algebraic geometry
 *
 * \{
 */

/**
 * Division of polynomials.
 *
 * Given a polynomial \f$f\f$ and polynomials \f$g_1, \ldots, g_s\f$,
 * the function calculates the polynomial \f$r\f$ and the polynomials
 * \f$a_1, \ldots, a_s\f$ such that \f$f = a_1g_1 + \ldots + a_sg_s +
 * r\f$ and either \f$r = 0\f$ or no monomial of \f$r\f$ is divisible
 * by any of leading terms of \f$g_1, \ldots, g_s\f$. Furhtermore, if
 * \f$a_if_i \neq 0\f$, then the leading term of \f$a_if_i\f$ is not
 * greater than the leading term of \f$f\f$ (with respect to monomial
 * order).
 *
 * See \cite cox-little-oshea-2007, page 64, theorem 3.
 */
struct Polynomial_div {
     void divide(Polynomial const& f,
                 std::vector<Polynomial> const& g);
     Polynomial r{};
     std::vector<Polynomial> a{};
};

/**
 * Normal form of the polynomial with respect to polynomials.
 *
 * Returns the normal form of the polynomial \f$f\f$ with respect to
 * polynomials \f$g_1, \ldots, g_s\f$. Calls Polynomial_div and
 * returns \c r.
 */
Polynomial normal_form(Polynomial const& f,
                       std::vector<Polynomial> const& g);

/**
 * Calculates the S-polynomial of two polynomials. See
 * \cite cox-little-oshea-2007, page 83, definition 4.
 */
Polynomial s_polynomial(Polynomial const& f, Polynomial const& g);

Polynomial s_polynomial_unsafe(Polynomial const& f,
                               Polynomial const& g);

/**
 * Buchberger algorithm.
 * See \cite buchberger-1985, algorithm 6.3, pages 196-197.
 */
class Buchberger_improved {
public:
     using P = Polynomial;
     using S = std::vector<P>;

     void run(S const& f);
     S const& g() { return g_; }

private:
     struct Pair {
          Pair() = default;
          Pair(P const& f1, P const& f2) : f1_(f1), f2_(f2) {}
          P f1_;
          P f2_;
     };
     friend bool operator==(Pair const& lhs, Pair const& rhs);
     using SP = std::vector<Pair>;

     void reduce_all();
     void new_basis();
     bool criterion1(P const& f1, P const& f2);
     bool criterion2(P const& f1, P const& f2);

     S r_{};
     S p_{};
     S g_{};
     SP b_{};
};

/** \} */ /* end of group alggeom */

/** \} */ /* end of group algebra */

}  // namespace SHG::ALGEBRA::ALGGEOM

#endif
