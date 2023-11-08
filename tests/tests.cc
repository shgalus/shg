#define BOOST_TEST_MODULE testshg
#ifdef _MSC_VER
#define NOMINMAX
#include <Windows.h>
#endif
#include <gsl/gsl_errno.h>
#include "tests.h"

struct Global_fixture {
     Global_fixture() = default;
     void setup();
     void teardown();
     ~Global_fixture() = default;
};

void Global_fixture::setup() {
     gsl_set_error_handler_off();
#ifdef _MSC_VER
     BOOST_CHECK(SetConsoleOutputCP(CP_UTF8) != 0);
#endif
}

void Global_fixture::teardown() {
     gsl_set_error_handler(nullptr);  // will abort on error
}

BOOST_TEST_GLOBAL_FIXTURE(Global_fixture);

namespace TESTS {

#if defined(_MSC_VER)
char const* const datadir = SHG_DATADIR;
#else
char const* const datadir = "./data/";
#endif

}  // namespace TESTS
