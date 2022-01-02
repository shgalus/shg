#include <shg/vector.h>
#include <shg/utils.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(vector_test)

const std::ios_base::openmode binary_io_mode =
     std::ios_base::out | std::ios_base::in | std::ios_base::binary;

BOOST_AUTO_TEST_CASE(vector_constructor_test) {
     {
          const Vecint v;
          BOOST_CHECK(v.size() == 0);
     }
     {
          const Vecint v(10);
          BOOST_CHECK(v.size() == 10);
     }
     {
          const Vecint v(10, 11);
          BOOST_CHECK(v.size() == 10);
          for (Vecint::size_type i = 0; i < v.size(); i++)
               BOOST_CHECK(v[i] == 11);
     }
     {
          const int a[]{0, 1, 2, 3};
          const Vecint v(3, a);
          BOOST_CHECK(v.size() == 3);
          for (Vecint::size_type i = 0; i < v.size(); i++)
               BOOST_CHECK(v[i] == a[i]);
     }
     {
          const std::initializer_list<int> il = {1, 3, 5, 7, 9};
          const Vecint v(il);
          BOOST_CHECK(v.size() == il.size());
          std::initializer_list<int>::const_iterator it = il.begin();
          for (Vecint::size_type i = 0; i < v.size(); i++, ++it)
               BOOST_CHECK(v[i] == *it);
          BOOST_CHECK(it == il.end());
     }
     {
          const Vecint v{1, 3, 5, 7, 9};
          BOOST_CHECK(v.size() == 5);
          for (Vecint::size_type i = 0; i < v.size(); i++)
               BOOST_CHECK(v[i] == narrow_cast<int>(2 * i + 1));
     }
     {
          const Vecint v{};
          BOOST_CHECK(v.size() == 0);
     }
     {
          const Vecint v{1, 3, 5, 7, 9};
          const Vecint w(v);
          BOOST_CHECK(w == v);
     }
     {
          Vecint v{};
          const Vecint w(std::move(v));
          BOOST_CHECK(v.size() == 0);
          BOOST_CHECK(w.size() == 0);
     }
     {
          Vecint v{1, 3, 5, 7, 9};
          const Vecint w(std::move(v));
          BOOST_CHECK(v.size() == 0);
          BOOST_CHECK(w == Vecint({1, 3, 5, 7, 9}));
     }
     {
          const std::vector<int> v{1, 3, 5, 7, 9};
          const Vecint w(v);
          BOOST_CHECK(
               std::equal(v.begin(), v.end(), w.begin(), w.end()));
     }
     {
          const std::valarray<int> v{1, 3, 5, 7, 9};
          const Vecint w(v);
          BOOST_CHECK(std::equal(std::begin(v), std::end(v),
                                 std::begin(w), std::end(w)));
     }
}

BOOST_AUTO_TEST_CASE(vector_assignment_test) {
     {
          const Vecint v;
          Vecint w;
          w = v;
          BOOST_CHECK(w.size() == 0);
     }
     {
          const Vecint v{1, 3, 5, 7, 9};
          Vecint w;
          w = v;
          BOOST_CHECK(w == v);
     }
     {
          Vecint v{1, 3, 5, 7, 9};
          Vecint w;
          w = std::move(v);
          BOOST_CHECK(v.size() == 0);
          BOOST_CHECK(w == Vecint({1, 3, 5, 7, 9}));
     }
     {
          Vecint v(10);
          v = 13;
          BOOST_CHECK(v.size() == 10);
          BOOST_CHECK(std::all_of(v.cbegin(), v.cend(),
                                  [](int n) { return n == 13; }));
     }
     {
          Vecint v;
          v = {1, 3, 5, 7, 9};
          BOOST_CHECK(v == Vecint({1, 3, 5, 7, 9}));
     }
     {
          const std::vector<int> v{1, 3, 5, 7, 9};
          Vecint w;
          w = v;
          BOOST_CHECK(
               std::equal(v.begin(), v.end(), w.begin(), w.end()));
     }
     {
          const std::valarray<int> v{1, 3, 5, 7, 9};
          Vecint w;
          w = v;
          BOOST_CHECK(std::equal(std::begin(v), std::end(v),
                                 std::begin(w), std::end(w)));
     }
}

