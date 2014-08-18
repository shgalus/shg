/* matrixm.cc: matrix multiply_transposed */

/*
 * Output:
 *
 * 3
 * 39
 * 54
 * 69
 */

#include <iostream>
#include "shg/matrix.h"

int main() {
     SHG::Matint a(2, 3, {1, 2, 3, 4, 5, 6});
     SHG::Vecint v {7, 8};
     std::cout << multiply_transposed(a, v);
}
