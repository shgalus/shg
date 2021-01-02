/**
 * \file include/shg/alggeom.h
 * Algebraic geometry.
 */

#ifndef SHG_ALGGEOM_H
#define SHG_ALGGEOM_H

#include <algorithm>
#include <cstdlib>
#include <initializer_list>
#include <map>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <boost/rational.hpp>
#include <shg/subdomain.h>

/**
 * Algebraic geometry.
 */
namespace SHG::ALGGEOM {

/**
 * Returns true if and only if \c n is a prime number.
 *
 * \implementation Each integer number can be expressed as \f$6k +
 * i\f$ for certain integer \f$k\f$ and \f$i \in \{-1, 0, 1, 2, 3, 4\}
 * \f$. \f$6k + 0, 6k + 2, 6k + 3, 6k + 4\f$ are not prime and \f$6k -
 * 1, 6k + 1\f$ may be prime. The algorith checks if \f$n\f$ is
 * divisible by the numbers of the form \f$6k - 1, 6k + 1\f$.
 */
bool is_prime(int n);

/**
 * Extended Euclidean algorithm. For two given nonnegative integer
 * numbers \f$u\f$ and \f$v\f$, the algorithm determines three numbers
 * \f$u_1, u_2, u_3\f$ such that \f$uu_1 + vu_2 = u_3 =
 * \mathrm{gcd}(u, v)\f$.
 *
 * \implementation See \cite knuth-2002b, section 4.5.2, page 366-367.
 */
template <class T>
class Extended_gcd {
     static_assert(std::is_integral<T>::value &&
                        std::is_signed<T>::value,
                   "T must be a signed integer type.");

public:
     Extended_gcd() = default;
     Extended_gcd(const T& u, const T& v);
     void calculate(const T& u, const T& v);
     T u1{}, u2{}, u3{};

private:
     T v1{}, v2{}, v3{};
     T t{};
     T q{};
};

/**
 * Calculates \f$x^n\f$ (right-to-left binary method for
 * exponentiation).
 * \throws std::invalid_argument if \f$n < 0\f$.
 * \implementation See \cite knuth-2002b, section 4.6.3, page 497.
 */
template <typename T>
T pow(const T& x, int n);

/**
 * Commutative ring (with unity) \f$Z_n\f$ of integer numbers modulo
 * \f$n\f$.
 */
template <int N>
class Z_n {
public:
     static_assert(N >= 2, "N must be greater than or equal 2.");
     Z_n() = default;
     explicit Z_n(int z);
     Z_n& operator=(int z);
     Z_n& operator++();
     Z_n operator++(int);
     Z_n& operator--();
     Z_n operator--(int);
     Z_n& operator+=(const Z_n& z);
     Z_n& operator-=(const Z_n& z);
     Z_n& operator*=(const Z_n& z);

     template <int X>
     friend bool operator==(const Z_n<X>& x, const Z_n<X>& y);

     template <int X>
     friend Z_n<X> operator*(int x, const Z_n<X>& y);

     template <int X>
     friend Z_n<X> operator*(const Z_n<X>& x, int y);

     template <int X>
     friend Z_n<X> operator-(const Z_n<X>& z);

     template <int X>
     friend std::istream& operator>>(std::istream& stream, Z_n<X>& z);

     template <int X>
     friend std::ostream& operator<<(std::ostream& stream,
                                     const Z_n<X>& z);
     static constexpr int N_ = N;

protected:
     int z_{};
};

template <int N>
Z_n<N> operator+(const Z_n<N>& x, const Z_n<N>& y);

template <int N>
Z_n<N> operator-(const Z_n<N>& x, const Z_n<N>& y);

template <int N>
Z_n<N> operator*(const Z_n<N>& x, const Z_n<N>& y);

template <int N>
Z_n<N> operator*(int x, const Z_n<N>& y);

template <int N>
Z_n<N> operator*(const Z_n<N>& x, int y);

template <int N>
Z_n<N> operator+(const Z_n<N>& z);

template <int N>
Z_n<N> operator-(const Z_n<N>& z);

template <int N>
bool operator==(const Z_n<N>& x, const Z_n<N>& y);

template <int N>
bool operator!=(const Z_n<N>& x, const Z_n<N>& y);

template <int N>
std::istream& operator>>(std::istream& stream, Z_n<N>& z);

template <int N>
std::ostream& operator<<(std::ostream& stream, const Z_n<N>& z);

/**
 * Exception class for invalid field \f$F_p\f$.
 */
class Invalid_F_p : public std::runtime_error {
public:
     Invalid_F_p();
};

/**
 * Field \f$F_p\f$ of integer numbers modulo \f$p\f$.
 */
template <int P>
class F_p : public Z_n<P> {
public:
     using Z_n<P>::operator=;