BOOST_AUTO_TEST_CASE(vector_element_access_test) {
     {
          const Vecint v;
          BOOST_CHECK_THROW(v.at(1), std::out_of_range);
     }
     {
          const Vecint v{1, 3, 5, 7, 9};
          BOOST_CHECK(v.at(4) == 9);
          BOOST_CHECK_THROW(v.at(5), std::out_of_range);
     }
}

BOOST_AUTO_TEST_CASE(vector_memeber_functions_test) {
     {
          Vecint v;
          v.resize(3);
          BOOST_CHECK(v.size() == 3);
          v.resize(0);
          BOOST_CHECK(v.size() == 0);
     }
     {
          Vecint v;
          v.assign(10, 11);
          BOOST_CHECK(v.size() == 10);
          for (Vecint::size_type i = 0; i < v.size(); i++)
               BOOST_CHECK(v[i] == 11);
     }
     {
          const Vecint v{1, 3, 5, 7, 9};
          const int* a = v.c_vec();
          for (Vecint::size_type i = 0; i < v.size(); i++)
               BOOST_CHECK(a[i] == narrow_cast<int>(2 * i + 1));
     }
     {
          Vecint v(5);
          int* a = v.c_vec();
          for (Vecint::size_type i = 0; i < v.size(); i++)
               a[i] = narrow_cast<int>(2 * i + 1);
          for (Vecint::size_type i = 0; i < v.size(); i++)
               BOOST_CHECK(v[i] == narrow_cast<int>(2 * i + 1));
     }
     {
          const Vecint v(10, 11);
          const Vecint w(3, 7);
          Vecint v1(v);
          Vecint w1(w);
          v1.swap(w1);
          BOOST_CHECK(v1 == w);
          BOOST_CHECK(w1 == v);
     }
     {
          const Vecint v{1, 3, 5, 7, 9};
          const std::vector<int> w(v);
          BOOST_CHECK(
               std::equal(v.begin(), v.end(), w.begin(), w.end()));
     }
     {
          const Vecint v{1, 3, 5, 7, 9};
          const std::valarray<int> w(v);
          BOOST_CHECK(std::equal(std::begin(v), std::end(v),
                                 std::begin(w), std::end(w)));
     }
     {
          const Vecint v;
          Vecint w;
          std::stringstream ss(binary_io_mode);
          v.write(ss);
          BOOST_CHECK(!ss.fail());
          w.read(ss);
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(w == v);
     }
     {
          const Vecint v{1, 3, 5, 7, 9};
          Vecint w;
          std::stringstream ss(binary_io_mode);
          v.write(ss);
          BOOST_CHECK(!ss.fail());
          w.read(ss);
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(w == v);
     }
}

namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(vector_iterators_test, bdata::xrange(5), xr) {
     const int n = xr;
     Vecint w = arithmetic_progression(n, 0, 1);
     const Vecint v(w);
     int k = 0;
     for (Vecint::iterator i = w.begin(); i != w.end(); ++i)
          BOOST_CHECK(++*i == ++k);
     BOOST_CHECK(k == n);
     for (Vecint::reverse_iterator i = w.rbegin(); i != w.rend(); ++i)
          BOOST_CHECK(--*i == --k);
     BOOST_CHECK(k == 0);
     BOOST_CHECK(equal(v, w));
     for (Vecint::const_iterator i = w.begin(); i != w.end(); ++i)
          BOOST_CHECK(*i == k++);
     BOOST_CHECK(k == n);
     for (Vecint::const_reverse_iterator i = w.rbegin();
          i != w.rend(); ++i)
          BOOST_CHECK(*i == --k);
     BOOST_CHECK(k == 0);
     for (Vecint::const_iterator i = w.cbegin(); i != w.cend(); ++i)
          BOOST_CHECK(*i == k++);
     BOOST_CHECK(k == n);
     for (Vecint::const_reverse_iterator i = w.crbegin();
          i != w.crend(); ++i)
          BOOST_CHECK(*i == --k);
     BOOST_CHECK(k == 0);
}

