/**
 * \file src/ipart.cc
 * Partitions of integer numbers.
 */

#include "shg/ipart.h"

namespace SHG {

void partgen(int c[], int N, int K, bool& G) {
     int a, i, j;

     if (!G)
          goto first;
     j = 2;
     a = c[1];
test:
     if (a < j)
          goto B;
     c[j]++;
     c[1] = a - j;
zero:
     for (i = 2; i < j; i++)
          c[i] = 0;
     return;
B:
     if (j == K)
          goto last;
     a += j * c[j];
     j++;
     goto test;
first:
     G = true;
     c[1] = N;
     j = K + 1;
     goto zero;
last:
     G = false;
}

}  // namespace SHG
