/**
 * \file include/shg/polynomial_ring.h
 * Polynomial ring over a field.
 */

#ifndef SHG_POLYNOMIAL_RING_H
#define SHG_POLYNOMIAL_RING_H

#include <shg/polynomial.h>

namespace SHG::ALGEBRA {

/**
 * \addtogroup algebra
 *
 * \{
 */

class Polynomial_ring : public Ring {
public:
     using ET = Polynomial;

     Polynomial_ring() = default;
     explicit Polynomial_ring(Field const* k);
     explicit Polynomial_ring(int dim);
     Polynomial_ring(Field const* k, int dim);
     Polynomial_ring(Polynomial_ring const&) = default;
     Polynomial_ring& operator=(Polynomial_ring const&) = default;

     Field const* field() const { return k_; }
     int dim() const { return dim_; }

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

     Field const* k_{&q_};
     int dim_{1};
     static Field_Q const q_;
};

/** \} */ /* end of group algebra */

}  // namespace SHG::ALGEBRA

#endif
