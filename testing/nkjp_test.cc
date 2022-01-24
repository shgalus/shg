#include <shg/nkjp.h>
#include <filesystem>
#include <stdexcept>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(nkjp_test)

using namespace SHG::PLP;

BOOST_AUTO_TEST_CASE(basic_test) {
     using Path = std::filesystem::path;
     Path const p{"/usr/local/share/nkjp/milionowy1.2"};
     Path const q{"/home/sgalus/data/plpdata/nkjp2"};
     NKJP nkjp;
     nkjp.transform(p, q);
     BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
