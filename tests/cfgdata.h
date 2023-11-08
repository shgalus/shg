#ifndef CFGDATA_H
#define CFGDATA_H

#include <shg/cfg.h>

namespace TESTS {

extern std::vector<SHG::PLP::Vecprod> const test_grammars;
constexpr std::vector<SHG::PLP::Vecprod>::size_type const
     test_grammars_size = 21;

extern std::vector<SHG::PLP::Vecprod> const conversion_results;
constexpr std::vector<SHG::PLP::Vecprod>::size_type const
     conversion_results_size = 21;

}  // namespace TESTS

#endif
