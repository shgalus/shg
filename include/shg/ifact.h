/**
 * \file include/shg/ifact.h
 * Integer factorization.
 */

#ifndef SHG_IFACT_H
#define SHG_IFACT_H

#include <limits>
#include <vector>

namespace SHG {

/**
 * \defgroup integer_factorization Integer factorization
 *
 * Integer factorization.
 *
 * \{
 */

/**
 * Trial divisors for integer factorization and primality testing. See
 * \cite knuth-2002b, section 4.5.4, algorithm A.
 */
template <class T>
class Trial_divisors {
public:
     T operator()() { return (this->*next_function_)(); }

private:
     using Next_function = T (Trial_divisors::*)();

     T d_{2};
     bool even_{false};
     Next_function next_function_{&Trial_divisors::initial_next};

     T initial_next() {
          if (d_ == 2) {
               d_ = 3;
               return 2;
          } else if (d_ == 3) {
               d_ = 5;
               return 3;
          } else {
               next_function_ = &Trial_divisors::subsequent_next;
               return 5;
          }
     }

     T subsequent_next() {
          return (even_ = !even_) ? d_ += 2 : d_ += 4;
     }
};

template <class T>
bool is_prime(T const& n) {
     static_assert(std::numeric_limits<T>::is_integer);

     if (n < 2)
          return false;
     if (n == 2)
          return true;
     Trial_divisors<T> g;
     T q, r, d = g();

     for (;;) {
          q = n / d;
          r = n % d;
          if (r == 0) {
               return false;
          } else {
               if (q > d) {
                    d = g();
               } else {
                    return true;
               }
          }
     }
}

/**
 * Prime factorization of integers. Given \f$n \geq 2\f$, returns
 * prime numbers \f$p_1 < p_2 < \ldots < p_k\f$ and positive exponents
 * \f$n_1, n_2, \ldots, n_k\f$ such that \f$n = p_1^{n_1} p_2^{n_2}
 * \ldots p_k^{n_k}\f$. If \f$n < 2\f$, \f$k = 0\f$.
 * \implementation See \cite knuth-2002b, section 4.5.4, algorithm A.
 */
template <class T>
class Integer_factorization {
     static_assert(std::numeric_limits<T>::is_integer);

public:
     struct Factor {
          T p;
          T n;
     };
     void factorize(T n);
     std::vector<Factor> const& repr() const;

private:
     void push(T const& f);
     std::vector<Factor> r_{};
     typename std::vector<Factor>::iterator it_{r_.end()};
};

template <class T>
void Integer_factorization<T>::factorize(T n) {
     r_.clear();
     if (n < 2)
          return;
     Trial_divisors<T> g;
     T q, r, d = g();

     for (;;) {
          q = n / d;
          r = n % d;
          if (r == 0) {
               push(d);
               n = q;
               if (n == 1)
                    return;
          } else {
               if (q > d) {
                    d = g();
               } else {
                    push(n);
                    return;
               }
          }
     }
}

template <class T>
inline std::vector<typename Integer_factorization<T>::Factor> const&
Integer_factorization<T>::repr() const {
     return r_;
}

template <class T>
void Integer_factorization<T>::push(T const& f) {
     if (it_ == r_.end() || f != it_->p) {
          r_.push_back({f, 1});
          it_ = std::prev(r_.end());
     } else
          ++it_->n;
}

/** \} */

}  // namespace SHG

#endif
