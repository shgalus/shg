/**
 * \file src/commalg.cc
 * Commutative algebra.
 */

#include <algorithm>
#include <functional>
#include <shg/commalg.h>
#include <shg/except.h>
#include <shg/utils.h>

namespace SHG::ALGEBRA {

int is_commutative_ring_with_unity(SHG::Linalg::Matint const& add,
                                   SHG::Linalg::Matint const& mul) {
     int const n = add.rows();
     if (n <= 0)
          return 1;  // general error
     if (add.cols() != n || mul.rows() != n || mul.cols() != n)
          return 1;

     for (int x = 0; x < n; x++)
          for (int y = 0; y < x; y++)
               if (add(x, y) < 0 || add(x, y) >= n || mul(x, y) < 0 ||
                   mul(x, y) >= n)
                    return 1;

     // Check if both operations are commutative.
     for (int x = 0; x < n; x++)
          for (int y = 0; y < x; y++) {
               if (add(x, y) != add(y, x))
                    return 2;  // addition is not commutative
               if (mul(x, y) != mul(y, x))
                    return 3;  // multiplication is not commutative
          }
     // Check if zero is the neutral element of addition.
     for (int x = 0; x < n; x++)
          if (add(x, 0) != x)
               return 4;

     // Check if one is the neutral element of multiplication.
     for (int x = 0; x < n; x++)
          if (mul(x, 1) != x)
               return 5;

     // Check if multiplication is distributive: x(y + z) = xy + xz.
     for (int x = 0; x < n; x++)
          for (int y = 0; y < n; y++)
               for (int z = 0; z < n; z++) {
                    int const lhs = mul(x, add(y, z));
                    int const rhs = add(mul(x, y), mul(x, z));
                    if (lhs != rhs)
                         return 6;
               }

     // Check if both operations are associative: (x + y) + z = x + (y
     // + z), (xy)z = x(yz).
     for (int x = 0; x < n; x++)
          for (int y = 0; y < n; y++)
               for (int z = 0; z < n; z++) {
                    int lhs = add(add(x, y), z);
                    int rhs = add(x, add(y, z));
                    if (lhs != rhs)
                         return 7;
                    lhs = mul(mul(x, y), z);
                    rhs = mul(x, mul(y, z));
                    if (lhs != rhs)
                         return 8;
               }

     // Check if there exists additive inverse for each element.
     for (int x = 0; x < n; x++) {
          int y;
          for (y = 0; y < n && add(x, y) != 0; y++)
               ;
          if (y >= n)
               return 9;
     }

     return 0;
}

int is_field(SHG::Linalg::Matint const& add,
             SHG::Linalg::Matint const& mul) {
     int const r = is_commutative_ring_with_unity(add, mul);
     if (r != 0)
          return r;
     int const n = add.rows();
     // Check if there exists multiplicative inverse for each element.
     for (int x = 0; x < n; x++) {
          int y;
          for (y = 0; y < n && mul(x, y) != 1; y++)
               ;
          if (y >= n)
               return 10;
     }
     return 0;
}

Ring_Ru::Ring_Ru(Commutative_ring const* base_ring, int n)
     : base_ring_(base_ring), n_(n) {
     if (n < 1)
          SHG_THROW(std::invalid_argument, __func__);
}

Element Ring_Ru::do_add(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z(n_);
     std::transform(xp->cbegin(), xp->cend(), yp->cbegin(), z.begin(),
                    std::plus<Element>());
     return Element(this, z);
}

Element Ring_Ru::do_zero() const {
     ET z(n_);
     for (Index i = 0; i < z.size(); i++)
          z[i] = Element{base_ring_};
     return Element(this, z);
}

Element Ring_Ru::do_neg(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET z(n_);
     std::transform(xp->cbegin(), xp->cend(), z.begin(),
                    std::negate<Element>());
     return Element(this, z);
}

Element Ring_Ru::do_mul(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z(n_);
     Element s{base_ring_};
     for (Index i = 0; i < z.size(); i++) {
          s.set_to_zero();
          for (Index j = 0; j <= i; j++)
               s += (*xp)[j] * (*yp)[i - j];
          z[i] = s;
     }
     return Element(this, z);
}

Element Ring_Ru::do_one() const {
     ET z(n_);
     for (Index i = 0; i < z.size(); i++)
          z[i] = Element{base_ring_};
     z[0].set_to_one();
     return Element(this, z);
}

Element Ring_Ru::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET y(n_);
     Element s(base_ring_);
     y[0] = base_ring_->inv((*xp)[0]);
     for (Index i = 1; i < y.size(); i++) {
          s.set_to_zero();
          for (Index j = 1; j <= i; j++)
               s += (*xp)[j] * y[i - j];
          y[i] = -s * y[0];
     }
     return Element(this, y);
}

