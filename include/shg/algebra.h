/**
 * \file include/shg/algebra.h
 * Basic algebraic structures.
 */

#ifndef SHG_ALGEBRA_H
#define SHG_ALGEBRA_H

#include <any>
#include <istream>
#include <ostream>
#include <set>
#include <string>
#include <typeinfo>
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>
#include <shg/matrix.h>

/**
 * Algebra.
 */
namespace SHG::ALGEBRA {

/**
 * \defgroup algebra Algebra
 *
 * Classes and functions related to abstract algebra.
 *
 * \note Alternative names for to_string() and from_string() are
 * <tt>etos</tt> and <tt>stoe</tt>.
 *
 * \todo Does Direct_product_of_groups::do_equal() require a loop
 * inside?
 *
 * \todo As Algebraic_structure::do_equal() and do_element_type(),
 * do_value() are almost identical, write a template in
 * Algebraic_structure.
 *
 * \{
 */

class Algebraic_structure;

/** Element of an algebraic structure. */
class Element {
public:
     Element() = default;
     /** Initializes to zero. */
     explicit Element(Algebraic_structure const* as);
     Element(Algebraic_structure const* as, std::any const& v);
     Element(Element const&) = default;
     Element& operator=(Element const&) = default;
     void set_to_zero();
     void set_to_one();
     Element& operator+=(Element const& x);
     Element& operator-=(Element const& x);
     Element& operator*=(Element const& x);
     Element& operator/=(Element const& x);
     bool is_valid() const;
     Algebraic_structure const* as() const;
     std::any const& value() const;

     static bool is_valid(Element const& x, Element const& y);

private:
     Algebraic_structure const* as_{nullptr};
     std::any v_{};
};

bool operator==(Element const& x, Element const& y);
bool operator!=(Element const& x, Element const& y);
bool is_zero(Element const& x);
bool is_one(Element const& x);

/**  Returns \f$x\f$. */
Element operator+(Element const& x);

/** Returns \f$-x\f$. */
Element operator-(Element const& x);

/** Returns \f$x + y\f$. */
Element operator+(Element const& x, Element const& y);

/** Returns \f$x + (-y)\f$. */
Element operator-(Element const& x, Element const& y);

/** Returns \f$xy\f$. */
Element operator*(Element const& x, Element const& y);

/** Returns \f$xy^{-1}\f$. */
Element operator/(Element const& x, Element const& y);

/** Returns \f$x^{-1}\f$. */
Element inv(Element const& x);

/**
 * Calculates \f$x + x + \ldots + x\f$ (right-to-left binary
 * method for exponentiation). The structure must have defined
 * zero() and add(). If \f$n < 0\f$, the structure must also have
 * defined neg().
 *
 * \throws std::invalid_argument if a required method
 * throws.
 *
 * \implementation See \cite knuth-2002b, section 4.6.3,
 * page 497.
 */

Element times(Element const& x, int n);
/**
 * Calculates \f$x^n\f$ (right-to-left binary method for
 * exponentiation). The structure must have defined one() and
 * mul(). If \f$n < 0\f$, the structure must also have defined
 * inv().
 *
 * \throws std::invalid_argument if a required method
 * throws.
 *
 * \implementation See \cite knuth-2002b, section 4.6.3,
 * page 497.
 */
Element pow(Element const& x, int n);

std::ostream& operator<<(std::ostream& stream, Element const& x);
std::istream& operator>>(std::istream& stream, Element& x);

/**
 * Base class of algebraic structures.
 *
 * \warning All public methods which accept argument of type Element
 * are unsafe. Arguments passed must fulfill the condition
 * <tt>Element::is_valid(x, y)</tt> and <tt>x.as() == this</tt>.
 */
class Algebraic_structure {
public:
     virtual ~Algebraic_structure();
     Element add(Element const& x, Element const& y) const;
     Element zero() const;
     Element neg(Element const& x) const;
     Element mul(Element const& x, Element const& y) const;
     Element one() const;
     Element inv(Element const& x) const;
     bool is_zero(Element const& x) const;
     bool is_one(Element const& x) const;

     bool equal(Element const& x, Element const& y) const;
     std::ostream& output(std::ostream& stream,
                          Element const& x) const;
     std::istream& input(std::istream& stream, Element& x) const;

