/* matrixo.cc: matrix operator<< */

/*
 * Output:
 *
 * 2 3
 *    1   11  111
 * 1111  111   11
 */

#include <iostream>
#include <iomanip>
#include "shg/matrix.h"

int main() {
     std::cout << std::setw(4) << SHG::Matint(
          2, 3, {1, 11, 111, 1111, 111, 11});
}
