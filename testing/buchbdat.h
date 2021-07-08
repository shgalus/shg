#ifndef BUCHBDAT_H
#define BUCHBDAT_H

#include <vector>

namespace SHG::Testing {

struct Buchberger_test_case {
     const char* description;
     const char* ordering;
     std::vector<const char*> f;  // input polynomials
     std::vector<const char*> g;  // output polynomials
};

extern const std::vector<Buchberger_test_case> buchberger_test_data;

}  // namespace SHG::Testing

#endif
