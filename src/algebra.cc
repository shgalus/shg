/**
 * \file src/algebra.cc
 * Basic algebraic structures.
 */

#include <shg/algebra.h>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <numeric>

namespace SHG::ALGEBRA {

Invalid_operation::Invalid_operation()
     : std::logic_error("Invalid operation") {}

Element::Element(AS const* as) : as_(as) {
     if (as == nullptr)
          SHG_THROW(std::invalid_argument, __func__);
     v_ = as->zero().value();
}

Element::Element(AS const* as, std::any const& v) : as_(as), v_(v) {
     if (as == nullptr || !v.has_value() ||
         v.type() != as->element_type())
          SHG_THROW(std::invalid_argument, __func__);
}

void Element::set_to_zero() {
     if (as_ == nullptr)
          SHG_THROW(std::invalid_argument, __func__);
     *this = as_->zero();
}

void Element::set_to_one() {
     if (as_ == nullptr)
          SHG_THROW(std::invalid_argument, __func__);
     *this = as_->one();
}

Element& Element::operator+=(Element const& x) {
     if (!Element::is_valid(*this, x))
          SHG_THROW(std::invalid_argument, __func__);
     return *this = as_->add(*this, x);
}

Element& Element::operator-=(Element const& x) {
     if (!Element::is_valid(*this, x))
          SHG_THROW(std::invalid_argument, __func__);
     return *this = as_->add(*this, as_->neg(x));
}

Element& Element::operator*=(Element const& x) {
     if (!Element::is_valid(*this, x))
          SHG_THROW(std::invalid_argument, __func__);
     return *this = as_->mul(*this, x);
}

Element& Element::operator/=(Element const& x) {
     if (!Element::is_valid(*this, x))
          SHG_THROW(std::invalid_argument, __func__);
     return *this = as_->mul(*this, as_->inv(x));
}

bool operator==(Element const& x, Element const& y) {
     if (!Element::is_valid(x, y))
          SHG_THROW(std::invalid_argument, __func__);
     return x.as()->equal(x, y);
}

bool is_zero(Element const& x) {
     if (x.is_valid())
          return x.as()->is_zero(x);
     SHG_THROW(std::invalid_argument, __func__);
}

bool is_one(Element const& x) {
     if (x.is_valid())
          return x.as()->is_one(x);
     SHG_THROW(std::invalid_argument, __func__);
}

Element operator+(Element const& x) {
     if (!x.is_valid())
          SHG_THROW(std::invalid_argument, __func__);
     return x;
}

Element operator-(Element const& x) {
     if (!x.is_valid())
          SHG_THROW(std::invalid_argument, __func__);
     return x.as()->neg(x);
}

Element inv(Element const& x) {
     if (!x.is_valid())
          SHG_THROW(std::invalid_argument, __func__);
     return x.as()->inv(x);
}

Element times(Element const& x, int n) {
     if (!x.is_valid())
          SHG_THROW(std::invalid_argument, __func__);
     AS const* as = x.as();
     Element y = as->zero();
     Element z = x;
     if (n < 0) {
          z = as->neg(z);
          n = -n;
     }
     std::div_t dv;
     for (;;) {
          dv = std::div(n, 2);
          if (dv.rem != 0)
               y = as->add(y, z);
          n = dv.quot;
          if (n == 0)
               return y;
          z = as->add(z, z);
     }
}

Element pow(Element const& x, int n) {
     if (!x.is_valid())
          SHG_THROW(std::invalid_argument, __func__);
     AS const* as = x.as();
     Element y = as->one();
     Element z = x;
     if (n < 0) {
          z = as->inv(z);
          n = -n;
     }
     std::div_t dv;
     for (;;) {
          dv = std::div(n, 2);
          if (dv.rem != 0)
               y = as->mul(y, z);
          n = dv.quot;
          if (n == 0)
               return y;
          z = as->mul(z, z);
     }
}

std::ostream& operator<<(std::ostream& stream, Element const& x) {
     if (x.as() != nullptr) {
          return x.as()->output(stream, x);
     }
     stream.setstate(std::ios::failbit);
     return stream;
}

std::istream& operator>>(std::istream& stream, Element& x) {
     if (x.as() != nullptr) {
          return x.as()->input(stream, x);
     }
     stream.setstate(std::ios::failbit);
     return stream;
}

std::ostream& AS::output(std::ostream& stream,
                         Element const& x) const {
     do_output(stream, x);
     return stream;
}

std::istream& AS::input(std::istream& stream, Element& x) const {
     do_input(stream, x);
     return stream;
}

Group_Sn::Group_Sn(int n) : n_(n) {
     if (n < 1)
          SHG_THROW(std::invalid_argument, __func__);
     one_.resize(n);
     std::iota(one_.begin(), one_.end(), 0);
}

Element Group_Sn::element(ET const& x) const {
     if (!std::is_permutation(x.cbegin(), x.cend(), one_.cbegin()))
          SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

void Group_Sn::reset(int n) {
     if (n < 1)
          SHG_THROW(std::invalid_argument, __func__);
     n_ = n;
     one_.resize(n);
     std::iota(one_.begin(), one_.end(), 0);
}

Element Group_Sn::do_mul(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z(n_);
     for (int i = 0; i < n_; i++)
          z[i] = (*xp)[(*yp)[i]];
     return Element(this, z);
}

Element Group_Sn::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET z(n_);
     for (int i = 0; i < n_; i++)
          z[(*xp)[i]] = i;
     return Element(this, z);
}

void Group_Sn::do_output(std::ostream& stream,
                         Element const& x) const {
     auto const xp = element_cast<ET>(x);
     stream << n_;
     for (int i = 0; i < n_; i++)
          stream << ' ' << (*xp)[i];
}

void Group_Sn::do_input(std::istream& stream, Element& x) const {
     int n;
     if ((stream >> n) && n == n_) {
          ET z(n);
          int i;
          for (i = 0; i < n && (stream >> z[i]); i++)
               ;
          if (i >= n)
               if (std::is_permutation(z.cbegin(), z.cend(),
                                       one_.cbegin())) {
                    x = Element(this, z);
                    return;
               }
     }
     stream.setstate(std::ios::failbit);
}

Finite_group::Finite_group(Matrix<int> const& t)
     : n_(static_cast<int>(t.nrows())), t_(t) {
     if (t.nrows() > std::numeric_limits<int>::max() ||
         t.nrows() != t.ncols() || t.nrows() < 1 || !is_valid_table())
          SHG_THROW(std::invalid_argument, "invalid table");

     for (int i = 0; is_abelian_ && i < n_; i++)
          for (int j = i + 1; j < n_; j++)
               if (t_(i, j) != t_(j, i)) {
                    is_abelian_ = false;
                    break;
               }
}

Element Finite_group::element(ET const& x) const {
     if (x < 0 || x >= n_)
          SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

Element Finite_group::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     for (int i = 0; i < n_; i++)
          if (t_(*xp, i) == 0)
               return Element(this, i);
     SHG_THROW(std::runtime_error, __func__);
}

void Finite_group::do_input(std::istream& stream, Element& x) const {
     ET z;
     if ((stream >> z) && z >= 0 && z < n_)
          x = Element(this, z);
     else
          stream.setstate(std::ios::failbit);
}

bool Finite_group::is_valid_table() const {
     // Are the first row and the first column 0, ..., n - 1?
     for (int i = 0; i < n_; i++)
          if (t_(0, i) != i || t_(0, i) != t_(i, 0))
               return false;
     // Is each row and each column a permutation of 0, ..., n - 1?
     Vector<int> v(n_);
     auto const first1 = t_.vector().cbegin();
     auto const last1 = first1 + n_;
     for (int i = 1; i < n_; i++) {
          for (int j = 0; j < n_; j++)
               v(j) = t_(i, j);
          if (!std::is_permutation(first1, last1, v.cbegin()))
               return false;
          for (int j = 0; j < n_; j++)
               v(j) = t_(j, i);
          if (!std::is_permutation(first1, last1, v.cbegin()))
               return false;
     }
     // Is the operation associative?
     for (int i = 0; i < n_; i++)
          for (int j = 0; j < n_; j++)
               for (int k = 0; k < n_; k++)
                    if (t_(t_(i, j), k) != t_(i, t_(j, k)))
                         return false;
     return true;
}

Element Ring_Z::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     if (abs(*xp) == 1)
          return x;
     throw Invalid_operation();
}

