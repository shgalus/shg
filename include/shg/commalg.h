/**
 * \file include/shg/commalg.h
 * Commutative algebra.
 */

#ifndef SHG_COMMALG_H
#define SHG_COMMALG_H

#include <shg/algebra.h>
#include <shg/linalg.h>

namespace SHG::ALGEBRA {

/**
 * \addtogroup algebra
 *
 * \{
 */

/**
 * Checks if addition and multiplication tables form a commutative
 * ring with unity. Tables should be ordered in such a way that the
 * first row represents zero and the second represents one. For
 * example, tables for the ring \f$Z_6\f$ should be like these:
 *
 * \verbatim
   addition:     multiplication:
   0 1 2 3 4 5   0 0 0 0 0 0
   1 2 3 4 5 0   0 1 2 3 4 5
   2 3 4 5 0 1   0 2 4 0 2 4
   3 4 5 0 1 2   0 3 0 3 0 3
   4 5 0 1 2 3   0 4 2 0 4 2
   5 0 1 2 3 4   0 5 4 3 2 1
   \endverbatim
 */
int is_commutative_ring_with_unity(SHG::Linalg::Matint const& add,
                                   SHG::Linalg::Matint const& mul);
/**
 * Calls is_commutative_ring_with_unity(). If successful, checks for
 * multiplicative inverses.
 */
int is_field(SHG::Linalg::Matint const& add,
             SHG::Linalg::Matint const& mul);

/**
 * Ring \f$R[u] \cong R[x] / (x^n)\f$, \f$n \geq 1\f$, where \f$R\f$
 * is a commutative ring with unity.
 *
 * If \f[a = \sum_{i = 0}^{n - 1} a_ix^i, \quad b = \sum_{i = 0}^{n -
 * 1} b_ix^i,\f] then \f[a + b = \sum_{i = 0}^{n - 1} \left(a_i +
 * b_i\right)x^i, \quad ab = \sum_{i = 0}^{n - 1} \left( \sum_{j =
 * 0}^i a_jb_{i - j}\right)x^i.\f]
 */
class Ring_Ru : public Commutative_ring {
public:
     using ET = std::vector<Element>;
     using Index = ET::size_type;

     Ring_Ru() = default;
     explicit Ring_Ru(Commutative_ring const* base_ring);
     explicit Ring_Ru(int n);
     Ring_Ru(Commutative_ring const* base_ring, int n);
     virtual ~Ring_Ru();
     Ring_Ru(Ring_Ru const&) = default;
     Ring_Ru& operator=(Ring_Ru const&) = default;
     Ring_Ru(Ring_Ru&&) noexcept = default;
     Ring_Ru& operator=(Ring_Ru&&) noexcept = default;

     Commutative_ring const* base_ring() const;
     int n() const;

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

     std::type_info const& do_element_type() const override;

     bool do_is_zerodivisor(Element const& x) const override;
     bool do_is_nilpotent(Element const& x) const override;
     bool do_is_unit(Element const& x) const override;
     bool do_is_field() const override;

     Commutative_ring const* base_ring_{&z_};
     int n_{1};
     static Ring_Z const z_;
};

/**
 * Ring \f$\integern[x] / (x^n) / (a + bx)\f$, where \f$|a| > 1\f$.
 */
class Ring_Zu_quot : public Commutative_ring {
public:
     struct ET {
          ET(int x, int y);
          ET() = default;
          int x{0};
          int y{0};
     };

     Ring_Zu_quot() = default;
     Ring_Zu_quot(int a, int b);
     virtual ~Ring_Zu_quot();
     Ring_Zu_quot(Ring_Zu_quot const&) = default;
     Ring_Zu_quot& operator=(Ring_Zu_quot const&) = default;
     Ring_Zu_quot(Ring_Zu_quot&&) noexcept = default;
     Ring_Zu_quot& operator=(Ring_Zu_quot&&) noexcept = default;

     int a() const;
     int b() const;
     void reset(int a, int b);

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

     std::type_info const& do_element_type() const override;

     bool do_is_zerodivisor(Element const& x) const override;
     bool do_is_nilpotent(Element const& x) const override;
     bool do_is_unit(Element const& x) const override;
     bool do_is_field() const override;

     int a_{2};
     int b_{0};
     int absa_{2};
};

bool operator==(Ring_Zu_quot::ET const& x, Ring_Zu_quot::ET const& y);

}  // namespace SHG::ALGEBRA

#include <shg/commalg-inl.h>

#endif