     F_p();
     F_p(int n);
     F_p(const Z_n<P>& z);
     F_p inv() const;
     F_p& operator/=(const F_p& z);
};

template <int P>
F_p<P> operator/(const F_p<P>& x, const F_p<P>& y);

/**
 * Field \f$\rationaln\f$ of rational numbers.
 */
template <typename I>
using Rational = boost::rational<I>;

/**
 * Exception class for invalid monomials.
 */
class Negative_degree : public std::runtime_error {
public:
     Negative_degree();
};

class Monomial {
public:
     Monomial() = default;
     Monomial(std::initializer_list<int> il);
     Monomial(const std::vector<int>& d);
     int dim() const;
     int degree(int i) const;
     int operator[](int i) const;
     int total_degree() const;
     Monomial& operator+=(const Monomial& y);

     friend bool operator==(const Monomial& x, const Monomial& y);
     friend bool operator<(const Monomial& x, const Monomial& y);
     friend struct Lex_less;
     friend struct Glex_less;
     friend struct Grevlex_less;

private:
     void normalize();
     std::vector<int> d_{};
};

Monomial operator+(const Monomial& x, const Monomial& y);
bool operator==(const Monomial& x, const Monomial& y);
bool operator!=(const Monomial& x, const Monomial& y);
bool operator<(const Monomial& x, const Monomial& y);

struct Lex_less {
     bool operator()(const Monomial& x, const Monomial& y) const;
};

struct Grlex_less {
     bool operator()(const Monomial& x, const Monomial& y) const;
};

struct Grevlex_less {
     bool operator()(const Monomial& x, const Monomial& y) const;
};

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
     inline const std::vector<int>& get() const;

private:
     std::vector<int> a_{};
     std::vector<int> s_{};
     const int n_;
     const int n1_;
     const int n2_;
     const int k_;
};

/**
 * A term of a polynomial of \f$n\f$ variables over the field \f$k\f$.
 * \f$ax^{\alpha}, \; a \in k, \; a \neq 0, \; \alpha = \{ \alpha_1,
 * \ldots, \alpha_n \}\f$.
 */
template <typename K>
class Term {
public:
     inline Term(const K& a, const Monomial& m);
     inline const K& a() const;
     inline const Monomial& m() const;

private:
     const K a_;
     const Monomial m_;
};

/**
 * A point in \f$n\f$-dimensional affine space over the field K.
 */
template <typename K>
using Point = std::vector<K>;

/**
 * Polynomial ring \f$k[x_1, \ldots, x_n]\f$ of \f$n \geq 1\f$
 * variables over the field \f$k\f$.
 *
 * \implementation The zero polynomial is represented by the empty map
 * `t_`. The one polynomial has exactly one term \f$1x_1^0 \ldots
 * x_n^0\f$.
 */
template <typename K, typename Compare = std::less<Monomial>>
class Poly {
public:
     typedef K Field;
     typedef Compare Comp;
     typedef std::map<Monomial, K, Compare> Terms;

     /**
      * Constructs zero polynomial.
      */
     Poly(int dim);
     Poly(int dim, const K& a);
     Poly& operator=(const K& a);
     Poly& operator+=(const Term<K>& t);
     Poly& operator-=(const Term<K>& t);
     Poly& operator+=(const Poly<K, Compare>& p);
     Poly& operator-=(const Poly<K, Compare>& p);
     Poly& operator*=(const Poly<K, Compare>& p);

     int dim() const;
     int degree() const;
     const Terms& terms() const;
     K operator()(const Point<K>& x) const;

