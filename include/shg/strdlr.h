/* strdlr.h: NIST StRD Linear Regression */

/**
 * \file include/shg/strdlr.h
 * NIST StRD Linear Regression.
 * \date Created on 12 August 2014.
 */

#ifndef SHG_STRDLR_H
#define SHG_STRDLR_H

#include <cstddef>
#include <vector>
#include "shg/matrix.h"

namespace SHG {

/**
 * NIST Statistical Reference Datasets Linear Regression. For
 * additional information, see page \ref strdlr.
 *
 * \ingroup mathematical_statistics
 */
class StRDLR {
public:
     enum Dataset {norris, pontius, noint1, noint2, filip, longley,
                   wampler1, wampler2, wampler3, wampler4, wampler5};

     /** \name Data. */
     /** \{ */
     std::size_t nsets() {return v_.size();}    ///< number of datasets
     void active(int i);                        ///< sets active dataset
     const char* name() {return d_->name;}      ///< dataset name
     std::size_t n() {return d_->n;}            ///< n. of observations
     std::size_t k() {return d_->k;}            ///< n. of columns in X
     /**
      * predictor variables
      *
      * \note Returned matrix does not contain the column of ones
      * corresponding to the parameter B0. The column should be added
      * by the user. When using SHG::OLS, this column is automatically
      * added depending on add_intercept argument.
      *
      */
     Matdouble X();                             ///< predictor variables
     const Vecdouble& y() {return d_->y;}       ///< response variable
     /** \} */
     /** \name Certified values. */
     /** \{ */
     const Vecdouble& b() {return d_->b;}       ///< estimates
     const Vecdouble& sdevb() {return d_->sdevb;}  ///< std. deviations
     double sdevres() {return d_->sdevres;}     ///< residual std. dev.
     double r2() {return d_->r2;}               ///< R-squared
     int dfreg() {return d_->dfreg;}            ///< regression deg. f.
     double ssreg() {return d_->ssreg;}         ///< regression SS
     double msreg() {return d_->msreg;}         ///< regression mean sq.
     double fstat() {return d_->fstat;}         ///< F statistic
     int dfres() {return d_->dfres;}            ///< residual deg. f.
     double ssres() {return d_->ssres;}         ///< residual SS
     double msres() {return d_->msres;}         ///< residual mean sq.
     /** \} */
private:
     struct Data {
          const char* const name;    ///< problem name
          // data
          std::size_t n;             ///< number of observations
          std::size_t k;             ///< number of columns in X
          Matdouble X;               ///< predictor variables
          Vecdouble y;               ///< response variable
          // certified values
          Vecdouble b;               ///< estimates
          Vecdouble sdevb;           ///< standard deviations of est.
          double sdevres;            ///< residual standard deviation
          double r2;                 ///< R-squared
          int dfreg;                 ///< regression degrees of freedom
          double ssreg;              ///< regression sum of squares
          double msreg;              ///< regression mean squares
          double fstat;              ///< F statistic
          int dfres;                 ///< residual degrees of freedom
          double ssres;              ///< residual sum of squares
          double msres;              ///< residual mean squares
     };

     const Data* d_ {&v_[0]};
     static const std::vector<Data> v_;
};

}       // namespace SHG

#endif
