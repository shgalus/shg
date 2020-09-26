#define BOOST_TEST_MODULE testshg
#include <gsl/gsl_errno.h>
#include "testing.h"

struct Global_fixture {
     Global_fixture() = default;
     void setup();
     void teardown();
     ~Global_fixture() = default;
};

void Global_fixture::setup() {
     gsl_set_error_handler_off();
}

void Global_fixture::teardown() {
     gsl_set_error_handler(nullptr);  // will abort on error
}

BOOST_TEST_GLOBAL_FIXTURE(Global_fixture);
