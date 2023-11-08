#include <shg/geometry.h>
#include <iomanip>
#include <sstream>
#include <shg/mconsts.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(geometry_test)

using SHG::faeq;
using SHG::tolerance;
using SHG::Point;
using SHG::distance;
using SHG::equal;
using SHG::is_less;
using SHG::Line;
using SHG::Mutual_position;
using SHG::mutual_position;
using SHG::Rectangle;
using SHG::Vecpoint;
using SHG::Ellipse;

constexpr double pi = SHG::Constants::pi<double>;

BOOST_AUTO_TEST_CASE(radians_to_degrees_test) {
     using SHG::radians_to_degrees;
     constexpr double eps = min_dbl;
     double deg;

     deg = radians_to_degrees(0.0);
     BOOST_CHECK(faeq(deg, 0.0, eps));

     deg = radians_to_degrees(0.5 * pi);
     BOOST_CHECK(faeq(deg, 90.0, eps));
     deg = radians_to_degrees(-0.5 * pi);
     BOOST_CHECK(faeq(deg, -90.0, eps));

     deg = radians_to_degrees(pi);
     BOOST_CHECK(faeq(deg, 180.0, eps));
     deg = radians_to_degrees(-pi);
     BOOST_CHECK(faeq(deg, -180.0, eps));

     deg = radians_to_degrees(2.0 * pi);
     BOOST_CHECK(faeq(deg, 360.0, eps));
     deg = radians_to_degrees(-2.0 * pi);
     BOOST_CHECK(faeq(deg, -360.0, eps));

     deg = radians_to_degrees(200.0 * pi);
     BOOST_CHECK(faeq(deg, 36000.0, eps));
     deg = radians_to_degrees(-200.0 * pi);
     BOOST_CHECK(faeq(deg, -36000.0, eps));

     deg = std::numeric_limits<double>::max();

     BOOST_CHECK(!std::isfinite(radians_to_degrees(deg)));
     BOOST_CHECK(!std::isfinite(radians_to_degrees(-deg)));
}

BOOST_AUTO_TEST_CASE(degrees_to_radians_test) {
     using SHG::degrees_to_radians;
     constexpr double eps = min_dbl;
     double rad;

     rad = degrees_to_radians(0.0);
     BOOST_CHECK(faeq(rad, 0.0, eps));

     rad = degrees_to_radians(90.0);
     BOOST_CHECK(faeq(rad, 0.5 * pi, eps));
     rad = degrees_to_radians(-90.0);
     BOOST_CHECK(faeq(rad, -0.5 * pi, eps));

     rad = degrees_to_radians(180.0);
     BOOST_CHECK(faeq(rad, pi, eps));
     rad = degrees_to_radians(-180.0);
     BOOST_CHECK(faeq(rad, -pi, eps));

     rad = degrees_to_radians(360.0);
     BOOST_CHECK(faeq(rad, 2.0 * pi, eps));
     rad = degrees_to_radians(-360.0);
     BOOST_CHECK(faeq(rad, -2.0 * pi, eps));

     rad = degrees_to_radians(36000.0);
     BOOST_CHECK(faeq(rad, 200.0 * pi, eps));
     rad = degrees_to_radians(-36000.0);
     BOOST_CHECK(faeq(rad, -200.0 * pi, eps));

     rad = std::numeric_limits<double>::max();

     BOOST_CHECK(std::isfinite(degrees_to_radians(rad)));
     BOOST_CHECK(std::isfinite(degrees_to_radians(-rad)));
}

BOOST_AUTO_TEST_CASE(point_test) {
     using std::sqrt;
     constexpr double eps = tolerance;

     Point p;
     BOOST_CHECK(p.x() == 0.0);
     BOOST_CHECK(p.y() == 0.0);

     Point q{1.0, 2.0};
     BOOST_CHECK(q.x() == 1.0);
     BOOST_CHECK(q.y() == 2.0);

     p.move_to(q);
     BOOST_CHECK(p.x() == 1.0);
     BOOST_CHECK(p.y() == 2.0);

     p.move_by(1.0, -1.0);
     BOOST_CHECK(p.x() == 2.0);
     BOOST_CHECK(p.y() == 1.0);

     p = {1.0, 2.0};
     p.rotate(pi / 3.0, Point(0.0, 0.0));
     BOOST_CHECK(faeq(p.x(), 0.5 - sqrt(3.0), eps));
     BOOST_CHECK(faeq(p.y(), 1.0 + 0.5 * sqrt(3.0), eps));

     p = {1.0, 2.0};
     p.rotate(pi / 3.0, Point(3.0, 4.0));
     BOOST_CHECK(faeq(p.x(), 2.0 + sqrt(3.0), eps));
     BOOST_CHECK(faeq(p.y(), 3.0 - sqrt(3.0), eps));

     p = {1.0, 2.0};
     p.rotate(pi / 3.0, Point(1.0, 2.0));
     BOOST_CHECK(faeq(p.x(), 1.0, eps));
     BOOST_CHECK(faeq(p.y(), 2.0, eps));

     p = {1.0, 1.0};
     q = {4.0, 1.0};
     Point r{4.0, 5.0};
     BOOST_CHECK(faeq(distance(p, q), 3.0, eps));
     BOOST_CHECK(faeq(distance(q, r), 4.0, eps));
     BOOST_CHECK(faeq(distance(p, r), 5.0, eps));

     p = {1.0, 1.0};
     q = {1.0, 1.0001};
     BOOST_CHECK(equal(p, q, 0.00011));
     BOOST_CHECK(!equal(p, q, 0.00009));

     p = {1.0, 1.0};
     q = {4.0, 1.0};
     r = {4.0, 5.0};
     BOOST_CHECK(is_less(p, q));
     BOOST_CHECK(is_less(p, r));
     BOOST_CHECK(is_less(q, r));
     BOOST_CHECK(!is_less(q, p));
     BOOST_CHECK(!is_less(r, p));
     BOOST_CHECK(!is_less(r, q));
     BOOST_CHECK(!is_less(p, p));

     std::ostringstream oss;
     oss << std::fixed << std::setprecision(4) << p << '\n';
     BOOST_CHECK(oss.str() == "Point(1.0000, 1.0000)\n");
     oss.str("");
     oss << std::fixed << std::setprecision(4) << q << '\n';
     BOOST_CHECK(oss.str() == "Point(4.0000, 1.0000)\n");
     oss.str("");
     oss << std::fixed << std::setprecision(4) << r << '\n';
     BOOST_CHECK(oss.str() == "Point(4.0000, 5.0000)\n");
}

