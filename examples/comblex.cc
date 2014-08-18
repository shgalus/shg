/* comblex.cc: generates 3-element combinations of 4-element set */

/*
 * Output:
 *
 * 012
 * 013
 * 023
 * 123
 */

#include <iostream>
#include "shg/utils.h"

int main() {
     const int n = 4, k = 3;
     SHG::Comblex c(n, k);
     do {
          const std::vector<int>& v = c.get();
          for (int i = 0; i < k; i++)
               std::cout << v[i];
          std::cout << '\n';
     } while (c.next());
}
