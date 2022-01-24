#include "plp.h"
#include <cstdlib>
#include <exception>
#include <shg/except.h>

int main(int argc, char* argv[]) {
     using SHG::PROGPLP::run;
     using SHG::PROGPLP::progname;
     using SHG::Exception;
     using SHG::error;
     using std::exception;

     try {
          run(argc, argv);
          return EXIT_SUCCESS;
     } catch (Exception const& e) {
          e.print(progname);
     } catch (exception const& e) {
          error(e.what(), progname);
     }
     return EXIT_FAILURE;
}
