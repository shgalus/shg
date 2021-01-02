#include <shg/packellp.h>
#include <shg/mconsts.h>
#include <shg/utils.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(packellp_test)

namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(min_alpha_rectangle_test, bdata::xrange(36),
                     xr) {
     using SHG::PACKING::min_alpha_rectangle;
     std::vector<Ellipse> v = {
          {4.0, 2.0, 0.0, 0.0, 0.0},
          {2.0, 1.0, 0.5 * Constants::pi<double>, 5.0, 0.0}};
     Rectangle r{10.0, 4.0, 0.0, 1.0, 0.0};
     const double phi = degrees_to_radians(5.0 * xr);
     const double alpha =
          xr < 18 ? phi : phi - 0.5 * Constants::pi<double>;
     const Point p{10.0, 10.0};

     for (auto it = v.begin(); it != v.end(); ++it)
          it->rotate(phi, p);
     r.rotate(phi, p);

     double tol;
     switch (xr) {
          case 14:
          case 24:
          case 34:
               tol = 1e-13;
               break;
          default:
               tol = tolerance;
               break;
     }

     auto r1 = min_alpha_rectangle(v, alpha, tol);
     BOOST_CHECK(equal(r1, r, tol));
}

BOOST_DATA_TEST_CASE(is_feasible_packing_test, bdata::xrange(21),
                     xr) {
     using SHG::PACKING::is_feasible_packing;
     std::vector<Ellipse> v = {{4.0, 2.0, 0.0, 0.0, 0.0},
                               {2.0, 1.0, 0.5 * Constants::pi<double>,
                                4.9 + xr * 0.01, 0.0}};
     bool feas = is_feasible_packing(v);
     BOOST_CHECK(feas == (xr >= 10));
}

BOOST_AUTO_TEST_CASE(min_rectangle_test) {
     using SHG::PACKING::min_rectangle;
     std::vector<Ellipse> v = {
          {4.0, 2.0, 0.0, 0.0, 0.0},
          {2.0, 1.0, 0.5 * Constants::pi<double>, 5.0, 0.0}};
     Rectangle r{10.0, 4.0, 0.0, 1.0, 0.0};

     auto r1 = min_rectangle(v);
     BOOST_CHECK(equal(r1, r, tolerance));
}

BOOST_DATA_TEST_CASE(min_rectangle_test2, bdata::xrange(36), xr) {
     using SHG::PACKING::min_rectangle;
     std::vector<Ellipse> v = {
          {4.0, 2.0, 0.0, 0.0, 0.0},
          {2.0, 1.0, 0.5 * Constants::pi<double>, 5.0, 0.0}};
     Rectangle r{10.0, 4.0, 0.0, 1.0, 0.0};
     const double phi = degrees_to_radians(5.0 * xr);
     const Point p{10.0, 10.0};

     for (auto it = v.begin(); it != v.end(); ++it)
          it->rotate(phi, p);
     r.rotate(phi, p);

     double tol;
     switch (xr) {
          case 0:
               tol = tolerance;
               break;
          default:
               tol = 0.015;
               break;
     }

     auto r1 = min_rectangle(v);
     BOOST_CHECK(equal(r1, r, tol));
}

BOOST_AUTO_TEST_CASE(min_rectangle_for_semiaxes_test) {
     using SHG::PACKING::min_rectangle;
     using SHG::PACKING::Semiaxes;
     std::vector<Ellipse> ellipses;
     const std::vector<Semiaxes> semiaxes{{4.0, 2.0}, {2.0, 1.0}};

     Rectangle r0{10.0142, 4.00239, 0.0199466, 5.00749, 0.0200988};

     auto r = min_rectangle(semiaxes, ellipses);
     BOOST_CHECK(equal(r, r0, 1e-5));
}

BOOST_AUTO_TEST_CASE(min_rectangle_for_semiaxes_3_ellipses_test) {
     using SHG::PACKING::min_rectangle;
     using SHG::PACKING::Semiaxes;
     std::vector<Ellipse> e;
     const std::vector<Semiaxes> semiaxes{
          {4.0, 2.0}, {4.0, 2.0}, {0.1, 0.05}};

     const Rectangle r0{1.6149905e+01, 4.0000053e+00, 0.0000000e+00,
                        8.0749527e+00, -2.6463902e-06};
     const std::vector<Ellipse> e0{
          {4.0000000e+00, 2.0000000e+00, 0.0000000e+00, 4.0000000e+00,
           0.0000000e+00},
          {4.0000000e+00, 2.0000000e+00, 0.0000000e+00, 1.2000020e+01,
           -5.2927805e-06},
          {1.0000000e-01, 5.0000000e-02, 1.5627932e+00, 1.6099901e+01,
           -1.8756351e-01}};

     auto r = min_rectangle(semiaxes, e);
     BOOST_CHECK(equal(r, r0, 1e-6));
     for (std::vector<Ellipse>::size_type i = 0; i < e.size(); i++)
          BOOST_CHECK(equal(e[i], e0[i], 1e-6));
}

struct Congruent_regular_test_case {
     const int n;
     const double a;
     const double b;
     const std::vector<int> p;
};

std::ostream& operator<<(std::ostream& stream,
                         const Congruent_regular_test_case& c) {
     stream.width(0);
     stream << c.n << '\n';
     return stream;
}

const std::vector<Congruent_regular_test_case> crtc{
     {1, 2.0, 1.0, {1}},        {2, 2.0, 1.0, {2}},
     {3, 2.0, 1.0, {3}},        {4, 2.0, 1.0, {4}},
     {5, 2.0, 1.0, {5}},        {6, 2.0, 1.0, {6}},
     {7, 2.0, 1.0, {7}},        {8, 2.0, 1.0, {8}},
     {9, 2.0, 1.0, {9}},        {10, 2.0, 1.0, {10}},
     {11, 2.0, 1.0, {4, 3, 4}}, {12, 2.0, 1.0, {12}},
     {13, 2.0, 1.0, {13}},      {14, 2.0, 1.0, {5, 4, 5}},
     {15, 2.0, 1.0, {8, 7}},    {16, 2.0, 1.0, {8, 8}},
     {17, 2.0, 1.0, {6, 5, 6}}, {18, 2.0, 1.0, {9, 9}},
     {19, 2.0, 1.0, {10, 9}},   {20, 2.0, 1.0, {7, 6, 7}},
     {21, 2.0, 1.0, {7, 7, 7}}, {22, 2.0, 1.0, {11, 11}},
     {23, 2.0, 1.0, {8, 7, 8}}, {24, 2.0, 1.0, {8, 8, 8}}};

BOOST_DATA_TEST_CASE(congruent_regular_test, bdata::make(crtc), c) {
     using SHG::PACKING::Congruent_regular;
     Congruent_regular cr(c.n, c.a, c.b);
     cr.run();
     BOOST_CHECK(cr.p() == c.p);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