     template <typename X, typename Y>
     friend Poly<X, Y> operator-(const Poly<X, Y>& z);

private:
     int dim_{};
     Terms t_{};
};

template <typename K, typename Compare>
Poly<K, Compare> operator+(const Poly<K, Compare>& x,
                           const Poly<K, Compare>& y);

template <typename K, typename Compare>
Poly<K, Compare> operator-(const Poly<K, Compare>& x,
                           const Poly<K, Compare>& y);

template <typename K, typename Compare>
Poly<K, Compare> operator*(const Poly<K, Compare>& x,
                           const Poly<K, Compare>& y);

template <typename K, typename Compare>
Poly<K, Compare> operator+(const Poly<K, Compare>& z);

template <typename K, typename Compare>
Poly<K, Compare> operator-(const Poly<K, Compare>& z);

template <typename K, typename Compare>
bool operator==(const Poly<K, Compare>& x, const Poly<K, Compare>& y);

template <typename K, typename Compare>
std::ostream& operator<<(std::ostream& stream,
                         const Poly<K, Compare>& p);

template <class T>
Extended_gcd<T>::Extended_gcd(const T& u, const T& v) {
     calculate(u, v);
}

// clang-format off

template <class T>
void Extended_gcd<T>::calculate(const T& u, const T& v) {
     if (u < 0 || v < 0)
          throw std::invalid_argument(__func__);
     u1 = 1; u2 = 0; u3 = u;
     v1 = 0; v2 = 1; v3 = v;
     while (v3 != 0) {
          q = u3 / v3;
          t = u1 - v1 * q; u1 = v1; v1 = t;
          t = u2 - v2 * q; u2 = v2; v2 = t;
          t = u3 - v3 * q; u3 = v3; v3 = t;
     }
}

// clang-format on

template <typename T>
T pow(const T& x, int n) {
     if (n < 0)
          throw std::invalid_argument(__func__);
     T y = T(1);
     T z = x;
     std::div_t dv;
     for (;;) {
          dv = std::div(n, 2);
          if (dv.rem != 0)
               y *= z;
          n = dv.quot;
          if (n == 0)
               return y;
          z *= z;
     }
}

template <int N>
Z_n<N>::Z_n(int z) : z_(z) {
     if (z < 0)
          throw std::invalid_argument(__func__);
     z_ %= N;
}

template <int N>
Z_n<N>& Z_n<N>::operator=(int z) {
     if (z < 0)
          throw std::invalid_argument(__func__);
     z_ = z % N;
     return *this;
}

template <int N>
Z_n<N>& Z_n<N>::operator++() {
     if (++z_ == N)
          z_ = 0;
     return *this;
}

template <int N>
Z_n<N> Z_n<N>::operator++(int) {
     Z_n<N> r(*this);
     operator++();
     return r;
}

template <int N>
Z_n<N>& Z_n<N>::operator--() {
     if (z_ == 0)
          z_ = N;
     --z_;
     return *this;
}

template <int N>
Z_n<N> Z_n<N>::operator--(int) {
     Z_n<N> r(*this);
     operator--();
     return r;
}

template <int N>
Z_n<N>& Z_n<N>::operator+=(const Z_n<N>& z) {
     z_ = (z_ + z.z_) % N;
     return *this;
}

template <int N>
Z_n<N>& Z_n<N>::operator-=(const Z_n<N>& z) {
     if (z.z_ != 0)
          z_ = (z_ + (N - z.z_)) % N;
     return *this;
}

template <int N>
Z_n<N>& Z_n<N>::operator*=(const Z_n<N>& z) {
     z_ = (z_ * z.z_) % N;
     return *this;
}

template <int N>
Z_n<N> operator+(const Z_n<N>& x, const Z_n<N>& y) {
     Z_n r(x);
     return r += y;
}

template <int N>
Z_n<N> operator-(const Z_n<N>& x, const Z_n<N>& y) {
     Z_n r(x);
     return r -= y;
}

template <int N>
Z_n<N> operator*(const Z_n<N>& x, const Z_n<N>& y) {
     Z_n r(x);
     return r *= y;
}

