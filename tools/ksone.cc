/**
 * \file tools/ksone.cc
 * Results for ksnormtest_test in tests/mstat_test.cc.
 */

/*
 * Output:
 *
 * 1.08037435377046e-02   9.73012808686660e-01
 * 6.52407728998911e-02   7.35239129875459e-08
 */

#include <iomanip>
#include <iostream>
#include <nr/nr3.h>
#include <nr/sort.h>
#include <nr/ksdist.h>
#include <nr/kstests.h>
#include <nr/erf.h>
#include <nr/moment.h>
#include <shg/mzt.h>

int const n = 2000;
double mu, sigma;

double F(double x) {
     return Normaldist(mu, sigma).cdf(x);
}

void set(VecDoub const& x) {
     double ave, adev, sdev, var, skew, curt;
     moment(x, ave, adev, sdev, var, skew, curt);
     var = (n - 1) * var / n;
     mu = ave;
     sigma = sqrt(var);
}

int main() {
     SHG::MZT g;
     VecDoub x(n);
     double d, prob;
     cout << scientific << setprecision(14);

     for (int i = 0; i < n; i++)
          x[i] = 3 + 0.5 * g.normal();
     set(x);
     ksone(x, F, d, prob);
     cout << d << "   " << prob << '\n';

     for (int i = 0; i < n; i++)
          x[i] = g();
     set(x);
     ksone(x, F, d, prob);
     cout << d << "   " << prob << '\n';
}
