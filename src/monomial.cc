/**
 * \file src/monomial.cc
 * The class Monomial.
 */

#include <shg/monomial.h>
#include <algorithm>
#include <functional>
#include <numeric>
#include <shg/except.h>

namespace SHG::ALGEBRA {

Monomial::Monomial(int dim) : exp_() {
     if (dim < 0)
          SHG_THROW(std::invalid_argument, __func__);
     exp_.resize(dim, 0);
}

Monomial::Monomial(std::initializer_list<int> il) : exp_(il) {
     check();
}

Monomial::Monomial(std::vector<int> const& d) : exp_(d) {
     check();
}

int Monomial::deg() const {
     return std::accumulate(exp_.cbegin(), exp_.cend(), 0);
}

Monomial& Monomial::operator*=(Monomial const& y) {
     if (y.exp_.size() != exp_.size())
          SHG_THROW(std::invalid_argument, __func__);
     std::transform(y.exp_.begin(), y.exp_.end(), exp_.begin(),
                    exp_.begin(), std::plus<int>());
     return *this;
}

Monomial& Monomial::operator/=(Monomial const& y) {
     if (y.exp_.size() != exp_.size())
          SHG_THROW(std::invalid_argument, __func__);
     Monomial z(*this);
     std::transform(z.exp_.begin(), z.exp_.end(), y.exp_.begin(),
                    z.exp_.begin(), std::minus<int>());
     if (std::any_of(z.exp_.cbegin(), z.exp_.cend(),
                     [](int i) { return i < 0; }))
          SHG_THROW(std::invalid_argument, __func__);
     exp_ = z.exp_;
     return *this;
}

bool Monomial::divides(Monomial const& y) const {
     if (exp_.size() == 0 || y.exp_.size() != exp_.size())
          SHG_THROW(std::invalid_argument, __func__);
     for (std::vector<int>::size_type i = 0; i < exp_.size(); i++)
          if (exp_[i] > y.exp_[i])
               return false;
     return true;
}

void Monomial::check() {
     if (std::any_of(exp_.cbegin(), exp_.cend(),
                     [](int i) { return i < 0; }))
          SHG_THROW(std::invalid_argument, __func__);
}

Monomial operator*(Monomial const& x, Monomial const& y) {
     Monomial r(x);
     return r *= y;
}

Monomial operator/(Monomial const& x, Monomial const& y) {
     Monomial r(x);
     return r /= y;
}

Monomial gcd(Monomial const& x, Monomial const& y) {
     int const dim = x.dim();
     if (dim == 0 || y.dim() != dim)
          SHG_THROW(std::invalid_argument, __func__);
     std::vector<int> z(dim);
     for (int i = 0; i < dim; i++)
          z[i] = std::min(x[i], y[i]);
     return Monomial(z);
}

Monomial lcm(Monomial const& x, Monomial const& y) {
     int const dim = x.dim();
     if (dim == 0 || y.dim() != dim)
          SHG_THROW(std::invalid_argument, __func__);
     std::vector<int> z(dim);
     for (int i = 0; i < dim; i++)
          z[i] = std::max(x[i], y[i]);
     return Monomial(z);
}

std::ostream& operator<<(std::ostream& stream,
                         Monomial const& alpha) {
     stream.width(0);
     stream << alpha.dim();
     for (int i = 0; i < alpha.dim(); i++)
          stream << ' ' << alpha[i];
     return stream;
}

std::istream& operator>>(std::istream& stream, Monomial& alpha) {
     bool fail = true;
     int n;

     if ((stream >> n) && n >= 0) {
          std::vector<int> a(n);
          int i;
          for (i = 0; i < n && (stream >> a[i]) && a[i] >= 0; i++)
               ;
          if (i >= n) {
               alpha = Monomial(a);
               fail = false;
          }
     }
     if (fail)
          stream.setstate(std::ios::failbit);
     return stream;
}

bool lex_cmp(Monomial const& x, Monomial const& y) {
     auto const a = std::mismatch(x.exp().cbegin(), x.exp().cend(),
                                  y.exp().cbegin(), y.exp().cend());
     if (a.second != y.exp().cend()) {
          if (a.first != x.exp().cend())
               return *a.first < *a.second;
          else
               return true;
     }
     return false;
}

bool grlex_cmp(Monomial const& x, Monomial const& y) {
     int const xtdeg = x.deg();
     int const ytdeg = y.deg();
     if (xtdeg != ytdeg)
          return xtdeg < ytdeg;
     return lex_cmp(x, y);
}

bool grevlex_cmp(Monomial const& x, Monomial const& y) {
     int const xtdeg = x.deg();
     int const ytdeg = y.deg();
     if (xtdeg != ytdeg)
          return xtdeg < ytdeg;
     if (x.exp().size() != y.exp().size())
          return x.exp().size() > y.exp().size();
     auto const a = std::mismatch(x.exp().crbegin(), x.exp().crend(),
                                  y.exp().crbegin());
     if (a.first != x.exp().crend())
          return *a.first > *a.second;
     return false;
}

Iterative_monomial_generator::Iterative_monomial_generator(
     Positive<int> n, Nonnegative<int> k)
     : a_(n, 0), s_(n, 0), n_(n), n1_(n - 1), n2_(n - 2), k_(k) {
     a_[n1_] = k;
     // s_[n1_] is not used
}

bool Iterative_monomial_generator::next() {
     int l = n_ - 2;
     for (; l >= 0 && s_[l] == k_; l--) {
          int const al = a_[l];
          a_[l] = 0;
          for (int i = l; i < n1_; i++)
               s_[i] -= al;
          a_[l] = 0;
     }
     if (l < 0)
          return false;
     a_[l]++;
     for (int i = l; i < n1_; i++)
          s_[i]++;
     a_[n1_] = k_ - s_[n2_];
     return true;
}

}  // namespace SHG::ALGEBRA