BOOST_AUTO_TEST_CASE(vector_non_member_functions_test) {
     const Vecint wirth{44, 55, 12, 42, 94, 18, 6, 67}, empty;
     {
          BOOST_CHECK(sum(wirth) == 338);
          BOOST_CHECK(min(wirth) == 6);
          BOOST_CHECK(max(wirth) == 94);
          const auto mm = minmax(wirth);
          BOOST_CHECK(mm.first == 6 && mm.second == 94);
          BOOST_CHECK(minloc(wirth) == 6);
          BOOST_CHECK(maxloc(wirth) == 4);
          auto mml = minmaxloc(wirth);
          BOOST_CHECK(mml.first == 6 && mml.second == 4);
          BOOST_CHECK(sum(empty) == 0);
          BOOST_CHECK(minloc(empty) == 0);
          BOOST_CHECK(maxloc(empty) == 0);
          mml = minmaxloc(empty);
          BOOST_CHECK(mml.first == 0 && mml.second == 0);
     }
     {
          Vecint v(wirth), w(empty);
          clear(v);
          BOOST_CHECK(v.size() == 0);
          clear(w);
          BOOST_CHECK(w.size() == 0);
          v = wirth;
          w = {1, 2, 3, 4};
          swap(v, w);
          BOOST_CHECK(equal(v, {1, 2, 3, 4}) && equal(w, wirth));
          clear(v);
          swap(v, w);
          BOOST_CHECK(equal(v, wirth) && equal(w, empty));
     }
     {
          Vecint v;
          sort(v = wirth);
          BOOST_CHECK(equal(v, {6, 12, 18, 42, 44, 55, 67, 94}));
          sort(v = wirth, 2, 7);
          BOOST_CHECK(equal(v, {44, 55, 6, 12, 18, 42, 94, 67}));
          reverse_sort(v = wirth);
          BOOST_CHECK(equal(v, {94, 67, 55, 44, 42, 18, 12, 6}));
          reverse_sort(v = wirth, 2, 7);
          BOOST_CHECK(equal(v, {44, 55, 94, 42, 18, 12, 6, 67}));
     }
     {
          Vecint v;
          {
               std::stringstream ss;
               ss << wirth;
               ss >> v;
               BOOST_CHECK(!ss.fail());
               BOOST_CHECK(equal(v, wirth));
          }
          clear(v);
          {
               std::stringstream ss;
               ss << v;
               ss >> v;
               BOOST_CHECK(!ss.fail());
               BOOST_CHECK(equal(v, {}));
          }
          v = wirth;
          {
               std::stringstream ss;
               ss << "a\n1\n2\n3\n";
               ss >> v;
               BOOST_CHECK(ss.fail() && equal(v, wirth));
          }
          {
               std::stringstream ss;
               ss << "3\na\n2\n3\n";
               ss >> v;
               BOOST_CHECK(ss.fail() && equal(v, wirth));
          }
          {
               std::stringstream ss;
               ss << "3\n1\na\n3\n";
               ss >> v;
               BOOST_CHECK(ss.fail() && equal(v, wirth));
          }
          {
               std::stringstream ss;
               ss << "3\n1\n2\na\n";
               ss >> v;
               BOOST_CHECK(ss.fail() && equal(v, wirth));
          }
     }
     {
          const Vecint v;
          Vecint w;
          std::stringstream ss(binary_io_mode);
          write(v, ss);
          BOOST_CHECK(!ss.fail());
          read(w, ss);
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(w == v);
     }
     {
          const Vecint v{1, 3, 5, 7, 9};
          Vecint w;
          std::stringstream ss(binary_io_mode);
          write(v, ss);
          BOOST_CHECK(!ss.fail());
          read(w, ss);
          BOOST_CHECK(!ss.fail());
          BOOST_CHECK(w == v);
     }
     {
          Vecint v, w;
          v = w = wirth;
          BOOST_CHECK(maximum_norm_distance(v, w) == 0);
          w(0)++;
          BOOST_CHECK(maximum_norm_distance(v, w) == 1);
     }
     {
          Vecint v = arithmetic_progression(7, 2, 3);
          BOOST_CHECK(v.size() == 7);
          for (Vecint::size_type i = 0; i < v.size(); i++)
               BOOST_CHECK(v[i] == narrow_cast<int>(2 + 3 * i));
          v = arithmetic_progression(0, 1, 2);
          BOOST_CHECK(v.size() == 0);
     }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