BOOST_AUTO_TEST_SUITE(line_test)

BOOST_AUTO_TEST_CASE(constructor_test) {
     Line line;
     BOOST_CHECK(line.A() == 0.0);
     BOOST_CHECK(line.B() == 1.0);
     BOOST_CHECK(line.C() == 0.0);

     line = {1.0, -1.0, 0.0};
     BOOST_CHECK(line.A() == 1.0);
     BOOST_CHECK(line.B() == -1.0);
     BOOST_CHECK(line.C() == 0.0);

     BOOST_CHECK_THROW(Line(0.0, 0.0, 0.0), std::invalid_argument);
     BOOST_CHECK_THROW(Line(0.0, 0.0, 1.0), std::invalid_argument);

     BOOST_CHECK(
          equal({{0.0, 1.0}, {0.0, 2.0}}, {1.0, 0.0, 0.0}, min_dbl));
     BOOST_CHECK(
          equal({{1.0, 0.0}, {2.0, 0.0}}, {0.0, 1.0, 0.0}, min_dbl));
     BOOST_CHECK(
          equal({{0.0, 0.0}, {1.0, 1.0}}, {1.0, -1.0, 0.0}, min_dbl));
}

BOOST_AUTO_TEST_CASE(method_test) {
     Line line;

     line = {0.0, 1.0, 0.0};
     BOOST_CHECK(equal(line.perpendicular({1.0, 0.0}),
                       {1.0, 0.0, -1.0}, min_dbl));
     BOOST_CHECK(equal(line.perpendicular({1.0, 1.0}),
                       {1.0, 0.0, -1.0}, min_dbl));
     BOOST_CHECK(!equal(line.perpendicular({1.1, 0.0}),
                        {1.0, 0.0, -1.0}, min_dbl));

     line = {1.0, -1.0, 0.0};
     line.move_to({0.0, 1.0});
     BOOST_CHECK(equal(line, {1.0, -1.0, 1.0}, min_dbl));

     line = {1.0, -1.0, 0.0};
     line.move_by(2.0, 3.0);
     BOOST_CHECK(equal(line, {1.0, -1.0, 1.0}, min_dbl));

     line = {0.0, 1.0, 0.0};
     line.rotate(0.5 * pi, {0.0, 0.0});
     BOOST_CHECK(equal(line, {1.0, 0.0, 0.0}));

     line = {0.0, 1.0, 0.0};
     line.rotate(pi, {0.0, 0.0});
     BOOST_CHECK(equal(line, {0.0, 1.0, 0.0}));

     line = {0.0, 1.0, 0.0};
     line.rotate(1.5 * pi, {0.0, 0.0});
     BOOST_CHECK(equal(line, {1.0, 0.0, 0.0}));

     line = {1.0, -1.0, -1.0};
     Point p1(2.0, 1.0);
     Point p2(3.0, 2.0);
     p1.rotate(pi / 6.0, {3.0, 1.0});
     p2.rotate(pi / 6.0, {3.0, 1.0});
     line.rotate(pi / 6.0, {3.0, 1.0});
     BOOST_CHECK(equal(line, {p1, p2}));
}

BOOST_AUTO_TEST_CASE(equal_test) {
     BOOST_CHECK(equal({1.0, 2.0, 3.0}, {2.0, 4.0, 6.0}, min_dbl));
     BOOST_CHECK(!equal({1.0, 2.0, 3.0}, {2.0, 4.0, 0.0}, min_dbl));
     BOOST_CHECK(equal({0.0, 1.0, 1.0}, {0.0, 2.0, 2.0}, min_dbl));
     BOOST_CHECK(!equal({0.0, 1.0, 1.0}, {0.0, 2.0, 1.0}, min_dbl));
     BOOST_CHECK(equal({1.0, 0.0, 1.0}, {2.0, 0.0, 2.0}, min_dbl));
     BOOST_CHECK(!equal({1.0, 0.0, 1.0}, {2.0, 0.0, 1.0}, min_dbl));
     BOOST_CHECK(equal({0.0, 1.0, 0.0}, {0.0, 2.0, 0.0}, min_dbl));
}

