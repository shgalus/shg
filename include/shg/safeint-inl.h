/**
 * \file include/shg/safeint-inl.h
 * Implementation of inline functions and templates from safeint.h.
 */

#ifndef SAFEINT_INL_H
#define SAFEINT_INL_H

namespace SHG {

template <typename T>
constexpr bool is_add_safe(T const& x, T const& y)
     requires(Unsigned_integer<T>) {
     return x <= std::numeric_limits<T>::max() - y;
}

template <typename T>
constexpr bool is_sub_safe(T const& x, T const& y)
     requires(Unsigned_integer<T>) {
     return x >= y;
}

template <typename T>
constexpr bool is_mul_safe(T const& x, T const& y)
     requires(Unsigned_integer<T>) {
     return x == 0 || y <= std::numeric_limits<T>::max() / x;
}

template <typename T>
constexpr bool is_div_safe([[maybe_unused]] T const& x, T const& y)
     requires(Unsigned_integer<T>) {
     return y != 0;
}

template <typename T>
constexpr bool is_add_safe(T const& x, T const& y)
     requires(Signed_integer<T>) {
     if (x >= 0)
          return y <= std::numeric_limits<T>::max() - x;
     else
          return y >= std::numeric_limits<T>::min() - x;
}

/**
 * Returns true iff difference is allowed in type T.
 */
template <typename T>
constexpr bool is_sub_safe(T const& x, T const& y)
     requires(Signed_integer<T>) {
     if (x >= 0)
          return y >= 0 || y >= x - std::numeric_limits<T>::max();
     else
          return y <= 0 || x >= y + std::numeric_limits<T>::min();
}

template <typename T>
constexpr bool is_mul_safe(T const& x, T const& y)
     requires(Signed_integer<T>) {
     if (x == 0 || y == 0)
          return true;
     if (x > 0) {
          if (y > 0)
               return x <= std::numeric_limits<T>::max() / y;
          else
               return x <= std::numeric_limits<T>::min() / y;
     } else {
          if (y > 0)
               return y <= std::numeric_limits<T>::min() / x;
          else
               return x != std::numeric_limits<T>::min() &&
                      y != std::numeric_limits<T>::min() &&
                      -x <= std::numeric_limits<T>::max() / -y;
     }
}

template <typename T>
constexpr bool is_div_safe(T const& x, T const& y)
     requires(Signed_integer<T>) {
     if (y == 0)
          return false;
     return x != std::numeric_limits<T>::min() ||
            y != static_cast<T>(-1);
}

}  // namespace SHG

#endif