template <int N>
Z_n<N> operator*(int x, const Z_n<N>& y) {
     Z_n r(y);
     if (x < 0) {
          r.z_ = (r.z_ * (-x)) % N;
          r = -r;
     } else
          r.z_ = (r.z_ * x) % N;
     return r;
}

template <int N>
Z_n<N> operator*(const Z_n<N>& x, int y) {
     return y * x;
}

template <int N>
Z_n<N> operator+(const Z_n<N>& z) {
     return z;
}

template <int N>
Z_n<N> operator-(const Z_n<N>& z) {
     return z.z_ == 0 ? z : Z_n<N>(N - z.z_);
}

template <int N>
bool operator==(const Z_n<N>& x, const Z_n<N>& y) {
     return x.z_ == y.z_;
}

template <int N>
bool operator!=(const Z_n<N>& x, const Z_n<N>& y) {
     return !(x == y);
}

template <int N>
std::istream& operator>>(std::istream& stream, Z_n<N>& z) {
     bool fail = true;
     int r;
     if (stream >> r)
          if (r >= 0 && r < N) {
               z = Z_n<N>(r);
               fail = false;
          }
     if (fail)
          stream.setstate(std::ios::failbit);
     return stream;
}

template <int N>
std::ostream& operator<<(std::ostream& stream, const Z_n<N>& z) {
     stream.width(0);
     stream << z.z_;
     return stream;
}

template <int P>
F_p<P>::F_p() : Z_n<P>() {
     if (!is_prime(Z_n<P>::N_))
          throw Invalid_F_p();
}

template <int P>
F_p<P>::F_p(int n) : Z_n<P>(n) {
     if (!is_prime(Z_n<P>::N_))
          throw Invalid_F_p();
}

template <int P>
F_p<P>::F_p(const Z_n<P>& z) : Z_n<P>(z) {
     if (!is_prime(Z_n<P>::N_))
          throw Invalid_F_p();
}

template <int P>
F_p<P> F_p<P>::inv() const {
     if (Z_n<P>::z_ == 0)
          throw std::invalid_argument(__func__);
     Extended_gcd gcd(Z_n<P>::z_, Z_n<P>::N_);
     int u1 = gcd.u1;
     while (u1 < 0)  // if instead of while is enough here!
          u1 += Z_n<P>::N_;
     return F_p<P>(u1);
}

template <int P>
F_p<P>& F_p<P>::operator/=(const F_p<P>& z) {
     F_p tmp = z.inv();
     *this *= tmp;
     return *this;
}

template <int P>
F_p<P> operator/(const F_p<P>& x, const F_p<P>& y) {
     F_p r(x);
     return r /= y;
}

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

const std::vector<int>& Iterative_monomial_generator::get() const {
     return a_;
}

template <typename K>
Term<K>::Term(const K& a, const Monomial& m) : a_(a), m_(m) {}

template <typename K>
const K& Term<K>::a() const {
     return a_;
}

template <typename K>
const Monomial& Term<K>::m() const {
     return m_;
}

template <typename K, typename Compare>
Poly<K, Compare>::Poly(int dim) : dim_(dim) {
     if (dim < 1)
          throw std::invalid_argument(__func__);
}

template <typename K, typename Compare>
Poly<K, Compare>::Poly(int dim, const K& a) : Poly(dim) {
     if (dim < 1)
          throw std::invalid_argument(__func__);
     if (a != K(0))
          t_[Monomial()] = a;
}

template <typename K, typename Compare>
Poly<K, Compare>& Poly<K, Compare>::operator=(const K& a) {
     t_.clear();
     if (a != K(0))
          t_[Monomial()] = a;
     return *this;
}

template <typename K, typename Compare>
Poly<K, Compare>& Poly<K, Compare>::operator+=(const Term<K>& t) {
     if (t.m().dim() > dim_)
          throw std::invalid_argument(__func__);
     auto it = t_.find(t.m());
     if (it == t_.end()) {
          if (t.a() != K(0))
               t_[t.m()] = t.a();
     } else {
          const K a = it->second + t.a();
          if (a == K(0))
               t_.erase(it);
          else
               it->second = a;
     }
     return *this;
}

