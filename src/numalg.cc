/* numalg.cc: numerical algorithms */

/**
 * \file src/numalg.cc
 * Numerical algorithms.
 * Created on  2 April 2017.
 */

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_poly.h>
#include "shg/numalg.h"

namespace SHG {

int solve_linear(Matdouble& a, Vecdouble& x) {
     gsl_error_handler_t* h = gsl_set_error_handler_off();
     gsl_matrix_view av
          = gsl_matrix_view_array(a.c_vec(), a.nrows(), a.ncols());
     gsl_vector_view xv
          = gsl_vector_view_array(x.c_vec(), x.size());
     gsl_permutation* p = gsl_permutation_alloc(x.size());
     int status = p == nullptr ? GSL_ENOMEM : 0;
     if (!status) {
          int signum;
          status = gsl_linalg_LU_decomp(&av.matrix, p, &signum);
          if (!status)
               status = gsl_linalg_LU_svx(&av.matrix, p, &xv.vector);
     }
     if (p != nullptr)
          gsl_permutation_free(p);
     gsl_set_error_handler(h);
     return status;
}

int solve_polynomial(const Vecdouble& a,
                     Vector<std::complex<double>>& x) {
     int status = 0;
     if (a.size() < 1)
          return GSL_EINVAL;
     x.resize(a.size() - 1);
     gsl_error_handler_t *h = gsl_set_error_handler_off();
     gsl_poly_complex_workspace *w
          = gsl_poly_complex_workspace_alloc(a.size());
     if (w == nullptr)
          status = GSL_ENOMEM;
     else {
          status = gsl_poly_complex_solve(
               a.c_vec(),
               a.size(),
               w,
               reinterpret_cast<gsl_complex_packed_ptr>(x.c_vec()));
          gsl_poly_complex_workspace_free(w);
     }
     gsl_set_error_handler(h);
     return status;
}

}       // namespace SHG