void Ring_Z::do_input(std::istream& stream, Element& x) const {
     ET z;
     try {
          if (stream >> z)
               x = Element(this, z);
     } catch (boost::exception const&) {
          stream.setstate(std::ios::failbit);
     }
}

Element Ring_Zn::element(ET const& x) const {
     if (x < 0 || x >= n_)
          SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

void Ring_Zn::reset(int n) {
     if (n < 1)
          SHG_THROW(std::invalid_argument, __func__);
     n_ = n;
     is_prime_ = is_prime(n);
     Integer_factorization<int> f;
     f.factorize(n);
     int d = 1;
     bool ex = false;
     for (auto it = f.repr().cbegin(); it != f.repr().cend(); ++it) {
          d *= it->p;
          if (it->n > 1)
               ex = true;
     }
     p1pm_ = ex ? d : -1;
}

Element Ring_Zn::do_add(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z = *xp + *yp;
     if (z >= n_)
          z -= n_;
     return Element(this, z);
}

Element Ring_Zn::do_neg(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET z = *xp == 0 ? 0 : n_ - *xp;
     return Element(this, z);
}

Element Ring_Zn::do_mul(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return Element(this, (*xp * *yp) % n_);
}

Element Ring_Zn::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     Extended_gcd gcd(*xp, n_);
     if (gcd.u3 != 1)
          throw Invalid_operation();
     while (gcd.u1 < 0)  // if instead of while is enough here!
          gcd.u1 += n_;
     return Element(this, gcd.u1);
}

