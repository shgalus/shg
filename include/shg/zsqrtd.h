/**
 * \file include/shg/zsqrtd.h
 * Rings \f$\integern[\sqrt{d}]\f$ and related.
 */

#ifndef SHG_ZSQRTD_H
#define SHG_ZSQRTD_H

#include <shg/algebra.h>
#include <shg/commalg.h>

namespace SHG::ALGEBRA {

/**
 * Ring \f$\integern[\sqrt{d}]\f$.
 */
class Ring_Zsqrtd : public Commutative_ring {
public:
     struct ET {
          ET() = default;
          ET(int x, int y) : x(x), y(y) {}
          int x{0};
          int y{0};
          friend bool operator==(ET const& lhs, ET const& rhs);
     };
     Ring_Zsqrtd() : Ring_Zsqrtd(-1) {}
     Ring_Zsqrtd(int d);
     virtual ~Ring_Zsqrtd() {}
     Ring_Zsqrtd(Ring_Zsqrtd const&) = default;
     Ring_Zsqrtd& operator=(Ring_Zsqrtd const&) = default;
     Ring_Zsqrtd(Ring_Zsqrtd&&) noexcept = default;
     Ring_Zsqrtd& operator=(Ring_Zsqrtd&&) noexcept = default;

     ET const& value(Element const& x) const {
          return *element_cast<ET>(x);
     }
     Element element(ET const& x) const { return Element(this, x); }
     Element element(int x, int y) const {
          return Element(this, ET(x, y));
     }

     /**
      * \throws std::invalid_argument if x = 0
      */
     bool divides(Element const& x, Element const& y) const;

     /**
      * \throws std::invalid_argument if den = 0
      */
     bool divide(Element const& x, Element const& y,
                 Element& z) const;
     int euclidean_norm(Element const& x) const;

     int d() const { return d_; }

private:
     Element do_add(Element const& x,
                    Element const& y) const override;
     Element do_zero() const override { return Element(this, ET()); }
     Element do_neg(Element const& x) const override;
     Element do_mul(Element const& x,
                    Element const& y) const override;
     Element do_one() const override {
          return Element(this, ET(1, 0));
     }
     Element do_inv(Element const& x) const override;
     bool do_is_zero(Element const& x) const override {
          return *element_cast<ET>(x) == ET();
     }
     bool do_is_one(Element const& x) const override {
          return *element_cast<ET>(x) == ET(1, 0);
     }

     bool do_equal(Element const& x, Element const& y) const override;
     void do_output(std::ostream& stream,
                    Element const& x) const override;
     void do_input(std::istream& stream, Element& x) const override;

     std::type_info const& do_element_type() const override {
          return typeid(ET);
     }

     bool do_is_zerodivisor(Element const& x) const override;
     bool do_is_nilpotent(Element const& x) const override;
     bool do_is_unit(Element const& x) const override;
     bool do_is_field() const override;

     int d_;
};

inline bool operator==(Ring_Zsqrtd::ET const& lhs,
                       Ring_Zsqrtd::ET const& rhs) {
     return lhs.x == rhs.x && lhs.y == rhs.y;
}

/**
 * Ring \f$\integern[\sqrt{d}] / (a + b \sqrt{d})\f$.
 */
class Ring_Zsqrtd_quot : public Commutative_ring {
public:
     struct ET {
          ET() = default;
          ET(int x, int y) : x(x), y(y) {}
          int x{0};
          int y{0};
          friend bool operator==(ET const& lhs, ET const& rhs);
     };

     Ring_Zsqrtd_quot() : Ring_Zsqrtd_quot(2, 4, 2) {}
     Ring_Zsqrtd_quot(int d, int a, int b);
     virtual ~Ring_Zsqrtd_quot() {}
     Ring_Zsqrtd_quot(Ring_Zsqrtd_quot const&) = default;
     Ring_Zsqrtd_quot& operator=(Ring_Zsqrtd_quot const&) = default;
     Ring_Zsqrtd_quot(Ring_Zsqrtd_quot&&) noexcept = default;
     Ring_Zsqrtd_quot& operator=(Ring_Zsqrtd_quot&&) noexcept =
          default;

     ET const& value(Element const& x) const {
          return *element_cast<ET>(x);
     }
     Element element(ET const& x) const { return Element(this, x); }

     int d() const { return d_; }
     int a() const { return a_; }
     int b() const { return b_; }
     int d2() const { return d2_; }
     int N() const { return N_; }
     int absN() const { return absN_; }
     int d1() const { return d1_; }
     int x0() const { return x0_; }

     void tables(SHG::Linalg::Matint& add,
                 SHG::Linalg::Matint& mul) const;

private:
     Element do_add(Element const& x,
                    Element const& y) const override;
     Element do_zero() const override { return Element(this, ET()); }
     Element do_neg(Element const& x) const override;
     Element do_mul(Element const& x,
                    Element const& y) const override;
     Element do_one() const override {
          return Element(this, ET(1, 0));
     }
     Element do_inv(Element const& x) const override;
     bool do_is_zero(Element const& x) const override {
          return *element_cast<ET>(x) == ET();
     }
     bool do_is_one(Element const& x) const override {
          return *element_cast<ET>(x) == ET(1, 0);
     }

     bool do_equal(Element const& x, Element const& y) const override;
     void do_output(std::ostream& stream,
                    Element const& x) const override;
     void do_input(std::istream& stream, Element& x) const override;

     std::type_info const& do_element_type() const override {
          return typeid(ET);
     }

     bool do_is_zerodivisor(Element const& x) const override;
     bool do_is_nilpotent(Element const& x) const override;
     bool do_is_unit(Element const& x) const override;
     bool do_is_field() const override;

     void reduce(ET& a) const;

     int d_;
     int a_;
     int b_;
     int d2_;  // 0 <= y < d2_
     int N_;
     int absN_;
     int d1_;  // 0 <= x < d1_
     int x0_;
};

inline bool operator==(Ring_Zsqrtd_quot::ET const& lhs,
                       Ring_Zsqrtd_quot::ET const& rhs) {
     return lhs.x == rhs.x && lhs.y == rhs.y;
}

}  // namespace SHG::ALGEBRA

#include <shg/zsqrtd-inl.h>

#endif
