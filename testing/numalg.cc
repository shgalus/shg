/* numalg.cc: testing numerical algorithms */

/**
 * \file testing/numalg.cc
 * Testing numerical algorithms.
 * Created on  2 April 2017.
 */
#include <iomanip>
#include <cmath>
#include "shg/numalg.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

/*
 * GNU Scientific Library - Reference Manual, section 14.15.
 */
void test_solve_linear() {
     Matdouble A(4, 4, {0.18, 0.60, 0.57, 0.96,
                        0.41, 0.24, 0.99, 0.58,
                        0.14, 0.30, 0.97, 0.66,
                        0.51, 0.13, 0.19, 0.85 });
     Vecdouble b {1.0, 2.0, 3.0, 4.0};
     const Vecdouble x {-4.05205, -12.6056, 1.66091, 8.69377};
     const int status = SHG::solve_linear(A, b);
     SHG_ASSERT(status == 0);
     SHG_ASSERT(x.size() == b.size());
     for (Vecdouble::size_type i = 0; i < b.size(); i++)
          SHG_ASSERT(std::abs(x(i) - b(i)) < 2e-5);
}

/*
 * GNU Scientific Library - Reference Manual, section 6.6.
 */
void test_solve_polynomial() {
     typedef std::complex<double> cplx;
     const Vecdouble a {-1, 0, 0, 0, 0, 1};
     const Vector<cplx> z {
          cplx(-0.809016994374947451, +0.587785252292473137),
          cplx(-0.809016994374947451, -0.587785252292473137),
          cplx(+0.309016994374947451, +0.951056516295153642),
          cplx(+0.309016994374947451, -0.951056516295153642),
          cplx(+1.000000000000000000, +0.000000000000000000)};
     Vector<cplx> x;
     const int status = solve_polynomial(a, x);
     SHG_ASSERT(status == 0);
     SHG_ASSERT(x.size() == z.size());
     for (Vector<cplx>::size_type i = 0; i < x.size(); i++)
          SHG_ASSERT(std::abs(x(i) - z(i)) < 1e-15);
}

}       // anonymous namespace

void test_numalg() {
     test_solve_linear();
     test_solve_polynomial();
}

}       // namespace Testing
}       // namespace SHG
