/**
 * \file include/shg/album.h
 * Generating HTML album.
 */

#ifndef SHG_ALBUM_H
#define SHG_ALBUM_H

#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>

namespace SHG::ALBUM {

class Error : public std::runtime_error {
public:
     Error();
     Error(char const* message);
};

struct Album_data {
     int level{};  // level in the tree, 0 for leaves
     std::string title{};
     std::string text{};
     std::string image_path{};
     std::string rotation{};  // rotation of the image in degrees
};

bool operator==(Album_data const& lhs, Album_data const& rhs);

std::vector<Album_data> convert(std::string const& s);

void build(std::filesystem::path const& path, std::string const& lang,
           std::string const& title,
           std::vector<Album_data> const& data);

}  // namespace SHG::ALBUM

#endif
