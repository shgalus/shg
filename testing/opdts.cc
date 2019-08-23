/* date.cc: testing ordinal patterns distribution in time series */

/**
 * \file testing/opdts.cc
 * Testing ordinal patterns distribution in time series.
 * \date Created on 5 February 2017.
 */

#include "shg/opdts.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

struct Result {
     SHG::OPDTS<int>::pattern p;
     SHG::OPDTS<int>::sztp n;
}
     const
     res2[] = {{{0, 1}, 8081}, {{1, 0}, 1918}},
     res3[] = {{{0, 1, 2}, 6162}, {{0, 2, 1}, 1429},
               {{1, 0, 2}, 502}, {{1, 2, 0}, 1416},
               {{2, 0, 1}, 489}},
     res4[] = {{{0, 1, 2, 3}, 4608}, {{0, 1, 3, 2}, 1064},
               {{0, 2, 1, 3}, 369}, {{0, 2, 3, 1}, 1060},
               {{0, 3, 1, 2}, 386}, {{1, 0, 2, 3}, 138},
               {{1, 2, 0, 3}, 354}, {{1, 2, 3, 0}, 1062},
               {{1, 3, 0, 2}, 364}, {{2, 0, 1, 3}, 133},
               {{2, 3, 0, 1}, 356}, {{3, 0, 1, 2}, 103}};

const Result *const results[] = {res2, res3, res4};
const std::size_t nelems[] = {NELEMS(res2), NELEMS(res3),
                              NELEMS(res4)};

/**
 * Borosh-Niederreiter random number generator. See \cite knuth-2002b,
 * p. 113.
 */
double bn() {
     static unsigned long int x = 1ul;
     x = (1812433253ul * x) & 0xfffffffful;
     return x / 4294967296.0;
}

void check(const std::vector<int>& x, std::size_t d) {
     SHG_ASSERT(d >= 2 && d <= 4);
     SHG::OPDTS<int> opdts(x.data(), 0, x.size(), d);
     std::size_t i;
     SHG::OPDTS<int>::Map::const_iterator it;
     const Result* const r = results[d - 2];
     const std::size_t ne = nelems[d - 2];
     for (it = opdts.frequency.cbegin(), i = 0;
          it != opdts.frequency.cend(); ++it, ++i) {
          SHG_ASSERT(i < ne);
          SHG_ASSERT(it->first == r[i].p);
          SHG_ASSERT(it->second == r[i].n);
     }
     SHG_ASSERT(i == ne);
}

void test1() {
     std::vector<int> x(10000);
     for (std::size_t i = 0; i < x.size(); i++)
          x[i] = bn() < 0.25 ? 1 : 0;
     check(x, 2);
     check(x, 3);
     check(x, 4);
}

}       // anonymous namespace

void test_opdts() {
     test1();
}

}       // namespace Testing
}       // namespace SHG
