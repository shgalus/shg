#include "plp.h"
#include <fstream>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <shg/dict.h>
#include <shg/except.h>

namespace SHG::PROGPLP {

void dict_stat(Vecstring const& ifnames, std::string const& ofname) {
     std::ifstream f;
     std::ofstream g;
     std::ostream* os = ofname.empty() ? &std::cout : &g;
     if (!ofname.empty())
          g.open(ofname);
     SHG_ASSERT(os->good());

     SHG::PLP::Dictionary d;
     unsigned long total = 0;

     for (Vecstring::size_type i = 0; i < ifnames.size(); i++) {
          f.open(ifnames[i], bininp);
          SHG_ASSERT(!f.fail());
          d.load_source_word_file(f);
          SHG_ASSERT(f.eof());
          SHG_ASSERT(f.fail());
          SHG_ASSERT(!f.bad());
          f.close();
          auto const report = d.report();
          unsigned long n = 0;
          for (auto const& r : report) {
               n += r.nentries;
               *os << std::setw(6) << r.nentries << ' '
                   << r.entry_type << '\n';
          }
          *os << std::setw(6) << n << " total in " << ifnames[i]
              << '\n';
          total += n;
     }

     *os << std::setw(6) << total << " total\n";

     SHG_ASSERT(os->good());
     if (!ofname.empty()) {
          g.close();
     }
}

}  // namespace SHG::PROGPLP
