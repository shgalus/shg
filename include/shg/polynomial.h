/**
 * \file include/shg/polynomial.h
 * Polynomials over an algebraic structure.
 */

#ifndef SHG_POLYNOMIAL_H
#define SHG_POLYNOMIAL_H

#include <map>
#include <shg/algebra.h>
#include <shg/term.h>

namespace SHG::ALGEBRA {

/**
 * \addtogroup algebra
 *
 * \{
 */

/**
 * %Polynomial over an algebraic structure.
 */
class Polynomial {
public:
     using Terms = std::map<Monomial, Element, Lex_less>;
     using Point = std::vector<Element>;

     Polynomial() = default;
     explicit Polynomial(AS const* as);
     explicit Polynomial(int dim);
     Polynomial(AS const* as, int dim);
     Polynomial(Polynomial const&) = default;
     Polynomial& operator=(Polynomial const&) = default;
     Polynomial(Polynomial&&) noexcept = default;
     Polynomial& operator=(Polynomial&&) noexcept = default;

     int dim() const;
     void dim(int d);
     int deg() const;
     bool is_zero() const;
     Monomial const& leading_monomial() const;
     Element const& leading_coefficient() const;
     Term leading_term() const;
     Element operator()(Point const& x) const;
     Element operator()(Element const& x) const;
     AS const* as() const;
     Terms const& terms() const;

     Monomial_cmp order() const;
     void order(Monomial_cmp cmp);

     void set_to_zero();
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

     AS const* as_{&q_};
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
 * Returns \f$a_0 + a_1x + a_2x^2 + \ldots + a_nx^n\f$.
 */
Polynomial one_var(std::vector<Element> const& a);

/**
 * %Polynomial ring over an algebraic structure.
 */
class Polynomial_ring : public Commutative_ring {
public:
     using ET = Polynomial;

     Polynomial_ring() = default;
     explicit Polynomial_ring(AS const* as);
     explicit Polynomial_ring(int dim);
     Polynomial_ring(AS const* as, int dim);
     virtual ~Polynomial_ring();
     Polynomial_ring(Polynomial_ring const&) = default;
     Polynomial_ring& operator=(Polynomial_ring const&) = default;
     Polynomial_ring(Polynomial_ring&&) noexcept = default;
     Polynomial_ring& operator=(Polynomial_ring&&) noexcept = default;

     AS const* as() const { return as_; }
     int dim() const { return dim_; }

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;
     void reset(AS const* as, int dim);

private:
     Element do_add(Element const& x,
                    Element const& y) const override;
     Element do_zero() const override;
     Element do_neg(Element const& x) const override;
     Element do_mul(Element const& x,
                    Element const& y) const override;
     Element do_one() const override;
     Element do_inv(Element const& x) const override;
     bool do_is_zero(Element const& x) const override;
     bool do_is_one(Element const& x) const override;

     bool do_equal(Element const& x, Element const& y) const override;
     void do_output(std::ostream& stream,
                    Element const& x) const override;
     void do_input(std::istream& stream, Element& x) const override;

     std::type_info const& do_element_type() const override;

     bool do_is_zerodivisor(Element const& x) const override;
     bool do_is_nilpotent(Element const& x) const override;
     bool do_is_unit(Element const& x) const override;
     bool do_is_field() const override;

     AS const* as_{&q_};
     int dim_{1};
     static Field_Q const q_;
};

inline int Polynomial::dim() const {
     return dim_;
}

inline bool Polynomial::is_zero() const {
     return t_.empty();
}

inline Element Polynomial::operator()(Element const& x) const {
     return operator()(Point{x});
}

inline AS const* Polynomial::as() const {
     return as_;
}

inline Polynomial::Terms const& Polynomial::terms() const {
     return t_;
}

inline Monomial_cmp Polynomial::order() const {
     return order_;
}

inline void Polynomial::set_to_zero() {
     t_.clear();
}

/** \} */ /* end of group algebra */

}  // namespace SHG::ALGEBRA

#endif
