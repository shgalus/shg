/**
 * \file include/shg/ols.h
 * Ordinary least squares.
 * \date Created on 28 November 2010.
 */

#ifndef SHG_OLS_H
#define SHG_OLS_H

#include <string>
#include "shg/except.h"
#include "shg/matrix.h"
#include "shg/utils.h"

namespace SHG {

/**
 * Ordinary Least Squares. The class estimates a multiple regression
 * model by the least squares method and provides summary statistics.
 *
 * Let \f$y = X\beta + u\f$ be a multiple linear regression model
 * \cite maddala-2006, chapter 4. \f$y\f$ is an \f$n \times 1\f$
 * vector of \f$n \geq 1\f$ observations on the dependent variable, X
 * is an \f$n \times k\f$, \f$1 \leq k \leq n\f$, matrix of
 * observations on the indepenent variables, \f$\beta\f$ is a \f$k
 * \times 1\f$ vector of unknown parameters and \f$u\f$ is an \f$n
 * \times 1\f$ vector of disturbances. The least squares estimator of
 * \f$\beta\f$ is \f$\hat{\beta} = (X^TX)^{-1}X^Ty\f$.
 *
 * Methods for prediction may be added.
 *
 * \ingroup mathematical_statistics
 */
class OLS {
public:
     /**
      * Type of exceptions thrown by this class.
      */
     struct Error : public virtual Exception {};

     /**
      * Exception thrown by the constructor in case of invalid
      * dimensions of arguments.
      */
     struct Invalid_argument : public Error {
          Invalid_argument();
     };

     /**
      * Exception thrown by the constructor if the matrix \f$X^TX\f$
      * is singular.
      */
     struct Singular_covariance_matrix : public Error {
          Singular_covariance_matrix();
     };

     /**
      * Exception thrown by the constructor in case of an internal
      * error.
      */
     struct Internal_error : public Error {
          Internal_error();
     };

     /**
      * Constructs and performs the majority of computations.
      *
      * \param [in] X the matrix \f$X\f$
      * \param [in] y the vector \f$y\f$
      * \param [in] add_intercept if it is true, the intercept is
      * added to the regression model, ie. a column of ones is added
      * to \f$X\f$, the remaining colums are shifted to the right and
      * \f$k\f$ is increased by one
      *
      * \exception SHG::OLS::Invalid_argument if \f$n < 1\f$ or \f$k <
      * 1\f$ or \f$n < k\f$ or \f$X\f$ and \f$y\f$ have different
      * number of rows
      *
      * \exception SHG::OLS::Singular_covariance_matrix if the matrix
      * \f$X^TX\f$ is singular
      *
      * \exception SHG::OLS::Internal_error in case of an internal
      * error
      */
     OLS(const Matdouble& X,
         const Vecdouble& y,
         bool add_intercept = true);
     OLS(const OLS&) = delete;
     OLS& operator=(const OLS&) = delete;

     /**
      * Sets problem name.
      */
     void set_problem_name(const std::string& s) {problem_name_ = s;}

     /**
      * Returns the problem name.
      */
     const std::string& get_problem_name() const {return problem_name_;}

     /**
      * Returns the number of observations \f$n\f$.
      */
     int nobs() const {return n_;}

     /**
      * Returns the number of parameters \f$k\f$.
      *
      * \warning If the object was constructed with \a add_intercept set
      * to true, \f$k\f$ is greater by one than the number of columns in
      * \f$X\f$.
      *
      */
     int nparams() const {return k_;}

     /**
      * Returns the number of degrees of freedom \f$n - k\f$.
      */
     int dof() const {return dof_;}

     /**
      * Returns true if and only if the object was constructed with \a
      * add_intercept set to true.
      */
     bool intercept() const {return intercept_;}

