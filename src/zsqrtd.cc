/**
 * \file src/zsqrtd.cc
 * Rings \f$\integern[\sqrt{d}]\f$ and related.
 */

#include <shg/zsqrtd.h>
#include <shg/utils.h>
#include <shg/mathutils.h>

namespace SHG::ALGEBRA {

Ring_Zsqrtd::Ring_Zsqrtd(int d) : d_(d) {
     if (!is_square_free(d))
          throw std::invalid_argument("Ring_Zsqrtd::Ring_Zsqrtd");
}

bool Ring_Zsqrtd::divides(Element const& x, Element const& y) const {
     auto const yp = element_cast<ET>(y);
     auto const yx = Element(this, ET(yp->x, -yp->y));
     auto const num = element_cast<ET>(do_mul(y, yx));
     auto const N = euclidean_norm(x);
     if (SHG::divides(N, num->x) && SHG::divides(N, num->y))
          return true;
     return false;
}

bool Ring_Zsqrtd::divide(Element const& x, Element const& y,
                         Element& z) const {
     auto const xe = element_cast<ET>(x);
     auto const ye = element_cast<ET>(y);
     auto const N = euclidean_norm(y);
     if (N == 0)
          throw std::invalid_argument("division by zero");
     ET ze;
     ze.x = xe->x * ye->x - d_ * xe->y * ye->y;
     ze.y = xe->y * ye->x - xe->x * ye->y;
     if (SHG::divides(N, ze.x) && SHG::divides(N, ze.y)) {
          ze.x /= N;
          ze.y /= N;
          z = Element(this, ze);
          return true;
     }
     return false;
}

int Ring_Zsqrtd::euclidean_norm(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return sqr(xp->x) - d_ * sqr(xp->y);
}

Element Ring_Zsqrtd::do_add(Element const& x,
                            Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z{xp->x + yp->x, xp->y + yp->y};
     return Element(this, z);
}

Element Ring_Zsqrtd::do_neg(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET z{-xp->x, -xp->y};
     return Element(this, z);
}

Element Ring_Zsqrtd::do_mul(Element const& x,
                            Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z{xp->x * yp->x + d_ * xp->y * yp->y,
          xp->x * yp->y + xp->y * yp->x};
     return Element(this, z);
}

Element Ring_Zsqrtd::do_inv([[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Ring_Zsqrtd::do_equal(Element const& x, Element const& y) const {
     return equalt<ET>(x, y);
}

void Ring_Zsqrtd::do_output(std::ostream& stream,
                            Element const& x) const {
     ET z{*element_cast<ET>(x)};
     stream << z.x << ' ' << z.y;
}

void Ring_Zsqrtd::do_input(std::istream& stream, Element& x) const {
     ET z;
     if ((stream >> z.x) && (stream >> z.y))
          x = Element(this, z);
     else
          stream.setstate(std::ios::failbit);
}

bool Ring_Zsqrtd::do_is_zerodivisor(Element const& x) const {
     // For each squre-free d, Z[dqrt(d)] is an integral domain.
     auto const xp = element_cast<ET>(x);
     return xp->x == 0 && xp->y == 0;
}

bool Ring_Zsqrtd::do_is_nilpotent(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Ring_Zsqrtd::do_is_unit(Element const& x) const {
     return std::abs(euclidean_norm(x)) == 1;
}

bool Ring_Zsqrtd::do_is_field() const {
     // TODO
     throw Invalid_operation();
}

Ring_Zsqrtd_quot::Ring_Zsqrtd_quot(int d, int a, int b)
     : d_(d),
       a_(a),
       b_(b),
       d2_(),
       N_(a * a - d * b * b),
       absN_(std::abs(N_)),
       d1_(),
       x0_() {
     if (!is_square_free(d) || (a == 0 && b == 0))
          throw std::invalid_argument(
               "Ring_Zsqrtd_quot::Ring_Zsqrtd_quot");
     d2_ = gcd(a, b);
     d1_ = absN_ / d2_;
     Lin_Dioph2 ld(b_, a_, d2_);
     if (ld.status != Status::success)
          throw std::runtime_error(
               "Ring_Zsqrtd_quot::Ring_Zsqrtd_quot");
     x0_ = a_ * ld.x1 + d_ * b_ * ld.x2;
}

void Ring_Zsqrtd_quot::tables(SHG::Linalg::Matint& add,
                              SHG::Linalg::Matint& mul) const {
     auto const nr = [this](ET const& x) { return x.y * d1_ + x.x; };
     add.resize(absN_, absN_);
     mul.resize(absN_, absN_);
     for (int xy = 0; xy < d2_; xy++)
          for (int xx = 0; xx < d1_; xx++) {
               ET const x{xx, xy};
               Element const ex = element(x);
               for (int yy = 0; yy < d2_; yy++)
                    for (int yx = 0; yx < d1_; yx++) {
                         ET const y{yx, yy};
                         Element const ey = element(y);
                         Element ez = ex + ey;
                         ET z = value(ez);
                         add(nr(x), nr(y)) = nr(z);
                         ez = ex * ey;
                         z = value(ez);
                         mul(nr(x), nr(y)) = nr(z);
                    }
          }
}

Element Ring_Zsqrtd_quot::do_add(Element const& x,
                                 Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z{xp->x + yp->x, xp->y + yp->y};
     reduce(z);
     return Element(this, z);
}

Element Ring_Zsqrtd_quot::do_neg(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET z{-xp->x, -xp->y};
     reduce(z);
     return Element(this, z);
}

Element Ring_Zsqrtd_quot::do_mul(Element const& x,
                                 Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z{xp->x * yp->x + d_ * xp->y * yp->y,
          xp->x * yp->y + xp->y * yp->x};
     reduce(z);
     return Element(this, z);
}

Element Ring_Zsqrtd_quot::do_inv(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Ring_Zsqrtd_quot::do_equal(Element const& x,
                                Element const& y) const {
     return equalt<ET>(x, y);
}

void Ring_Zsqrtd_quot::do_output(std::ostream& stream,
                                 Element const& x) const {
     ET z{*element_cast<ET>(x)};
     reduce(z);
     stream << z.x << ' ' << z.y;
}

void Ring_Zsqrtd_quot::do_input(std::istream& stream,
                                Element& x) const {
     ET z;
     if ((stream >> z.x) && (stream >> z.y)) {
          reduce(z);
          x = Element(this, z);
     } else
          stream.setstate(std::ios::failbit);
}

bool Ring_Zsqrtd_quot::do_is_zerodivisor(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Ring_Zsqrtd_quot::do_is_nilpotent(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Ring_Zsqrtd_quot::do_is_unit(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Ring_Zsqrtd_quot::do_is_field() const {
     // TODO
     throw Invalid_operation();
}

void Ring_Zsqrtd_quot::reduce(ET& x) const {
     int q, r;
     divide(x.y, d2_, q, r);
     x.y = r;
     x.x -= q * x0_;
     divide(x.x, d1_, q, r);
     x.x = r;
}

}  // namespace SHG::ALGEBRA
