#include <shg/opdts.h>
#include <shg/mzt.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(opdts_test)

struct Opdts_data {
     Opdts_data();
     std::vector<int> x;

private:
     MZT mzt;
};

Opdts_data::Opdts_data() : x(10000), mzt() {
     for (std::vector<int>::size_type i = 0; i < x.size(); i++)
          x[i] = mzt.uni(2);
}

struct Result {
     OPDTS<int>::pattern p;
     OPDTS<int>::sztp n;
};

BOOST_FIXTURE_TEST_CASE(length_2_test, Opdts_data) {
     const std::vector<Result> result{{{0, 1}, 7530}, {{1, 0}, 2469}};
     OPDTS<int> opdts(x.data(), 0, x.size(), 2);
     std::size_t i;
     OPDTS<int>::Map::const_iterator it;

     for (it = opdts.frequency.cbegin(), i = 0;
          it != opdts.frequency.cend(); ++it, ++i) {
          BOOST_REQUIRE(i < result.size());
          BOOST_CHECK(it->first == result[i].p);
          BOOST_CHECK(it->second == result[i].n);
     }
     BOOST_CHECK(i == result.size());
}

BOOST_FIXTURE_TEST_CASE(length_3_test, Opdts_data) {
     const std::vector<Result> result{{{0, 1, 2}, 5061},
                                      {{0, 2, 1}, 1206},
                                      {{1, 0, 2}, 1193},
                                      {{1, 2, 0}, 1275},
                                      {{2, 0, 1}, 1263}};
     OPDTS<int> opdts(x.data(), 0, x.size(), 3);
     std::size_t i;
     OPDTS<int>::Map::const_iterator it;

     for (it = opdts.frequency.cbegin(), i = 0;
          it != opdts.frequency.cend(); ++it, ++i) {
          BOOST_REQUIRE(i < result.size());
          BOOST_CHECK(it->first == result[i].p);
          BOOST_CHECK(it->second == result[i].n);
     }
     BOOST_CHECK(i == result.size());
}

BOOST_FIXTURE_TEST_CASE(length_4_test, Opdts_data) {
     const std::vector<Result> result{
          {{0, 1, 2, 3}, 3161}, {{0, 1, 3, 2}, 637},
          {{0, 2, 1, 3}, 572},  {{0, 2, 3, 1}, 633},
          {{0, 3, 1, 2}, 633},  {{1, 0, 2, 3}, 624},
          {{1, 2, 0, 3}, 655},  {{1, 2, 3, 0}, 620},
          {{1, 3, 0, 2}, 569},  {{2, 0, 1, 3}, 621},
          {{2, 3, 0, 1}, 642},  {{3, 0, 1, 2}, 630}};
     OPDTS<int> opdts(x.data(), 0, x.size(), 4);
     std::size_t i;
     OPDTS<int>::Map::const_iterator it;

     for (it = opdts.frequency.cbegin(), i = 0;
          it != opdts.frequency.cend(); ++it, ++i) {
          BOOST_REQUIRE(i < result.size());
          BOOST_CHECK(it->first == result[i].p);
          BOOST_CHECK(it->second == result[i].n);
     }
     BOOST_CHECK(i == result.size());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
