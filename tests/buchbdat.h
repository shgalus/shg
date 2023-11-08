#ifndef BUCHBDAT_H
#define BUCHBDAT_H

#include <vector>

namespace TESTS {

struct Buchberger_test_case {
     char const* const description;
     char const* const ordering;
     std::vector<const char*> f;  // input polynomials
     std::vector<const char*> g;  // output polynomials
};

extern std::vector<Buchberger_test_case> const buchberger_test_data;

}  // namespace TESTS

#endif
