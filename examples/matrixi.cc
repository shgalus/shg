/* matrixi.cc: outputs matrix as initializer list */

/*
 * Output:
 *
 * {1, 11, 111, 1111, 111, 11}
 */

#include <iostream>
#include "shg/matrix.h"

int main() {
     SHG::print(SHG::Matint(2, 3, {1, 11, 111, 1111, 111, 11}),
                std::cout);
     std::cout << '\n';
}