     bool is_abelian() const;
     std::type_info const& element_type() const;

protected:
     template <typename T>
     inline static T const* element_cast(Element const& x) {
          return std::any_cast<T const>(&x.value());
     }

private:
     virtual Element do_add(Element const& x,
                            Element const& y) const = 0;
     virtual Element do_zero() const = 0;
     virtual Element do_neg(Element const& x) const = 0;
     virtual Element do_mul(Element const& x,
                            Element const& y) const = 0;
     virtual Element do_one() const = 0;
     virtual Element do_inv(Element const& x) const = 0;
     virtual bool do_is_zero(Element const& x) const = 0;
     virtual bool do_is_one(Element const& x) const = 0;

     virtual bool do_equal(Element const& x,
                           Element const& y) const = 0;

     virtual void do_output(std::ostream& stream,
                            Element const& x) const = 0;
     virtual void do_input(std::istream& stream,
                           Element& x) const = 0;

     virtual bool do_is_abelian() const = 0;
     virtual std::type_info const& do_element_type() const = 0;
};

/** A semigroup with neutral element, also called a monoid. */
class Semigroup : public Algebraic_structure {
public:
     virtual ~Semigroup();

private:
     Element do_add(Element const& x,
                    Element const& y) const override final;
     Element do_zero() const override final;
     Element do_neg(Element const& x) const override final;
     Element do_inv(Element const& x) const override final;
     bool do_is_zero(Element const& x) const override final;
};

class Group : public Algebraic_structure {
public:
     virtual ~Group();

private:
     Element do_add(Element const& x,
                    Element const& y) const override final;
     Element do_zero() const override final;
     Element do_neg(Element const& x) const override final;
     bool do_is_zero(Element const& x) const override final;
};

class Ring : public Algebraic_structure {
public:
     virtual ~Ring();

private:
     Element do_inv(Element const& x) const override final;
};

class Field : public Algebraic_structure {
public:
     virtual ~Field();
};

/**
 * The set of all finite strings over an alphabet with concatenation.
 */
class Finite_strings : public Semigroup {
public:
     using ET = std::string;

     Finite_strings() = default;
     explicit Finite_strings(const std::set<char>& alphabet);

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

private:
     Element do_mul(Element const& x,
                    Element const& y) const override;
     Element do_one() const override;
     bool do_is_one(Element const& x) const override;

     bool do_equal(Element const& x, Element const& y) const override;
     void do_output(std::ostream& stream,
                    Element const& x) const override;
     void do_input(std::istream& stream, Element& x) const override;

     bool do_is_abelian() const override;
     std::type_info const& do_element_type() const override;

     std::set<char> a_{};
};

/**
 * Permutation group \f$S_n\f$. This is a group of permutations of the
 * set \f$\{1, 2, \ldots, n\}\f$, \f$n \geq 1\f$. The product of two
 * permutations \f$f_1\f$, \f$f_2\f$ is superposition \f$(f_1 \circ
 * f_2)(x) = f_1(f_2(x))\f$. For eample, if \f$f_1 = \left(
 * \begin{array}{ccc} 1 & 2 & 3 \\ 3 & 1 & 2 \end{array} \right) = (3,
 * 1, 2)\f$, \f$f_2 = \left( \begin{array}{ccc} 1 & 2 & 3 \\ 3 & 2 & 1
 * \end{array} \right) = (3, 2, 1)\f$, then \f$f_1 \circ f_2 = \left(
 * \begin{array}{ccc} 1 & 2 & 3 \\ 2 & 1 & 3 \end{array} \right) = (2,
 * 1, 3)\f$. Permutations are stored as vectors of integers, the
 * result of composition is calculated as \f$(f_1 \circ f_2)[i] =
 * f_1[f_2[i]]\f$, and the inverse of \f$f\f$ is calculated as
 * \f$f^{-1}[f[i]] = i\f$. Elements in the set are numbered from 0 to
 * \f$n - 1\f$.
 */
class Group_Sn : public Group {
public:
     using ET = std::vector<int>;

     Group_Sn() = default;
     explicit Group_Sn(int n);

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

private:
     Element do_mul(Element const& x,
                    Element const& y) const override;
     Element do_one() const override;
     Element do_inv(Element const& x) const override;
     bool do_is_one(Element const& x) const override;

