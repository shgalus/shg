/**
 * \file include/shg/ipart-inl.h
 * Implementation of inline functions and templates from ipart.h.
 */

#ifndef SHG_IPART_INL_H
#define SHG_IPART_INL_H

namespace SHG {

template <typename F>
void accel_asc(int n, F& f) {
     if (n < 1)
          throw std::invalid_argument(
               "n must be greater than 0 in accel_asc");
     std::vector<int> a(n);
     int k, l, x, y;

     k = 1;
     a[0] = 0;
     y = n - 1;
     while (k != 0) {
          k--;
          x = a[k] + 1;
          while (2 * x <= y) {
               a[k] = x;
               y -= x;
               k++;
          }
          l = k + 1;
          while (x <= y) {
               a[k] = x;
               a[l] = y;
               f(l + 1, a);
               x++;
               y--;
          }
          y += x - 1;
          a[k] = y + 1;
          f(k + 1, a);
     }
}

template <typename F>
void rule_asc(int n, F& f) {
     if (n < 1)
          throw std::invalid_argument(
               "n must be greater than 0 in rule_asc");
     std::vector<int> a(n + 1);
     int k, x, y;

     k = 1;
     a[0] = 0;
     a[1] = n;
     while (k != 0) {
          y = a[k] - 1;
          k--;
          x = a[k] + 1;
          while (x <= y) {
               a[k] = x;
               y -= x;
               k++;
          }
          a[k] = x + y;
          f(k + 1, a);
     }
}

template <typename F>
Ordered_partitions_generator<F>::Ordered_partitions_generator(int n,
                                                              F& f)
     : f_(f) {
     if (n < 1)
          throw std::invalid_argument(
               "n must be greater than 0 in "
               "Ordered_partitions_generator<>::Ordered_partitions_"
               "generator()");
     a_.resize(n);
}

template <typename F>
inline void Ordered_partitions_generator<F>::generate() {
     generate(a_.size(), 0);
}

template <typename F>
void Ordered_partitions_generator<F>::generate(int n, int k) {
     if (n == 0)
          f_(k, a_);
     else
          for (int i = 1; i <= n; i++) {
               a_[k] = i;
               generate(n - i, k + 1);
          }
}

}  // namespace SHG

#endif
