/**
 * \file include/shg/gsgts.h
 * Generating stationary Gaussian time series.
 * Created on 26 February 2017.
 */

#ifndef SHG_GSGTS_H
#define SHG_GSGTS_H

#include <complex>
#include <cstddef>
#include <functional>
#include <vector>

namespace SHG {

/**
 * \addtogroup mathematical_statistics
 *
 * \{
 */

/**
 * \defgroup time_series Time series
 *
 * Classes and functions on time series
 * \{
 */

/**
 * Generating stationary Gaussian time series given its autocovariance
 * function. The implementation follows that of \cite davies-2001 (<a
 * href = "StatGaus.pdf">local copy</a>).
 *
 * The elements \f$g_k\f$ in (2) of \cite davies-2001 can be
 * calculated as \f[ g_k = c_0 + 2 \sum_{j = 1}^{n - 1} c_j \cos
 * \frac{\pi j k}{n} + (-1)^k c_n, \quad k = 0, \ldots, n. \f]
 *
 * The elements \f$X_j\f$ in (3) of \cite davies-2001 can be
 * calculated as \f[ X_j = \frac{1}{2 \sqrt{n}} \left[ u_0 \sqrt{g_0}
 * + 2 \sum_{k = 1}^{n - 1} \sqrt{g_k} \left( u_k \cos \frac{2 \pi j
 * k}{2n} - v_k \sin \frac{2 \pi j k}{2n} \right) + (-1)^j u_n
 * \sqrt{g_n} \right], \quad j = 0, \ldots, n, \f] where \f$u_k = \Re
 * z_k\f$, \f$v_k = \Im z_k\f$ for all \f$k\f$.
 *
 * %Sample implementations of cosine transform and real transform
 * using <a href = "http://www.fftw.org/">FFTW library</a> could be as
 * follows:
 *
 * \code
 * void cosft_fftw(std::vector<double> const& h,
 *                 std::vector<double>& H) {
 *      fftw_plan p = fftw_plan_r2r_1d(
 *           h.size(),
 *           const_cast<double*>(h.data()),
 *           H.data(),
 *           FFTW_REDFT00,
 *           FFTW_ESTIMATE);
 *      fftw_execute(p);
 *      fftw_destroy_plan(p);
 *      fftw_cleanup();
 *      for (std::size_t k = 0; k < H.size(); k++)
 *           H[k] *= 0.5;
 * }
 *
 * void realft_fftw(std::vector<std::complex<double>> const& z,
 *                  std::vector<double>& X) {
 *      double const c = 1.0 / (2.0 * std::sqrt(z.size() - 1));
 *      std::size_t const n = 2 * (z.size() - 1);
 *      std::vector<double> out(n);
 *      fftw_plan p = fftw_plan_dft_c2r_1d(
 *           n,
 *           reinterpret_cast<fftw_complex*>(
 *                const_cast<std::complex<double>*>(z.data())),
 *           out.data(),
 *           FFTW_ESTIMATE);
 *      fftw_execute(p);
 *      fftw_cleanup();
 *      for (std::size_t i = 0; i < z.size(); i++)
 *           X[i] = c * out[i];
 *      fftw_destroy_plan(p);
 * }
 * \endcode
 *
 * Implementations using
 * \cite press-teukolsky-vetterling-flannery-2007 could be as follows:
 *
 * \code
 * #define _USESTDVECTOR_
 * #include "nr3.h"
 * #include "fourier.h"
 *
 * void cosft_numrec(std::vector<double> const& h,
 *                   std::vector<double>& H) {
 *      H = h;
 *      cosft1(H);
 * }
 *
 * void realft_numrec(std::vector<std::complex<double>> const& z,
 *                    std::vector<double>& X) {
 *      std::size_t const n = z.size() - 1;
 *      double const c = 1.0 / (2.0 * std::sqrt(n));
 *      std::vector<std::complex<double>> data(2 * z.size() - 2);
 *      for (std::size_t i = 0; i < z.size(); i++)
 *           data[i] = z[i];
 *      for (std::size_t i = z.size(); i < data.size(); i++)
 *           data[i] = std::conj(data[data.size() - i]);
 *      four1(data, 1);
 *      for (std::size_t i = 0; i < z.size(); i++)
 *           X[i] = c * data[i].real();
 * }
 * \endcode
 *
 * \note Given the sequence (1) of \cite davies-2001 we calculate
 * \f$g_k\f$ and \f$Z_k\f$ only for \f$k = 0, \ldots, n\f$ because
 * \f$g_k = g_{2n - k}\f$, \f$Z_k = \overline{Z}_{2n - k}\f$ for \f$n
 * < k < 2n\f$.
 */
class GSGTS {
public:
     /**
      * Prototype for discrete cosine transform passed to constructor.
      */
     typedef void (*Cosine_transform)(std::vector<double> const& h,
                                      std::vector<double>& H);

