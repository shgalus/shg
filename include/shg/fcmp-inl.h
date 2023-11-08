/**
 * \file include/shg/fcmp-inl.h
 * Implementation of inline functions and templates from fcmp.h.
 */

#ifndef SHG_FCMP_INL_H
#define SHG_FCMP_INL_H

namespace SHG {

template <typename T>
int facmp(T u, T v, T eps) {
     static_assert(std::is_floating_point<T>::value,
                   "floating-point type required");
     if (!(eps >= 0.0 && std::isfinite(u) && std::isfinite(v)))
          throw std::invalid_argument("invalid argument in facmp");
     const T d = v - u;
     return d > eps ? -1 : (-d > eps ? 1 : 0);
}

template <typename T>
int frcmp(T u, T v, T eps) {
     static_assert(std::is_floating_point<T>::value,
                   "floating-point type required");
     if (!(eps >= 0.0 && eps < 1.0) ||
         !(std::isfinite(u) && std::isfinite(v)))
          throw std::invalid_argument("invalid argument in frcmp");
     const T d = v - u;
     const T max = eps * std::max(std::abs(u), std::abs(v));
     return d > max ? -1 : (-d > max ? 1 : 0);
}

template <typename T>
inline bool faeq(T u, T v, T eps) {
     return facmp(u, v, eps) == 0;
}

template <typename T>
inline bool fane(T u, T v, T eps) {
     return facmp(u, v, eps) != 0;
}

template <typename T>
inline bool falt(T u, T v, T eps) {
     return facmp(u, v, eps) < 0;
}

template <typename T>
inline bool fale(T u, T v, T eps) {
     return facmp(u, v, eps) <= 0;
}

template <typename T>
inline bool fagt(T u, T v, T eps) {
     return facmp(u, v, eps) > 0;
}

template <typename T>
inline bool fage(T u, T v, T eps) {
     return facmp(u, v, eps) >= 0;
}

template <typename T>
inline bool freq(T u, T v, T eps) {
     return frcmp(u, v, eps) == 0;
}

template <typename T>
inline bool frne(T u, T v, T eps) {
     return frcmp(u, v, eps) != 0;
}

template <typename T>
inline bool frlt(T u, T v, T eps) {
     return frcmp(u, v, eps) < 0;
}

template <typename T>
inline bool frle(T u, T v, T eps) {
     return frcmp(u, v, eps) <= 0;
}

template <typename T>
inline bool frgt(T u, T v, T eps) {
     return frcmp(u, v, eps) > 0;
}

template <typename T>
inline bool frge(T u, T v, T eps) {
     return frcmp(u, v, eps) >= 0;
}

}  // namespace SHG

#endif
