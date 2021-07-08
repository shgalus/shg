/**
 * \file src/polynomial_ring.cc
 * Polynomial ring over a field.
 */

#include <shg/polynomial_ring.h>
#include <shg/except.h>

namespace SHG::ALGEBRA {

Polynomial_ring::Polynomial_ring(Field const* k)
     : Polynomial_ring(k, 1) {}

Polynomial_ring::Polynomial_ring(int dim)
     : Polynomial_ring(&q_, dim) {}

Polynomial_ring::Polynomial_ring(Field const* k, int dim)
     : k_(k), dim_(dim) {
     if (k == nullptr || !k->is_abelian() || dim < 1)
          SHG_THROW_IA(__func__);
}

Polynomial_ring::ET const& Polynomial_ring::value(
     Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp;
}

Element Polynomial_ring::element(ET const& x) const {
     if (x.field() != k_ || x.dim() != dim_)
          SHG_THROW_IA(__func__);
     return Element(this, x);
}

Element Polynomial_ring::do_add(Element const& x,
                                Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET const z = *xp + *yp;
     return Element(this, z);
}

Element Polynomial_ring::do_zero() const {
     ET const z = Polynomial(k_, dim_);
     return Element(this, z);
}

Element Polynomial_ring::do_neg(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET const z = -*xp;
     return Element(this, z);
}

Element Polynomial_ring::do_mul(Element const& x,
                                Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET const z = *xp * *yp;
     return Element(this, z);
}

Element Polynomial_ring::do_one() const {
     ET const z = Polynomial(k_, dim_) + k_->one();
     return Element(this, z);
}

bool Polynomial_ring::do_is_zero(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return xp->is_zero();
}

bool Polynomial_ring::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET const z = Polynomial(k_, dim_) + k_->one();
     return *xp == z;
}

bool Polynomial_ring::do_equal(Element const& x,
                               Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return *xp == *yp;
}

void Polynomial_ring::do_output(std::ostream& stream,
                                Element const& x) const {
     auto const xp = element_cast<ET>(x);
     stream << *xp;
}

void Polynomial_ring::do_input(std::istream& stream,
                               Element& x) const {
     ET z = Polynomial(k_);
     if ((stream >> z) && z.dim() == dim_)
          x = Element(this, z);
     else
          stream.setstate(std::ios::failbit);
}

bool Polynomial_ring::do_is_abelian() const {
     return true;
}

std::type_info const& Polynomial_ring::do_element_type() const {
     return typeid(ET);
}

Field_Q const Polynomial_ring::q_ = Field_Q();

}  // namespace SHG::ALGEBRA
