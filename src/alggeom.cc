/**
 * \file src/alggeom.cc
 * Algebraic geometry.
 */

#include <shg/alggeom.h>
#include <cctype>
#include <algorithm>
#include <shg/except.h>
#include <shg/utils.h>

namespace SHG::ALGEBRA::ALGGEOM {

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
          ai = Polynomial(f.as(), f.dim());
          ai.order(f.order());
     }
     r = Polynomial(f.as(), f.dim());
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
     auto const one = f.as()->one();
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
     AS const* const as = f[0].as();
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
               q *= as->one() / p.leading_coefficient();
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
          *it *= as->one() / it->leading_coefficient();
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

}  // namespace SHG::ALGEBRA::ALGGEOM
