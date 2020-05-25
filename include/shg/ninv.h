/* ninv.h: number of inversions in a sequence */

/**
 * \file include/shg/ninv.h
 * Number of inversions in a sequence.
 * Created on  3 March 2016.
 */

#ifndef SHG_NINV_H
#define SHG_NINV_H

#include <cstddef>
#include <algorithm>
#include <memory>

namespace SHG {

/**
 * \addtogroup mathematical_statistics
 *
 * \{
 */

/**
 * Returns the number of inversions in a sequence. The elements
 * \f$a_i\f$ and \f$a_j\f$ of the sequence \f$a_0, a_1, \ldots, a_{n -
 * 1}\f$ make an inversion if \f$j > i\f$ and \f$a_j < a_i\f$. The
 * algorithm is based on merge-sort algorithm and is \f$O(n \log
 * n)\f$. See \cite cormen-etal-2009, p. 42, \cite knuth-2002c,
 * section 5.1.1.
 *
 * \tparam T A type with defined operator <.
 *
 * \param[in,out] a The sequence \f$a_0, a_1, \ldots, a_{n - 1}\f$,
 * ordered increasingly on exit.
 *
 * \param[out] b Work array used for sorting, must be of size at least
 * n.
 *
 * \param[in] n The number of elements in a.
 *
 * \return The number of inversions. It is equal to 0 for \f$n < 2\f$.
 */
template <class T>
std::size_t ninv(T* a, T* b, std::size_t n) {
     std::size_t ninv = 0;
     for (size_t k = 1; k < n; k *= 2) {
          const size_t k2 = 2 * k;
          for (size_t p = 0; p + k < n; p += k2) {
               const size_t q = p + k - 1;
               const size_t r = std::min(q + k, n - 1);
               size_t p1 = p, p2 = q + 1, i = p;
               while (p1 <= q && p2 <= r)
                    if (a[p1] > a[p2]) {
                         ninv += (q - p1 + 1);
                         b[i++] = a[p2++];
                    } else {
                         b[i++] = a[p1++];
                    }
               while (p1 <= q)
                    b[i++] = a[p1++];
               while (p2 <= r)
                    b[i++] = a[p2++];
               for (i = p; i <= r; i++)
                    a[i] = b[i];
          }
     }
     return ninv;
}

/**
 * Returns the number of inversions in a sequence. It is a wrapper
 * function for SHG::ninv(T*, T*, std::size_t).
 *
 * \tparam T A type with defined operator <.
 *
 * \param[in] a The sequence \f$a_0, a_1, \ldots, a_{n - 1}\f$.
 *
 * \param[in] n The number of elements in a.
 *
 * \return The number of inversions. It is equal to 0 for \f$n < 2\f$.
 *
 * \sa ninv(T*, T*, std::size_t)
 */
template <class T>
std::size_t ninv(const T* a, std::size_t n) {
     std::unique_ptr<T[]> a1(new T[n]), b(new T[n]);
     std::copy_n(a, n, a1.get());
     return ninv(a1.get(), b.get(), n);
}

/**
 * Returns the normalized number of inversions. The maximum number of
 * inversions in a an \f$n\f$-element sequence is \f$n(n - 1) / 2\f$.
 * The function returns \f$\frac{i}{n(n - 1) / 2}\f$, where \f$i\f$ is
 * the number of inversions.
 *
 * \param[in] i The number of inversions in the sequence.
 * \param[in] n The number of elements in the sequence.
 * \return Normalized number of inversions.
 * \exception std::invalid_argument if \f$n < 2\f$ or \f$i > n(n - 1)
 * / 2\f$
 */
double nninv(std::size_t i, std::size_t n);

/** \} */ /* end of group mathematical_statistics */

}       // namespace SHG

#endif
