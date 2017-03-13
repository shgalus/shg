/* opdts.h: ordinal patterns distribution in time series */

/**
 * \file include/shg/opdts.h
 * Ordinal patterns distribution in time series.
 * \date Created on 5 February 2017.
 */

#ifndef SHG_OPDTS_H
#define SHG_OPDTS_H

#include <cstddef>
#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace SHG {

/**
 * Ordinal patterns distribution in time series.
 *
 * Let \f$(x_i)_{i = 0}^{n - 1}\f$ be a sequence of elements. A
 * subsequence \f$x_i, x_{i + 1}, \ldots, x_{i + d - 1}\f$ of length
 * \f$d > 1\f$ defines the ordinal pattern \f$\pi = (\pi_0, \pi_1,
 * \ldots, \pi_{d - 1})\f$ if \f[x_{i + \pi_0} < x_{i + \pi_1} <
 * \ldots < x_{i + \pi_{d - 1}},\f] and \f$x_k < x_l\f$ if \f$x_k =
 * x_l\f$ but \f$k < l\f$ \cite amigo-2010, p. 52.
 *
 * The constructor accepts the sequence \f$(x_i)_{i = i_0}^{i_1 -
 * 1}\f$ and the block length \f$d > 1\f$. Then the map frequency
 * returns the distribution of ordinal patterns of length \f$d\f$
 * existing in the sequence.
 */
template <class T> class OPDTS {
private:
     struct Less;
public:
     typedef std::size_t sztp;
     typedef std::vector<sztp> pattern;
     typedef std::map<pattern, sztp, Less> Map;

     OPDTS(const T x[], sztp i0, sztp i1, sztp d);

     Map frequency;
     void print(std::ostream& f = std::cout);
private:
     /**
      * \internal
      *
      * The ordinal pattern \f$\pi = (\pi_0, \pi_1, \ldots, \pi_{d -
      * 1})\f$ corresponds to the condition \f[x_{i + \pi_0} < x_{i +
      * \pi_1} < \ldots < x_{i + \pi_{d - 1}},\f] wherein if \f$\pi_k
      * < \pi_{k + 1}\f$, then the relation between \f$x_{i +
      * \pi_k}\f$ and \f$x_{i + \pi_{k + 1}}\f$ is not \f$<\f$ but
      * \f$\leq\f$.
      */
     void calc(const T x[], sztp imax, sztp d);
     void calc2(const T x[], sztp imax);
     void calc3(const T x[], sztp imax);
};

template <class T>
struct OPDTS<T>::Less {
     bool operator()(const pattern& x, const pattern& y) const {
          for (pattern::size_type i = 0; i < x.size(); i++)
               if (x[i] < y[i])
                    return true;
               else
                    if (x[i] > y[i])
                         return false;
          return false;
     }
};

template <class T>
OPDTS<T>::OPDTS(const T x[], sztp i0, sztp i1, sztp d)
     : frequency() {
     const sztp n = i1 - i0;
     if (d <= 1 || i1 < i0 || n < d)
          throw std::invalid_argument(__func__);
     const sztp imax = n - d + 1;
     x += i0;
     // if (d == 2)
     //      calc2(x, imax);
     // else if (d == 3)
     //      calc3(x, imax);
     // else
          calc(x, imax, d);
}

template <class T>
void OPDTS<T>::print(std::ostream& f) {
     for (auto it = frequency.cbegin(); it != frequency.cend(); ++it) {
          for (pattern::size_type i = 0; i < it->first.size(); i++)
               f << it->first[i];
          f << "   " << it->second << '\n';
     }
}

template <class T>
void OPDTS<T>::calc(const T x[], sztp imax, sztp d) {
     pattern p(d);
     for (sztp i = 0; i < imax; i++) {
          std::iota(p.begin(), p.end(), 0);
          std::stable_sort(p.begin(), p.end(),
                           [i, &x, &p](const sztp k, const sztp l) {
                                return x[i + k] < x[i + l];
                           });
          frequency[p]++;
     }
}

template <class T>
void OPDTS<T>::calc2(const T x[], sztp imax) {
     for (sztp i = 0; i < imax; i++)
          if (x[i + 1] < x[i])
               frequency[{1, 0}]++;
          else
               frequency[{0, 1}]++;
}

template <class T>
void OPDTS<T>::calc3(const T x[], sztp imax) {
     for (sztp i = 0; i < imax; i++)
          if (x[i + 1] < x[i])
               if (x[i + 2] < x[i + 1])
                    frequency[{2, 1, 0}]++;
               else if (x[i + 2] < x[i])
                    frequency[{1, 2, 0}]++;
               else
                    frequency[{1, 0, 2}]++;
          else
               if (x[i + 2] < x[i])
                    frequency[{2, 0, 1}]++;
               else
                    if (x[i + 2] < x[i + 1])
                         frequency[{0, 2, 1}]++;
                    else
                         frequency[{0, 1, 2}]++;
}

}       // namespace SHG

#endif