     /**
      * Returns the vector of estimated parameters \f$\hat{\beta} =
      * (X^TX)^{-1}X^Ty\f$. \a beta()[i] is an estimate of \f$\beta_{i
      * + 1}\f$ for \f$0 \leq i < k\f$.
      */
     const Vecdouble& beta() const {return beta_;}

     /**
      * Returns vector of fitted values \f$\hat{y} = X \hat{\beta}\f$.
      */
     const Vecdouble& fitted() const {return fitted_;}

     /**
      * Returns the vector of residuals \f$\hat{u} = y - \hat{y}\f$.
      */
     const Vecdouble& residuals() const {return residuals_;}

     /**
      * Returns residual sum of squares \f$\mathit{RSS} =
      * \hat{u}^T\hat{u} = \sum_{i = 1}^n \hat{u_i}^2\f$.
      */
     double rss() const {return rss_;}

     /**
      * Returns total sum of squares. Total sum of squares is defined
      * by \f[ \mathit{TSS} = \left\{ \begin{array}{ll} \sum_{i = 1}^n
      * (y_i - \bar{y})^2 & \mbox{if there is an intercept,} \\
      * \sum_{i = 1}^n y_i^2 = y^Ty & \mbox{if there is no intercept.}
      * \end{array} \right. \f]
      */
     double tss() const {return tss_;}

     /**
      * Returns explained sum of squares \f$\mathit{ESS} =
      * \mathit{TSS} - \mathit{RSS}\f$.
      */
     double ess() const {return ess_;}

     /**
      * Returns coefficient of determination \f$R^2 = 1 - \mathit{RSS}
      * / \mathit{TSS}\f$.
      */
     double r2() const {return r2_;}

     /**
      * Returns adjusted coefficient of determination. It is defined
      * by \f[ \bar{R}^2 = 1 - \frac{n - 1}{n - k}(1 - R^2).\f]
      */
     double rbar2() const {return rbar2_;}

     /**
      * Returns variance of residuals. It is defined by
      * \f[\hat{\sigma}^2 = \frac{1}{n - k}\hat{u}^T\hat{u}\f] if \f$k
      * < n\f$, 0 otherwise.
      */
     double var() const {return var_;}

     /**
      * Returns standard error of residuals. It is defined by
      * \f$\hat{\sigma} = \sqrt{\hat{\sigma}^2}\f$.
      */
     double ser() const {return ser_;}

     /**
      * Returns estimated covariance matrix of parameters \f$\hat{D}^2
      * \hat{\beta} = \hat{\sigma}^2 (X^TX)^{-1}\f$.
      */
     const Matdouble& cov() const {return cov_;}

     /**
      * Returns standard errors of coefficients
      * \f$\hat{D}\hat{\beta_i}\f$. \a standard_err()[i] is an
      * estimate of \f$\hat{D}\hat{\beta}_{i + 1}\f$ for \f$0 \leq i <
      * k\f$.
      */
     const Vecdouble& standard_err() const {return standard_err_;}

     /**
      * Returns mean of dependent variable \f$\bar{y}\f$.
      */
     double mean_y() const {return mean_y_;}

     /**
      * Returns standard deviation of dependent variable. It is
      * defined by \f[ \frac{1}{n} \sum_{i = 1}^n (y_i - \bar{y})^2.
      * \f]
      */
     double stddev_y() const {return stddev_y_;}

     /**
      * Returns true if \f$y_i > 0\f$ for all \f$1 \leq i \leq n\f$.
      */
     bool positive_y() const {return positive_y_;}

     /**
      * Returns coefficient of variation of dependent variable. It is
      * defined by stddev_y() / mean_y() if positive_y() returns \a
      * true, otherwise it is set to 0.
      */
     double variation_y() const {return variation_y_;}

