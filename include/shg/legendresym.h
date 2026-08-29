/**
 * \file include/shg/legendresym.h
 * Legendre symbol.
 */

#ifndef SHG_LEGENDRESYM_H
#define SHG_LEGENDRESYM_H

#include <stdexcept>
#include <shg/ifact.h>
#include <shg/utils.h>

namespace SHG {

/**
 * Returns \c true if and only if \f$a \equiv b \pmod n\f$. If \f$n =
 * 0\f$, the behaviour is undefined.
 */
template <typename T>
inline bool congruent(T const& a, T const& b, T const& n) {
     static_assert(std::numeric_limits<T>::is_integer &&
                        std::numeric_limits<T>::is_signed,
                   "T must be a signed integer type.");

     return (a - b) % n == 0;
}

template <class T>
int legendre_unsafe(T const& a, T const& p) {
     static_assert(std::numeric_limits<T>::is_integer &&
                        std::numeric_limits<T>::is_signed,
                   "T must be a signed integer type.");

     auto const ap = SHG::powmod(a, p * (p - 1) / 2, p);
     if (congruent(ap, -1, p))
          return -1;
     if (congruent(ap, 1, p))
          return 1;
     if (congruent(ap, 0, p))
          return 0;
     throw std::runtime_error(__func__);
}

/**
 * Returns Legendre symbol \f$\left(\frac{a}{p}\right) = \left\{
 * \begin{array}{cl} 0 & \mbox{if $a \equiv 0 \pmod p$}, \\ 1 &
 * \mbox{if $a \not\equiv 0 \pmod p$ and a is a quadratic residue
 * modulo $p$,} \\ -1 & \mbox{if $a \not\equiv 0 \pmod p$ ane q is is
 * a quadratic nonresidue modulo $p$} \end{array} \right. \f$
 *
 * If the congruence \f$x^2 \equiv a \pmod p\f$ has a solution,
 * \f$a\f$ is called a quadratic residue modulo \f$p\f$, otherwise
 * \f$a\f$ is called a quadratic nonresidue modulo \f$p\f$.
 *
 *  See \cite kostrikin-2012c, page 157.
 *
 * \throws std::invalid_argument if \f$p\f$ is not a prime greater
 * than 2
 */
template <class T>
int legendre(T const& a, T const& p) {
     static_assert(std::numeric_limits<T>::is_integer &&
                        std::numeric_limits<T>::is_signed,
                   "T must be a signed integer type.");

     if (p <= 2 || !SHG::is_prime(p))
          throw std::invalid_argument("not a prime in legendre");
     return legendre_unsafe(a, p);
}

}  // namespace SHG

#endif