template <typename K, typename Compare>
Poly<K, Compare>& Poly<K, Compare>::operator-=(const Term<K>& t) {
     if (t.m().dim() > dim_)
          throw std::invalid_argument(__func__);
     auto it = t_.find(t.m());
     if (it == t_.end()) {
          if (t.a() != K(0))
               t_[t.m()] = -t.a();
     } else {
          const K a = it->second - t.a();
          if (a == K(0))
               t_.erase(it);
          else
               it->second = a;
     }
     return *this;
}

template <typename K, typename Compare>
Poly<K, Compare>& Poly<K, Compare>::operator+=(
     const Poly<K, Compare>& p) {
     if (dim_ != p.dim_)
          throw std::invalid_argument(__func__);
     for (auto const& [m, a] : p.t_)
          operator+=(Term<K>(a, m));
     return *this;
}

template <typename K, typename Compare>
Poly<K, Compare>& Poly<K, Compare>::operator-=(
     const Poly<K, Compare>& p) {
     if (dim_ != p.dim_)
          throw std::invalid_argument(__func__);
     for (auto const& [m, a] : p.t_)
          operator-=(Term<K>(a, m));
     return *this;
}

template <typename K, typename Compare>
Poly<K, Compare>& Poly<K, Compare>::operator*=(
     const Poly<K, Compare>& p) {
     if (dim_ != p.dim_)
          throw std::invalid_argument(__func__);
     Poly<K, Compare> q(dim_);
     for (auto const& [m1, a1] : t_)
          for (auto const& [m2, a2] : p.t_)
               q += Term<K>(a1 * a2, m1 + m2);
     *this = q;
     return *this;
}

template <typename K, typename Compare>
int Poly<K, Compare>::dim() const {
     return dim_;
}

template <typename K, typename Compare>
int Poly<K, Compare>::degree() const {
     int d = -1;
     for (const auto& [m, a] : t_) {
          const int td = m.total_degree();
          if (td > d)
               d = td;
     }
     return d;
}

template <typename K, typename Compare>
const typename Poly<K, Compare>::Terms& Poly<K, Compare>::terms()
     const {
     return t_;
}

template <typename K, typename Compare>
K Poly<K, Compare>::operator()(const Point<K>& x) const {
     if (static_cast<typename Point<K>::size_type>(dim_) != x.size())
          throw std::invalid_argument(__func__);
     K y(0);
     for (auto const& [m, a] : t_) {
          K t = a;
          for (int i = 0; i < m.dim(); i++)
               t *= pow(x[i], m[i]);
          y += t;
     }
     return y;
}

template <typename K, typename Compare>
Poly<K, Compare> operator+(const Poly<K, Compare>& x,
                           const Poly<K, Compare>& y) {
     Poly r(x);
     return r += y;
}

template <typename K, typename Compare>
Poly<K, Compare> operator-(const Poly<K, Compare>& x,
                           const Poly<K, Compare>& y) {
     Poly r(x);
     return r -= y;
}

template <typename K, typename Compare>
Poly<K, Compare> operator*(const Poly<K, Compare>& x,
                           const Poly<K, Compare>& y) {
     Poly r(x);
     return r *= y;
}

template <typename K, typename Compare>
Poly<K, Compare> operator+(const Poly<K, Compare>& z) {
     return z;
}

template <typename K, typename Compare>
Poly<K, Compare> operator-(const Poly<K, Compare>& z) {
     Poly t(z);
     for (auto& [m, a] : t.t_)
          a = -a;
     return t;
}

template <typename K, typename Compare>
bool operator==(const Poly<K, Compare>& x,
                const Poly<K, Compare>& y) {
     return x.dim() == y.dim() && x.terms() == y.terms();
}

template <typename K, typename Compare>
std::ostream& operator<<(std::ostream& stream,
                         const Poly<K, Compare>& p) {
     auto t = p.terms();
     if (t.empty())
          return stream << "0";
     for (auto it = t.cbegin(); it != t.cend(); ++it) {
          if (it != t.cbegin())
               stream << " + ";
          stream << it->second;
          for (int i = 0; i < p.dim(); i++)
               stream << " x" << i + 1 << "^" << it->first[i];
     }
     return stream;
}

}  // namespace SHG::ALGGEOM

#endif