     /**
      * Prototype for discrete Fourier transform passed to
      * constructor.
      */
     typedef void (*Real_transform)(
          std::vector<std::complex<double>> const& z,
          std::vector<double>& X);

     /**
      * The constructor requires the autocovariance function
      * <tt>acf[0], ..., acf[n]</tt> and a pointer to the function
      * performing cosine transform. If the pointer is nullptr,
      * private function cosft() is used.
      *
      * \exception std::invalid_argument if <tt>n <= 0</tt>, ie.
      * <tt>acf.size() <= 1</tt>
      *
      * \exception std::invalid_argument if the method fails (see
      * \cite davies-2001, page 1).
      *
      * \warning It is not checked if \c acf is an autocovariance
      * function, ie. if it is non-negative definite (see
      * \cite brockwell-davis-2006, section 1.5, page 27).
      */
     GSGTS(std::vector<double> const& acf, Cosine_transform f);

     /**
      * Generates <tt>n + 1</tt> values of stationary Gaussian time
      * series with autocovariance function passed to the constructor,
      * where \c n is the same as in the constructor.
      * <tt>normal()</tt> must be a pointer to a function generating
      * standard normal distribution. \c f must be a pointer to the
      * function performing discrete Fourier transform. If it is
      * nullptr, private function realft() is used. If <tt>X.size() >
      * n + 1</tt>, only elements <tt>X[0], ..., X[n]</tt> are
      * changed.
      *
      * \exception std::invalid_argument if <tt>X.size() < n + 1</tt>
      *
      */
     void generate(std::vector<double>& X,
                   std::function<double()> normal, Real_transform f);

private:
     std::size_t const n;
     std::vector<double> g;

     /**
      * Discrete real Fourier transform used privately if no other
      * function is passed to generate(). For the input vector \f$z_0,
      * \ldots, z_n\f$, where <tt>n = z.size() - 1</tt>, it calculates
      * \f[ X_j = \frac{1}{\sqrt{n}} \left[ \frac{1}{2} z_0 + \sum_{k
      * = 1}^{n - 1} \left( \Re(z_k) \cos \frac{\pi j k}{n} - \Im(z_k)
      * \sin \frac{\pi j k}{n} \right) + \frac{1}{2} (-1)^j z_n
      * \right], \quad j = 0, \ldots, n. \f] Complexity of this
      * function is \f$O(n^2)\f$.
      */
     static void realft(std::vector<std::complex<double>> const& z,
                        std::vector<double>& X);

     /**
      * Discrete cosine transform used privately if no other function
      * is passed to constructor. For the input vector \f$h_0, \ldots,
      * h_n\f$, where <tt>n = h.size() - 1</tt>, it calculates \f[ H_k
      * = \frac{1}{2}h_0 + \sum_{j = 1}^{n - 1} h_j \cos \frac{\pi j
      * k}{n} + \frac{1}{2} (-1)^k h_n, \quad k = 0, \ldots, n. \f]
      * Complexity of this function is \f$O(n^2)\f$.
      * See. \cite press-teukolsky-vetterling-flannery-2007, section
      * 12.4.2, formula 12.4.11.
      */
     static void cosft(std::vector<double> const& h,
                       std::vector<double>& H);
};

/**
 * Returns autocovariance function of AR(1). For x(t) = phi1 * x(t -
 * 1) + e(t), e(t) being white noise with variance sigma2, the
 * function returns the first n elements of autocovariance function:
 * acfar1(0) - variance, acfar1(1) = gamma(1), gamma(h) = acfar1(h), h
 * = 0, 1, ..., n - 1.
 *
 * \throws std::invalid_argument if n < 1 or |phi1| >= 1
 *
 * See \cite brockwell-davis-2006, page 81.
 */
std::vector<double> acfar1(double sigma2, double phi1, size_t n);

/** \} */ /* end of group time_series */

/** \} */ /* end of group mathematical_statistics */

}  // namespace SHG

#endif