bool Ring_Ru::do_is_zero(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     for (Index i = 0; i < xp->size(); i++)
          if (!base_ring_->is_zero((*xp)[i]))
               return false;
     return true;
}

bool Ring_Ru::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     if (!base_ring_->is_one((*xp)[0]))
          return false;
     for (Index i = 1; i < xp->size(); i++)
          if (!base_ring_->is_zero((*xp)[i]))
               return false;
     return true;
}

void Ring_Ru::do_output(std::ostream& stream,
                        Element const& x) const {
     auto const xp = element_cast<ET>(x);
     for (Index i = 0; i < xp->size(); i++) {
          stream << (*xp)[i];
          if (i + 1 < xp->size())
               stream << ' ';
     }
}

void Ring_Ru::do_input(std::istream& stream, Element& x) const {
     ET z(n_);
     for (Index i = 0; i < z.size(); i++) {
          z[i] = Element{base_ring_};
          if (!(stream >> z[i]))
               return;
     }
     x = Element(this, z);
}

bool Ring_Ru::do_is_field() const {
     // TODO
     throw Invalid_operation();
}

Ring_Z const Ring_Ru::z_;

Ring_Zu_quot::Ring_Zu_quot(int a, int b)
     : a_{a}, b_{b}, absa_{std::abs(a)} {
     if (absa_ <= 1)
          SHG_THROW(std::invalid_argument, __func__);
}

void Ring_Zu_quot::reset(int a, int b) {
     if (std::abs(a) <= 1)
          SHG_THROW(std::invalid_argument, __func__);
     a_ = a;
     b_ = b;
     absa_ = std::abs(a);
}

Element Ring_Zu_quot::do_add(Element const& x,
                             Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z;
     int q, r;
     divide(xp->x + yp->x, absa_, q, r);
     z.x = r;
     z.y = xp->y + yp->y - q * b_;
     divide(z.y, absa_, q, r);
     z.y = r;
     return Element(this, z);
}

Element Ring_Zu_quot::do_neg(Element const& x) const {
     ignore_unused_variable(x);
     SHG_THROW(std::invalid_argument, __func__);
     return Element();
}

Element Ring_Zu_quot::do_mul(Element const& x,
                             Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z;
     int q, r;
     divide(xp->x * yp->x, absa_, q, r);
     z.x = r;
     z.y = xp->x * yp->y + xp->y * yp->x - q * b_;
     divide(z.y, absa_, q, r);
     z.y = r;
     return Element(this, z);
}

Element Ring_Zu_quot::do_inv(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Ring_Zu_quot::do_is_zero(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return xp->x == 0 && xp->y == 0;
}

bool Ring_Zu_quot::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return xp->x == 1 && xp->y == 0;
}

void Ring_Zu_quot::do_output(std::ostream& stream,
                             Element const& x) const {
     auto const xp = element_cast<ET>(x);
     stream << xp->x << ' ' << xp->y;
}

void Ring_Zu_quot::do_input(std::istream& stream, Element& x) const {
     ET z;
     if ((stream >> z.x) && z.x >= 0 && z.x < absa_)
          if ((stream >> z.y) && z.y >= 0 && z.y < absa_) {
               x = Element(this, z);
               return;
          }
     stream.setstate(std::ios::failbit);
}

bool Ring_Zu_quot::do_is_zerodivisor(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Ring_Zu_quot::do_is_nilpotent(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Ring_Zu_quot::do_is_unit(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Ring_Zu_quot::do_is_field() const {
     // TODO
     throw Invalid_operation();
}

}  // namespace SHG::ALGEBRA