void Ring_Zn::do_input(std::istream& stream, Element& x) const {
     ET z;
     if ((stream >> z) && z >= 0 && z < n_)
          x = Element(this, z);
     else
          stream.setstate(std::ios::failbit);
}

bool Ring_Zn::do_is_nilpotent(Element const& x) const {
     if (*element_cast<ET>(x) == 0)
          return true;
     if (p1pm_ == -1)
          return false;
     return *element_cast<ET>(x) % p1pm_ == 0;
}

bool Ring_Zn::do_is_unit(Element const& x) const {
     if (n_ == 1)
          return true;
     return gcd(*element_cast<ET>(x), n_) == 1;
}

Element Field_Q::element(int num, int den) const {
     if (den == 0)
          SHG_THROW(std::invalid_argument, __func__);
     ET x = ET(num) / ET(den);
     return Element(this, x);
}

Element Field_Q::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     if (*xp == 0)
          SHG_THROW(std::invalid_argument, __func__);
     return Element(this, ET(1 / *xp));
}

void Field_Q::do_input(std::istream& stream, Element& x) const {
     ET z;
     try {
          if (stream >> z)
               x = Element(this, z);
     } catch (boost::exception const&) {
          stream.setstate(std::ios::failbit);
     }
}

Element Direct_product::element(ET const& x) const {
     if (x.size() != v_.size())
          SHG_THROW(std::invalid_argument, __func__);
     for (Sztp i = 0; i < v_.size(); i++)
          if (!x[i].is_valid() || x[i].as() != v_[i])
               SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

void Direct_product::reset(std::vector<AS const*> const& v) {
     if (v.empty() ||
         std::any_of(v.cbegin(), v.cend(),
                     [](AS const* p) { return p == nullptr; }))
          SHG_THROW(std::invalid_argument, __func__);
     v_ = v;
}

Element Direct_product::do_add(Element const& x,
                               Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z(v_.size());
     std::transform(xp->cbegin(), xp->cend(), yp->cbegin(), z.begin(),
                    std::plus<Element>());
     return Element(this, z);
}

Element Direct_product::do_zero() const {
     ET z(v_.size());
     for (Sztp i = 0; i < v_.size(); i++)
          z[i] = v_[i]->zero();
     return Element(this, z);
}

Element Direct_product::do_neg(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET z(v_.size());
     std::transform(xp->cbegin(), xp->cend(), z.begin(),
                    std::negate<Element>());
     return Element(this, z);
}

Element Direct_product::do_mul(Element const& x,
                               Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z(v_.size());
     std::transform(xp->cbegin(), xp->cend(), yp->cbegin(), z.begin(),
                    std::multiplies<Element>());
     return Element(this, z);
}

Element Direct_product::do_one() const {
     ET z(v_.size());
     for (Sztp i = 0; i < v_.size(); i++)
          z[i] = v_[i]->one();
     return Element(this, z);
}

Element Direct_product::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET z(v_.size());
     for (Sztp i = 0; i < v_.size(); i++)
          z[i] = v_[i]->inv((*xp)[i]);
     return Element(this, z);
}

bool Direct_product::do_is_zero(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     for (Sztp i = 0; i < v_.size(); i++)
          if (!v_[i]->is_zero((*xp)[i]))
               return false;
     return true;
}

bool Direct_product::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     for (Sztp i = 0; i < v_.size(); i++)
          if (!v_[i]->is_one((*xp)[i]))
               return false;
     return true;
}

void Direct_product::do_output(std::ostream& stream,
                               Element const& x) const {
     auto const xp = element_cast<ET>(x);
     for (Sztp i = 0; i < v_.size(); i++) {
          stream << (*xp)[i];
          if (i + 1 < v_.size())
               stream << ' ';
     }
}

void Direct_product::do_input(std::istream& stream,
                              Element& x) const {
     ET z(v_.size());
     for (Sztp i = 0; i < v_.size(); i++)
          if (!v_[i]->input(stream, z[i])) {
               stream.setstate(std::ios::failbit);
               return;
          }
     x = Element(this, z);
}

}  // namespace SHG::ALGEBRA
