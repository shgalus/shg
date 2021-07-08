/**
 * \file include/shg/monomial.h
 * The class Monomial.
 */

#ifndef SHG_MONOMIAL_H
#define SHG_MONOMIAL_H

#include <initializer_list>
#include <istream>
#include <ostream>
#include <vector>
#include <shg/subdomain.h>

namespace SHG::ALGEBRA {

/**
 * \addtogroup algebra
 *
 * \{
 */

class Monomial {
public:
     Monomial() = default;
     explicit Monomial(int dim);
     explicit Monomial(std::initializer_list<int> il);
     explicit Monomial(std::vector<int> const& d);
     int dim() const { return exp_.size(); }
     int operator[](int i) const { return exp_[i]; }
     int deg(int i) const { return exp_[i]; }
     int deg() const;
     Monomial& operator*=(Monomial const& y);
     Monomial& operator/=(Monomial const& y);
     bool divides(Monomial const& y) const;
     std::vector<int> const& exp() const { return exp_; }

private:
     void check();
     std::vector<int> exp_{};
};

inline bool operator==(Monomial const& x, Monomial const& y) {
     return x.exp() == y.exp();
}

inline bool operator!=(Monomial const& x, Monomial const& y) {
     return !(x == y);
}

Monomial operator*(Monomial const& x, Monomial const& y);
Monomial operator/(Monomial const& x, Monomial const& y);
Monomial gcd(Monomial const& x, Monomial const& y);
Monomial lcm(Monomial const& x, Monomial const& y);
std::ostream& operator<<(std::ostream& stream, Monomial const& alpha);
std::istream& operator>>(std::istream& stream, Monomial& alpha);

/**
 * Type of lex_cmp(), grlex_cmp(), grevlex_cmp().
 *
 * \note The same as <tt>typedef bool (*Monomial_cmp)(Monomial
 * const& x, Monomial const& y);</tt>.
 */
using Monomial_cmp = bool (*)(Monomial const& x, Monomial const& y);

bool lex_cmp(Monomial const& x, Monomial const& y);
bool grlex_cmp(Monomial const& x, Monomial const& y);
bool grevlex_cmp(Monomial const& x, Monomial const& y);

struct Lex_less {
     bool operator()(Monomial const& x, Monomial const& y) const {
          return lex_cmp(x, y);
     }
};

struct Grlex_less {
     bool operator()(Monomial const& x, Monomial const& y) const {
          return grlex_cmp(x, y);
     }
};

struct Grevlex_less {
     bool operator()(Monomial const& x, Monomial const& y) const {
          return grevlex_cmp(x, y);
     }
};

/**
 * Recursive monomial generator. The number of monomials of the degree
 * \f$k \geq 0 \f$ of \f$n \geq 1\f$ variables is equal to \f$
 * \binom{n - 1 + k}{n - 1} \f$.
 */
template <typename F>
class Recursive_monomial_generator {
public:
     Recursive_monomial_generator(int n, int k, F f);
     void generate();

private:
     void generate(int n, int k, int p);
     int n_;
     int k_;
     std::vector<int> a_{};
     F f_;
};

class Iterative_monomial_generator {
public:
     Iterative_monomial_generator(Positive<int> n,
                                  Nonnegative<int> k);
     bool next();
     const std::vector<int>& get() const { return a_; }

private:
     std::vector<int> a_{};
     std::vector<int> s_{};
     const int n_;
     const int n1_;
     const int n2_;
     const int k_;
};

template <typename F>
Recursive_monomial_generator<F>::Recursive_monomial_generator(int n,
                                                              int k,
                                                              F f)
     : n_(n), k_(k), f_(f) {
     if (n < 1 || k < 0)
          throw std::invalid_argument(__func__);
     a_.resize(n);
}

template <typename F>
void Recursive_monomial_generator<F>::generate() {
     generate(n_, k_, 0);
}

template <typename F>
void Recursive_monomial_generator<F>::generate(int n, int k, int p) {
     if (n == 1) {
          a_[p] = k;
          f_(const_cast<const std::vector<int>&>(a_));
     } else {
          for (int i = 0; i <= k; i++) {
               a_[p] = i;
               generate(n - 1, k - i, p + 1);
          }
     }
}

/** \} */ /* end of group algebra */

}  // namespace SHG::ALGEBRA

#endif
