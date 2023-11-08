#include <shg/ipart.h>
#include <algorithm>
#include <iterator>
#include <numeric>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(ipart_test)

using SHG::partgen;
using SHG::accel_asc;
using SHG::rule_asc;
using SHG::Ordered_partitions_generator;

struct Partitions {
     std::vector<std::vector<int>> tab{};
     void operator()(int k, std::vector<int> const& a);
     void push_back(int const c[], int K);
     void sort();
     bool is_lexicographically_sorted() const;
     bool is_each_row_sorted() const;
     static bool less(std::vector<int> const& v1,
                      std::vector<int> const& v2);
};

void Partitions::operator()(int k, std::vector<int> const& a) {
     std::vector<int> v(a.cbegin(), a.cbegin() + k);
     tab.push_back(v);
}

void Partitions::push_back(int const c[], int K) {
     std::vector<int> v;
     for (int i = 1; i <= K; i++)
          v.insert(v.end(), c[i], i);
     tab.push_back(v);
}

void Partitions::sort() {
     std::sort(tab.begin(), tab.end(), less);
}

bool Partitions::is_lexicographically_sorted() const {
     return std::is_sorted(tab.cbegin(), tab.cend(), less);
}

bool Partitions::is_each_row_sorted() const {
     for (auto it = tab.cbegin(); it != tab.cend(); ++it)
          if (!std::is_sorted(it->cbegin(), it->cend()))
               return false;
     return true;
}

bool Partitions::less(std::vector<int> const& v1,
                      std::vector<int> const& v2) {
     return std::lexicographical_compare(v1.cbegin(), v1.cend(),
                                         v2.cbegin(), v2.cend());
}

BOOST_DATA_TEST_CASE(rule_asc_vs_accel_asc_test, bdata::xrange(20),
                     xr) {
     // Number of partitions (https://oeis.org/A000041).
     std::vector<std::size_t> const p{
          1,  1,  2,   3,   5,   7,   11,  15,  22,  30, 42,
          56, 77, 101, 135, 176, 231, 297, 385, 490, 627};
     int const n = xr + 1;

     Partitions part_rule;
     Partitions part_accel;
     Partitions part_partgen;

     rule_asc(n, part_rule);
     accel_asc(n, part_accel);

     if (n > 1) {
          std::vector<int> c(n + 1);
          int const N = n;
          int const K = n;
          bool G = false;

          partgen(c.data(), N, K, G);
          while (G) {
               part_partgen.push_back(c.data(), K);
               partgen(c.data(), N, K, G);
          }
          part_partgen.sort();
     }

     BOOST_CHECK(part_rule.tab == part_accel.tab);
     BOOST_CHECK(part_accel.tab.size() == p[n]);
     if (n > 1)
          BOOST_CHECK(part_rule.tab == part_partgen.tab);
     for (auto it = part_rule.tab.cbegin();
          it != part_rule.tab.cend(); ++it) {
          BOOST_CHECK(std::accumulate(it->cbegin(), it->cend(), 0) ==
                      n);
          auto const itnext = std::next(it);
          if (itnext != part_rule.tab.cend())
               BOOST_CHECK(*it != *itnext);
     }
}

struct Ordered_partition_generator_test_case {
     int const n;
     std::vector<std::vector<int>> const a;
};

std::ostream& operator<<(
     std::ostream& stream,
     Ordered_partition_generator_test_case const& c) {
     stream.width(0);
     stream << c.n << " elements";
     return stream;
}

std::vector<Ordered_partition_generator_test_case> const opgtc{
     {1, {{1}}},
     {2, {{1, 1}, {2}}},
     {3, {{1, 1, 1}, {1, 2}, {2, 1}, {3}}},
     {4,
      {{1, 1, 1, 1},
       {1, 1, 2},
       {1, 2, 1},
       {1, 3},
       {2, 1, 1},
       {2, 2},
       {3, 1},
       {4}}},
     {5,
      {{1, 1, 1, 1, 1},
       {1, 1, 1, 2},
       {1, 1, 2, 1},
       {1, 1, 3},
       {1, 2, 1, 1},
       {1, 2, 2},
       {1, 3, 1},
       {1, 4},
       {2, 1, 1, 1},
       {2, 1, 2},
       {2, 2, 1},
       {2, 3},
       {3, 1, 1},
       {3, 2},
       {4, 1},
       {5}}},
     {6, {{1, 1, 1, 1, 1, 1},
          {1, 1, 1, 1, 2},
          {1, 1, 1, 2, 1},
          {1, 1, 1, 3},
          {1, 1, 2, 1, 1},
          {1, 1, 2, 2},
          {1, 1, 3, 1},
          {1, 1, 4},
          {1, 2, 1, 1, 1},
          {1, 2, 1, 2},
          {1, 2, 2, 1},
          {1, 2, 3},
          {1, 3, 1, 1},
          {1, 3, 2},
          {1, 4, 1},
          {1, 5},
          {2, 1, 1, 1, 1},
          {2, 1, 1, 2},
          {2, 1, 2, 1},
          {2, 1, 3},
          {2, 2, 1, 1},
          {2, 2, 2},
          {2, 3, 1},
          {2, 4},
          {3, 1, 1, 1},
          {3, 1, 2},
          {3, 2, 1},
          {3, 3},
          {4, 1, 1},
          {4, 2},
          {5, 1},
          {6}}}};

BOOST_DATA_TEST_CASE(ordered_partition_generator_test,
                     bdata::make(opgtc), c) {
     Partitions p;
     Ordered_partitions_generator g(c.n, p);
     g.generate();
     BOOST_CHECK(p.tab == c.a);
     BOOST_CHECK(p.is_lexicographically_sorted());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
