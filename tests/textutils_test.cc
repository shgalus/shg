#include <shg/textutils.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(textutils_test)

BOOST_AUTO_TEST_SUITE(static_getline_test)

struct Test_case {
     char const* const input;
     std::vector<char const*> const output;
};

Test_case const tc[]{
     {"", {}},
     {"aaa", {"aaa"}},
     {"aaa\n", {"aaa"}},
     {"aaa \\\nbbb \\\nccc", {"aaa bbb ccc"}},
     {"\\\n", {}},
     {"\\\n\\", {""}},
     {"\\\n\\\n", {}},
     {"aaa\\\\", {"aaa\\\\"}},
     {"aaa\\\\\\", {"aaa\\\\"}},
     {"\\", {""}},
     {"\\\\", {"\\\\"}},
     {"\\\n", {}},          // This result is strange.
     {"\\\n\\\n\\\n", {}},  // This result is strange.
     {"aaa\\", {"aaa"}},
};

BOOST_DATA_TEST_CASE(basic_test, bdata::xrange(std::size(tc)), xr) {
     using Vecstring = std::vector<std::string>;
     Vecstring v;
     std::istringstream input(bininp);
     std::string str;

     input.str(tc[xr].input);
     while (SHG::Logical_line::getline(input, str))
          v.push_back(str);
     BOOST_CHECK(input.eof());
     BOOST_REQUIRE(v.size() == tc[xr].output.size());
     for (Vecstring::size_type i = 0; i < v.size(); i++)
          BOOST_CHECK(v[i] == tc[xr].output[i]);

     std::string s;
     for (char const* c = tc[xr].input; *c; c++)
          if (*c == '\n')
               s += "   \n";
          else
               s += *c;
     input.clear();
     input.str(s);
     v.clear();
     while (SHG::Logical_line::getline(input, str))
          v.push_back(str);
     BOOST_CHECK(input.eof());
     BOOST_REQUIRE(v.size() == tc[xr].output.size());
     for (Vecstring::size_type i = 0; i < v.size(); i++)
          BOOST_CHECK(v[i] == tc[xr].output[i]);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(non_static_getline_test)

struct Test_case {
     char const* const input;
     std::vector<char const*> const output;
};

Test_case const tc[]{
     {"", {}},
     {"aaa", {"aaa"}},
     {"aaa\n", {"aaa"}},
     {"aaa \\\nbbb \\\nccc", {"aaa bbb ccc"}},
     {"\\\n", {}},
     {"\\\n\\", {""}},
     {"\\\n\\\n", {}},
     {"aaa\\\\", {"aaa\\\\"}},
     {"aaa\\\\\\", {"aaa\\\\"}},
     {"\\", {""}},
     {"\\\\", {"\\\\"}},
     {"\\\n", {}},          // This result is strange.
     {"\\\n\\\n\\\n", {}},  // This result is strange.
     {"aaa\\", {"aaa"}},
     // Here starts other than in static_getline_test/tc
     {"aaa\n bbb\n ccc", {"aaa bbb ccc"}},
     {"aaa\nbbb\n ccc", {"aaa", "bbb ccc"}},
     {"aaa\nbbb\nccc", {"aaa", "bbb", "ccc"}},
     {" aaa\nbbb\nccc", {" aaa", "bbb", "ccc"}},
     {"aaa\n  bbb\n  ccc", {"aaa bbb ccc"}},
     {"aaa\nbbb\n  ccc", {"aaa", "bbb ccc"}},
     {"aaa\n bbb\n ccc\n", {"aaa bbb ccc"}},
     {"aaa\nbbb\n ccc\n", {"aaa", "bbb ccc"}},
     {"aaa\nbbb\nccc\n", {"aaa", "bbb", "ccc"}},
     {" aaa\nbbb\nccc\n", {" aaa", "bbb", "ccc"}},
     {"aaa\n  bbb\n  ccc\n", {"aaa bbb ccc"}},
     {"aaa\nbbb\n  ccc\n", {"aaa", "bbb ccc"}},
};

BOOST_DATA_TEST_CASE(basic_test, bdata::xrange(std::size(tc)), xr) {
     using Vecstring = std::vector<std::string>;
     using SHG::Logical_line;
     Vecstring v;
     std::istringstream input(bininp);
     std::string str;
     Test_case const* const p = &tc[xr];

     input.str(p->input);
     Logical_line ll1(input);

     while (ll1.getline(str))
          v.push_back(str);
     BOOST_CHECK(input.eof());
     BOOST_REQUIRE(v.size() == p->output.size());
     for (Vecstring::size_type i = 0; i < v.size(); i++)
          BOOST_CHECK(v[i] == p->output[i]);

     std::string s;
     for (char const* c = p->input; *c; c++)
          if (*c == '\n')
               s += "   \n";
          else
               s += *c;
     input.clear();
     input.str(s);
     v.clear();
     Logical_line ll2(input);
     while (ll2.getline(str))
          v.push_back(str);
     BOOST_CHECK(input.eof());
     BOOST_REQUIRE(v.size() == p->output.size());
     for (Vecstring::size_type i = 0; i < v.size(); i++)
          BOOST_CHECK(v[i] == p->output[i]);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(string_to_fields_test)

struct Test_case {
     char const* const input;
     bool const success;
     std::vector<char const*> const output;
};

Test_case const tc[]{
     {"0:1:2:3: 4 ::a:b:c:",
      true,
      {"0", "1", "2", "3", " 4 ", "", "a", "b", "c", ""}},
     {"", true, {""}},
     {":", true, {"", ""}},
     {"::", true, {"", "", ""}},
     {":::", true, {"", "", "", ""}},
     {"aa", true, {"aa"}},
     {"aa:", true, {"aa", ""}},
     {":aa", true, {"", "aa"}},
     {"aa:bb:cc", true, {"aa", "bb", "cc"}},
     {"\\\\", true, {"\\"}},
     {"\\\\\\\\", true, {"\\\\"}},
     {"\\\\:", true, {"\\", ""}},
     {"\\:", true, {":"}},
     {"aaa:bbb\\:::", true, {"aaa", "bbb:", "", ""}},
     {"\\m", false, {}},
};

BOOST_DATA_TEST_CASE(basic_test, bdata::xrange(std::size(tc)), xr) {
     using Vecstring = std::vector<std::string>;
     Vecstring v;
     auto const success = SHG::string_to_fields(tc[xr].input, ':', v);
     if (tc[xr].success) {
          BOOST_REQUIRE(success);
          BOOST_REQUIRE(v.size() == tc[xr].output.size());
          for (Vecstring::size_type i = 0; i < v.size(); i++)
               BOOST_CHECK(v[i] == tc[xr].output[i]);
     } else {
          BOOST_REQUIRE(!success);
     }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
