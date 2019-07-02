/* gsgts.cc: testing generating stationary Gaussian time series */

/**
 * \file testing/gsgts.cc
 * Testing generating stationary Gaussian time series.
 * Created on 26 February 2017.
 */

#include <cmath>
#include <fftw3.h>
#include "shg/mzt.h"
#include "shg/gsgts.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

namespace {

const double result1[129] = {
     -0.1853455, -0.1532896, -0.1108232, -0.2036469, -0.1754326,
     -0.1098344, -0.1704000, -0.1704131, -0.1563406, -0.1918795,
     -0.0037222,  0.0975940,  0.0355298, -0.0634263,  0.0374293,
      0.0977770,  0.0736954,  0.2835564,  0.2788326,  0.2212461,
      0.1910693,  0.1717203,  0.1263446,  0.0584663, -0.1037948,
     -0.2123793, -0.3292821, -0.2957615, -0.0796072, -0.0078135,
      0.1049731, -0.1195140,  0.0005337, -0.0356647,  0.2934005,
      0.2276349,  0.4457550,  0.4160869,  0.4105583,  0.1995325,
      0.0474566,  0.1518003,  0.2249002,  0.1612962,  0.1916418,
      0.1283362,  0.0949260,  0.1025253,  0.1695968,  0.3228145,
      0.0881576,  0.2352667,  0.2379230,  0.0875150, -0.1102473,
     -0.2148395, -0.0491461, -0.1555136, -0.0795668,  0.0525286,
      0.0295757,  0.2391820,  0.1089352,  0.2395681,  0.1216882,
      0.2793094,  0.3333728,  0.1691596, -0.0416942,  0.1816499,
      0.4151612,  0.2889556,  0.2731870,  0.3562827,  0.1409797,
      0.1811236,  0.1518109,  0.0900939,  0.1209136,  0.0668564,
      0.1764099,  0.2728195,  0.2470544,  0.1540218,  0.0997584,
      0.1230065, -0.0144507,  0.1192095,  0.1224003,  0.1762245,
      0.1806781,  0.1696920, -0.0684979, -0.1765898, -0.1535255,
     -0.0453403, -0.0492510,  0.0043458,  0.0749827,  0.0774716,
      0.1105852,  0.1633152,  0.3711041,  0.3320104,  0.3874592,
      0.2527283,  0.0685019,  0.0296893, -0.1128779, -0.1337343,
     -0.0479861, -0.0095436, -0.0649834, -0.0609462, -0.1011171,
     -0.1103539, -0.0297745,  0.0183672,  0.1219624,  0.3562367,
      0.4008174,  0.4294983,  0.2980393,  0.1350898, -0.0283490,
     -0.0908774, -0.0056641, -0.0478976,  0.1339543
};

const double result2[80] = {
     -0.6316450, -0.5236583, -0.6450972, -0.6190345, -0.5010952,
     -0.9558759, -0.1537523,  0.4321690, -0.2066403,  0.1795031,
      0.6131393,  1.3266019,  0.8642400,  0.7363808,  0.3988882,
     -0.6462101, -1.5355407, -1.0049679,  0.3972050, -0.2645621,
     -0.3155533,  0.8387987,  1.5328002,  2.0032490,  0.8197588,
      0.0549526,  0.9641574,  0.4819636,  0.5519046,  0.0725050,
      1.2768633,  0.4071369,  1.3226040, -0.3431683, -0.7920558,
     -0.6202403, -0.2992066,  0.3171086,  0.8226870,  0.5258867,
      1.0262011,  1.1781231, -0.4041209,  1.4476297,  1.2358308,
      1.1499674,  0.5927083,  0.4290790,  0.2877266,  0.2509648,
      1.2343851,  0.6633968,  0.4232300, -0.0151701,  0.4706398,
      0.6900624,  0.8829870, -0.7137435, -0.7393445, -0.2582170,
      0.0220044,  0.3048083,  0.4480417,  1.4495389,  1.7003893,
      0.8326820, -0.1140906, -0.6896338, -0.2440786, -0.1348664,
     -0.4987745, -0.4856796, -0.1759956,  0.7788311,  1.9449494,
      1.6053352,  0.3524270, -0.6395870, -0.1899812, -0.0128994
};

/*
 * Returns autocovariance function of AR(1) with white noise variance
 * sigma2 and phi1, ie. x(t) = phi1 * x(t - 1) + e(t), e(t) being
 * white noise with variance sigma2.
 *
 * acfar1(0) - variance
 * acfar1(1) = gamma(1)
 * gamma(h) = acfar1(h), h = 0, 1, ..., n - 1.
 *
 * brockwell-davis-2006, page 81.
 */
std::vector<double> acfar1(double sigma2, double phi1, size_t n) {
     SHG_ASSERT(n > 0);
     SHG_ASSERT(std::abs(phi1) < 1.0);
     std::vector<double> g(n);
     g[0] = sigma2 / (1.0 - phi1 * phi1);
     for (std::vector<double>::size_type i = 1; i < n; i++)
          g[i] = phi1 * g[i - 1];
     return g;
}

const double eps = 5e-8;
const std::vector<double> acf1 = acfar1(1.0 / 64.0, 0.8, 129);
const std::vector<double> acf2 = acfar1(0.5, 0.6, 80);

SHG::MZT mzt;

double normal() {
     return mzt.normal();
}

void test(SHG::GSGTS::Cosine_transform ct,
          SHG::GSGTS::Real_transform rt) {
     std::vector<double>::size_type i;

     mzt = SHG::MZT();
     {
          std::vector<double> X(acf1.size());
          SHG::GSGTS gsgts(acf1, ct);
          gsgts.generate(X, normal, rt);
          SHG_ASSERT(X.size() == acf1.size());
          for (i = 0; i < X.size(); i++)
               SHG_ASSERT(std::abs(X[i] - result1[i]) < eps);
     }
     mzt = SHG::MZT();
     {
          std::vector<double> X(acf2.size());
          SHG::GSGTS gsgts(acf2, ct);
          gsgts.generate(X, normal, rt);
          SHG_ASSERT(X.size() == acf2.size());
          for (i = 0; i < X.size(); i++)
               SHG_ASSERT(std::abs(X[i] - result2[i]) < eps);
     }
}

void cosft_fftw(const std::vector<double>& h,
                std::vector<double>& H) {
     /* FFTW User Manual says: If in != out, the transform is
        out-of-place and the input array in is not modified. */
     fftw_plan p = fftw_plan_r2r_1d(
          h.size(),
          const_cast<double*>(h.data()),
          H.data(),
          FFTW_REDFT00,
          FFTW_ESTIMATE);
     fftw_execute(p);
     fftw_destroy_plan(p);
     fftw_cleanup();
     for (std::size_t k = 0; k < H.size(); k++)
          H[k] *= 0.5;
}

void realft_fftw(const std::vector<std::complex<double>>& z,
                 std::vector<double>& X) {
     const double c = 1.0 / (2.0 * std::sqrt(z.size() - 1));
     const std::size_t n = 2 * (z.size() - 1);
     std::vector<double> out(n);
     fftw_plan p = fftw_plan_dft_c2r_1d(
          n,
          reinterpret_cast<fftw_complex*>(
               const_cast<std::complex<double>*>(z.data())),
          out.data(),
          FFTW_ESTIMATE);
     fftw_execute(p);
     fftw_cleanup();
     for (std::size_t i = 0; i < z.size(); i++)
          X[i] = c * out[i];
     fftw_destroy_plan(p);
}

}       // anonymous namespace

void test_gsgts() {
     test(nullptr, nullptr);
     test(cosft_fftw, realft_fftw);
}

}       // namespace Testing
}       // namespace SHG
