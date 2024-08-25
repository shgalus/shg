/**
 * \file include/shg/algebra-inl.h
 * Implementation of inline functions and templates from algebra.h.
 */

#ifndef SHG_ALGEBRA_INL_H
#define SHG_ALGEBRA_INL_H

namespace SHG::ALGEBRA {

inline bool Element::is_valid() const {
     return as_ != nullptr && v_.has_value() &&
            v_.type() == as_->element_type();
}

inline AS const* Element::as() const {
     return as_;
}

inline std::any const& Element::value() const {
     return v_;
}

inline bool Element::is_valid(Element const& x, Element const& y) {
     return x.is_valid() && y.is_valid() && x.as_ == y.as_;
}

inline bool operator!=(Element const& x, Element const& y) {
     return !(x == y);
}

inline Element operator+(Element const& x, Element const& y) {
     return Element(x) += y;
}

inline Element operator-(Element const& x, Element const& y) {
     return Element(x) -= y;
}

inline Element operator*(Element const& x, Element const& y) {
     return Element(x) *= y;
}

inline Element operator/(Element const& x, Element const& y) {
     return Element(x) /= y;
}

inline AS::~AS() {}

inline Element AS::add(Element const& x, Element const& y) const {
     return do_add(x, y);
}

inline Element AS::zero() const {
     return do_zero();
}

inline Element AS::neg(Element const& x) const {
     return do_neg(x);
}

inline Element AS::mul(Element const& x, Element const& y) const {
     return do_mul(x, y);
}

inline Element AS::one() const {
     return do_one();
}

inline Element AS::inv(Element const& x) const {
     return do_inv(x);
}

inline bool AS::is_zero(Element const& x) const {
     return do_is_zero(x);
}

inline bool AS::is_one(Element const& x) const {
     return do_is_one(x);
}

inline bool AS::equal(Element const& x, Element const& y) const {
     return do_equal(x, y);
}

inline std::type_info const& AS::element_type() const {
     return do_element_type();
}

template <typename T>
inline T const* AS::element_cast(Element const& x) {
     return std::any_cast<T const>(&x.value());
}

template <typename T>
inline bool AS::equalt(Element const& x, Element const& y) {
     return *element_cast<T>(x) == *element_cast<T>(y);
}

inline bool is_group(AS const* as) {
     return dynamic_cast<Group const*>(as) != nullptr;
}

inline bool is_commutative_ring(AS const* as) {
     return dynamic_cast<Commutative_ring const*>(as) != nullptr;
}

inline Group::~Group() {}

inline bool Group::is_abelian() const {
     return do_is_abelian();
}

inline Element Group::do_add(Element const& x,
                             Element const& y) const {
     return this->mul(x, y);
}

inline Element Group::do_zero() const {
     return this->one();
}

inline Element Group::do_neg(Element const& x) const {
     return this->inv(x);
}

inline bool Group::do_is_zero(Element const& x) const {
     return this->is_one(x);
}

inline Commutative_ring::~Commutative_ring() {}

inline bool Commutative_ring::is_zerodivisor(Element const& x) const {
     return do_is_zerodivisor(x);
}

inline bool Commutative_ring::is_nilpotent(Element const& x) const {
     return do_is_nilpotent(x);
}

inline bool Commutative_ring::is_unit(Element const& x) const {
     return do_is_unit(x);
}

inline bool Commutative_ring::is_field() const {
     return do_is_field();
}

inline Group_Sn::~Group_Sn() {}

inline Group_Sn::ET const& Group_Sn::value(Element const& x) const {
     return *element_cast<ET>(x);
}

inline int Group_Sn::n() const {
     return n_;
}

inline Element Group_Sn::do_one() const {
     return Element(this, one_);
}

inline bool Group_Sn::do_is_one(Element const& x) const {
     return *element_cast<ET>(x) == one_;
}

inline bool Group_Sn::do_equal(Element const& x,
                               Element const& y) const {
     return equalt<ET>(x, y);
}

inline bool Group_Sn::do_is_abelian() const {
     return n_ < 3;
}

inline std::type_info const& Group_Sn::do_element_type() const {
     return typeid(ET);
}

inline Finite_group::~Finite_group() {}

inline Finite_group::ET const& Finite_group::value(
     Element const& x) const {
     return *element_cast<ET>(x);
}

inline int Finite_group::order() const {
     return n_;
}

inline Element Finite_group::do_mul(Element const& x,
                                    Element const& y) const {
     return Element(this,
                    t_(*element_cast<ET>(x), *element_cast<ET>(y)));
}

inline Element Finite_group::do_one() const {
     return Element(this, ET(0));
}

inline bool Finite_group::do_is_one(Element const& x) const {
     return *element_cast<ET>(x) == 0;
}

inline bool Finite_group::do_equal(Element const& x,
                                   Element const& y) const {
     return equalt<ET>(x, y);
}

inline void Finite_group::do_output(std::ostream& stream,
                                    Element const& x) const {
     stream << *element_cast<ET>(x);
}

inline bool Finite_group::do_is_abelian() const {
     return is_abelian_;
}

inline std::type_info const& Finite_group::do_element_type() const {
     return typeid(ET);
}

inline Ring_Z::~Ring_Z() {}

inline Ring_Z::ET const& Ring_Z::value(Element const& x) const {
     return *element_cast<ET>(x);
}

inline Element Ring_Z::element(ET const& x) const {
     return Element(this, x);
}

inline Element Ring_Z::element(int x) const {
     return Element(this, ET(x));
}

inline Element Ring_Z::do_add(Element const& x,
                              Element const& y) const {
     return Element(this,
                    ET(*element_cast<ET>(x) + *element_cast<ET>(y)));
}

inline Element Ring_Z::do_zero() const {
     return Element(this, ET(0));
}

inline Element Ring_Z::do_neg(Element const& x) const {
     return Element(this, ET(-*element_cast<ET>(x)));
}

inline Element Ring_Z::do_mul(Element const& x,
                              Element const& y) const {
     return Element(this,
                    ET(*element_cast<ET>(x) * *element_cast<ET>(y)));
}

inline Element Ring_Z::do_one() const {
     return Element(this, ET(1));
}

inline bool Ring_Z::do_is_zero(Element const& x) const {
     return *element_cast<ET>(x) == ET(0);
}

inline bool Ring_Z::do_is_one(Element const& x) const {
     return *element_cast<ET>(x) == ET(1);
}

inline bool Ring_Z::do_equal(Element const& x,
                             Element const& y) const {
     return equalt<ET>(x, y);
}

inline void Ring_Z::do_output(std::ostream& stream,
                              Element const& x) const {
     stream << *element_cast<ET>(x);
}

inline std::type_info const& Ring_Z::do_element_type() const {
     return typeid(ET);
}

inline bool Ring_Z::do_is_zerodivisor(Element const& x) const {
     return *element_cast<ET>(x) == ET(0);
}

inline bool Ring_Z::do_is_nilpotent(Element const& x) const {
     return *element_cast<ET>(x) == ET(0);
}

inline bool Ring_Z::do_is_unit(Element const& x) const {
     return abs(*element_cast<ET>(x)) == 1;
}

inline bool Ring_Z::do_is_field() const {
     return false;
}

inline Ring_Zn::Ring_Zn(int n) {
     reset(n);
}

inline Ring_Zn::~Ring_Zn() {}

inline Ring_Zn::ET const& Ring_Zn::value(Element const& x) const {
     return *element_cast<ET>(x);
}

inline int Ring_Zn::n() const {
     return n_;
}

inline Element Ring_Zn::do_zero() const {
     return Element(this, ET(0));
}

inline Element Ring_Zn::do_one() const {
     return Element(this, n_ > 1 ? 1 : 0);
}

inline bool Ring_Zn::do_is_zero(Element const& x) const {
     return *element_cast<ET>(x) == ET(0);
}

inline bool Ring_Zn::do_is_one(Element const& x) const {
     return n_ > 1 ? *element_cast<ET>(x) == 1
                   : *element_cast<ET>(x) == 0;
}

inline bool Ring_Zn::do_equal(Element const& x,
                              Element const& y) const {
     return equalt<ET>(x, y);
}

inline void Ring_Zn::do_output(std::ostream& stream,
                               Element const& x) const {
     stream << *element_cast<ET>(x);
}

inline std::type_info const& Ring_Zn::do_element_type() const {
     return typeid(ET);
}

inline bool Ring_Zn::do_is_zerodivisor(Element const& x) const {
     return gcd(*element_cast<ET>(x), n_) > 1;
}

inline bool Ring_Zn::do_is_field() const {
     return is_prime_;
}

inline Field_Q::~Field_Q() {}

inline Field_Q::ET const& Field_Q::value(Element const& x) const {
     return *element_cast<ET>(x);
}

inline Element Field_Q::element(ET const& x) const {
     return Element(this, x);
}

inline Element Field_Q::element(int x) const {
     return Element(this, ET(x));
}

inline Element Field_Q::do_add(Element const& x,
                               Element const& y) const {
     return Element(this,
                    ET(*element_cast<ET>(x) + *element_cast<ET>(y)));
}

inline Element Field_Q::do_zero() const {
     return Element(this, ET(0));
}

inline Element Field_Q::do_neg(Element const& x) const {
     return Element(this, ET(-*element_cast<ET>(x)));
}

inline Element Field_Q::do_mul(Element const& x,
                               Element const& y) const {
     return Element(this,
                    ET(*element_cast<ET>(x) * *element_cast<ET>(y)));
}

inline Element Field_Q::do_one() const {
     return Element(this, ET(1));
}

inline bool Field_Q::do_is_zero(Element const& x) const {
     return *element_cast<ET>(x) == ET(0);
}

inline bool Field_Q::do_is_one(Element const& x) const {
     return *element_cast<ET>(x) == ET(1);
}

inline bool Field_Q::do_equal(Element const& x,
                              Element const& y) const {
     return equalt<ET>(x, y);
}

inline void Field_Q::do_output(std::ostream& stream,
                               Element const& x) const {
     stream << *element_cast<ET>(x);
}

inline std::type_info const& Field_Q::do_element_type() const {
     return typeid(ET);
}

inline bool Field_Q::do_is_zerodivisor(Element const& x) const {
     return *element_cast<ET>(x) == 0;
}

inline bool Field_Q::do_is_nilpotent(Element const& x) const {
     return *element_cast<ET>(x) == 0;
}

inline bool Field_Q::do_is_unit(Element const& x) const {
     return *element_cast<ET>(x) != 0;
}

inline bool Field_Q::do_is_field() const {
     return true;
}

inline Direct_product::Direct_product(std::vector<AS const*> const& v)
     : v_{} {
     reset(v);
}

inline Direct_product::~Direct_product() {}

inline Direct_product::ET const& Direct_product::value(
     Element const& x) const {
     return *element_cast<ET>(x);
}

inline std::vector<AS const*> const& Direct_product::v() const {
     return v_;
}

inline bool Direct_product::do_equal(Element const& x,
                                     Element const& y) const {
     return equalt<ET>(x, y);
}

inline std::type_info const& Direct_product::do_element_type() const {
     return typeid(ET);
}

}  // namespace SHG::ALGEBRA

#endif
