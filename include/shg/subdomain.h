/**
 * \file include/shg/subdomain.h
 * Subdomain and related classes.
 */

#ifndef SHG_SUBDOMAIN_H
#define SHG_SUBDOMAIN_H

#include <stdexcept>
#include <string>
#include <type_traits>
#include <shg/utils.h>

namespace SHG {

/**
 * \defgroup subdomain_and_related Subdomain and related classes
 *
 * The class %Subdomain and related classes.
 *
 * These classes are taken from \cite barton-nackman-1994, paragraph
 * 16.3.2, pages 483-484.
 *
 * \{
 */

template <typename Domain>
struct Subdomain_error : public std::domain_error {
     Subdomain_error(const Domain& x, const std::string& description)
          : std::domain_error(std::to_string(x) + " " + description) {
     }
};

template <typename Domain, typename Predicate>
struct Subdomain {
     Subdomain(const Domain& x) : value_(x) {
          if (!Predicate::is_satisfied(x))
               throw Subdomain_error<Domain>(
                    x, Predicate::description());
     }
     operator Domain() const { return value_; }

private:
     Domain value_;
};

template <typename T>
struct Positive_predicate {
     static bool is_satisfied(const T& x) { return x > 0; }
     static std::string description() { return "is not positive"; }
};

template <typename T>
struct Positive : public Subdomain<T, Positive_predicate<T>> {
     Positive(const T& x) : Subdomain<T, Positive_predicate<T>>(x) {}
};

template <typename T>
struct Nonnegative_predicate {
     static_assert(std::is_arithmetic<T>::value,
                   "T must be an arithmetic type.");
     static bool is_satisfied(const T& x) { return x >= 0; }
     static std::string description() { return "is not nonnegative"; }
};

template <typename T>
struct Nonnegative : public Subdomain<T, Nonnegative_predicate<T>> {
     Nonnegative(const T& x)
          : Subdomain<T, Nonnegative_predicate<T>>(x) {}
};

template <typename T>
struct Prime_predicate {
     static_assert(std::is_integral<T>::value &&
                        std::is_signed<T>::value,
                   "T must be a signed integer type.");
     static bool is_satisfied(const T& x) { return is_prime(x); }
     static std::string description() { return "is not prime"; }
};

template <typename T>
struct Prime : public Subdomain<T, Prime_predicate<T>> {
     Prime(const T& x) : Subdomain<T, Prime_predicate<T>>(x) {}
};

/** \} */ /* end of group subdomain_and_related */

}  // namespace SHG

#endif
