/**
 * \file include/shg/nkjp.h
 * National Corpus of Polish processing.
 */

#ifndef SHG_NKJP_H
#define SHG_NKJP_H

#include <string>
#include <vector>
#include <filesystem>
#include <pugixml.hpp>

namespace SHG::PLP {

class NKJP {
public:
     using Path = std::filesystem::path;
     void transform(Path const& ipath, Path const& opath);

private:
     using Vecstring = std::vector<std::string>;
     static std::string const srcfn;
     Path ipath_{};
     Path opath_{};
     Vecstring file_names_{};

     Path ofpath(Vecstring::size_type k);
     void collect_file_names();
     void transform_one_file(Vecstring::size_type k);
     void transform_one_segment(pugi::xml_node const& segment,
                                std::ofstream& g);
};

}  // namespace SHG::PLP

#endif
