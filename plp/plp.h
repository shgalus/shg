#ifndef SHG_PLP_H
#define SHG_PLP_H

#include <ios>
#include <string>
#include <vector>

namespace SHG::PROGPLP {

using Vecstring = std::vector<std::string>;

constexpr std::ios_base::openmode const bininp =
     std::ios_base::in | std::ios_base::binary;
constexpr std::ios_base::openmode const binout =
     std::ios_base::out | std::ios_base::binary;

constexpr char const* const progname{"plp"};
void run(int argc, char const* const argv[]);

void join_dicts(Vecstring const& ifnames, std::string const& ofname);

}  // namespace SHG::PROGPLP

#endif
