/**
 * \file src/polynomial.cc
 * Polynomial over a field.
 */

#include <cctype>
#include <algorithm>
#include <shg/polynomial.h>
#include <shg/except.h>
#include <shg/utils.h>

namespace SHG::ALGEBRA {

bool operator==(Term const& x, Term const& y) {
     return x.a() == y.a() && x.m() == y.m();
}

bool operator!=(Term const& x, Term const& y) {
     return !(x == y);
}

std::ostream& operator<<(std::ostream& stream, Term const& x) {
     stream << x.a() << ' ' << x.m();
     return stream;
}

std::istream& operator>>(std::istream& stream, Term& x) {
     bool fail = true;
     if (x.as() != nullptr) {
          Element z(x.as());
          Monomial m;
          if ((stream >> z) && (stream >> m)) {
               x = Term(z, m);
               fail = false;
          }
     }
     if (fail)
          stream.setstate(std::ios::failbit);
     return stream;
}

Polynomial::Polynomial(Field const* k) : Polynomial(k, 1) {}

Polynomial::Polynomial(int dim) : Polynomial(&q_, dim) {}

Polynomial::Polynomial(Field const* k, int dim) : k_(k), dim_(dim) {
     if (k == nullptr || !k->is_abelian() || dim < 1)
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
          if (xi.as() != k_)
               SHG_THROW(std::invalid_argument, __func__);
     Element y = k_->zero();
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
     Polynomial y(k_, dim_);
     for (auto const& [m, a] : t_)
          for (auto const& [mx, ax] : x.t_)
               y.add(k_->mul(a, ax), m * mx);
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
     return x.k_ == y.k_ && x.dim_ == y.dim_;
}

bool Polynomial::is_valid(Polynomial const& x, Term const& y) {
     return x.k_ == y.a().as() && x.dim_ == y.m().dim();
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
     if (a != k_->zero()) {
          auto it = t_.find(x);
          if (it == t_.end()) {
               t_[x] = a;
          } else {
               Element const a1 = k_->add(it->second, a);
               if (a1 == k_->zero())
                    t_.erase(it);
               else
                    it->second = a1;
          }
     }
}

void Polynomial::sub(Element const& a, Monomial const& x) {
     add(k_->neg(a), x);
}

void Polynomial::mul(Element const& a, Monomial const& x) {
     Polynomial q(k_, dim_);
     if (a != k_->zero())
          for (auto const& [m, c] : t_)
               q.add(k_->mul(a, c), x * m);
     t_ = q.t_;
}

Field_Q const Polynomial::q_ = Field_Q();

bool operator==(Polynomial const& x, Polynomial const& y) {
     if (x.field() != y.field())
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
     Polynomial y(x.field(), x.dim());
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
     Term t(x.field());

     if ((stream >> n >> m) && n >= 1) {
          Polynomial z(x.field(), n);
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

Polynomial from_chars(char const* s) {
     Polynomial x;
     std::istringstream iss(s);
     iss >> x;
     if (!iss)
          throw std::invalid_argument(__func__);
     return x;
}

void Polynomial_div::divide(Polynomial const& f,
                            std::vector<Polynomial> const& g) {
     using Sztp = std::vector<Polynomial>::size_type;

     Sztp const s = g.size();
     if (s == 0)
          throw std::invalid_argument(
               "division of polynomials: no divisors");
     for (auto const& gi : g)
          if (gi.dim() != f.dim())
               throw std::invalid_argument(
                    "division of polynomials: bad number of "
                    "variables");
          else if (gi.deg() == -1)
               throw std::invalid_argument(
                    "division of polynomials: divisor is a zero "
                    "polynomial");
          else if (gi.order() != f.order())
               throw std::invalid_argument(
                    "division of polynomials: divisor has a wrong "
                    "order");
     a.resize(s);
     for (auto& ai : a) {
          ai = Polynomial(f.field(), f.dim());
          ai.order(f.order());
     }
     r = Polynomial(f.field(), f.dim());
     r.order(f.order());
     Polynomial p{f};

     while (!p.is_zero()) {
          auto const ltp = p.leading_term();
          bool division_occured = false;
          for (Sztp i = 0; i < s && !division_occured; i++) {
               auto const& gi = g[i];
               auto const ltgi = gi.leading_term();
               if (ltgi.m().divides(ltp.m())) {
                    Term const w{ltp.a() / ltgi.a(),
                                 ltp.m() / ltgi.m()};
                    a[i] += w;
                    p -= w * gi;
                    division_occured = true;
               }
          }
          if (!division_occured) {
               r += ltp;
               p -= ltp;
          }
     }
}

Polynomial normal_form(Polynomial const& f,
                       std::vector<Polynomial> const& g) {
     Polynomial_div pd;
     pd.divide(f, g);
     return pd.r;
}

Polynomial s_polynomial(Polynomial const& f, Polynomial const& g) {
     if (!(Polynomial::is_valid(f, g) && f.order() == g.order()))
          SHG_THROW(std::invalid_argument, __func__);
     if (f.is_zero() || g.is_zero())
          SHG_THROW(std::invalid_argument, __func__);
     return s_polynomial_unsafe(f, g);
}

Polynomial s_polynomial_unsafe(Polynomial const& f,
                               Polynomial const& g) {
     auto const ltf = f.leading_term();
     auto const ltg = g.leading_term();
     auto const gamma = lcm(ltf.m(), ltg.m());
     auto const one = f.field()->one();
     Term const tf{one / ltf.a(), gamma / ltf.m()};
     Term const tg{one / ltg.a(), gamma / ltg.m()};
     Polynomial const p = tf * f - tg * g;
     return p;
}

void Buchberger_improved::run(S const& f) {
     if (f.empty())
          throw std::invalid_argument(
               "Buchberger: no polynomial given");
     int const dim = f.front().dim();
     Field const* const field = f[0].field();
     if (dim < 1)
          throw std::invalid_argument(
               "Buchberger: invalid dimension");

     r_.clear();
     for (auto const& p : f) {
          if (p.dim() != dim)
               throw std::invalid_argument(
                    "Buchberger: bad number of variables");
          if (p.deg() != -1 && !contains(r_, p)) {
               P q{p};
               q *= field->one() / p.leading_coefficient();
               r_.push_back(q);
          }
     }
     if (r_.empty()) {
          g_.clear();
          return;
     }

     Monomial_cmp cmp = f[0].order();
     p_.clear();
     g_.clear();
     b_.clear();
     reduce_all();
     new_basis();
     while (!b_.empty()) {
          auto it = b_.cbegin();
          P f1{it->f1_};
          P f2{it->f2_};
          auto it0 = it;
          Monomial lcm_min =
               lcm(f1.leading_monomial(), f2.leading_monomial());
          while (++it != b_.cend()) {
               auto const& lm1{it->f1_.leading_monomial()};
               auto const& lm2{it->f2_.leading_monomial()};
               Monomial const lcm_pmin = lcm(lm1, lm2);
               if (cmp(lcm_pmin, lcm_min)) {
                    f1 = it->f1_;
                    f2 = it->f2_;
                    lcm_min = lcm_pmin;
                    it0 = it;
               }
          }
          b_.erase(it0);
          if (!criterion1(f1, f2) && !criterion2(f1, f2)) {
               auto const spq = s_polynomial(f1, f2);
               assert(!g_.empty());
               P const h = normal_form(spq, g_);
               if (h.deg() != -1) {
                    S g0;
                    for (auto it = g_.cbegin(); it != g_.cend(); ++it)
                         if (h.leading_monomial().divides(
                                  it->leading_monomial()))
                              g0.push_back(*it);
                    r_ = g0;
                    p_ = {h};
                    // g_ := g_ - g0
                    for (auto it = g0.cbegin(); it != g0.cend();
                         ++it) {
                         auto const jt =
                              std::find(g_.cbegin(), g_.cend(), *it);
                         if (jt != g_.cend())
                              g_.erase(jt);
                    }
                    // b_ := b_ - {(f1, f2): f1 in g0 or f2 in g0}
                    for (auto it = b_.begin(); it != b_.end();) {
                         if (std::find(g0.cbegin(), g0.cend(),
                                       it->f1_) != g0.cend() ||
                             std::find(g0.cbegin(), g0.cend(),
                                       it->f2_) != g0.cend())
                              it = b_.erase(it);
                         else
                              ++it;
                    }
                    reduce_all();
                    new_basis();
               }
          }
     }
     // Normalizations seems to be necessary.
     for (auto it = g_.begin(); it != g_.end(); ++it)
          *it *= field->one() / it->leading_coefficient();
}

bool operator==(Buchberger_improved::Pair const& lhs,
                Buchberger_improved::Pair const& rhs) {
     if (lhs.f1_ == rhs.f1_ && lhs.f2_ == rhs.f2_)
          return true;
     if (lhs.f1_ == rhs.f2_ && lhs.f2_ == rhs.f1_)
          return true;
     return false;
}

void Buchberger_improved::reduce_all() {
     while (!r_.empty()) {
          P h = r_.back();
          r_.pop_back();
          {
               S t = g_;
               t.insert(t.end(), p_.cbegin(), p_.cend());
               if (!t.empty())
                    h = normal_form(h, t);
          }

          if (h.deg() != -1) {
               S g0, p0;
               auto const& lmh = h.leading_monomial();
               for (auto it = g_.cbegin(); it != g_.cend(); ++it)
                    if (lmh.divides(it->leading_monomial()))
                         g0.push_back(*it);
               for (auto it = p_.cbegin(); it != p_.cend(); ++it)
                    if (lmh.divides(it->leading_monomial()))
                         p0.push_back(*it);

               // g_ := g_ - g0
               for (auto it = g0.cbegin(); it != g0.cend(); ++it) {
                    auto const jt =
                         std::find(g_.cbegin(), g_.cend(), *it);
                    if (jt != g_.cend())
                         g_.erase(jt);
               }
               // p_ := p_ - p0
               for (auto it = p0.cbegin(); it != p0.cend(); ++it) {
                    auto const jt =
                         std::find(p_.cbegin(), p_.cend(), *it);
                    if (jt != p_.cend())
                         p_.erase(jt);
               }
               // r_ := r_ + g0 + p0
               r_.insert(r_.end(), g0.cbegin(), g0.cend());
               r_.insert(r_.end(), p0.cbegin(), p0.cend());
               // b_ := b_ - {(f1, f2): f1 in g0 or f2 in g0}
               for (auto it = b_.begin(); it != b_.end();) {
                    if (std::find(g0.cbegin(), g0.cend(), it->f1_) !=
                             g0.cend() ||
                        std::find(g0.cbegin(), g0.cend(), it->f2_) !=
                             g0.cend())
                         it = b_.erase(it);
                    else
                         ++it;
               }
               p_.push_back(h);
          }
     }
}

void Buchberger_improved::new_basis() {
     // g_ := g_ + p_
     g_.insert(g_.end(), p_.cbegin(), p_.cend());
     // b_ := b_ + {(g, p): g in G, p in P, g != p}
     for (auto it = g_.cbegin(); it != g_.cend(); ++it)
          for (auto jt = p_.cbegin(); jt != p_.cend(); ++jt)
               if (*it != *jt)
                    b_.push_back(Pair(*it, *jt));
     S H{g_};
     S k;
     while (!H.empty()) {
          P h = H.back();
          H.pop_back();
          S t{g_};
          auto const it = std::find(t.cbegin(), t.cend(), h);
          assert(it != t.cend());
          t.erase(it);
          if (!t.empty())
               h = normal_form(h, t);
          k.push_back(h);
     }
     g_ = k;
}

bool Buchberger_improved::criterion1(P const& f1, P const& f2) {
     for (auto it = g_.cbegin(); it != g_.cend(); ++it) {
          auto const& p = *it;
          if (p == f1 || p == f2)
               continue;
          auto const& lmp = p.leading_monomial();
          auto const& lmf1 = f1.leading_monomial();
          auto const& lmf2 = f2.leading_monomial();
          if (!lmp.divides(lcm(lmf1, lmf2)))
               continue;
          if (std::find(b_.cbegin(), b_.cend(), Pair(f1, p)) ==
              b_.cend())
               continue;
          if (std::find(b_.cbegin(), b_.cend(), Pair(p, f2)) ==
              b_.cend())
               continue;
          return true;
     }
     return false;
}

bool Buchberger_improved::criterion2(P const& f1, P const& f2) {
     auto const& lmf1 = f1.leading_monomial();
     auto const& lmf2 = f2.leading_monomial();
     return lcm(lmf1, lmf2) == lmf1 * lmf2;
}

}  // namespace SHG::ALGEBRA