BOOST_AUTO_TEST_CASE(mutual_position_test) {
     BOOST_CHECK(mutual_position({1.0, 2.0, 3.0}, {2.0, 4.0, 6.0},
                                 min_dbl) ==
                 Mutual_position::overlap);
     BOOST_CHECK(mutual_position({0.0, 1.0, 1.0}, {0.0, 2.0, 2.0},
                                 min_dbl) ==
                 Mutual_position::overlap);
     BOOST_CHECK(mutual_position({1.0, 0.0, 1.0}, {2.0, 0.0, 2.0},
                                 min_dbl) ==
                 Mutual_position::overlap);
     BOOST_CHECK(mutual_position({0.0, 1.0, 0.0}, {0.0, 2.0, 0.0},
                                 min_dbl) ==
                 Mutual_position::overlap);

     BOOST_CHECK(mutual_position({2.0, 3.0, 4.0}, {4.0, 6.0, 9.0},
                                 min_dbl) ==
                 Mutual_position::parallel);
     BOOST_CHECK(mutual_position({0.0, 3.0, 4.0}, {0.0, 6.0, 9.0},
                                 min_dbl) ==
                 Mutual_position::parallel);
     BOOST_CHECK(mutual_position({2.0, 0.0, 4.0}, {4.0, 0.0, 9.0},
                                 min_dbl) ==
                 Mutual_position::parallel);

     BOOST_CHECK(mutual_position({0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
                                 min_dbl) ==
                 Mutual_position::perpendicular);
     BOOST_CHECK(mutual_position({-1.0, 1.0, 0.0}, {1.0, 1.0, -1.0},
                                 min_dbl) ==
                 Mutual_position::perpendicular);

     BOOST_CHECK(mutual_position({1.0, -1.0, 0.0}, {1.0, 0.0, 0.0},
                                 min_dbl) ==
                 Mutual_position::intersect);
     BOOST_CHECK(mutual_position({1.0, -1.0, 0.0}, {0.0, 1.0, 0.0},
                                 min_dbl) ==
                 Mutual_position::intersect);
}

BOOST_AUTO_TEST_CASE(distance_test) {
     BOOST_CHECK(distance(Line{0.0, 1.0, 0.0}, Line{1.0, 0.0, 0.0},
                          min_dbl) == 0.0);
     BOOST_CHECK(distance(Line{0.0, 1.0, 0.0}, Line{0.0, 1.0, -1.0},
                          min_dbl) == 1.0);
     BOOST_CHECK(faeq(distance(Line{1.0, -1.0, 0.0},
                               Line{1.0, -1.0, 2.0}, min_dbl),
                      std::sqrt(2.0), tolerance));
}

BOOST_AUTO_TEST_CASE(intersection_point_test) {
     Line line1{1.0, -1.0, 0.0};
     Line line2{1.0, 1.0, -6.0};
     Point p = intersection_point(line1, line2, tolerance);
     BOOST_CHECK(faeq(p.x(), 3.0, min_dbl));
     BOOST_CHECK(faeq(p.y(), 3.0, min_dbl));

     line1 = {1.0, -1.0, 0.0};
     line2 = {-1.0, 1.0, 1.0};
     BOOST_CHECK_THROW(intersection_point(line1, line2, tolerance),
                       std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(stream_insertion_test) {
     std::ostringstream oss;
     Line const line{1.0, 2.0, 3.0};
     oss << std::fixed << std::setprecision(4) << line << '\n';
     BOOST_CHECK(oss.str() == "Line(1.0000, 2.0000, 3.0000)\n");
}

BOOST_AUTO_TEST_CASE(angle_to_x_axis_test) {
     using SHG::angle_to_x_axis;
     using std::atan;

     BOOST_CHECK_THROW(angle_to_x_axis(0.0, 0.0),
                       std::invalid_argument);

     BOOST_CHECK(angle_to_x_axis(0.0, 1.0) == 0.0);
     BOOST_CHECK(angle_to_x_axis(0.0, -1.0) == 0.0);
     BOOST_CHECK(angle_to_x_axis(1.0, -10.0) == atan(0.1));
     BOOST_CHECK(angle_to_x_axis(-1.0, 10.0) == atan(0.1));
     BOOST_CHECK(angle_to_x_axis(1.0, -1.0) == 0.25 * pi);
     BOOST_CHECK(angle_to_x_axis(-1.0, 1.0) == 0.25 * pi);
     BOOST_CHECK(angle_to_x_axis(1.0, 0.0) == 0.5 * pi);
     BOOST_CHECK(angle_to_x_axis(-1.0, 0.0) == 0.5 * pi);
     BOOST_CHECK(angle_to_x_axis(-1.0, -1.0) == 0.75 * pi);
     BOOST_CHECK(angle_to_x_axis(1.0, 1.0) == 0.75 * pi);
     BOOST_CHECK(angle_to_x_axis(-1.0, -10.0) == atan(-0.1) + pi);
     BOOST_CHECK(angle_to_x_axis(1.0, 10.0) == atan(-0.1) + pi);

     BOOST_CHECK(angle_to_x_axis(0.0, 1.0) == 0.0);
     BOOST_CHECK(angle_to_x_axis(0.01, -1.0) == atan(0.01));
     BOOST_CHECK(angle_to_x_axis(1.0, -1.0) == 0.25 * pi);
     BOOST_CHECK(angle_to_x_axis(2.0, -1.0) == atan(2.0));
     BOOST_CHECK(angle_to_x_axis(1.0, 0.0) == 0.5 * pi);
     BOOST_CHECK(angle_to_x_axis(2.0, 1.0) == atan(-2.0) + pi);
     BOOST_CHECK(angle_to_x_axis(1.0, 1.0) == 0.75 * pi);
     BOOST_CHECK(angle_to_x_axis(0.01, 1.0) == atan(-0.01) + pi);

     BOOST_CHECK(angle_to_x_axis(max_dbl, min_dbl) == 0.5 * pi);
     BOOST_CHECK(angle_to_x_axis(-max_dbl, -min_dbl) == 0.5 * pi);
     BOOST_CHECK(angle_to_x_axis(min_dbl, max_dbl) == 0.0);
     BOOST_CHECK(angle_to_x_axis(-min_dbl, -max_dbl) == 0.0);
     BOOST_CHECK(angle_to_x_axis(max_dbl, -min_dbl) == 0.5 * pi);
     BOOST_CHECK(angle_to_x_axis(-max_dbl, min_dbl) == 0.5 * pi);
     BOOST_CHECK(angle_to_x_axis(min_dbl, -max_dbl) == 0.0);
     BOOST_CHECK(angle_to_x_axis(-min_dbl, max_dbl) == 0.0);

     BOOST_CHECK(angle_to_x_axis(min_dbl, -min_dbl) == 0.25 * pi);
     BOOST_CHECK(angle_to_x_axis(-min_dbl, min_dbl) == 0.25 * pi);
     BOOST_CHECK(angle_to_x_axis(max_dbl, -max_dbl) == 0.25 * pi);
     BOOST_CHECK(angle_to_x_axis(-max_dbl, max_dbl) == 0.25 * pi);
     BOOST_CHECK(angle_to_x_axis(min_dbl, min_dbl) == 0.75 * pi);
     BOOST_CHECK(angle_to_x_axis(-min_dbl, -min_dbl) == 0.75 * pi);
     BOOST_CHECK(angle_to_x_axis(max_dbl, max_dbl) == 0.75 * pi);
     BOOST_CHECK(angle_to_x_axis(-max_dbl, -max_dbl) == 0.75 * pi);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(rectangle_test)

BOOST_AUTO_TEST_CASE(constructor_test) {
     Rectangle r;
     BOOST_CHECK(r.a() == 1.0);
     BOOST_CHECK(r.b() == 1.0);
     BOOST_CHECK(r.phi() == 0.0);
     BOOST_CHECK(r.h() == 0.0);
     BOOST_CHECK(r.k() == 0.0);

     r = {2.0, 1.0, 0.5 * pi, 3.0, 4.0};
     BOOST_CHECK(r.a() == 2.0);
     BOOST_CHECK(r.b() == 1.0);
     BOOST_CHECK(r.phi() == 0.5 * pi);
     BOOST_CHECK(r.h() == 3.0);
     BOOST_CHECK(r.k() == 4.0);

     BOOST_CHECK_THROW(Rectangle(1.0, 2.0, 0.5 * pi, 3.0, 4.0),
                       std::invalid_argument);
     BOOST_CHECK_THROW(Rectangle(2.0, 1.0, 1.5 * pi, 3.0, 4.0),
                       std::invalid_argument);

     Vecpoint v{{2.0, -1.0}, {2.0, 1.0}, {-2.0, 1.0}, {-2.0, -1.0}};
     std::sort(v.begin(), v.end(), is_less);
     do {
          Rectangle const r(v[0], v[1], v[2], v[3]);
          BOOST_CHECK(faeq(r.a(), 4.0, tolerance));
          BOOST_CHECK(faeq(r.b(), 2.0, tolerance));
          BOOST_CHECK(faeq(r.h(), 0.0, tolerance));
          BOOST_CHECK(faeq(r.k(), 0.0, tolerance));
          BOOST_CHECK(faeq(r.phi(), 0.0, tolerance));
     } while (std::next_permutation(v.begin(), v.end(), is_less));

     v[0].x(0.0);
     BOOST_CHECK_THROW(Rectangle(v[0], v[1], v[2], v[3]),
                       std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(verticees_test) {
     Rectangle const r{4.0, 2.0, 0.0, 0.0, 0.0};

     Vecpoint const v0{
          {-2.0, -1.0}, {-2.0, 1.0}, {2.0, -1.0}, {2.0, 1.0}};
     auto v = r.vertices();
     BOOST_REQUIRE(v.size() == 4);
     std::sort(v.begin(), v.end(), is_less);
     for (Vecpoint::size_type i = 0; i < v.size(); i++)
          BOOST_CHECK(equal(v[i], v0[i], min_dbl));
}

BOOST_AUTO_TEST_CASE(move_to_test) {
     Rectangle r{4.0, 2.0, 0.0, 0.0, 0.0};
     r.move_to({3.0, 3.0});
     BOOST_CHECK(faeq(r.a(), 4.0, min_dbl));
     BOOST_CHECK(faeq(r.b(), 2.0, min_dbl));
     BOOST_CHECK(faeq(r.h(), 3.0, min_dbl));
     BOOST_CHECK(faeq(r.k(), 3.0, min_dbl));
     BOOST_CHECK(faeq(r.phi(), 0.0, min_dbl));
}

BOOST_AUTO_TEST_CASE(move_by_test) {
     Rectangle r{4.0, 2.0, 0.0, 0.0, 0.0};
     r.move_by(3.0, 3.0);
     BOOST_CHECK(faeq(r.a(), 4.0, min_dbl));
     BOOST_CHECK(faeq(r.b(), 2.0, min_dbl));
     BOOST_CHECK(faeq(r.h(), 3.0, min_dbl));
     BOOST_CHECK(faeq(r.k(), 3.0, min_dbl));
     BOOST_CHECK(faeq(r.phi(), 0.0, min_dbl));
}

BOOST_AUTO_TEST_CASE(rotate_test) {
     Point p1{10.0, 2.0};
     Point p2{10.0, 6.0};
     Point p3{2.0, 6.0};
     Point p4{2.0, 2.0};
     Rectangle r{p1, p2, p3, p4};
     double const phi = pi / 6.0;
     Point const c{12.0, 8.0};

     p1.rotate(phi, c);
     p2.rotate(phi, c);
     p3.rotate(phi, c);
     p4.rotate(phi, c);
     r.rotate(phi, c);

     Rectangle const r1{p1, p2, p3, p4};

     BOOST_CHECK(faeq(r.a(), r1.a(), tolerance));
     BOOST_CHECK(faeq(r.b(), r1.b(), tolerance));
     BOOST_CHECK(faeq(r.phi(), r1.phi(), tolerance));
     BOOST_CHECK(faeq(r.h(), r1.h(), tolerance));
     BOOST_CHECK(faeq(r.k(), r1.k(), tolerance));
}

BOOST_AUTO_TEST_CASE(equal_test) {
     double const a = 4.0;
     double const b = 2.0;
     double const phi = 0.0;
     double const h = 0.0;
     double const k = 0.0;
     double const eps = std::ldexp(1.0, -4);
     double const delta1 = std::ldexp(1.0, -5);
     double const delta2 = std::ldexp(1.0, -3);
     Rectangle r1{a, b, phi, h, k};
     Rectangle r2{a, b, phi, h, k};

     r1 = {a, b, phi, h, k};
     r2 = {a, b, phi, h, k};
     BOOST_CHECK(equal(r1, r2, min_dbl));

     r2 = {a + delta1, b, phi, h, k};
     BOOST_CHECK(equal(r1, r2, eps));
     r2 = {a, b + delta1, phi, h, k};
     BOOST_CHECK(equal(r1, r2, eps));
     r2 = {a, b, phi + delta1, h, k};
     BOOST_CHECK(equal(r1, r2, eps));
     r2 = {a, b, phi, h + delta1, k};
     BOOST_CHECK(equal(r1, r2, eps));
     r2 = {a, b, phi, h, k + delta1};
     BOOST_CHECK(equal(r1, r2, eps));

     r2 = {a + delta2, b, phi, h, k};
     BOOST_CHECK(!equal(r1, r2, eps));
     r2 = {a, b + delta2, phi, h, k};
     BOOST_CHECK(!equal(r1, r2, eps));
     r2 = {a, b, phi + delta2, h, k};
     BOOST_CHECK(!equal(r1, r2, eps));
     r2 = {a, b, phi, h + delta2, k};
     BOOST_CHECK(!equal(r1, r2, eps));
     r2 = {a, b, phi, h, k + delta2};
     BOOST_CHECK(!equal(r1, r2, eps));
}

BOOST_AUTO_TEST_CASE(stream_insertion_test) {
     std::ostringstream oss;
     Rectangle const r{4.0, 2.0, 1.57, 8.0, 9.0};
     oss << std::fixed << std::setprecision(4) << r << '\n';
     BOOST_CHECK(
          oss.str() ==
          "Rectangle(4.0000, 2.0000, 1.5700, 8.0000, 9.0000)\n");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ellipse_test)

BOOST_AUTO_TEST_CASE(is_ellipse_test) {
     using SHG::is_ellipse;
     // (x^2 / 2^2) + (y^2 / 3^2) = 1
     BOOST_CHECK(is_ellipse(9.0, 0.0, 4.0, 0.0, 0.0, -36.0));
     // y = x^2
     BOOST_CHECK(!is_ellipse(1.0, 0.0, 0.0, 0.0, -0.5, 0.0));
     // x^2 + y^2 = 1
     BOOST_CHECK(is_ellipse(1.0, 0.0, 1.0, 0.0, 0.0, -1.0));
     // x^2 + y^2 = 0
     BOOST_CHECK(!is_ellipse(1.0, 0.0, 1.0, 0.0, 0.0, 0.0));
}

BOOST_AUTO_TEST_CASE(constructor_test) {
     Ellipse e;
     BOOST_CHECK(e.a() == 1.0);
     BOOST_CHECK(e.b() == 1.0);
     BOOST_CHECK(e.phi() == 0.0);
     BOOST_CHECK(e.h() == 0.0);
     BOOST_CHECK(e.k() == 0.0);

     e = {2.0, 1.0, 0.5 * pi, 3.0, 4.0};
     BOOST_CHECK(e.a() == 2.0);
     BOOST_CHECK(e.b() == 1.0);
     BOOST_CHECK(e.phi() == 0.5 * pi);
     BOOST_CHECK(e.h() == 3.0);
     BOOST_CHECK(e.k() == 4.0);

     BOOST_CHECK_THROW(Ellipse(1.0, 2.0, 0.5 * pi, 3.0, 4.0),
                       std::invalid_argument);
     BOOST_CHECK_THROW(Ellipse(2.0, 1.0, 1.5 * pi, 3.0, 4.0),
                       std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(relative_position_test) {
     using Rel_pos = Ellipse::Relative_position;
     Ellipse e;
     BOOST_CHECK(e.relative_position({0.0, 0.0}, min_dbl) ==
                 Rel_pos::interior);
     BOOST_CHECK(e.relative_position({1.0, 0.0}, min_dbl) ==
                 Rel_pos::boundary);
     BOOST_CHECK(e.relative_position({1.0, 1.0}, min_dbl) ==
                 Rel_pos::exterior);
}

BOOST_AUTO_TEST_CASE(trigonometric_test) {
     Ellipse const e{2.0, 1.0, 0.0, 0.0, 0.0};
     Point p;
     double t;

     t = 0.0;
     p = {2.0, 0.0};
     BOOST_CHECK(equal(e.trigonometric(t), p, min_dbl));

     t = 0.5 * pi;
     p = {0.0, 1.0};
     BOOST_CHECK(equal(e.trigonometric(t), p, tolerance));

     t = pi;
     p = {-2.0, 0.0};
     BOOST_CHECK(equal(e.trigonometric(t), p, tolerance));

     t = 1.5 * pi;
     p = {0.0, -1.0};
     BOOST_CHECK(equal(e.trigonometric(t), p, tolerance));
}

BOOST_AUTO_TEST_CASE(rational_test) {
     Ellipse const e{2.0, 1.0, 0.0, 0.0, 0.0};
     Point p;
     double t;

     t = -1.0;
     p = {0.0, -1.0};
     BOOST_CHECK(equal(e.rational(t), p, min_dbl));

     t = 0.0;
     p = {2.0, 0.0};
     BOOST_CHECK(equal(e.rational(t), p, tolerance));

     t = 1.0;
     p = {0.0, 1.0};
     BOOST_CHECK(equal(e.rational(t), p, tolerance));

     t = 1e20;
     p = {-2.0, 0.0};
     BOOST_CHECK(equal(e.rational(t), p, tolerance));

     t = -1e20;
     p = {-2.0, 0.0};
     BOOST_CHECK(equal(e.rational(t), p, tolerance));
}

BOOST_AUTO_TEST_CASE(tangent_test) {
     Ellipse const e{4.0, 2.0, 0.0, 0.0, 0.0};
     Point p;
     Line result;
     Line line;

     p = {4.0, 0.0};
     line = {1.0, 0.0, -4.0};
     result = e.tangent(p);
     BOOST_CHECK(equal(line, result, tolerance));

     p = {-4.0, 0.0};
     line = {1.0, 0.0, 4.0};
     result = e.tangent(p);
     BOOST_CHECK(equal(line, result, tolerance));

     p = {0.0, 2.0};
     line = {0.0, 1.0, -2.0};
     result = e.tangent(p);
     BOOST_CHECK(equal(line, result, tolerance));

     p = {0.0, -2.0};
     line = {0.0, 1.0, 2.0};
     result = e.tangent(p);
     BOOST_CHECK(equal(line, result, tolerance));

     p = {2.0, std::sqrt(3.0)};
     line = {8.0, 16.0 * std::sqrt(3.0), -64.0};
     result = e.tangent(p);
     BOOST_CHECK(equal(line, result, tolerance));
}

BOOST_AUTO_TEST_CASE(tangent_trigonometric_test) {
     Ellipse const e{4.0, 2.0, 0.0, 0.0, 0.0};
     double t;
     Line result;
     Line line;

     t = 0.0;
     line = {1.0, 0.0, -4.0};
     result = e.tangent_trigonometric(t);
     BOOST_CHECK(equal(line, result, tolerance));

     t = pi;
     line = {1.0, 0.0, 4.0};
     result = e.tangent_trigonometric(t);
     BOOST_CHECK(equal(line, result, tolerance));

     t = 0.5 * pi;
     line = {0.0, 1.0, -2.0};
     result = e.tangent_trigonometric(t);
     BOOST_CHECK(equal(line, result, tolerance));

     t = 1.5 * pi;
     line = {0.0, 1.0, 2.0};
     result = e.tangent_trigonometric(t);
     BOOST_CHECK(equal(line, result, tolerance));

     t = pi / 3.0;
     line = {8.0, 16.0 * std::sqrt(3.0), -64.0};
     result = e.tangent_trigonometric(t);
     BOOST_CHECK(equal(line, result, tolerance));
}

BOOST_AUTO_TEST_CASE(tangent_rational_test) {
     Ellipse const e{4.0, 2.0, 0.0, 0.0, 0.0};
     double t;
     Line result;
     Line line;

     t = 0.0;
     t = e.b() * std::tan(0.5 * t);
     line = {1.0, 0.0, -4.0};
     result = e.tangent_rational(t);
     BOOST_CHECK(equal(line, result, tolerance));

     t = pi;
     t = e.b() * std::tan(0.5 * t);
     line = {1.0, 0.0, 4.0};
     result = e.tangent_rational(t);
     BOOST_CHECK(equal(line, result, tolerance));

     t = 0.5 * pi;
     t = e.b() * std::tan(0.5 * t);
     line = {0.0, 1.0, -2.0};
     result = e.tangent_rational(t);
     BOOST_CHECK(equal(line, result, tolerance));

     t = 1.5 * pi;
     t = e.b() * std::tan(0.5 * t);
     line = {0.0, 1.0, 2.0};
     result = e.tangent_rational(t);
     BOOST_CHECK(equal(line, result, tolerance));

     t = pi / 3.0;
     t = e.b() * std::tan(0.5 * t);
     line = {8.0, 16.0 * std::sqrt(3.0), -64.0};
     result = e.tangent_rational(t);
     BOOST_CHECK(equal(line, result, tolerance));
}

BOOST_AUTO_TEST_CASE(tangent_pair_test) {
     Ellipse const e(4.0, 2.0, 0.0, 0.0, 0.0);
     Line line;
     Line result1;
     Line result2;
     std::pair<Line, Line> t;

     line = {0.0, 1.0, 0.0};
     result1 = {0.0, 1.0, -2.0};
     result2 = {0.0, 1.0, 2.0};
     t = e.tangent(line);
     BOOST_CHECK(equal(t.first, result1, tolerance));
     BOOST_CHECK(equal(t.second, result2, tolerance));

     line = {1.0, 0.0, 0.0};
     result1 = {1.0, 0.0, -4.0};
     result2 = {1.0, 0.0, 4.0};
     t = e.tangent(line);
     BOOST_CHECK(equal(t.first, result1, tolerance));
     BOOST_CHECK(equal(t.second, result2, tolerance));
}

BOOST_AUTO_TEST_CASE(ellipse_tangent_to_line_test) {
     Line const line(1.0, 0.0, -2.0);
     Point const p(2.0, 0.0);
     Ellipse e;

     e = Ellipse::tangent(line, p, 2.0, 1.0, 0.0, false, tolerance);
     BOOST_CHECK(
          equal(e, Ellipse(2.0, 1.0, 0.0, 4.0, 0.0), tolerance));
     e = Ellipse::tangent(line, p, 2.0, 1.0, 0.0, true, tolerance);
     BOOST_CHECK(
          equal(e, Ellipse(2.0, 1.0, 0.0, 0.0, 0.0), tolerance));
}

BOOST_AUTO_TEST_CASE(ellipse_tangent_to_line_throws_test) {
     Line const line(1.0, 0.0, -2.0);
     Point const p(1.0, 0.0);
     BOOST_CHECK_THROW(
          Ellipse::tangent(line, p, 2.0, 1.0, 0.0, false, tolerance),
          std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(move_to_test) {
     Ellipse e{4.0, 2.0, 0.0, 0.0, 0.0};
     e.move_to({3.0, 3.0});
     BOOST_CHECK(faeq(e.a(), 4.0, min_dbl));
     BOOST_CHECK(faeq(e.b(), 2.0, min_dbl));
     BOOST_CHECK(faeq(e.h(), 3.0, min_dbl));
     BOOST_CHECK(faeq(e.k(), 3.0, min_dbl));
     BOOST_CHECK(faeq(e.phi(), 0.0, min_dbl));
}

BOOST_AUTO_TEST_CASE(move_by_test) {
     Ellipse e{4.0, 2.0, 0.0, 0.0, 0.0};
     e.move_by(3.0, 3.0);
     BOOST_CHECK(faeq(e.a(), 4.0, min_dbl));
     BOOST_CHECK(faeq(e.b(), 2.0, min_dbl));
     BOOST_CHECK(faeq(e.h(), 3.0, min_dbl));
     BOOST_CHECK(faeq(e.k(), 3.0, min_dbl));
     BOOST_CHECK(faeq(e.phi(), 0.0, min_dbl));
}

BOOST_AUTO_TEST_CASE(rotate_test) {
     Ellipse e(4.0, 2.0, 0.0, 0.0, 0.0);
     e.rotate(pi / 6.0, {0.0, 0.0});
     BOOST_CHECK(faeq(e.a(), 4.0, tolerance));
     BOOST_CHECK(faeq(e.b(), 2.0, tolerance));
     BOOST_CHECK(faeq(e.phi(), pi / 6.0, tolerance));
     BOOST_CHECK(faeq(e.h(), 0.0, tolerance));
     BOOST_CHECK(faeq(e.k(), 0.0, tolerance));
}

BOOST_AUTO_TEST_CASE(equal_test) {
     double const a = 4.0;
     double const b = 2.0;
     double const phi = 0.0;
     double const h = 0.0;
     double const k = 0.0;
     double const eps = std::ldexp(1.0, -4);
     double const delta1 = std::ldexp(1.0, -5);
     double const delta2 = std::ldexp(1.0, -3);
     Ellipse e1{a, b, phi, h, k};
     Ellipse e2{a, b, phi, h, k};

     e1 = {a, b, phi, h, k};
     e2 = {a, b, phi, h, k};
     BOOST_CHECK(equal(e1, e2, min_dbl));

     e2 = {a + delta1, b, phi, h, k};
     BOOST_CHECK(equal(e1, e2, eps));
     e2 = {a, b + delta1, phi, h, k};
     BOOST_CHECK(equal(e1, e2, eps));
     e2 = {a, b, phi + delta1, h, k};
     BOOST_CHECK(equal(e1, e2, eps));
     e2 = {a, b, phi, h + delta1, k};
     BOOST_CHECK(equal(e1, e2, eps));
     e2 = {a, b, phi, h, k + delta1};
     BOOST_CHECK(equal(e1, e2, eps));

     e2 = {a + delta2, b, phi, h, k};
     BOOST_CHECK(!equal(e1, e2, eps));
     e2 = {a, b + delta2, phi, h, k};
     BOOST_CHECK(!equal(e1, e2, eps));
     e2 = {a, b, phi + delta2, h, k};
     BOOST_CHECK(!equal(e1, e2, eps));
     e2 = {a, b, phi, h + delta2, k};
     BOOST_CHECK(!equal(e1, e2, eps));
     e2 = {a, b, phi, h, k + delta2};
     BOOST_CHECK(!equal(e1, e2, eps));
}

BOOST_AUTO_TEST_CASE(stream_insertion_test) {
     std::ostringstream oss;
     Ellipse const e{4.0, 2.0, 1.57, 8.0, 9.0};
     oss << std::fixed << std::setprecision(4) << e << '\n';
     BOOST_CHECK(oss.str() ==
                 "Ellipse(4.0000, 2.0000, 1.5700, 8.0000, 9.0000)\n");
}

struct Common_points_test_case {
     Ellipse e1;
     Ellipse e2;
     int result;
     Vecpoint p;
};

std::ostream& operator<<(std::ostream& stream,
                         Common_points_test_case const& c) {
     stream.width(0);
     stream << c.e1 << ", " << c.e2 << '\n';
     return stream;
}

std::vector<Common_points_test_case> const cptc{
     // the second outside the first, no common points
     {{4.0, 2.0, 0.0, 0.0, 0.0}, {2.0, 1.0, 0.0, 8.0, 0.0}, 0, {}},

     // the second inside the first, no common points
     {{4.0, 2.0, 0.0, 0.0, 0.0}, {2.0, 1.0, 0.0, 0.0, 0.0}, 0, {}},

     // the second outside the first, one common point of tangency
     {{4.0, 2.0, 0.0, 0.0, 0.0},
      {2.0, 1.0, 0.0, 6.0, 0.0},
      0,
      {{4.0, 0.0}}},

     // the second inside the first, one common point of tangency
     {{4.0, 2.0, 0.0, 0.0, 0.0},
      {2.0, 1.0, 0.0, 2.0, 0.0},
      0,
      {{4.0, 0.0}}},

     // the second inside the first, one common point of tangency,
     // both ellipses at left vertex
     {{4.0, 2.0, 0.0, 0.0, 0.0},
      {2.0, 1.0, 0.0, -2.0, 0.0},
      0,
      {{-4.0, 0.0}}},

     // two common points of intersection
     {{4.0, 2.0, 0.0, 0.0, 0.0},
      {2.0, 1.0, 0.0, 5.0, 0.0},
      0,
      {{3.7, -std::sqrt(231.0 / 400.0)},
       {3.7, std::sqrt(231.0 / 400.0)}}},

     // the second inside the first, two common points of tangency
     {{4.0, 2.0, 0.0, 0.0, 0.0},
      {2.0, 1.0, 0.5 * pi, 0.0, 0.0},
      0,
      {{0.0, -2.0}, {0.0, 2.0}}},

     // one common point of tangency and two common points of
     // intersection
     {{4.0, 2.0, 0.0, 0.0, 0.0},
      {4.0, 2.0, 0.5 * pi, 0.0, -2.0},
      0,
      {{-8.0 * std::sqrt(14.0) / 15.0, -26.0 / 15.0},
       {0.0, 2.0},
       {8.0 * std::sqrt(14.0) / 15.0, -26.0 / 15.0}}},

     // four points of intersection
     {{4.0, 2.0, 0.0, 0.0, 0.0},
      {4.0, 2.0, 0.5 * pi, 0.0, 0.0},
      0,
      {{-4.0 / std::sqrt(5.0), -4.0 / std::sqrt(5.0)},
       {-4.0 / std::sqrt(5.0), 4.0 / std::sqrt(5.0)},
       {4.0 / std::sqrt(5.0), -4.0 / std::sqrt(5.0)},
       {4.0 / std::sqrt(5.0), 4.0 / std::sqrt(5.0)}}},

     // two overlapping ellipses
     {{4.0, 2.0, 0.0, 0.0, 0.0}, {4.0, 2.0, 0.0, 0.0, 0.0}, 1, {}},

     // the second outside the first, one common point of tangency at
     // left vertex of the first ellipse
     {{4.0, 2.0, 0.0, 0.0, 0.0},
      {4.0, 2.0, 0.0, -8.0, 0.0},
      0,
      {{-4.0, 0.0}}}};

BOOST_DATA_TEST_CASE(common_points_test, bdata::make(cptc), c) {
     BOOST_REQUIRE(std::is_sorted(c.p.cbegin(), c.p.cend(), is_less));
     Vecpoint p;
     int const r = common_points(c.e1, c.e2, p);
     BOOST_CHECK(r == c.result);

     if (equal(c.e2, Ellipse(4.0, 2.0, 0.5 * pi, 0.0, -2.0),
               tolerance)) {
          /*
           * Output of GSL::solve_polynomial(a1, x_complex):
           * (-1.15641,0), (2,2.15094e-08), (2,-2.15094e-08),
           * (-3.45897,0) The imaginary part is too big to be 0.
           */
          BOOST_WARN(p.size() == c.p.size());
          return;
     }

     BOOST_REQUIRE(p.size() == c.p.size());
     std::sort(p.begin(), p.end(), is_less);
     for (std::size_t i = 0; i < p.size(); i++) {
          BOOST_CHECK(faeq(p[i].x(), c.p[i].x(), 1e-7));
          BOOST_CHECK(faeq(p[i].y(), c.p[i].y(), 1e-9));
     }
}

BOOST_AUTO_TEST_CASE(distance_from_point_to_ellipse_test) {
     Ellipse const e(4.0, 2.0, 0.0, 0.0, 0.0);
     Point p;
     double d;
     Vecpoint q;

     p = {0.0, 4.0};  // outside ellipse
     distance(p, e, d, q);
     BOOST_CHECK(faeq(d, 2.0, tolerance));
     BOOST_REQUIRE(q.size() == 1);
     BOOST_CHECK(faeq(q[0].x(), 0.0, tolerance));
     BOOST_CHECK(faeq(q[0].y(), 2.0, tolerance));

     p = {0.0, 0.0};  // inside ellipse
     distance(p, e, d, q);
     BOOST_CHECK(faeq(d, 2.0, tolerance));
     BOOST_REQUIRE(q.size() == 2);
     BOOST_CHECK(faeq(q[0].x(), 0.0, tolerance));
     BOOST_CHECK(faeq(q[0].y(), -2.0, tolerance));
     BOOST_CHECK(faeq(q[1].x(), 0.0, tolerance));
     BOOST_CHECK(faeq(q[1].y(), 2.0, tolerance));

     p = {4.0, 0.0};  // on ellipse
     distance(p, e, d, q);
     BOOST_CHECK(faeq(d, 0.0, tolerance));
     BOOST_REQUIRE(q.size() == 1);
     BOOST_CHECK(faeq(q[0].x(), 4.0, tolerance));
     BOOST_CHECK(faeq(q[0].y(), 0.0, tolerance));

     // points on normal to ellipse at the point p0
     Point const p0{2.0, std::sqrt(3.0)};
     Line normal = e.tangent(p0).perpendicular(p0);
     for (int i = 0; i < 4; i++) {
          double const x = 2.0 + i;
          double const y =
               (-normal.A() * x - normal.C()) / normal.B();
          p = {x, y};
          distance(p, e, d, q);
          BOOST_CHECK(faeq(d, distance(p, p0), tolerance));
          BOOST_REQUIRE(q.size() == 1);
          BOOST_CHECK(faeq(q[0].x(), p0.x(), tolerance));
          BOOST_CHECK(faeq(q[0].y(), p0.y(), tolerance));
     }
}

BOOST_AUTO_TEST_CASE(is_tangent_point_test) {
     Ellipse e1{4.0, 2.0, 0.0, 0.0, 0.0};
     Ellipse e2{2.0, 1.0, 0.0, 6.0, 0.0};
     Point p{4.0, 0.0};

     BOOST_CHECK(is_tangent_point(e1, e2, p));

     Point const p0{4.0, 0.0};
     double const phi = pi / 3.0;

     e1.rotate(phi, p0);
     e2.rotate(phi, p0);
     p.rotate(phi, p0);
     BOOST_CHECK(is_tangent_point(e1, e2, p));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
