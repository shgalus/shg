#include "shg/vector.h"
#include <boost/test/unit_test.hpp>
#include "shg/utils.h"

namespace SHG::BTesting {

BOOST_AUTO_TEST_SUITE(vector_test)

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
          const std::basic_string<int> s{1, 3, 5, 7, 9};
          const Vecint v(s);
          BOOST_CHECK(
               std::equal(s.begin(), s.end(), v.begin(), v.end()));
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
     /// \todo move assignment
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
          const std::basic_string<int> s{1, 3, 5, 7, 9};
          Vecint v;
          v = s;
          BOOST_CHECK(
               std::equal(s.begin(), s.end(), v.begin(), v.end()));
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
     /// \todo here continue
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting
