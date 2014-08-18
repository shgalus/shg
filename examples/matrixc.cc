/* matrixc.cc: matrix constructor from C two-dimensional array */

/*
 * Output:
 *
 * 3 4
 * 0 1 2 3
 * 1 2 3 4
 * 2 3 4 5
 * 3 4
 * 0 1 2 3
 * 1 2 3 4
 * 2 3 4 5
 */

#include <iostream>
#include "shg/matrix.h"

int main() {
     const int m = 3, n = 4;
     int** q = new int*[m];
     for (int i = 0; i < m; i++)
          q[i] = new int[n];
     for (int i = 0; i < m; i++)
          for (int j = 0; j < n; j++)
               q[i][j] = i + j;
     const int* const* r = q;
     SHG::Matint a(m, n, q), b(m, n, r);
     for (int i = m - 1; i >= 0; i--)
          delete[] q[i];
     delete[] q;
     std::cout << a << b;
}
