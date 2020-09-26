/**
 * \file src/geometry.cc
 * Geometric functions.
 */

#include <cmath>
#include <stdexcept>
#include <utility>
#include "shg/fcmp.h"
#include "shg/mconsts.h"
#include "shg/numalg.h"
#include "shg/utils.h"
#include "shg/geometry.h"

using std::atan;
using std::cos;
using std::hypot;
using std::invalid_argument;
using std::isfinite;
using std::runtime_error;
using std::sin;

namespace SHG {

double radians_to_degrees(double radians) noexcept {
     return SHG::Constants::radian<double> * radians;
}

double degrees_to_radians(double degrees) noexcept {
     return SHG::Constants::degree<double> * degrees;
}

Point::Point(double x, double y) : x_(x), y_(y) {}

void Point::do_move_to(const Point& p) {
     x_ = p.x_;
     y_ = p.y_;
}

void Point::do_move_by(double a, double b) {
     x_ += a;
     y_ += b;
}

void Point::do_rotate(double phi, const Point& p) {
     const double sin_phi = sin(phi);
     const double cos_phi = cos(phi);
     const double a = x_ - p.x();
     const double b = y_ - p.y();
     x_ = p.x() + a * cos_phi - b * sin_phi;
     y_ = p.y() + a * sin_phi + b * cos_phi;
}

double distance(const Point& p1, const Point& p2) {
     return hypot(p1.x() - p2.x(), p1.y() - p2.y());
}

bool equal(const Point& p1, const Point& p2, double eps) {
     return faeq(p1.x(), p2.x(), eps) && faeq(p1.y(), p2.y(), eps);
}

bool is_less(const Point& p1, const Point& p2) {
     if (p1.x() < p2.x())
          return true;
     if (p1.x() > p2.x())
          return false;
     if (p1.y() < p2.y())
          return true;
     return false;
}

std::ostream& operator<<(std::ostream& stream, const Point& p) {
     stream.width(0);
     stream << "Point(" << p.x() << ", " << p.y() << ')';
     return stream;
}

Line::Line(double A, double B, double C) : A_(A), B_(B), C_(C) {
     if (!parameters_correct(A, B, C))
          throw invalid_argument(
               "invalid argument in Line constructor");
}

Line::Line(const Point& p1, const Point& p2)
     : Line(p2.y() - p1.y(), p1.x() - p2.x(),
            p1.y() * p2.x() - p1.x() * p2.y()) {}

Line Line::perpendicular(const Point& p) const {
     return Line(-B_, A_, B_ * p.x() - A_ * p.y());
}

bool Line::parameters_correct(double A, double B, double C) {
     if (!isfinite(A))
          return false;
     if (!isfinite(B))
          return false;
     if (!isfinite(C))
          return false;
     if (A == 0.0 && B == 0.0)
          return false;
     return true;
}

void Line::do_move_to(const Point& p) {
     const double C = -A_ * p.x() - B_ * p.y();
     if (!parameters_correct(A_, B_, C))
          throw runtime_error("invalid line in Line::move_to");
     C_ = C;
}

void Line::do_move_by(double a, double b) {
     const double C = C_ - A_ * a - B_ * b;
     if (!parameters_correct(A_, B_, C))
          throw runtime_error("invalid line in Line::move_by");
     C_ = C;
}

void Line::do_rotate(double phi, const Point& p) {
     const double c = cos(phi);
     const double s = sin(phi);
     const double A = A_ * c - B_ * s;
     const double B = A_ * s + B_ * c;
     const double C = C_ + (A_ - A) * p.x() + (B_ - B) * p.y();
     if (!parameters_correct(A, B, C))
          throw runtime_error("invalid line in Line::rotate");
     A_ = A;
     B_ = B;
     C_ = C;
}

bool equal(const Line& line1, const Line& line2, double tol) {
     return mutual_position(line1, line2, tol) ==
            Mutual_position::overlap;
}

Mutual_position mutual_position(const Line& line1, const Line& line2,
                                double tol) {
     if (faeq(line1.A() * line2.B(), line2.A() * line1.B(), tol)) {
          if (faeq(line1.A() * line2.C(), line2.A() * line1.C(),
                   tol) &&
              faeq(line1.B() * line2.C(), line2.B() * line1.C(), tol))
               return Mutual_position::overlap;
          return Mutual_position::parallel;
     } else if (faeq(line1.A() * line2.A(), -line1.B() * line2.B(),
                     tol))
          return Mutual_position::perpendicular;
     else
          return Mutual_position::intersect;
}

double distance(const Line& line1, const Line& line2, double tol) {
     if (mutual_position(line1, line2, tol) ==
         Mutual_position::parallel) {
          const double k = faeq(line2.A(), 0.0, tol)
                                ? line1.B() / line2.B()
                                : line1.A() / line2.A();
          return std::abs(line1.C() - k * line2.C()) /
                 hypot(line1.A(), line1.B());
     }
     return 0.0;
}

Point intersection_point(const Line& line1, const Line& line2,
                         double tol) {
     const double delta =
          line1.A() * line2.B() - line1.B() * line2.A();
     if (faeq(delta, 0.0, tol))
          throw invalid_argument("lines do not intersect");
     const double delta_x =
          line1.B() * line2.C() - line1.C() * line2.B();
     const double delta_y =
          line1.C() * line2.A() - line1.A() * line2.C();
     const double x = delta_x / delta;
     const double y = delta_y / delta;
     if (!isfinite(x) || !isfinite(y))
          throw runtime_error("intersection_point");
     return Point(x, y);
}

std::ostream& operator<<(std::ostream& stream, const Line& line) {
     stream.width(0);
     stream << "Line(" << line.A() << ", " << line.B() << ", "
            << line.C() << ')';
     return stream;
}

double angle_to_x_axis(double A, double B) {
     if (A == 0.0 && B == 0.0)
          throw invalid_argument(
               "invalid argument in angle_to_x_axis");
     const double k = -A / B;
     return k < 0.0 ? atan(k) + Constants::pi<double> : atan(k);
}

Rectangle::Rectangle(double a, double b, double phi, double h,
                     double k)
     : a_(a),
       b_(b),
       phi_(phi),
       h_(h),
       k_(k),
       sin_phi_(sin(phi)),
       cos_phi_(cos(phi)) {
     if (a < b || b <= 0.0 || phi < 0.0 ||
         phi >= Constants::pi<double>)
          throw invalid_argument(
               "incorrect parameter in Rectangle constructor");
}

/*
 *       p3 ----------- p2
 *         |           |
 *         |           |
 *         |           |
 *       p4 ----------- p1
 */
Rectangle::Rectangle(const Point& p1, const Point& p2,
                     const Point& p3, const Point& p4, double tol) {
     using std::swap;
     Point q1(p1);
     Point q2(p2);
     Point q3(p3);
     Point q4(p4);

     if (mutual_position(Line(q1, q2), Line(q3, q4), tol) !=
         Mutual_position::parallel) {
          swap(q2, q3);
          if (mutual_position(Line(q1, q2), Line(q3, q4), tol) !=
              Mutual_position::parallel)
               throw invalid_argument(
                    "invalid points in rectangle constructor");
     }
     if (mutual_position(Line(q2, q3), Line(q1, q4), tol) !=
         Mutual_position::parallel) {
          swap(q3, q4);
          if (mutual_position(Line(q2, q3), Line(q1, q4), tol) !=
              Mutual_position::parallel)
               throw invalid_argument(
                    "invalid points in rectangle constructor");
     }

     if (mutual_position(Line(q1, q2), Line(q3, q4), tol) !=
              Mutual_position::parallel ||
         mutual_position(Line(q2, q3), Line(q1, q4), tol) !=
              Mutual_position::parallel)
          throw runtime_error(
               "runtime error in rectangle constructor");
     a_ = distance(Line(q1, q2), Line(q3, q4), tol);
     b_ = distance(Line(q2, q3), Line(q1, q4), tol);
     Line line;
     if (a_ < b_) {
          line = Line(q1, q2);
          swap(a_, b_);
     } else {
          line = Line(q1, q4);
     }
     phi_ = angle_to_x_axis(line.A(), line.B());
     const auto p = intersection_point(Line(q1, q3), Line(q2, q4));
     h_ = p.x();
     k_ = p.y();
}

Vecpoint Rectangle::vertices() const {
     const double ac = a_ * 0.5 * cos_phi_;
     const double as = a_ * 0.5 * sin_phi_;
     const double bc = b_ * 0.5 * cos_phi_;
     const double bs = b_ * 0.5 * sin_phi_;
     const Point p1(h_ + ac + bs, k_ + as - bc);
     const Point p2(h_ - ac + bs, k_ - as - bc);
     const Point p3(h_ + ac - bs, k_ + as + bc);
     const Point p4(h_ - ac - bs, k_ - as + bc);
     return {p1, p2, p3, p4};
}

void Rectangle::reduce() {
     phi_ = mod1(phi_, Constants::pi<double>);
     if (phi_ < 0.0 || phi_ >= Constants::pi<double>)
          throw runtime_error(__func__);
}

void Rectangle::do_move_to(const Point& p) {
     h_ = p.x();
     k_ = p.y();
}

void Rectangle::do_move_by(double a, double b) {
     h_ += a;
     k_ += b;
}

void Rectangle::do_rotate(double phi, const Point& p) {
     const double sin_phi = sin(phi);
     const double cos_phi = cos(phi);
     const double hmx = h_ - p.x();
     const double kmy = k_ - p.y();
     h_ = p.x() + hmx * cos_phi - kmy * sin_phi;
     k_ = p.y() + hmx * sin_phi + kmy * cos_phi;
     phi_ += phi;
     reduce();
     sin_phi_ = sin(phi_);
     cos_phi_ = cos(phi_);
}

bool equal(const Rectangle& r1, const Rectangle& r2, double tol) {
     if (!faeq(r1.a(), r2.a(), tol))
          return false;
     if (!faeq(r1.b(), r2.b(), tol))
          return false;
     if (!faeq(r1.phi(), r2.phi(), tol))
          return false;
     if (!faeq(r1.h(), r2.h(), tol))
          return false;
     if (!faeq(r1.k(), r2.k(), tol))
          return false;
     return true;
}

std::ostream& operator<<(std::ostream& stream, const Rectangle& r) {
     stream.width(0);
     stream << "Rectangle(" << r.a() << ", " << r.b() << ", "
            << r.phi() << ", " << r.h() << ", " << r.k() << ')';
     return stream;
}

bool is_ellipse(double a, double b, double c, double d, double e,
                double f, double eps) {
     const double Delta = a * c * f + 2.0 * b * e * d - d * c * d -
                          e * e * a - f * b * b;
     const double delta = a * c - b * b;
     const double S = a + c;
     return delta > eps && Delta * S < -eps;
}

Ellipse::Ellipse(double a, double b, double phi, double h, double k)
     : a_(a),
       b_(b),
       phi_(phi),
       h_(h),
       k_(k),
       sin_phi_(sin(phi)),
       cos_phi_(cos(phi)) {
     if (a < b || b <= 0.0 || phi < 0.0 ||
         phi >= Constants::pi<double>)
          throw invalid_argument(
               "incorrect parameter in Ellipse constructor");
}

Ellipse::Relative_position Ellipse::relative_position(
     const Point& p, double tol) const {
     const double x = p.x() - h_;
     const double y = p.y() - k_;
     const double u = (x * cos_phi_ + y * sin_phi_) / a_;
     const double v = (y * cos_phi_ - x * sin_phi_) / b_;
     const double w = sqr(u) + sqr(v) - 1.0;
     if (w < -tol)
          return Relative_position::interior;
     if (w > tol)
          return Relative_position::exterior;
     return Relative_position::boundary;
}

Point Ellipse::trigonometric(double t) const {
     const double acos_t = a_ * cos(t);
     const double bsin_t = b_ * sin(t);
     const double x = h_ + cos_phi_ * acos_t - sin_phi_ * bsin_t;
     const double y = k_ + sin_phi_ * acos_t + cos_phi_ * bsin_t;
     return {x, y};
}

Point Ellipse::rational(double t) const {
     const double den = sqr(b_) + sqr(t);
     const double ct = (sqr(b_) - sqr(t)) / den;
     const double st = (2.0 * b_ * t) / den;
     const double x = h_ + a_ * cos_phi_ * ct - b_ * sin_phi_ * st;
     const double y = k_ + a_ * sin_phi_ * ct + b_ * cos_phi_ * st;
     return {x, y};
}

Line Ellipse::tangent(const Point& p) const {
     const double x1 = p.x() - h_;
     const double y1 = p.y() - k_;
     const double alpha = (x1 * cos_phi_ + y1 * sin_phi_) / sqr(a_);
     const double beta = (-x1 * sin_phi_ + y1 * cos_phi_) / sqr(b_);
     const double A = alpha * cos_phi_ - beta * sin_phi_;
     const double B = alpha * sin_phi_ + beta * cos_phi_;
     const double C = -A * p.x() - B * p.y();
     return {A, B, C};
}

Line Ellipse::tangent_trigonometric(double t) const {
     const double c0 = cos(t);
     const double s0 = sin(t);
     const double A = b_ * cos_phi_ * c0 - a_ * sin_phi_ * s0;
     const double B = a_ * cos_phi_ * s0 + b_ * sin_phi_ * c0;
     const Point p = trigonometric(t);
     const double C = -A * p.x() - B * p.y();
     return {A, B, C};
}

Line Ellipse::tangent_rational(double t) const {
     const double den = sqr(b_) + sqr(t);
     const double ct = (sqr(b_) - sqr(t)) / den;
     const double dt = 2.0 * b_ / den;
     const double st = t * dt;
     const double A =
          b_ * cos_phi_ * dt * ct - a_ * sin_phi_ * dt * st;
     const double B =
          a_ * cos_phi_ * dt * st + b_ * sin_phi_ * dt * ct;
     const Point p = rational(t);
     const double C = -A * p.x() - B * p.y();
     return {A, B, C};
}

std::pair<Line, Line> Ellipse::tangent(const Line& line) const {
     double A = line.A();
     double B = line.B();
     if (A < 0.0) {
          A = -A;
          B = -B;
     }
     const double term1 = a_ * (A * cos_phi_ + B * sin_phi_);
     const double term2 = b_ * (A * sin_phi_ - B * cos_phi_);
     const double D = hypot(term1, term2);
     const double d = A * h_ + B * k_;
     return std::make_pair(Line(A, B, -D - d), Line(A, B, D - d));
}

Ellipse Ellipse::tangent(const Line& l, const Point& p, double a,
                         double b, double phi, bool negative,
                         double tol) {
     Ellipse e(a, b, phi, p.x(), p.y());
     if (fane(l.A() * p.x() + l.B() * p.y() + l.C(), 0.0, tol))
          throw invalid_argument(__func__);
     const double u = a * (l.A() * e.cos_phi_ + l.B() * e.sin_phi_);
     const double v = b * (l.B() * e.cos_phi_ - l.A() * e.sin_phi_);
     const double gamma = 1.0 / hypot(u, v);
     const double w = e.cos_phi_ * e.sin_phi_ * (sqr(a) - sqr(b));
     const double dh =
          l.A() * (sqr(b * e.sin_phi_) + sqr(a * e.cos_phi_)) +
          l.B() * w;
     const double dk =
          l.B() * (sqr(b * e.cos_phi_) + sqr(a * e.sin_phi_)) +
          l.A() * w;
     if (negative) {
          e.h_ -= gamma * dh;
          e.k_ -= gamma * dk;
     } else {
          e.h_ += gamma * dh;
          e.k_ += gamma * dk;
     }
     return e;
}

void Ellipse::reduce() {
     phi_ = mod1(phi_, Constants::pi<double>);
     if (phi_ < 0.0 || phi_ >= Constants::pi<double>)
          throw runtime_error(__func__);
}

void Ellipse::do_move_to(const Point& p) {
     h_ = p.x();
     k_ = p.y();
}

void Ellipse::do_move_by(double a, double b) {
     h_ += a;
     k_ += b;
}

void Ellipse::do_rotate(double phi, const Point& p) {
     const double sin_phi = sin(phi);
     const double cos_phi = cos(phi);
     const double hmx = h_ - p.x();
     const double kmy = k_ - p.y();
     h_ = p.x() + hmx * cos_phi - kmy * sin_phi;
     k_ = p.y() + hmx * sin_phi + kmy * cos_phi;
     phi_ += phi;
     reduce();
     sin_phi_ = sin(phi_);
     cos_phi_ = cos(phi_);
}

bool equal(const Ellipse& e1, const Ellipse& e2, double tol) {
     if (fane(e1.a(), e2.a(), tol))
          return false;
     if (fane(e1.b(), e2.b(), tol))
          return false;
     if (fane(e1.phi(), e2.phi(), tol))
          return false;
     if (fane(e1.h(), e2.h(), tol))
          return false;
     if (fane(e1.k(), e2.k(), tol))
          return false;
     return true;
}

std::ostream& operator<<(std::ostream& stream, const Ellipse& e) {
     stream.width(0);
     stream << "Ellipse(" << e.a() << ", " << e.b() << ", " << e.phi()
            << ", " << e.h() << ", " << e.k() << ')';
     return stream;
}

Vecdouble common_points_polynomial(const Ellipse& e1,
                                   const Ellipse& e2) {
     const double sin_phi1 = sin(e1.phi());
     const double cos_phi1 = cos(e1.phi());
     const double sin_phi2 = sin(e2.phi());
     const double cos_phi2 = cos(e2.phi());

     const double Ax = e1.h() - e2.h();
     const double Bx = e1.a() * cos_phi1;
     const double Cx = e1.b() * sin_phi1;
     const double Ay = e1.k() - e2.k();
     const double By = e1.a() * sin_phi1;
     const double Cy = e1.b() * cos_phi1;

     const double a1 = e2.b() * (Ax * cos_phi2 + Ay * sin_phi2);
     const double a2 = e2.b() * (Bx * cos_phi2 + By * sin_phi2);
     const double a3 = e2.b() * (-Cx * cos_phi2 + Cy * sin_phi2);
     const double b1 = e2.a() * (-Ax * sin_phi2 + Ay * cos_phi2);
     const double b2 = e2.a() * (-Bx * sin_phi2 + By * cos_phi2);
     const double b3 = e2.a() * (Cx * sin_phi2 + Cy * cos_phi2);

     const double A = sqr(a2) + sqr(b2);
     const double B = 2.0 * (a2 * a3 + b2 * b3);
     const double C = sqr(a3) + sqr(b3);
     const double D = 2.0 * (a1 * a2 + b1 * b2);
     const double E = 2.0 * (a1 * a3 + b1 * b3);
     const double F = sqr(a1) + sqr(b1) - sqr(e2.a() * e2.b());

     Vecdouble a(5);
     double f = e1.b();
     a[4] = A - D + F;
     a[3] = 2.0 * f * (E - B);
     f *= e1.b();
     a[2] = 2.0 * f * (F - A + 2.0 * C);
     f *= e1.b();
     a[1] = 2.0 * f * (E + B);
     f *= e1.b();
     a[0] = f * (A + D + F);

     return a;
}

int common_points(const Ellipse& e1, const Ellipse& e2, Vecpoint& p) {
     Vecpoint pp;
     const Vecdouble a = common_points_polynomial(e1, e2);
     const int deg = degree_of_polynomial(a, tolerance);
     if (deg == -1)
          return 1;
     if (deg == 0)
          return 2;
     Vecdouble t;
     try {
          real_roots(a, t);
     } catch (const std::exception&) {
          return 3;
     }
     for (Vecdouble::size_type i = 0; i < t.size(); i++)
          pp.push_back(e1.rational(t[i]));
     // Does the left vertex of e1 belong to e2?
     const double x = e1.h() - e1.a() * cos(e1.phi());
     const double y = e1.k() - e1.a() * sin(e1.phi());
     const Point q(x, y);

     assert(e1.relative_position(q) ==
            Ellipse::Relative_position::boundary);
     if (e2.relative_position(q) ==
         Ellipse::Relative_position::boundary)
          pp.push_back(q);
     // Now we have all points, perhaps duplicated.

     p.resize(0);
     for (Vecpoint::size_type i = 0; i < pp.size(); i++) {
          Vecpoint::size_type j;
          for (j = 0; j < p.size(); j++)
               if (faeq(pp[i].x(), p[j].x(), 2e-7) &&
                   faeq(pp[i].y(), p[j].y(), 2e-7))
                    break;
          if (j >= p.size())
               p.push_back(pp[i]);
     }
     return 0;
}

Vecdouble closest_points_polynomial(const Point& p,
                                    const Ellipse& e) {
     const double sin_phi = sin(e.phi());
     const double cos_phi = cos(e.phi());

     const double Ax = e.h() - p.x();
     const double Bx = e.a() * cos_phi;
     const double Cx = e.b() * sin_phi;
     const double Ay = e.k() - p.y();
     const double By = e.a() * sin_phi;
     const double Cy = e.b() * cos_phi;
     const double C = sqr(e.b()) - sqr(e.a());
     const double D = Ay * Cy - Ax * Cx;
     const double E = -Ax * Bx - Ay * By;
     Vecdouble a(5);
     a[4] = -D;
     a[3] = 2.0 * e.b() * (E - C);
     a[2] = 0.0;
     a[1] = 2.0 * cube(e.b()) * (E + C);
     a[0] = sqr(sqr(e.b())) * D;
     return a;
}

void distance(const Point& p, const Ellipse& e, double& d,
              Vecpoint& q) {
     const Vecdouble a = closest_points_polynomial(p, e);
     const int deg = degree_of_polynomial(a, tolerance);
     if (deg < 0)
          throw runtime_error(__func__);
     Vecdouble t;
     real_roots(a, t);
     Vecdouble dists(t.size());
     q.resize(t.size());
     for (Vecdouble::size_type i = 0; i < t.size(); i++) {
          q[i] = e.rational(t[i]);
          dists[i] = distance(p, q[i]);
     }
     auto it = std::min_element(dists.cbegin(), dists.cend());
     if (it == dists.cend())
          throw runtime_error(__func__);
     d = *it;
     Vecpoint z;
     z.push_back(q[it - dists.cbegin()]);
     while (++it != dists.cend())
          if (faeq(*it, d, tolerance))
               z.push_back(q[it - dists.cbegin()]);
     q = z;
}

bool is_tangent_point(const Ellipse& e1, const Ellipse& e2,
                      const Point& p, double tol) {
     if (e1.relative_position(p, tol) !=
         Ellipse::Relative_position::boundary)
          throw invalid_argument(__func__);
     if (e2.relative_position(p, tol) !=
         Ellipse::Relative_position::boundary)
          throw invalid_argument(__func__);

     const Line line1 = e1.tangent(p);
     const Line line2 = e2.tangent(p);

     return mutual_position(line1, line2, tol) ==
            Mutual_position::overlap;
}

}  // namespace SHG
