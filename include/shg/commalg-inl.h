/**
 * \file include/shg/commalg-inl.h
 * Implementation of inline functions and templates from commalg.h.
 */

#ifndef SHG_COMMALG_INL_H
#define SHG_COMMALG_INL_H

namespace SHG::ALGEBRA {

inline Ring_Ru::Ring_Ru(Commutative_ring const* base_ring)
     : Ring_Ru(base_ring, 1) {}

inline Ring_Ru::Ring_Ru(int n) : Ring_Ru(&z_, n) {}

inline Ring_Ru::~Ring_Ru() {}

inline Commutative_ring const* Ring_Ru::base_ring() const {
     return base_ring_;
}

inline int Ring_Ru::n() const {
     return n_;
}

inline Ring_Ru::ET const& Ring_Ru::value(Element const& x) const {
     return *element_cast<ET>(x);
}

inline Element Ring_Ru::element(ET const& x) const {
     return Element(this, x);
}

inline bool Ring_Ru::do_equal(Element const& x,
                              Element const& y) const {
     return equalt<ET>(x, y);
}

inline std::type_info const& Ring_Ru::do_element_type() const {
     return typeid(ET);
}

inline bool Ring_Ru::do_is_zerodivisor(Element const& x) const {
     return base_ring_->is_zerodivisor((*element_cast<ET>(x))[0]);
}

inline bool Ring_Ru::do_is_nilpotent(Element const& x) const {
     return base_ring_->is_nilpotent((*element_cast<ET>(x))[0]);
}

inline bool Ring_Ru::do_is_unit(Element const& x) const {
     return base_ring_->is_unit((*element_cast<ET>(x))[0]);
}

inline Ring_Zu_quot::~Ring_Zu_quot() {}

inline Ring_Zu_quot::ET::ET(int x, int y) : x{x}, y{y} {}

inline int Ring_Zu_quot::a() const {
     return a_;
}

inline int Ring_Zu_quot::b() const {
     return b_;
}

inline Ring_Zu_quot::ET const& Ring_Zu_quot::value(
     Element const& x) const {
     return *element_cast<ET>(x);
}

inline Element Ring_Zu_quot::element(ET const& x) const {
     return Element(this, x);
}

inline Element Ring_Zu_quot::do_zero() const {
     return Element(this, ET());
}

inline Element Ring_Zu_quot::do_one() const {
     return Element(this, ET(1, 0));
}

inline bool Ring_Zu_quot::do_equal(Element const& x,
                                   Element const& y) const {
     return equalt<ET>(x, y);
}

inline std::type_info const& Ring_Zu_quot::do_element_type() const {
     return typeid(ET);
}

inline bool operator==(Ring_Zu_quot::ET const& x,
                       Ring_Zu_quot::ET const& y) {
     return x.x == y.x && x.y == y.y;
}

}  // namespace SHG::ALGEBRA

#endif