     /**
      * Returns the \f$F\f$-statistic. If there is an intercept and at
      * leat one additional independent variable, then the
      * \f$F\f$-statistic is defined as \f[ F = \frac{\mathit{ESS} /
      * (k - 1)}{RSS / (n - k)}. \f] If there is no intercept or only
      * the intercept, \f$F\f$-statistic is defined as \f[ F =
      * \frac{\mathit{ESS} / k}{RSS / (n - k)}. \f] In the first case
      * the zero hypothesis says that all independent variables
      * (without intercept) do not influence dependent variable. In
      * the second case it says that all independent variables do not
      * influence dependent variable. \sa pvalf()
      */
     double fstat() const {return fstat_;}

     /**
      * Returns \f$p\f$-value for the \f$F\f$-statistic. If the
      * hypothesis \f$H_0 \colon \beta_1 = \ldots = \beta_k = 0\f$ is
      * true, \f$F\f$ has the Fisher-Snedecor \f$F(k, n - k)\f$
      * distribution. The \f$p\f$-value is the probability, that the
      * variable with \f$F(k, n - k)\f$ distribution takes on a value
      * greater than calculated \f$F\f$. \sa fstat()
      */
     double pvalf() const {return pvalf_;}

     /**
      * Returns vector of \f$t\f$-statistics \f$t_i = \hat{\beta}_i /
      * \hat{D}\hat{\beta}_i\f$. \a tstat()[i] is the
      * \f$t\f$-statistic for the parameter \f$\hat{\beta}_{i + 1}\f$
      * for \f$0 \leq i < k\f$. \sa pvalt()
      */
     const Vecdouble& tstat() const {return tstat_;}

     /**
      * Returns \f$p\f$-values for the \f$t\f$-statistics. If the
      * hypothesis \f$H_0 \colon \beta_i = 0 \f$ is true, \f$t_i\f$
      * has the Student \f$t(n - k)\f$ distribution. The \f$p\f$-value
      * is the probability that the variable with \f$t(n - k)\f$
      * distribution takes on a value grater than calculated
      * \f$t_i\f$. pvalt()[i] is the \f$p\f$-value for the statistic
      * tstat()[i]. \sa tstat()
      */
     const Vecdouble& pvalt() const {return pvalt_;}

     /**
      * Calculates the Durbin-Watson \f$d\f$ statistic and its
      * \f$p\f$-values for positive and negative autocorrelation. To
      * check if the calculation succeeded, check dw_d(), dw_pvalpos()
      * and dw_pvalneg().
      *
      * \implementation
      *
      * First it is checked if the intercept and at least one
      * regressor are included. Then it is checked if the number of
      * observations is at least 6 and (n_ - (k_ - 1)) >= 5 (these two
      * conditions are present in tables \cite savin-white-1977). Then
      * it is checked if rss is greater than 0. If any of these
      * condition is not met, the function returns and OLS::dw_d_,
      * OLS::dw_pvalpos_ and OLS::dw_pvalneg_ are not changed. Then
      * the statistic d is calculated and assigned to OLS::dw_d_. Then
      * p-value for positive autocorrelation is calculated and
      * assigned to OLS::dw_pvalpos_; if the calculation fails,
      * OLS::dw_pvalpos_ is not changed. Then p-value for negative
      * autocorrelation is calculated and assigned to
      * OLS::dw_pvalneg_; if the calculation fails, OLS::dw_pvalneg_
      * is not changed.
      *
      * p-values are calculating as follows. It is clear that if
      * random variables are such that \f$X \leq Y\f$, then \f$\Pr(Y <
      * z) \leq \Pr(X < z)\f$. For the inequality \f$d_L \leq d \leq
      * d_U\f$ this means that \f$\Pr(d_U < x) \leq \Pr(d < x) \leq
      * \Pr(d_L < x)\f$.
      *
      * For testing \f$H_0 \colon \rho_1 = 0\f$ against \f$H_1 \colon
      * \rho_1 > 0\f$, under \f$H_0\f$, low (near 0) values of \f$d\f$
      * should reject \f$H_0\f$ as \f$\Pr(d < x) \leq \Pr(d_L < x) =
      * \mathit{dwcdf}(x, \mathit{true})\f$. So, we accept
      * \f$\mathit{dwcdf}(x, \mathit{true})\f$ as a p-value even
      * though it is only an upper bound for the true p-value.
      *
      * For testing \f$H_0 \colon \rho_1 = 0\f$ against \f$H_1 \colon
      * \rho_1 < 0\f$, under \f$H_0\f$, high (near 4) values of
      * \f$d\f$ should reject \f$H_0\f$ as \f$\Pr(d \geq x) \leq
      * \Pr(d_U \geq x) = 1 - \Pr(d_U < x) = 1 - \mathit{dwcdf}(x,
      * \mathit{false})\f$. So, we accept \f$1 - \mathit{dwcdf}(x,
      * \mathit{false})\f$ as a p-value even though it is only an
      * upper bound for the true p-value.
      *
      * In both cases true p-values are lower then their upper bounds.
      * On print, it should be marked that these are upper bounds, not
      * p-values.
      */
     void dw();

