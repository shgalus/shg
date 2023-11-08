#include <shg/album.h>
#include <boost/format.hpp>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(album_test)

using SHG::ALBUM::Album_data;

BOOST_AUTO_TEST_CASE(boost_format_test) {
     using boost::format;
     std::string s;
     s = str(boost::format("writing %1%,  x=%2% : %3%-th try") %
             "toto" % 40.23 % 50);
     BOOST_CHECK(s == "writing toto,  x=40.23 : 50-th try");
     format fmter("%2% %1%");
     fmter % 36;
     fmter % 77;
     s = fmter.str();
     BOOST_CHECK(s == "77 36");
     s = str(format("%2% %1%") % 36 % 77);
     BOOST_CHECK(s == "77 36");
}

/*
 * Section 1
 *   Subsection 1.1
 *     first image
 *   Subsection 1.2
 *     Subsubsection 1.2.1
 *       second image
 *     Subsubsection 1.2.2
 * Section 2
 * Section 3
 *   third image
 *   Subsection 3.1
 *     fourth image
 *     fifth image
 *   Subsection 3.2
 *     sixth image
 *   Subsection 3.3
 */
std::vector<Album_data> const album_data{
     {
          // i = 0
          1,
          "Section 1",
          "",
          "",
     },
     {
          // i = 1
          2,
          "Subsection 1.1",
          "",
          "",
     },
     {
          // i = 2
          0,
          "The first image",
          "This is a description of the first image. It contains <, "
          ">, \", \' and & characters.",
          "images/image01.jpg",
          "90",
     },
     {
          // i = 3
          2,
          "Subsection 1.2",
          "",
          "",
     },
     {
          // i = 4
          3,
          "Subsubsection 1.2.1",
          "",
          "",
     },
     {
          // i = 5
          0,
          "The second image",
          "",
          "images/image02.jpg",
     },
     {
          // i = 6
          3,
          "Subsubsection 1.2.2",
          "",
          "",
     },
     {
          // i = 7
          1,
          "Section 2",
          "",
          "",
     },
     {
          // i = 8
          1,
          "Section 3",
          "",
          "",
     },
     {
          // i = 9
          0,
          "The third image",
          "",
          "images/image03.jpg",
     },
     {
          // i = 10
          2,
          "Subsection 3.1",
          "",
          "",
     },
     {
          // i = 11
          0,
          "The fourth image",
          "",
          "images/image04.jpg",
     },
     {
          // i = 12
          0,
          "The fifth image",
          "",
          "images/image05.jpg",
     },
     {
          // i = 13
          2,
          "Subsection 3.2",
          "",
          "",
     },
     {
          // i = 14
          0,
          "The sixth image",
          "",
          "images/image06.jpg",
     },
     {
          // i = 15
          2,
          "Subsection 3.3",
          "",
          "",
     },
};

char const* const album_data_text{R"(
* 1
* Section 1
*
*
%%
* 2
* Subsection 1.1
*
*
%%
* 0
* The first image
* This is a description of the first image. It contains <,
>, ", ' and & characters.
* images/image01.jpg
* 90
%%
* 2
* Subsection 1.2
*
*
%%
* 3
* Subsubsection 1.2.1
*
*
%%
* 0
* The second image
*
* images/image02.jpg
%%
* 3
* Subsubsection 1.2.2
*
*
%%
* 1
* Section 2
*
*
%%
* 1
* Section 3
*
*
%%
* 0
* The third image
*
* images/image03.jpg
%%
* 2
* Subsection 3.1
*
*
%%
* 0
* The fourth image
*
* images/image04.jpg
%%
* 0
* The fifth image
*
* images/image05.jpg
%%
* 2
* Subsection 3.2
*
*
%%
* 0
* The sixth image
*
* images/image06.jpg
%%
* 2
* Subsection 3.3
*
*
)"};

BOOST_AUTO_TEST_CASE(convert_test) {
     using SHG::ALBUM::convert;
     auto const d = convert(album_data_text);
     BOOST_CHECK(d == album_data);
}

BOOST_AUTO_TEST_CASE(basic_test) {
     namespace fs = std::filesystem;
     BOOST_REQUIRE(album_data.size() == 16);
     fs::path const path = fs::temp_directory_path() / "shg/album";
     BOOST_REQUIRE_NO_THROW(fs::remove_all(path));
     bool ok = false;
     BOOST_REQUIRE_NO_THROW(ok = fs::create_directories(path));
     BOOST_REQUIRE(ok);
     BOOST_CHECK_NO_THROW(build(path, "en", "Album", album_data));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
