/**
 * \file src/rzxdabc.cc
 * Ring Z[x] / (x^2 - d, ax + b, c).
 */

#include <shg/rzxdabc.h>
#include <shg/utils.h>
#include <shg/mathutils.h>
#include <shg/legendresym.h>

namespace SHG::ALGEBRA::RZXDABC {

Ring_Zxdabc::Ring_Zxdabc(int d, int a, int b, int c) {
     reset(d, a, b, c);
}

void Ring_Zxdabc::reset(int d, int a, int b, int c) {
     d_ = d;
     a_ = a;
     b_ = b;
     c_ = c;
     N_ = sqr(b) - sqr(a) * d;
     if (a == 0 && b == 0)
          D_ = 0;
     else
          D_ = gcd(a, b);
     if (D_ > 0 && (N_ != 0 || c != 0))
          D0_ = gcd(N_ / D_, c);
     else
          D0_ = 0;
     if (a != 0 || b != 0 || c != 0)
          D1_ = a == 0 ? gcd(b, c) : gcd(gcd(a, b), c);
     else
          D1_ = 0;
     P_ = ring_case(d, a, b, c);
     if (P_ == Case::P4 || P_ == Case::P5)
          C_ = calculate_C(a, b, c, d, D_, D1_);
}

Ring_Zxdabc::Element Ring_Zxdabc::add(Element const& e1,
                                      Element const& e2) const {
     if (!is_valid(e1) || !is_valid(e2))
          throw std::invalid_argument(__func__);
     Element const e{e1.a0 + e2.a0, e1.a1 + e2.a1};
     return reduce(e);
}

Ring_Zxdabc::Element Ring_Zxdabc::sub(Element const& e1,
                                      Element const& e2) const {
     if (!is_valid(e1) || !is_valid(e2))
          throw std::invalid_argument(__func__);
     Element const e{e1.a0 - e2.a0, e1.a1 - e2.a1};
     return reduce(e);
}

Ring_Zxdabc::Element Ring_Zxdabc::mul(Element const& e1,
                                      Element const& e2) const {
     if (!is_valid(e1) || !is_valid(e2))
          throw std::invalid_argument(__func__);
     Element const e{e1.a0 * e2.a0 + e1.a1 * e2.a1 * d_,
                     e1.a0 * e2.a1 + e1.a1 * e2.a0};
     return reduce(e);
}

Ring_Zxdabc::Element Ring_Zxdabc::neg(Element const& r) const {
     using enum Case;
     if (!is_valid(r))
          throw std::invalid_argument(__func__);
     switch (P_) {
     case P1:
     case P2:
          return Element(-r.a0, -r.a1);
     case P3: {
          Element s(std::abs(c_) - r.a0, std::abs(c_) - r.a1);
          if (r.a0 == 0)
               s.a0 = 0;
          if (r.a1 == 0)
               s.a1 = 0;
          return s;
     }
     case P4: {
          Element s;
          if (r.a1 == 0) {
               s.a1 = 0;
               s.a0 = r.a0 == 0 ? 0 : D0_ - r.a0;
          } else {
               s.a1 = D1_ - r.a1;
               s.a0 = remainder(C_ - r.a0, D0_);
          }
          return s;
     }
     case P5: {
          Element s;
          if (r.a1 == 0) {
               s.a1 = 0;
               s.a0 = -r.a0;
          } else {
               s.a1 = D_ - r.a1;
               s.a0 = C_ - r.a0;
          }
          return s;
     }
     }
     throw std::runtime_error(__func__);
}

bool Ring_Zxdabc::is_unit(Element const& e) const {
     using enum Case;
     if (!is_valid(e))
          throw std::invalid_argument(__func__);

     if (size() == 1)
          return false;

     switch (P_) {
     case P1:
          return false;
     case P2: {
          int const delta = sqr(e.a0) - sqr(e.a1) * d_;
          return std::abs(delta) == 1;
     }
     case P3: {
          int const n = sqr(e.a0) - sqr(e.a1) * d_;
          return gcd(n, c_) == 1;
     }
     case P4: {
          Lin_Dioph<int> ld;
          Lin_Dioph<int>::Mat const A(
               2, 4,
               {e.a1, e.a0, -D1_, 0, e.a0, d_ * e.a1, -C_, -D0_});
          return ld.solve(A, {0, 1});
     }
     case P5: {
          if (e.a0 == 0 && e.a1 == 0)
               return false;
          Lin_Dioph<int> ld;
          Lin_Dioph<int>::Mat const A(
               2, 3, {e.a1, e.a0, -D1_, e.a0, d_ * e.a1, -C_});
          return ld.solve(A, {0, 1});
     }
     }
     throw std::runtime_error(__func__);
}

std::optional<Ring_Zxdabc::Element> Ring_Zxdabc::inv(
     Element const& e) const {
     using enum Case;
     if (!is_valid(e))
          throw std::invalid_argument(__func__);

     switch (P_) {
     case P1:
          return std::nullopt;
     case P2: {
          int const delta = sqr(e.a0) - sqr(e.a1) * d_;
          if (delta == 1)
               return Element(e.a0, -e.a1);
          else if (delta == -1)
               return Element(-e.a0, e.a1);
          else
               return std::nullopt;
     }
     case P3: {
          if (!is_unit(e))
               return std::nullopt;
          Element e1;
          int const n = sqr(e.a0) - sqr(e.a1) * d_;
          Lin_Dioph2<int> ld;
          ld.solve(n, c_, 1);
          if (ld.status != Status::success)
               throw std::runtime_error(__func__);
          e1.a0 = remainder(e.a0 * ld.x1, std::abs(c_));
          e1.a1 = remainder(e.a1 * ld.x1, std::abs(c_));
          if (e1.a1 != 0)
               e1.a1 = std::abs(c_) - e1.a1;
          return e1;
     }
     case P4: {
          if (size() == 1)
               return std::nullopt;
          Lin_Dioph<int> ld;
          Lin_Dioph<int>::Mat const A(
               2, 4,
               {e.a1, e.a0, -D1_, 0, e.a0, d_ * e.a1, -C_, -D0_});
          if (ld.solve(A, {0, 1})) {
               Element e1;
               int q;
               divide(ld.x0(1), D1_, q, e1.a1);
               e1.a0 = remainder(ld.x0(0) - q * C_, D0_);
               return e1;
          } else {
               return std::nullopt;
          }
     }
     case P5: {
          if (e.a0 == 0 && e.a1 == 0)
               return std::nullopt;
          Lin_Dioph<int> ld;
          Lin_Dioph<int>::Mat const A(
               2, 3, {e.a1, e.a0, -D1_, e.a0, d_ * e.a1, -C_});
          if (ld.solve(A, {0, 1})) {
               Element e1;
               int q;
               divide(ld.x0(1), D1_, q, e1.a1);
               e1.a0 = ld.x0(0) - q * C_;
               return e1;
          } else {
               return std::nullopt;
          }
     }
     }
     throw std::runtime_error(__func__);
}

int Ring_Zxdabc::nr(Element const& e) const {
     using enum Case;
     int n0, n1;
     switch (P_) {
     case P1:
          n0 = n1 = 1;
          break;
     case P3:
          n0 = n1 = std::abs(c_);
          break;
     case P4:
          n0 = D0_;
          n1 = D1_;
          break;
     default:
          throw std::invalid_argument(__func__);
     }
     if (e.a0 >= 0 || e.a0 < n0)
          if (e.a1 >= 0 || e.a1 < n1)
               return e.a0 + e.a1 * n0;
     throw std::invalid_argument(__func__);
}

Ring_Zxdabc::Element Ring_Zxdabc::element(int nr) const {
     using enum Case;
     int n0, n1;
     switch (P_) {
     case P1:
          n0 = n1 = 1;
          break;
     case P3:
          n0 = n1 = std::abs(c_);
          break;
     case P4:
          n0 = D0_;
          n1 = D1_;
          break;
     default:
          throw std::invalid_argument(__func__);
     }
     if (nr >= 0 && nr < n0 * n1)
          return Element(nr % n0, nr / n0);
     throw std::invalid_argument(__func__);
}

bool Ring_Zxdabc::is_finite() const {
     return c_ != 0 || N_ != 0;
}

bool Ring_Zxdabc::is_zero() const {
     return gcd(N_, c_) == 1;
}

bool Ring_Zxdabc::is_field() const {
     using enum Case;
     int const p = P_ == P4 ? D0_ : std::abs(c_);
     if (!is_prime(p))
          return false;
     if (a_ % p != 0) {
          return true;
     } else {
          if (p == 2)
               return false;
          return legendre_unsafe(d_, p) == -1;
     }
}

int Ring_Zxdabc::size() const {
     using enum Case;
     switch (P_) {
     case P1:
          return 1;
     case P2:
          return 0;
     case P3:
          return sqr(c_);
     case P4:
          return D0_ * D1_;
     case P5:
          return 0;
     }
     throw std::runtime_error(__func__);
}

Ring_Zxdabc::Tables Ring_Zxdabc::tables() const {
     using enum Case;
     int n0, n1;

     switch (P_) {
     case P1:
          n0 = 1;
          n1 = 1;
          break;
     case P3:
          n0 = std::abs(c_);
          n1 = std::abs(c_);
          break;
     case P4:
          n0 = D0_;
          n1 = D1_;
          break;
     default:
          throw std::invalid_argument(__func__);
     }

     Tables t(n0 * n1);
     Element a, b, e;
     for (int a0 = 0; a0 < n0; a0++) {
          a.a0 = a0;
          for (int a1 = 0; a1 < n1; a1++) {
               a.a1 = a1;
               for (int b0 = 0; b0 < n0; b0++) {
                    b.a0 = b0;
                    for (int b1 = 0; b1 < n1; b1++) {
                         b.a1 = b1;
                         e = add(a, b);
                         t.add(a0 + a1 * n0, b0 + b1 * n0) =
                              e.a0 + e.a1 * n0;
                         e = mul(a, b);
                         t.mul(a0 + a1 * n0, b0 + b1 * n0) =
                              e.a0 + e.a1 * n0;
                    }
               }
          }
     }
     return t;
}

Ring_Zxdabc::Element Ring_Zxdabc::coset(Polynomial const& p) const {
     using enum Case;
     if (p.size() == 0)
          throw std::invalid_argument(__func__);
     if (P_ == P1)
          return Element();
     auto e = modx2d(p);
     if (P_ == P2)
          return e;
     if (P_ == P3) {
          Element e1;
          int q;
          divide(e.a0, c_, q, e1.a0);
          divide(e.a1, c_, q, e1.a1);
          return e1;
     }
     if (P_ == P4) {
          Element e1;
          int q1, q0;
          divide(e.a1, D1_, q1, e1.a1);
          divide(e.a0 - q1 * C_, D0_, q0, e1.a0);
          return e1;
     }
     if (P_ == P5) {
          Element e1;
          int q;
          divide(e.a1, D1_, q, e1.a1);
          e1.a0 = e.a0 - q * D1_;
          return e1;
     }
     throw std::runtime_error(__func__);
}

Ring_Zxdabc::Element Ring_Zxdabc::modx2d(Polynomial const& p) const {
     Polynomial::size_type const n = p.size();
     int a0 = 0, a1 = 0;
     int pow = 1;
     for (Polynomial::size_type i = 0; i < n; i++) {
          if (i % 2 == 0) {
               a0 += p[i] * pow;
          } else {
               a1 += p[i] * pow;
               pow *= d_;
          }
     }
     return Element(a0, a1);
}

Ring_Zxdabc::Element Ring_Zxdabc::reduce(Element const& e) const {
     using enum Case;

     Element e1;
     int q;

     switch (P_) {
     case P1:
          return e;
     case P2:
          return e;
     case P3:
          e1.a0 = remainder(e.a0, c_);
          e1.a1 = remainder(e.a1, c_);
          return e1;
     case P4:
          divide(e.a1, D1_, q, e1.a1);
          e1.a0 = remainder(e.a0 - q * C_, D0_);
          return e1;
     case P5:
          divide(e.a1, D1_, q, e1.a1);
          e1.a0 = e.a0 - q * C_;
          return e1;
     }
     throw std::runtime_error(__func__);
}

bool Ring_Zxdabc::is_valid(Element const& e) const {
     using enum Case;
     switch (P_) {
     case P1:
          return e.a0 == 0 && e.a1 == 0;
     case P2:
          return true;
     case P3:
          return e.a0 >= 0 && e.a0 < std::abs(c_) && e.a1 >= 0 &&
                 e.a1 < std::abs(c_);
     case P4:
          return e.a0 >= 0 && e.a0 < D0_ && e.a1 >= 0 && e.a1 < D1_;
     case P5:
          return e.a1 >= 0 && e.a1 < D_;
     default:
          throw std::runtime_error(__func__);
     }
}

Ring_Zxdabc::Case Ring_Zxdabc::ring_case(int d, int a, int b, int c) {
     using enum Case;
     if (std::abs(c) == 1)
          return P1;
     if (a == 0 && b == 0)
          return c == 0 ? P2 : P3;
     if (std::abs(c) > 1)
          return P4;
     int const N = sqr(b) - sqr(a) * d;
     return N == 0 ? P5 : P4;
}

int Ring_Zxdabc::calculate_C(int a, int b, int c, int d, int D,
                             int D1) {
     Lin_Dioph2<int> ld1, ld2;
     ld1.solve(a, b, D);
     if (ld1.status != Status::success)
          throw std::runtime_error(__func__);
     ld2.solve(D, c, D1);
     if (ld2.status != Status::success)
          throw std::runtime_error(__func__);
     return (b * ld1.x1 + a * d * ld1.x2) * ld2.x1;
}

}  // namespace SHG::ALGEBRA::RZXDABC
