/**
 * \file include/shg/ifact.h
 * Integer factorization.
 */

#ifndef SHG_IFACT_H
#define SHG_IFACT_H

#include <vector>

namespace SHG {

// Trial divisors.
template <class T>
class Divisors {
public:
     T operator()() { return (this->*next_function_)(); }

private:
     using Next_function = T (Divisors::*)();

     T d_{2};
     bool even_{false};
     Next_function next_function_{&Divisors::initial_next};

     T initial_next() {
          if (d_ == 2) {
               d_ = 3;
               return 2;
          } else if (d_ == 3) {
               d_ = 5;
               return 3;
          } else {
               next_function_ = &Divisors::subsequent_next;
               return 5;
          }
     }

     T subsequent_next() {
          return (even_ = !even_) ? d_ += 2 : d_ += 4;
     }
};

template <class T>
void factorize(T n, std::vector<T>& v) {
     v.clear();
     if (n < 2)
          return;
     Divisors<T> g;
     T q, r, d = g();

     for (;;) {
          q = n / d;
          r = n % d;
          if (r == 0) {
               v.push_back(d);
               n = q;
               if (n == 1)
                    return;
          } else {
               if (q > d) {
                    d = g();
               } else {
                    v.push_back(n);
                    return;
               }
          }
     }
}

}  // namespace SHG

#endif
