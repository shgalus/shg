// Generates tables for pchar.cc.

#include <iostream>
#include <iomanip>
#include <locale>
#include <bitset>
#include <shg/pchar.h>

int main() {
     using std::cout, std::setw;
     using SHG::PLP::Pchar, SHG::PLP::character_data;
     std::locale loc("en_US.UTF-8");
     cout << "std::bitset<12> constexpr ctype_table[256] = {\n";
     cout << "     //              XUSPPLGDCBAA\n";
     cout << std::hex << std::setfill('0');
     for (int i = 0; i < 256; i++) {
          const wchar_t c = character_data(i).code;
          std::bitset<12> a;
          a[0] = std::isalnum(c, loc);
          a[1] = std::isalpha(c, loc);
          a[2] = std::isblank(c, loc);
          a[3] = std::iscntrl(c, loc);
          a[4] = std::isdigit(c, loc);
          a[5] = std::isgraph(c, loc);
          a[6] = std::islower(c, loc);
          a[7] = std::isprint(c, loc);
          a[8] = std::ispunct(c, loc);
          a[9] = std::isspace(c, loc);
          a[10] = std::isupper(c, loc);
          a[11] = std::isxdigit(c, loc);
          cout << "     0x" << setw(3) << a.to_ulong() << ",  // 0x"
               << setw(2) << i << " " << a << '\n';
     }
     cout << "};\n\n";
     cout << "unsigned char constexpr toupper_table[256] = {\n";
     for (int i = 0; i < 256; i++) {
          const wchar_t c = character_data(i).code;
          const wchar_t d = std::toupper(c, loc);
          cout << "     0x" << setw(2) << Pchar(d).internal_code()
               << ",  // 0x" << setw(2) << i << '\n';
     }
     cout << "};\n\n";
     cout << "unsigned char constexpr tolower_table[256] = {\n";
     for (int i = 0; i < 256; i++) {
          const wchar_t c = character_data(i).code;
          const wchar_t d = std::tolower(c, loc);
          cout << "     0x" << setw(2) << Pchar(d).internal_code()
               << ",  // 0x" << setw(2) << i << '\n';
     }
     cout << "};\n";
}
