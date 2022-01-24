#include "plp.h"
#include <fstream>
#include <iostream>
#include <shg/dict.h>
#include <shg/except.h>

namespace SHG::PROGPLP {

void join_dicts(Vecstring const& ifnames, std::string const& ofname) {
     using SHG::PLP::Dictionary;
     std::ifstream f;
     Dictionary d;
     bool first = true;

     for (Vecstring::size_type i = 0; i < ifnames.size(); i++) {
          f.open(ifnames[i], bininp);
          SHG_ASSERT(!f.fail());
          if (first) {
               d.load_source_word_file(f);
               SHG_ASSERT(f.eof());
               SHG_ASSERT(f.fail());
               SHG_ASSERT(!f.bad());
               first = false;
          } else {
               d.add_source_word_file(f);
          }
          f.close();
     }
     if (ofname.empty()) {
          d.write_source_word_file(std::cout, true);
          SHG_ASSERT(std::cout.good());
     } else {
          std::ofstream g(ofname, binout);
          d.write_source_word_file(g, true);
          SHG_ASSERT(g.good());
     }
}

}  // namespace SHG::PROGPLP
