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
#include <shg/except.h>
#include <shg/ifact.h>
#include <shg/matrix.h>
#include <shg/utils.h>

/**
 * Algebra.
 *
 * Classes and functions related to abstract algebra.
 *
 * \see \ref aaai.
 * \see \ref eia.
 */
namespace SHG::ALGEBRA {

/**
 * %Exception class for invalid operation.
 */
class Invalid_operation : public std::logic_error {
public:
     Invalid_operation();
};

class AS;

/** %Element of an algebraic structure. */
class Element {
public:
     Element() = default;
     /** Initializes to zero. */
     explicit Element(AS const* as);
     Element(AS const* as, std::any const& v);
     Element(Element const&) = default;
     Element& operator=(Element const&) = default;
     Element(Element&&) noexcept = default;
     Element& operator=(Element&&) noexcept = default;
     void set_to_zero();
     void set_to_one();
     Element& operator+=(Element const& x);
     Element& operator-=(Element const& x);
     Element& operator*=(Element const& x);
     Element& operator/=(Element const& x);
     bool is_valid() const;
     AS const* as() const;
     std::any const& value() const;

     static bool is_valid(Element const& x, Element const& y);

private:
     AS const* as_{nullptr};
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
class AS {
public:
     virtual ~AS();
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
     std::type_info const& element_type() const;

protected:
     template <typename T>
     static T const* element_cast(Element const& x);
     template <typename T>
     static bool equalt(Element const& x, Element const& y);

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
     virtual std::type_info const& do_element_type() const = 0;
};

bool is_group(AS const* as);
bool is_commutative_ring(AS const* as);

class Group : public AS {
public:
     virtual ~Group();
     bool is_abelian() const;

private:
     Element do_add(Element const& x,
                    Element const& y) const override final;
     Element do_zero() const override final;
     Element do_neg(Element const& x) const override final;
     bool do_is_zero(Element const& x) const override final;
     virtual bool do_is_abelian() const = 0;
};

class Commutative_ring : public AS {
public:
     virtual ~Commutative_ring();
     bool is_zerodivisor(Element const& x) const;
     bool is_nilpotent(Element const& x) const;
     bool is_unit(Element const& x) const;
     bool is_field() const;

private:
     virtual bool do_is_zerodivisor(Element const& x) const = 0;
     virtual bool do_is_nilpotent(Element const& x) const = 0;
     virtual bool do_is_unit(Element const& x) const = 0;
     virtual bool do_is_field() const = 0;
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
     virtual ~Group_Sn();
     Group_Sn(Group_Sn&&) noexcept = default;
     Group_Sn& operator=(Group_Sn&&) noexcept = default;

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

     int n() const;
     void reset(int n);

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
     explicit Finite_group(Matrix<int> const& t);
     virtual ~Finite_group();
     Finite_group(Finite_group&&) noexcept = default;
     Finite_group& operator=(Finite_group&&) noexcept = default;

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

class Ring_Z : public Commutative_ring {
public:
     using ET = boost::multiprecision::cpp_int;

     Ring_Z() = default;
     virtual ~Ring_Z();
     Ring_Z(Ring_Z const&) = default;
     Ring_Z& operator=(Ring_Z const&) = default;
     Ring_Z(Ring_Z&&) noexcept = default;
     Ring_Z& operator=(Ring_Z&&) noexcept = default;

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
};

class Ring_Zn : public Commutative_ring {
public:
     using ET = int;

     Ring_Zn() = default;
     explicit Ring_Zn(int n);
     virtual ~Ring_Zn();
     Ring_Zn(Ring_Zn const&) = default;
     Ring_Zn& operator=(Ring_Zn const&) = default;
     Ring_Zn(Ring_Zn&&) noexcept = default;
     Ring_Zn& operator=(Ring_Zn&&) noexcept = default;

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

     int n() const;
     void reset(int n);

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

     int n_{1};
     bool is_prime_{false};
     int p1pm_{-1};
};

class Field_Q : public Commutative_ring {
public:
     using ET = boost::multiprecision::cpp_rational;

     Field_Q() = default;
     virtual ~Field_Q();
     Field_Q(Field_Q const&) = default;
     Field_Q& operator=(Field_Q const&) = default;
     Field_Q(Field_Q&&) noexcept = default;
     Field_Q& operator=(Field_Q&&) noexcept = default;

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

     std::type_info const& do_element_type() const override;

     bool do_is_zerodivisor(Element const& x) const override;
     bool do_is_nilpotent(Element const& x) const override;
     bool do_is_unit(Element const& x) const override;
     bool do_is_field() const override;
};

class Direct_product : public AS {
public:
     using ET = std::vector<Element>;

     Direct_product() = default;
     explicit Direct_product(std::vector<AS const*> const& v);
     virtual ~Direct_product();
     Direct_product(Direct_product const&) = default;
     Direct_product& operator=(Direct_product const&) = default;
     Direct_product(Direct_product&&) noexcept = default;
     Direct_product& operator=(Direct_product&&) noexcept = default;

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

     std::vector<AS const*> const& v() const;
     void reset(std::vector<AS const*> const& v);

private:
     using Sztp = std::vector<AS const*>::size_type;
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

     std::vector<AS const*> v_{};
};

/**
 * Direct product of groups.
 */
class DPG : Group {
public:
     using ET = std::vector<Element>;

     DPG() = default;
     explicit DPG(std::vector<Group const*> const& v);
     virtual ~DPG();
     DPG(DPG const&) = default;
     DPG& operator=(DPG const&) = default;
     DPG(DPG&&) noexcept = default;
     DPG& operator=(DPG&&) noexcept = default;

     ET const& value(Element const& x) const;
     Element element(ET const& x) const;

     std::vector<Group const*> const& v() const;
     void reset(std::vector<Group const*> const& v);

private:
     using Sztp = std::vector<Group const*>::size_type;
     Element do_mul(Element const& x,
                    Element const& y) const override;
     Element do_one() const override;
     Element do_inv(Element const& x) const override;
     bool do_is_one(Element const& x) const override;

     bool do_equal(Element const& x, Element const& y) const override;
     void do_output(std::ostream& stream,
                    Element const& x) const override;
     void do_input(std::istream& stream, Element& x) const override;

     std::type_info const& do_element_type() const override;

     std::vector<Group const*> v_{};
};

}  // namespace SHG::ALGEBRA

#include <shg/algebra-inl.h>

#endif
