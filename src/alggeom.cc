/**
 * \file src/alggeom.cc
 * Algebraic geometry.
 */

#include <shg/alggeom.h>
#include <functional>
#include <iterator>

namespace SHG::ALGGEOM {

bool is_prime(int n) {
     if (n <= 3)
          return n > 1;
     if (n % 2 == 0 || n % 3 == 0)
          return false;
     for (int i = 5; i * i <= n; i += 6)
          if (n % i == 0 || n % (i + 2) == 0)
               return false;
     return true;
}

Invalid_F_p::Invalid_F_p()
     : std::runtime_error("p is not prime for F_p field.") {}

Negative_degree::Negative_degree()
     : std::runtime_error("Negative degree in monomial.") {}

Monomial::Monomial(std::initializer_list<int> il) : d_(il) {
     normalize();
}

Monomial::Monomial(const std::vector<int>& d) : d_(d) {
     normalize();
}

int Monomial::dim() const {
     return d_.size();
}

int Monomial::degree(int i) const {
     return d_[i];
}

int Monomial::operator[](int i) const {
     return d_[i];
}

int Monomial::total_degree() const {
     return std::accumulate(d_.cbegin(), d_.cend(), 0);
}

Monomial& Monomial::operator+=(const Monomial& y) {
     if (y.d_.size() > d_.size())
          d_.resize(y.d_.size(), 0);
     std::transform(y.d_.begin(), y.d_.end(), d_.begin(), d_.begin(),
                    std::plus<int>());
     return *this;
}

void Monomial::normalize() {
     if (std::any_of(d_.cbegin(), d_.cend(),
                     [](int i) { return i < 0; }))
          throw Negative_degree();
     const auto it = std::find_if(d_.rbegin(), d_.rend(),
                                  [](int i) { return i != 0; });
     // See \cite josuttis-2012, section 9.4.1, \cite stroustrup-2013,
     // section 33.2.1.
     d_.erase(it.base(), d_.end());
}

Monomial operator+(const Monomial& x, const Monomial& y) {
     Monomial r(x);
     return r += y;
}

bool operator==(const Monomial& x, const Monomial& y) {
     return x.d_ == y.d_;
}

bool operator!=(const Monomial& x, const Monomial& y) {
     return !(x == y);
}

bool operator<(const Monomial& x, const Monomial& y) {
     return x.d_ < y.d_;
}

bool Lex_less::operator()(const Monomial& x,
                          const Monomial& y) const {
     const auto a = std::mismatch(x.d_.cbegin(), x.d_.cend(),
                                  y.d_.cbegin(), y.d_.cend());
     if (a.second != y.d_.cend()) {
          if (a.first != x.d_.cend())
               return *a.first < *a.second;
          else
               return true;
     }
     return false;
}

bool Grlex_less::operator()(const Monomial& x,
                            const Monomial& y) const {
     const int xtdeg = x.total_degree();
     const int ytdeg = y.total_degree();
     if (xtdeg != ytdeg)
          return xtdeg < ytdeg;
     return Lex_less().operator()(x, y);
}

bool Grevlex_less::operator()(const Monomial& x,
                              const Monomial& y) const {
     const int xtdeg = x.total_degree();
     const int ytdeg = y.total_degree();
     if (xtdeg != ytdeg)
          return xtdeg < ytdeg;
     if (x.d_.size() != y.d_.size())
          return x.d_.size() > y.d_.size();
     const auto a =
          std::mismatch(x.d_.crbegin(), x.d_.crend(), y.d_.crbegin());
     if (a.first != x.d_.crend())
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
          const int al = a_[l];
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

}  // namespace SHG::ALGGEOM
