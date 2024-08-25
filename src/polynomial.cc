/**
 * \file src/polynomial.cc
 * Polynomials over an algebraic structure.
 */

#include <shg/polynomial.h>
#include <algorithm>
#include <shg/except.h>
#include <shg/utils.h>

namespace SHG::ALGEBRA {

Polynomial::Polynomial(AS const* as) : Polynomial(as, 1) {}

Polynomial::Polynomial(int dim) : Polynomial(&q_, dim) {}

Polynomial::Polynomial(AS const* as, int dim) : as_(as), dim_(dim) {
     if (as == nullptr || dim < 1)
          SHG_THROW(std::invalid_argument, __func__);
}

void Polynomial::dim(int d) {
     if (d < 1)
          SHG_THROW(std::invalid_argument, __func__);
     dim_ = d;
     t_.clear();
}

int Polynomial::deg() const {
     int d = -1;
     for (auto const& [m, a] : t_) {
          int const td = m.deg();
          if (td > d)
               d = td;
     }
     return d;
}

Monomial const& Polynomial::leading_monomial() const {
     auto const it = max_term();
     if (it == t_.cend())
          SHG_THROW(std::invalid_argument, __func__);
     return it->first;
}

Element const& Polynomial::leading_coefficient() const {
     auto const it = max_term();
     if (it == t_.cend())
          SHG_THROW(std::invalid_argument, __func__);
     return it->second;
}

Term Polynomial::leading_term() const {
     auto const it = max_term();
     if (it == t_.cend())
          SHG_THROW(std::invalid_argument, __func__);
     return Term(it->second, it->first);
}

Element Polynomial::operator()(Point const& x) const {
     if (static_cast<Point::size_type>(dim_) != x.size())
          SHG_THROW(std::invalid_argument, __func__);
     for (auto const& xi : x)
          if (xi.as() != as_)
               SHG_THROW(std::invalid_argument, __func__);
     Element y = as_->zero();
     for (auto const& [m, a] : t_) {
          Element t = a;
          for (int i = 0; i < dim_; i++)
               t *= pow(x[i], m[i]);
          y += t;
     }
     return y;
}

void Polynomial::order(Monomial_cmp cmp) {
     if (cmp == nullptr)
          SHG_THROW(std::invalid_argument, __func__);
     order_ = cmp;
}

Polynomial& Polynomial::operator+=(Polynomial const& x) {
     if (!is_valid(*this, x))
          SHG_THROW(std::invalid_argument, __func__);
     for (auto const& [m, a] : x.t_)
          add(a, m);
     return *this;
}

Polynomial& Polynomial::operator-=(Polynomial const& x) {
     if (!is_valid(*this, x))
          SHG_THROW(std::invalid_argument, __func__);
     for (auto const& [m, a] : x.t_)
          sub(a, m);
     return *this;
}

Polynomial& Polynomial::operator*=(Polynomial const& x) {
     if (!is_valid(*this, x))
          SHG_THROW(std::invalid_argument, __func__);
     Polynomial y(as_, dim_);
     for (auto const& [m, a] : t_)
          for (auto const& [mx, ax] : x.t_)
               y.add(as_->mul(a, ax), m * mx);
     t_ = y.t_;
     return *this;
}

Polynomial& Polynomial::operator+=(Term const& x) {
     if (!is_valid(*this, x))
          SHG_THROW(std::invalid_argument, __func__);
     add(x.a(), x.m());
     return *this;
}

Polynomial& Polynomial::operator-=(Term const& x) {
     if (!is_valid(*this, x))
          SHG_THROW(std::invalid_argument, __func__);
     sub(x.a(), x.m());
     return *this;
}

Polynomial& Polynomial::operator*=(Term const& x) {
     if (!is_valid(*this, x))
          SHG_THROW(std::invalid_argument, __func__);
     mul(x.a(), x.m());
     return *this;
}

Polynomial& Polynomial::operator+=(Element const& x) {
     return operator+=(Term(x, Monomial(dim_)));
}

Polynomial& Polynomial::operator-=(Element const& x) {
     return operator-=(Term(x, Monomial(dim_)));
}

Polynomial& Polynomial::operator*=(Element const& x) {
     return operator*=(Term(x, Monomial(dim_)));
}

bool Polynomial::is_valid(Polynomial const& x, Polynomial const& y) {
     return x.as_ == y.as_ && x.dim_ == y.dim_;
}

bool Polynomial::is_valid(Polynomial const& x, Term const& y) {
     return x.as_ == y.a().as() && x.dim_ == y.m().dim();
}

Polynomial::Terms::const_iterator Polynomial::max_term() const {
     return std::max_element(
          t_.cbegin(), t_.cend(),
          [this](std::pair<Monomial, Element> const& x,
                 std::pair<Monomial, Element> const& y) {
               return order_(x.first, y.first);
          });
}

void Polynomial::add(Element const& a, Monomial const& x) {
     if (a != as_->zero()) {
          auto it = t_.find(x);
          if (it == t_.end()) {
               t_[x] = a;
          } else {
               Element const a1 = as_->add(it->second, a);
               if (a1 == as_->zero())
                    t_.erase(it);
               else
                    it->second = a1;
          }
     }
}

void Polynomial::sub(Element const& a, Monomial const& x) {
     add(as_->neg(a), x);
}

void Polynomial::mul(Element const& a, Monomial const& x) {
     Polynomial q(as_, dim_);
     if (a != as_->zero())
          for (auto const& [m, c] : t_)
               q.add(as_->mul(a, c), x * m);
     t_ = q.t_;
}

Field_Q const Polynomial::q_ = Field_Q();

bool operator==(Polynomial const& x, Polynomial const& y) {
     if (x.as() != y.as())
          return false;
     if (x.dim() != y.dim())
          return false;
     if (x.terms() != y.terms())
          return false;
     return true;
}

bool operator!=(Polynomial const& x, Polynomial const& y) {
     return !(x == y);
}

bool is_zero(Polynomial const& x) {
     return x.is_zero();
}

Polynomial operator+(Polynomial const& x) {
     return x;
}

Polynomial operator-(Polynomial const& x) {
     Polynomial y(x.as(), x.dim());
     y -= x;
     return y;
}

Polynomial operator+(Polynomial const& x, Polynomial const& y) {
     Polynomial z(x);
     return z += y;
}

Polynomial operator-(Polynomial const& x, Polynomial const& y) {
     Polynomial z(x);
     return z -= y;
}

Polynomial operator*(Polynomial const& x, Polynomial const& y) {
     Polynomial z(x);
     return z *= y;
}

Polynomial operator+(Polynomial const& x, Term const& y) {
     Polynomial z(x);
     return z += y;
}

Polynomial operator-(Polynomial const& x, Term const& y) {
     Polynomial z(x);
     return z -= y;
}

Polynomial operator*(Polynomial const& x, Term const& y) {
     Polynomial z(x);
     return z *= y;
}

Polynomial operator+(Term const& x, Polynomial const& y) {
     Polynomial z(y);
     return z += x;
}

Polynomial operator-(Term const& x, Polynomial const& y) {
     Polynomial z(-y);
     return z += x;
}

Polynomial operator*(Term const& x, Polynomial const& y) {
     Polynomial z(y);
     return z *= x;
}

Polynomial operator+(Polynomial const& x, Element const& y) {
     Polynomial z(x);
     return z += y;
}

Polynomial operator-(Polynomial const& x, Element const& y) {
     Polynomial z(x);
     return z -= y;
}

Polynomial operator*(Polynomial const& x, Element const& y) {
     Polynomial z(x);
     return z *= y;
}

Polynomial operator+(Element const& x, Polynomial const& y) {
     Polynomial z(y);
     return z += x;
}

Polynomial operator-(Element const& x, Polynomial const& y) {
     Polynomial z(-y);
     return z += x;
}

Polynomial operator*(Element const& x, Polynomial const& y) {
     Polynomial z(y);
     return z *= x;
}

std::ostream& operator<<(std::ostream& stream, Polynomial const& x) {
     auto t = x.terms();
     stream << x.dim() << ' ' << t.size();
     for (auto it = t.crbegin(); it != t.crend(); ++it)
          stream << ' ' << it->second << ' ' << it->first;
     return stream;
}

std::istream& operator>>(std::istream& stream, Polynomial& x) {
     bool fail = true;
     int n;
     Polynomial::Terms::size_type m, i;
     Term t(x.as());

     if ((stream >> n >> m) && n >= 1) {
          Polynomial z(x.as(), n);
          for (i = 0; i < m && (stream >> t); i++)
               z += t;
          if (i >= m) {
               x = z;
               fail = false;
          }
     }
     if (fail)
          stream.setstate(std::ios::failbit);
     return stream;
}

Polynomial one_var(std::vector<Element> const& a) {
     if (a.size() == 0)
          return Polynomial(1);
     Polynomial x(a[0].as(), 1);
     for (std::vector<Element>::size_type i = 0; i < a.size(); i++)
          x += Term(a[i], Monomial{narrow_cast<int>(i)});
     return x;
}

Polynomial_ring::Polynomial_ring(AS const* as)
     : Polynomial_ring(as, 1) {
     if (as == nullptr)
          SHG_THROW(std::invalid_argument, __func__);
}

Polynomial_ring::Polynomial_ring(int dim)
     : Polynomial_ring(&q_, dim) {}

Polynomial_ring::Polynomial_ring(AS const* as, int dim)
     : as_(as), dim_(dim) {
     if (as == nullptr || dim < 1)
          SHG_THROW(std::invalid_argument, __func__);
}

Polynomial_ring::~Polynomial_ring() {}

Polynomial_ring::ET const& Polynomial_ring::value(
     Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp;
}

Element Polynomial_ring::element(ET const& x) const {
     if (x.as() != as_ || x.dim() != dim_)
          SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

void Polynomial_ring::reset(AS const* as, int dim) {
     if (as == nullptr || dim < 1)
          SHG_THROW(std::invalid_argument, __func__);
     as_ = as;
     dim_ = dim;
}

Element Polynomial_ring::do_add(Element const& x,
                                Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET const z = *xp + *yp;
     return Element(this, z);
}

Element Polynomial_ring::do_zero() const {
     ET const z = Polynomial(as_, dim_);
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
     ET const z = Polynomial(as_, dim_) + as_->one();
     return Element(this, z);
}

Element Polynomial_ring::do_inv(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Polynomial_ring::do_is_zero(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return xp->is_zero();
}

bool Polynomial_ring::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET const z = Polynomial(as_, dim_) + as_->one();
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
     ET z = Polynomial(as_);
     if ((stream >> z) && z.dim() == dim_)
          x = Element(this, z);
     else
          stream.setstate(std::ios::failbit);
}

std::type_info const& Polynomial_ring::do_element_type() const {
     return typeid(ET);
}

bool Polynomial_ring::do_is_zerodivisor(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Polynomial_ring::do_is_nilpotent(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Polynomial_ring::do_is_unit(
     [[maybe_unused]] Element const& x) const {
     // TODO
     throw Invalid_operation();
}

bool Polynomial_ring::do_is_field() const {
     // TODO
     throw Invalid_operation();
}

Field_Q const Polynomial_ring::q_ = Field_Q();

}  // namespace SHG::ALGEBRA