     bool do_equal(Element const& x, Element const& y) const override;
     void do_output(std::ostream& stream,
                    Element const& x) const override;
     void do_input(std::istream& stream, Element& x) const override;

     bool do_is_abelian() const override;
     std::type_info const& do_element_type() const override;

     int n_{1};
     ET one_{0};
};

class Finite_group : public Group {
public:
     using ET = int;

     Finite_group() = default;
     explicit Finite_group(const Matrix<int>& t);

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

     int order() const;

private:
     Element do_mul(Element const& x,
                    Element const& y) const override;
     Element do_one() const override;
     Element do_inv(Element const& x) const override;
     bool do_is_one(Element const& x) const override;

     bool do_equal(Element const& x, Element const& y) const override;
     void do_output(std::ostream& stream,
                    Element const& x) const override;
     void do_input(std::istream& stream, Element& x) const override;

     bool do_is_abelian() const override;
     std::type_info const& do_element_type() const override;

     bool is_valid_table() const;

     int n_{1};
     Matrix<int> t_{1, 1, {0}};
     bool is_abelian_{true};
};

class Ring_Z : public Ring {
public:
     using ET = boost::multiprecision::cpp_int;

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

     Element element(int x) const;

private:
     Element do_add(Element const& x,
                    Element const& y) const override;
     Element do_zero() const override;
     Element do_neg(Element const& x) const override;
     Element do_mul(Element const& x,
                    Element const& y) const override;
     Element do_one() const override;
     bool do_is_zero(Element const& x) const override;
     bool do_is_one(Element const& x) const override;

     bool do_equal(Element const& x, Element const& y) const override;
     void do_output(std::ostream& stream,
                    Element const& x) const override;
     void do_input(std::istream& stream, Element& x) const override;

     bool do_is_abelian() const override;
     std::type_info const& do_element_type() const override;
};

class Ring_Zn : public Ring {
public:
     using ET = int;

     Ring_Zn() = default;
     explicit Ring_Zn(int n);

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

private:
     Element do_add(Element const& x,
                    Element const& y) const override;
     Element do_zero() const override;
     Element do_neg(Element const& x) const override;
     Element do_mul(Element const& x,
                    Element const& y) const override;
     Element do_one() const override;
     bool do_is_zero(Element const& x) const override;
     bool do_is_one(Element const& x) const override;

     bool do_equal(Element const& x, Element const& y) const override;
     void do_output(std::ostream& stream,
                    Element const& x) const override;
     void do_input(std::istream& stream, Element& x) const override;

     bool do_is_abelian() const override;
     std::type_info const& do_element_type() const override;

     int n_{1};
};

class Field_Q : public Field {
public:
     using ET = boost::multiprecision::cpp_rational;

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

     Element element(int x) const;
     Element element(int num, int den) const;

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

     bool do_is_abelian() const override;
     std::type_info const& do_element_type() const override;
};

class Field_Fp : public Field {
public:
     using ET = int;

     Field_Fp() = default;
     explicit Field_Fp(int p);

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

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

     bool do_is_abelian() const override;
     std::type_info const& do_element_type() const override;

     int p_{2};
};

class Direct_product_of_groups : public Group {
public:
     using ET = std::vector<Element>;

     Direct_product_of_groups() = default;
     Direct_product_of_groups(std::vector<Group const*> const& v);

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

     /**
      * Sets separator which will be used to separate elements of the
      * groups in string representation of the product elements.
      * This separator should not appear in string representation of
      * elements of groups of the product.
      */
     void separator(std::string const& sep);
     /**
      * Returns the separator.
      */
     std::string const& separator() const;

private:
     using Sztp = std::vector<const Group*>::size_type;
     Element do_mul(Element const& x,
                    Element const& y) const override;
     Element do_one() const override;
     Element do_inv(Element const& x) const override;
     bool do_is_one(Element const& x) const override;

     bool do_equal(Element const& x, Element const& y) const override;
     void do_output(std::ostream& stream,
                    Element const& x) const override;
     void do_input(std::istream& stream, Element& x) const override;

     bool do_is_abelian() const override;
     std::type_info const& do_element_type() const override;

     std::vector<const Group*> v_{};
     std::string sep_{";"};
};

/** \} */ /* end of group algebra */

}  // namespace SHG::ALGEBRA

#endif
