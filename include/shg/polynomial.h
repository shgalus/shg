/**
 * \file include/shg/polynomial.h
 * Polynomial over a field.
 */

#ifndef SHG_POLYNOMIAL_H
#define SHG_POLYNOMIAL_H

#include <map>
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
     explicit Term(Algebraic_structure const* as) : a_(Element(as)) {}
     Element const& a() const { return a_; }
     Monomial const& m() const { return m_; }
     Algebraic_structure const* as() const { return a_.as(); }

private:
     Element a_{};
     Monomial m_{};
};

bool operator==(Term const& x, Term const& y);
bool operator!=(Term const& x, Term const& y);
std::ostream& operator<<(std::ostream& stream, Term const& x);
std::istream& operator>>(std::istream& stream, Term& x);

class Polynomial {
public:
     using Terms = std::map<Monomial, Element, Lex_less>;
     using Point = std::vector<Element>;

     Polynomial() = default;
     explicit Polynomial(Field const* k);
     explicit Polynomial(int dim);
     Polynomial(Field const* k, int dim);
     Polynomial(Polynomial const&) = default;
     Polynomial& operator=(Polynomial const&) = default;

     int dim() const { return dim_; }
     void dim(int d);
     int deg() const;
     bool is_zero() const { return t_.empty(); }
     Monomial const& leading_monomial() const;
     Element const& leading_coefficient() const;
     Term leading_term() const;
     Element operator()(Point const& x) const;
     Field const* field() const { return k_; }
     Terms const& terms() const { return t_; }

     Monomial_cmp order() const { return order_; }
     void order(Monomial_cmp cmp);

     void set_to_zero() { t_.clear(); }
     Polynomial& operator+=(Polynomial const& x);
     Polynomial& operator-=(Polynomial const& x);
     Polynomial& operator*=(Polynomial const& x);
     Polynomial& operator+=(Term const& x);
     Polynomial& operator-=(Term const& x);
     Polynomial& operator*=(Term const& x);
     Polynomial& operator+=(Element const& x);
     Polynomial& operator-=(Element const& x);
     Polynomial& operator*=(Element const& x);

     static bool is_valid(Polynomial const& x, Polynomial const& y);
     static bool is_valid(Polynomial const& x, Term const& y);

private:
     /**
      * Returns iterator to the element with maximum monomial
      * (according to order_).
      */
     Terms::const_iterator max_term() const;
     void add(Element const& a, Monomial const& x);
     void sub(Element const& a, Monomial const& x);
     void mul(Element const& a, Monomial const& x);

     Field const* k_{&q_};
     int dim_{1};
     Terms t_{};
     Monomial_cmp order_{lex_cmp};
     static Field_Q const q_;
};

bool operator==(Polynomial const& x, Polynomial const& y);
bool operator!=(Polynomial const& x, Polynomial const& y);
bool is_zero(Polynomial const& x);
Polynomial operator+(Polynomial const& x);
Polynomial operator-(Polynomial const& x);
Polynomial operator+(Polynomial const& x, Polynomial const& y);
Polynomial operator-(Polynomial const& x, Polynomial const& y);
Polynomial operator*(Polynomial const& x, Polynomial const& y);
Polynomial operator+(Polynomial const& x, Term const& y);
Polynomial operator-(Polynomial const& x, Term const& y);
Polynomial operator*(Polynomial const& x, Term const& y);
Polynomial operator+(Term const& x, Polynomial const& y);
Polynomial operator-(Term const& x, Polynomial const& y);
Polynomial operator*(Term const& x, Polynomial const& y);
Polynomial operator+(Polynomial const& x, Element const& y);
Polynomial operator-(Polynomial const& x, Element const& y);
Polynomial operator*(Polynomial const& x, Element const& y);
Polynomial operator+(Element const& x, Polynomial const& y);
Polynomial operator-(Element const& x, Polynomial const& y);
Polynomial operator*(Element const& x, Polynomial const& y);
std::ostream& operator<<(std::ostream& stream, Polynomial const& x);
std::istream& operator>>(std::istream& stream, Polynomial& x);

/**
 * \note This function is for polynomials over the field of rational
 * numbers.
 */
Polynomial from_chars(const char* s);

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
 * polynomials \f$g_1, \ldots, g_s\f$. Calls
 * \f$\var{Polynomial_div::divide}(f, g)\f$ and returns \f$r\f$.
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

/** \} */ /* end of group algebra */

}  // namespace SHG::ALGEBRA

#endif