     /**
      * Returns the Durbin-Watson \f$d\f$ statistic. This should be in
      * the interval \f$[0, 4]\f$. If the returned value is less than
      * 0, either dw() has not been called or it has failed.
      */
     double dw_d() const {return dw_d_;}

     /**
      * Returns \f$p\f$-value for positive autocorrelation of the
      * calculated Durbin-Watson \f$d\f$ statistic. If the returned
      * value is less than 0, either dw() has not been called or it
      * has failed.
      */
     double dw_pvalpos() const {return dw_pvalpos_;}

     /**
      * Returns \f$p\f$-value for negative autocorrelation of the
      * calculated Durbin-Watson \f$d\f$ statistic. If the returned
      * value is less than 0, either dw() has not been called or it
      * has failed.
      */
     double dw_pvalneg() const {return dw_pvalneg_;}

     /**
      * Prints results of estimation to the stream. The stream must
      * open for output. Formatting flags of the stream are not
      * changed on exit.
      */
     void print(std::ostream& f) const;

private:
     /**
      * Performs initial part of estimation.
      */
     void estimate(const Matdouble& X, const Vecdouble& y);

     /**
      * Returns stars to mark significance.
      */
     static const char* stars(double x);

     static const double tolerance_;
     static const int w_ = 13;  ///< field width to output float

     std::string
     problem_name_;             ///< used by print
     const int n_;              ///< number of observations
     const int k_;              ///< number of estimated parameters
     const int dof_;            ///< number of degrees of freedom
     const bool intercept_;     ///< true if intercept was added
     Vecdouble beta_;           ///< estimated parameters
     double ser_;               ///< standard error of regeression
     double r2_;                ///< coefficient of determination
     double rbar2_;             ///< adjusted coeff. of determination
     double var_;               ///< variance of residuals
     Vecdouble standard_err_;   ///< standard errors of parameters
     Matdouble cov_;            ///< covariance matrix of parameters
     Vecdouble fitted_;         ///< fitted values
     Vecdouble residuals_;      ///< residuals
     double rss_;               ///< residual sum of squares
     double tss_;               ///< total sum of squares
     double ess_;               ///< explained sum of squares
     double mean_y_;            ///< mean of dependent variable
     double stddev_y_;          ///< std. deviation of dependent var.
     bool positive_y_;          ///< true if all y are positive
     double variation_y_;       ///< coef. of variation of dep. var.
     Vecdouble tstat_;          ///< t statistics
     Vecdouble pvalt_;          ///< p-values for t statistics
     double fstat_;             ///< F statistic
     double pvalf_;             ///< p-value for F statistic
     double dw_d_;              ///< Durbin-Watson statistic
     double dw_pvalpos_;        ///< p-value positive autocorrelation
     double dw_pvalneg_;        ///< p-value negative autocorrelation
};

}       // namespace SHG

#endif
