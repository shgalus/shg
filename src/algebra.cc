/**
 * \file src/algebra.cc
 * Basic algebraic structures.
 */

#include <shg/algebra.h>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <shg/utils.h>
#include <shg/ifact.h>
#include <shg/except.h>

namespace SHG::ALGEBRA {

Element::Element(Algebraic_structure const* as) : as_(as) {
     if (as == nullptr)
          SHG_THROW(std::invalid_argument, __func__);
     v_ = as->zero().value();
}

Element::Element(Algebraic_structure const* as, std::any const& v)
     : as_(as), v_(v) {
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

bool Element::is_valid() const {
     return as_ != nullptr && v_.has_value() &&
            v_.type() == as_->element_type();
}

Algebraic_structure const* Element::as() const {
     return as_;
}

std::any const& Element::value() const {
     return v_;
}

bool Element::is_valid(Element const& x, Element const& y) {
     return x.is_valid() && y.is_valid() && x.as_ == y.as_;
}

bool operator==(Element const& x, Element const& y) {
     if (!Element::is_valid(x, y))
          SHG_THROW(std::invalid_argument, __func__);
     return x.as()->equal(x, y);
}

bool operator!=(Element const& x, Element const& y) {
     return !(x == y);
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

Element operator+(Element const& x, Element const& y) {
     Element z(x);
     return z += y;
}

Element operator-(Element const& x, Element const& y) {
     Element z(x);
     return z -= y;
}

Element operator*(Element const& x, Element const& y) {
     Element z(x);
     return z *= y;
}

Element operator/(Element const& x, Element const& y) {
     Element z(x);
     return z /= y;
}

Element inv(Element const& x) {
     if (!x.is_valid())
          SHG_THROW(std::invalid_argument, __func__);
     return x.as()->inv(x);
}

Element times(Element const& x, int n) {
     if (!x.is_valid())
          SHG_THROW(std::invalid_argument, __func__);
     Algebraic_structure const* as = x.as();
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
     Algebraic_structure const* as = x.as();
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

Algebraic_structure::~Algebraic_structure() {}

Element Algebraic_structure::add(Element const& x,
                                 Element const& y) const {
     return do_add(x, y);
}

Element Algebraic_structure::zero() const {
     return do_zero();
}

Element Algebraic_structure::neg(Element const& x) const {
     return do_neg(x);
}

Element Algebraic_structure::mul(Element const& x,
                                 Element const& y) const {
     return do_mul(x, y);
}

Element Algebraic_structure::one() const {
     return do_one();
}

Element Algebraic_structure::inv(Element const& x) const {
     return do_inv(x);
}

bool Algebraic_structure::is_zero(Element const& x) const {
     return do_is_zero(x);
}

bool Algebraic_structure::is_one(Element const& x) const {
     return do_is_one(x);
}

bool Algebraic_structure::equal(Element const& x,
                                Element const& y) const {
     return do_equal(x, y);
}

std::ostream& Algebraic_structure::output(std::ostream& stream,
                                          Element const& x) const {
     do_output(stream, x);
     return stream;
}

std::istream& Algebraic_structure::input(std::istream& stream,
                                         Element& x) const {
     do_input(stream, x);
     return stream;
}

bool Algebraic_structure::is_abelian() const {
     return do_is_abelian();
}

std::type_info const& Algebraic_structure::element_type() const {
     return do_element_type();
}

Semigroup::~Semigroup() {}

Element Semigroup::do_add(Element const& x, Element const& y) const {
     return this->mul(x, y);
}

Element Semigroup::do_zero() const {
     return this->one();
}

Element Semigroup::do_neg([[maybe_unused]] Element const& x) const {
     throw std::invalid_argument("there is no inverse in semigroup");
}

Element Semigroup::do_inv([[maybe_unused]] Element const& x) const {
     throw std::invalid_argument("there is no inverse in semigroup");
}

bool Semigroup::do_is_zero(Element const& x) const {
     return this->is_one(x);
}

Group::~Group() {}

Element Group::do_add(Element const& x, Element const& y) const {
     return this->mul(x, y);
}

Element Group::do_zero() const {
     return this->one();
}

Element Group::do_neg(Element const& x) const {
     return this->inv(x);
}

bool Group::do_is_zero(Element const& x) const {
     return this->is_one(x);
}

Ring::~Ring() {}

Element Ring::do_inv([[maybe_unused]] Element const& x) const {
     throw std::invalid_argument("there is no inverse in ring");
}

Field::~Field() {}

Finite_strings::Finite_strings(std::set<char> const& alphabet)
     : a_(alphabet) {
     for (auto const c : a_)
          if (!std::islower(c))
               SHG_THROW(std::invalid_argument, __func__);
}

Finite_strings::ET const& Finite_strings::value(
     Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp;
}

Element Finite_strings::element(ET const& x) const {
     for (auto const c : x)
          if (a_.find(c) == a_.end())
               SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

Element Finite_strings::do_mul(Element const& x,
                               Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET const z = *xp + *yp;
     return Element(this, z);
}

Element Finite_strings::do_one() const {
     return Element(this, ET());
}

bool Finite_strings::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return xp->empty();
}

bool Finite_strings::do_equal(Element const& x,
                              Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return *xp == *yp;
}

void Finite_strings::do_output(std::ostream& stream,
                               Element const& x) const {
     auto const xp = element_cast<ET>(x);
     stream << xp->size() << ' ' << *xp;
}

void Finite_strings::do_input(std::istream& stream,
                              Element& x) const {
     bool fail = true;
     ET::size_type n;
     ET z;
     if (stream >> n) {
          if (n > 0) {
               if ((stream >> z) && z.size() == n)
                    fail = false;

          } else {
               fail = false;
          }
     }
     if (fail)
          stream.setstate(std::ios::failbit);
     else
          x = Element(this, z);
}

bool Finite_strings::do_is_abelian() const {
     return a_.size() <= 1;
}

std::type_info const& Finite_strings::do_element_type() const {
     return typeid(ET);
}

Group_Sn::Group_Sn(int n) : n_(n) {
     if (n < 1)
          SHG_THROW(std::invalid_argument, __func__);
     one_.resize(n);
     std::iota(one_.begin(), one_.end(), 0);
}

Group_Sn::ET const& Group_Sn::value(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp;
}

Element Group_Sn::element(ET const& x) const {
     if (!std::is_permutation(x.cbegin(), x.cend(), one_.cbegin()))
          SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

Element Group_Sn::do_mul(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z(n_);
     for (int i = 0; i < n_; i++)
          z[i] = (*xp)[(*yp)[i]];
     return Element(this, z);
}

Element Group_Sn::do_one() const {
     return Element(this, one_);
}

Element Group_Sn::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET z(n_);
     for (int i = 0; i < n_; i++)
          z[(*xp)[i]] = i;
     return Element(this, z);
}

bool Group_Sn::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp == one_;
}

bool Group_Sn::do_equal(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return *xp == *yp;
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

bool Group_Sn::do_is_abelian() const {
     return n_ < 3;
}

std::type_info const& Group_Sn::do_element_type() const {
     return typeid(ET);
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

Finite_group::ET const& Finite_group::value(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp;
}

Element Finite_group::element(ET const& x) const {
     if (x < 0 || x >= n_)
          SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

int Finite_group::order() const {
     return n_;
}

Element Finite_group::do_mul(Element const& x,
                             Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return Element(this, t_(*xp, *yp));
}

Element Finite_group::do_one() const {
     return Element(this, ET(0));
}

Element Finite_group::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     for (int i = 0; i < n_; i++)
          if (t_(*xp, i) == 0)
               return Element(this, i);
     SHG_THROW(std::runtime_error, __func__);
}

bool Finite_group::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp == 0;
}

bool Finite_group::do_equal(Element const& x,
                            Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return *xp == *yp;
}

void Finite_group::do_output(std::ostream& stream,
                             Element const& x) const {
     auto const xp = element_cast<ET>(x);
     stream << *xp;
}

void Finite_group::do_input(std::istream& stream, Element& x) const {
     ET z;
     if ((stream >> z) && z >= 0 && z < n_)
          x = Element(this, z);
     else
          stream.setstate(std::ios::failbit);
}

bool Finite_group::do_is_abelian() const {
     return is_abelian_;
}

std::type_info const& Finite_group::do_element_type() const {
     return typeid(ET);
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

Ring_Z::ET const& Ring_Z::value(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp;
}

Element Ring_Z::element(ET const& x) const {
     return Element(this, x);
}

Element Ring_Z::element(int x) const {
     return Element(this, ET(x));
}

Element Ring_Z::do_add(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return Element(this, ET(*xp + *yp));
}

Element Ring_Z::do_zero() const {
     return Element(this, ET(0));
}

Element Ring_Z::do_neg(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return Element(this, ET(-*xp));
}

Element Ring_Z::do_mul(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return Element(this, ET(*xp * *yp));
}

Element Ring_Z::do_one() const {
     return Element(this, ET(1));
}

bool Ring_Z::do_is_zero(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp == ET(0);
}

bool Ring_Z::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp == ET(1);
}

bool Ring_Z::do_equal(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return *xp == *yp;
}

void Ring_Z::do_output(std::ostream& stream, Element const& x) const {
     auto const xp = element_cast<ET>(x);
     stream << *xp;
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

bool Ring_Z::do_is_abelian() const {
     return true;
}

std::type_info const& Ring_Z::do_element_type() const {
     return typeid(ET);
}

Ring_Zn::Ring_Zn(int n) : n_(n) {
     if (n < 1)
          SHG_THROW(std::invalid_argument, __func__);
}

Ring_Zn::ET const& Ring_Zn::value(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp;
}

Element Ring_Zn::element(ET const& x) const {
     if (x < 0 || x >= n_)
          SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

Element Ring_Zn::do_add(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z = *xp + *yp;
     if (z >= n_)
          z -= n_;
     return Element(this, z);
}

Element Ring_Zn::do_zero() const {
     return Element(this, ET(0));
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

Element Ring_Zn::do_one() const {
     return Element(this, n_ > 1 ? 1 : 0);
}

bool Ring_Zn::do_is_zero(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp == ET(0);
}

bool Ring_Zn::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return n_ > 1 ? *xp == 1 : *xp == 0;
}

bool Ring_Zn::do_equal(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return *xp == *yp;
}

void Ring_Zn::do_output(std::ostream& stream,
                        Element const& x) const {
     auto const xp = element_cast<ET>(x);
     stream << *xp;
}

void Ring_Zn::do_input(std::istream& stream, Element& x) const {
     ET z;
     if ((stream >> z) && z >= 0 && z < n_)
          x = Element(this, z);
     else
          stream.setstate(std::ios::failbit);
}

bool Ring_Zn::do_is_abelian() const {
     return true;
}

std::type_info const& Ring_Zn::do_element_type() const {
     return typeid(ET);
}

Field_Q::ET const& Field_Q::value(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp;
}

Element Field_Q::element(ET const& x) const {
     return Element(this, x);
}

Element Field_Q::element(int x) const {
     return Element(this, ET(x));
}

Element Field_Q::element(int num, int den) const {
     if (den == 0)
          SHG_THROW(std::invalid_argument, __func__);
     ET x = ET(num) / ET(den);
     return Element(this, x);
}

Element Field_Q::do_add(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return Element(this, ET(*xp + *yp));
}

Element Field_Q::do_zero() const {
     return Element(this, ET(0));
}

Element Field_Q::do_neg(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return Element(this, ET(-*xp));
}

Element Field_Q::do_mul(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return Element(this, ET(*xp * *yp));
}

Element Field_Q::do_one() const {
     return Element(this, ET(1));
}

Element Field_Q::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     if (*xp == 0)
          SHG_THROW(std::invalid_argument, __func__);
     return Element(this, ET(1 / *xp));
}

bool Field_Q::do_is_zero(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp == ET(0);
}

bool Field_Q::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp == ET(1);
}

bool Field_Q::do_equal(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return *xp == *yp;
}

void Field_Q::do_output(std::ostream& stream,
                        Element const& x) const {
     auto const xp = element_cast<ET>(x);
     stream << *xp;
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

bool Field_Q::do_is_abelian() const {
     return true;
}

std::type_info const& Field_Q::do_element_type() const {
     return typeid(ET);
}

Field_Fp::Field_Fp(int p) : p_(p) {
     if (!is_prime(p))
          SHG_THROW(std::invalid_argument, __func__);
}

Field_Fp::ET const& Field_Fp::value(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp;
}

Element Field_Fp::element(ET const& x) const {
     if (x < 0 || x >= p_)
          SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

Element Field_Fp::do_add(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z = *xp + *yp;
     if (z >= p_)
          z -= p_;
     return Element(this, z);
}

Element Field_Fp::do_zero() const {
     return Element(this, ET(0));
}

Element Field_Fp::do_neg(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET const z = *xp == 0 ? 0 : p_ - *xp;
     return Element(this, z);
}

Element Field_Fp::do_mul(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return Element(this, (*xp * *yp) % p_);
}

Element Field_Fp::do_one() const {
     return Element(this, ET(1));
}

Element Field_Fp::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     if (*xp == 0)
          SHG_THROW(std::invalid_argument, __func__);
     Extended_gcd gcd(*xp, p_);
     while (gcd.u1 < 0)  // if instead of while is enough here!
          gcd.u1 += p_;
     return Element(this, gcd.u1);
}

bool Field_Fp::do_is_zero(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp == ET(0);
}

bool Field_Fp::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp == ET(1);
}

bool Field_Fp::do_equal(Element const& x, Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     return *xp == *yp;
}

void Field_Fp::do_output(std::ostream& stream,
                         Element const& x) const {
     auto const xp = element_cast<ET>(x);
     stream << *xp;
}

void Field_Fp::do_input(std::istream& stream, Element& x) const {
     ET z;
     if ((stream >> z) && z >= 0 && z < p_)
          x = Element(this, z);
     else
          stream.setstate(std::ios::failbit);
}

bool Field_Fp::do_is_abelian() const {
     return true;
}

std::type_info const& Field_Fp::do_element_type() const {
     return typeid(ET);
}

Direct_product_of_groups::Direct_product_of_groups(
     std::vector<Group const*> const& v)
     : v_(v) {
     if (v_.size() == 0)
          SHG_THROW(std::invalid_argument, __func__);
     for (Sztp i = 0; i < v_.size(); i++)
          if (v_[i] == nullptr)
               SHG_THROW(std::invalid_argument, __func__);
}

Direct_product_of_groups::ET const& Direct_product_of_groups::value(
     Element const& x) const {
     auto const xp = element_cast<ET>(x);
     return *xp;
}

Element Direct_product_of_groups::element(ET const& x) const {
     if (x.size() != v_.size())
          SHG_THROW(std::invalid_argument, __func__);
     for (Sztp i = 0; i < v_.size(); i++)
          if (!x[i].is_valid() || x[i].as() != v_[i])
               SHG_THROW(std::invalid_argument, __func__);
     return Element(this, x);
}

void Direct_product_of_groups::separator(std::string const& sep) {
     sep_ = sep;
}

std::string const& Direct_product_of_groups::separator() const {
     return sep_;
}

Element Direct_product_of_groups::do_mul(Element const& x,
                                         Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     ET z(v_.size());
     for (Sztp i = 0; i < v_.size(); i++)
          z[i] = v_[i]->mul((*xp)[i], (*yp)[i]);
     return Element(this, z);
}

Element Direct_product_of_groups::do_one() const {
     ET z(v_.size());
     for (Sztp i = 0; i < v_.size(); i++)
          z[i] = v_[i]->one();
     return Element(this, z);
}

Element Direct_product_of_groups::do_inv(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     ET z(v_.size());
     for (Sztp i = 0; i < v_.size(); i++)
          z[i] = v_[i]->inv((*xp)[i]);
     return Element(this, z);
}

bool Direct_product_of_groups::do_is_one(Element const& x) const {
     auto const xp = element_cast<ET>(x);
     for (Sztp i = 0; i < v_.size(); i++)
          if (!v_[i]->is_one((*xp)[i]))
               return false;
     return true;
}

bool Direct_product_of_groups::do_equal(Element const& x,
                                        Element const& y) const {
     auto const xp = element_cast<ET>(x);
     auto const yp = element_cast<ET>(y);
     for (Sztp i = 0; i < v_.size(); i++)
          if (!v_[i]->equal((*xp)[i], (*yp)[i]))
               return false;
     return true;
}

void Direct_product_of_groups::do_output(std::ostream& stream,
                                         Element const& x) const {
     auto const xp = element_cast<ET>(x);
     for (Sztp i = 0; i < v_.size(); i++) {
          stream << (*xp)[i];
          if (i + 1 < v_.size())
               stream << ' ';
     }
}

void Direct_product_of_groups::do_input(std::istream& stream,
                                        Element& x) const {
     ET z(v_.size());
     for (Sztp i = 0; i < v_.size(); i++)
          if (!v_[i]->input(stream, z[i])) {
               stream.setstate(std::ios::failbit);
               return;
          }
     x = Element(this, z);
}

bool Direct_product_of_groups::do_is_abelian() const {
     return std::all_of(v_.cbegin(), v_.cend(), [](Group const* g) {
          return g->is_abelian();
     });
}

std::type_info const& Direct_product_of_groups::do_element_type()
     const {
     return typeid(ET);
}

}  // namespace SHG::ALGEBRA
