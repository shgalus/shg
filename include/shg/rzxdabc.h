/**
 * \file include/shg/rzxdabc.h
 * Ring Z[x] / (x^2 - d, ax + b, c).
 */

#ifndef SHG_RZXDABC_H
#define SHG_RZXDABC_H

#include <vector>
#include <optional>
#include <shg/commalg.h>

namespace SHG::ALGEBRA::RZXDABC {

/**
 * Ring \f$\mathbb{Z}[x] / (x^2 - d, ax + b, c)\f$.
 * See \cite galus-2026.
 */
class Ring_Zxdabc {
public:
     enum class Case { P1, P2, P3, P4, P5 };
     using Polynomial = std::vector<int>;
     struct Element {
          Element() = default;
          Element(int a0, int a1) : a0(a0), a1(a1) {}
          int a0{};
          int a1{};
     };
     struct Tables {
          Tables() = default;
          Tables(int n) : add(n, n), mul(n, n) {}
          SHG::Linalg::Matint add{};
          SHG::Linalg::Matint mul{};
     };

     Ring_Zxdabc() = default;
     Ring_Zxdabc(int d, int a, int b, int c);

     void reset(int d, int a, int b, int c);

     int d() const { return d_; }
     int a() const { return a_; }
     int b() const { return b_; }
     int c() const { return c_; }
     int N() const { return N_; }
     int D() const { return D_; }
     int D0() const { return D0_; }
     int D1() const { return D1_; }
     int C() const { return C_; }
     Case rcase() const { return P_; }

     /** Returns true iff R is a finite ring. See \cite galus-2026,
         corollary {cor:finiteness}. */
     bool is_finite() const;

     /** Returns true iff R is the zero ring. See \cite galus-2026,
         corollary {cor:finiteness}. */
     bool is_zero() const;

     /** Returns true iff R is a field. See \cite galus-2026,
         theorem {thm:obj_is_a_field}. */
     bool is_field() const;

     /** Returns number of elements in R. See \cite galus-2026,
         corollary {cor:finiteness}. */
     int size() const;

     /** Returns addition and multiplication tables of R if it is
         finite. \throws std::invalid_argument if R is not finite. */
     Tables tables() const;

     /** Returns the canonical representative of the coset to which p
         belongs. If p.size() == 0, std::invalid_argument is
         thrown. */
     Element coset(Polynomial const& p) const;

     Element add(Element const& e1, Element const& e2) const;
     Element sub(Element const& e1, Element const& e2) const;
     Element mul(Element const& e1, Element const& e2) const;
     Element neg(Element const& e) const;

     bool is_unit(Element const& e) const;
     std::optional<Element> inv(Element const& e) const;

     int nr(Element const& e) const;
     Element element(int nr) const;

private:
     /** Returns remainder of division of p by x^2 - d_. */
     Element modx2d(Polynomial const& p) const;

     /** Reduces element after addition or multiplication. */
     Element reduce(Element const& e) const;

     bool is_valid(Element const& e) const;

     static Case ring_case(int d, int a, int b, int c);

     /** Calculate and return C as in the proof of
         {thm:canonical_representatives} \cite galus-2026. */
     static int calculate_C(int a, int b, int c, int d, int D,
                            int D1);

     int d_{};
     int a_{};
     int b_{};
     int c_{};
     int N_{};
     int D_{};
     int D0_{};
     int D1_{};
     int C_{};
     Case P_{Case::P2};
};

inline bool operator==(Ring_Zxdabc::Element const& e1,
                       Ring_Zxdabc::Element const& e2) {
     return e1.a0 == e2.a0 && e1.a1 == e2.a1;
}

}  // namespace SHG::ALGEBRA::RZXDABC

#endif
