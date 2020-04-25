/* ols.cc: ordinary least squares */

/**
 * \file src/ols.cc
 * Ordinary least squares.
 * \date Created on 28 November 2010.
 */

#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include "shg/drbnwtsn.h"
#include "shg/specfunc.h"
#include "shg/utils.h"          // sqr
#include "shg/ols.h"

namespace SHG {

OLS::Invalid_argument::Invalid_argument()
     : Exception("invalid argument in OLS") {}

OLS::Singular_covariance_matrix::Singular_covariance_matrix()
     : Exception("singular covariance matrix in OLS") {}

OLS::Internal_error::Internal_error()
     : Exception("internal error in OLS") {}

OLS::OLS(const Matdouble& X,
         const Vecdouble& y,
         bool add_intercept) :
     problem_name_(),
     n_(X.nrows()),
     k_(add_intercept ? X.ncols() + 1: X.ncols()),
     dof_(n_ - k_),
     intercept_(add_intercept),
     beta_(k_),
     ser_(),
     r2_(),
     rbar2_(),
     var_(),
     standard_err_(k_),
     cov_(k_, k_),
     fitted_(n_),
     residuals_(n_),
     rss_(0.0),
     tss_(0.0),
     ess_(),
     mean_y_(),
     stddev_y_(0.0),
     positive_y_(true),
     variation_y_(),
     tstat_(k_, -1.0),
     pvalt_(k_),
     fstat_(-1.0),
     pvalf_(),
     dw_d_(-1.0),
     dw_pvalpos_(-1.0),
     dw_pvalneg_(-1.0) {
     if (n_ < 1 || k_ < 1 || n_ < k_ ||
         static_cast<std::size_t>(n_) != y.size())
          throw Invalid_argument();

     estimate(X, y);

     // Check cov_. Is this really needed? At least for cholesky?
     for (int i = 0; i < k_; i++)
          if (!(cov_(i, i) > 0.0))
               throw Singular_covariance_matrix();

     // Calculate mean of dependent variable, standard deviation of
     // dependent variable, coefficient of variation of dependent
     // variable and total sum of squares.

     mean_y_ = y(0);
     for (int i = 1; i < n_; i++) {
          const double yi = y(i);
          const double d = yi - mean_y_;
          mean_y_ += d / (i + 1);
          stddev_y_ += (yi - mean_y_) * d;
          if (yi <= 0.0)
               positive_y_ = false;
     }
     if (!(stddev_y_ >= 0.0))
          throw Internal_error();
     if (add_intercept) {
          tss_ = stddev_y_;
     } else {
          for (int t = 0; t < n_; t++)
               tss_ += sqr(y(t));
     }
     stddev_y_ = std::sqrt(stddev_y_ / n_);
     if (positive_y_)
          variation_y_ = stddev_y_ / mean_y_;

     if (dof_ == 0) {
          // If dof_ == 0, then n_ == k_. We assign:
          fitted_ = y;
          residuals_ = 0.0;
          rss_ = 0.0;
          ess_ = tss_;
          r2_ = 1.0;
          rbar2_ = 1.0;
          var_ = 0.0;
          ser_ = 0.0;
          standard_err_ = 0.0;
          cov_ = 0.0;
          return;
     }

     if (dof_ <= 0 || n_ <= k_)
          throw Internal_error();

     // Calculate fitted values, residuals, rss.
     if (add_intercept) {
          for (int t = 0; t < n_; t++) {
               double s = beta_(0);
               for (int i = 1; i < k_; i++)
                    s += beta_(i) * X(t, i - 1);
               fitted_(t) = s;
               const double e = y(t) - fitted_(t);
               residuals_(t) = e;
               rss_ += e * e;
          }
     } else {
          for (int t = 0; t < n_; t++) {
               double s = 0.0;
               for (int i = 0; i < k_; i++)
                    s += beta_(i) * X(t, i);
               fitted_(t) = s;
               const double e = y(t) - fitted_(t);
               residuals_(t) = e;
               rss_ += e * e;
          }
     }

     ess_ = tss_ - rss_;
     if (ess_ < 0.0) {
          rss_ = tss_;
          ess_ = 0.0;
     }

     var_ = 1.0 / dof_ * rss_;
     ser_ = std::sqrt(var_);

     // Calculate covariance matrix of parameters, standard errors of
     // parameters and t statistics.
     for (int i = 0; i < k_; i++) {
          const double s = standard_err_(i)
               = std::sqrt(cov_(i, i) *= var_);
          const double b = beta_(i);
          if (std::abs(b / std::numeric_limits<double>::max()) < s) {
               tstat_(i) = std::abs(b) / s;
               pvalt_(i) = 2.0 * (1.0 - probst(tstat_(i), dof_));
          }
          for (int j = i + 1; j < k_; j++)
               cov_(j, i) = cov_(i, j) *= var_;
     }

     // Calculate F-statistic.
     const double eps = n_ / std::numeric_limits<double>::max();
     double q;
     if (tss_ > 0.0 && (q = rss_ / tss_) > eps) {
          rbar2_ = 1.0 - static_cast<double>(n_ - 1) / dof_ * q;
          r2_ = 1.0 - q;
          if (add_intercept && k_ > 1)
               // fstat_ = (ess_ / (k_ - 1)) / (rss_ / (n_ - k_));
               fstat_ = (1.0 / q - 1.0) * (n_ - k_) / (k_ - 1);
          else
               // fstat_ = (ess_ / k_) / (rss_ / (n_ - k_));
               fstat_ = (1.0 / q - 1.0) * (n_ - k_) / k_;
          const int dfnum = add_intercept && k_ > 1 ? k_ - 1 : k_;
          pvalf_ = 1.0 - cdffdist(dfnum, n_ - k_, fstat_);
     } else {
          // If tss_ == 0, then rss_ == ess_ == 0. All y(i) are
          // constant. If there is no intercept, then y(i) = 0 for all
          // i and a model with b(i) = 0 explains all. If there is an
          // intercept, then y(i) = const for all i and a model with
          // b(0) = const explains all.
          r2_ = rbar2_ = 1.0;
     }
}

void OLS::dw() {
     if (!intercept_ || k_ <= 1 || n_ < 6 || dof_ < 4 ||
         rss_ < tolerance_)
          return;
     double x = 0.0;
     for (int t = 1; t < n_; t++)
          x += sqr(residuals_[t] - residuals_[t - 1]);
     x /= rss_;
     if (x < 0.0)
          x = 0.0;
     else if (x > 4.0)
          x = 4.0;
     dw_d_ = x;
     try {
          dw_pvalpos_ = dwcdf(n_, k_ - 1, x, true, 0.00001, 15);
     }
     catch (const std::range_error&) {}
     try {
          dw_pvalneg_ = 1.0 - dwcdf(n_, k_ - 1, x, false, 0.00001, 15);
     }
     catch (const std::range_error&) {}
}

void OLS::print(std::ostream& f) const {
     using std::setw;
     const std::string na(w_, '*');      // not applicable
     const std::ios_base::fmtflags opts = f.flags();

     f << std::scientific << std::setprecision(4)
       << "Ordinary least squares estimation results"
       << "\nProblem: "
       << get_problem_name()
       << "\nNumber of observations:          "
       << setw(5) << nobs()
       << "\nNumber of explanatory variables: "
       << setw(5) << nparams()
       << "\nNumber of degrees of freedom:    "
       << setw(5) << dof() << '\n';

     if (intercept())
          f << "The first parameter is an intercept.";
     else
          f << "There is no intercept.";

     f << "\n\nParam.   Estimate    Std. error   t statistic"
          "       p-value\n";
     for (int i = 0; i < nparams(); i++) {
          f << setw(2) << i + 1 << ". "
            << setw(w_) << beta()[i] << " "
            << setw(w_) << standard_err()[i];
          if (tstat()[i] >= 0.0)
               f << " " << setw(w_) << sgn(beta()[i]) * tstat()[i]
                 << " " << setw(w_) << pvalt()[i]
                 << stars(pvalt()[i]);
          else
               f << " " << na << " " << na;
          f << '\n';
     }

     f << "\nStandard error of regression:             "
       << setw(w_) << ser()
       << "\nCoefficient of determination:             "
       << setw(w_) << r2()
       << "\nAdjusted coefficient of determination:    "
       << setw(w_) << rbar2()
       << "\nVariance of residuals:                    "
       << setw(w_) << var()
       << "\nResidual sum of squares:                  "
       << setw(w_) << rss()
       << "\nExplained sum of squares:                 "
       << setw(w_) << ess()
       << "\nTotal sum of squares:                     "
       << setw(w_) << tss()
       << "\nMean of dependent variable:               "
       << setw(w_) << mean_y()
       << "\nStandard deviation of dependent variable: "
       << setw(w_) << stddev_y()
       << "\nVariation of dependent variable:          ";

     if (positive_y())
          f << setw(w_) << variation_y();
     else
          f << na;
     f << "\nF statistic:                              ";
     if (fstat() >= 0.0)
          f << setw(w_) << fstat();
     else
          f << na;
     f << "\np-value:                                  ";
     if (fstat() >= 0.0)
          f << setw(w_) << pvalf() << stars(pvalf());
     else
          f << na;
     f << "\nDurbin-Watson d statistic:                ";
     if (dw_d() >= 0.0)
          f << setw(w_) << dw_d();
     else
          f << na;
     f << "\np-value for positive autocorrelation:     ";
     if (dw_pvalpos() >= 0.0)
          f << setw(w_) << dw_pvalpos() << stars(dw_pvalpos());
     else
          f << na;
     f << "\np-value for negative autocorrelation:     ";
     if (dw_pvalneg() >= 0.0)
          f << setw(w_) << dw_pvalneg() << stars(dw_pvalneg());
     else
          f << na;

     f << '\n';
     f.flags(opts);
}

void OLS::estimate(const Matdouble& X, const Vecdouble& y) {
     Vecdouble xty(k_);
     if (intercept_) {
          int i, i1, j, j1, l;
          double s;
          cov_(0, 0) = n_;
          for (j = 1; j < k_; j++) {
               s = 0.0;
               j1 = j - 1;
               for (l = 0; l < n_; l++)
                    s += X(l, j1);
               cov_(0, j) = s;
          }
          s = 0.0;
          for (l = 0; l < n_; l++)
               s += y(l);
          xty(0) = s;
          for (i = 1; i < k_; i++) {
               i1 = i - 1;
               s = 0.0;
               for (j = i; j < k_; j++) {
                    j1 = j - 1;
                    s = 0.0;
                    for (l = 0; l < n_; l++)
                         s += X(l, i1) * X(l, j1);
                    cov_(i, j) = s;
               }
               s = 0.0;
               for (l = 0; l < n_; l++)
                    s += X(l, i1) * y(l);
               xty(i) = s;
          }
     } else {
          int i, j, l;
          double s;
          for (i = 0; i < k_; i++) {
               for (j = i; j < k_; j++) {
                    s = 0.0;
                    for (l = 0; l < n_; l++)
                         s += X(l, i) * X(l, j);
                    cov_(i, j) = s;
               }
               s = 0.0;
               for (l = 0; l < n_; l++)
                    s += X(l, i) * y(l);
               xty(i) = s;
          }
     }
     try {
          cholesky(cov_, tolerance_);
     } catch (const std::range_error&) {
          throw Singular_covariance_matrix();
     }

     for (std::size_t i = 0; i < cov_.nrows(); i++)
          if (!std::isfinite(cov_(i, i)))
               throw Singular_covariance_matrix();

     for (int i = 0; i < k_; i++) {
          double s = 0.0;
          for (int j = 0; j < k_; j++)
               s += cov_(i, j) * xty(j);
          beta_(i) = s;
     }
}

const char* OLS::stars(double x) {
     if (!(x >= 0.0))
          throw Internal_error();
     if (x < 0.01)
          return " ***";
     else if (x < 0.05)
          return " **";
     else if (x < 0.1)
          return " *";
     else
          return "";
}

const double OLS::tolerance_ =
     100.0 * std::numeric_limits<double>::epsilon();

}       // namespace SHG
