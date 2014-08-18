/* wolfe.cc: Wolfe method */

/*
 * Solves the problem
 *
 * minimize
 *      -6x_1 + 2x_1^2 - 2x_1x_2 + 2x_2^2
 * subject to
 *      x_1 + x_2 <= 2
 *      x_j >= 0
 */

/*
 * Output:
 *
 * 3
 * 1.5
 * 0.5
 * 0
 * -5.5
 */

#include <iostream>
#include "shg/shg.h"

int main() {
     SHG::Vecdouble
          p {-6, 0, 0},
          C {2, -1, 0, 2, 0, 0},
          b {2},
          x(3);
     SHG::Matdouble A(1, 3, {1, 1, 0});
     double f;
     const int st = SHG::wolfe(p, C, A, b, x, f);
     if (st == 0)
          std::cout << x << f << '\n';
}
