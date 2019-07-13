/* octal.cc: replace each byte greater than 127 by its octal code */

#include <cassert>
#include <iostream>
#include <iomanip>

using std::cin, std::cout, std::oct;

int main() {
     char c;
     unsigned char u;

     while (cin.get(c)) {
          u = reinterpret_cast<unsigned char&>(c);
          if (u < 128)
               cout << u;
          else
               cout << '\\' << oct << +u;
     }
}
