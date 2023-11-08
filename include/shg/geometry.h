/**
 * \file include/shg/geometry.h
 * Geometric functions.
 */

#ifndef SHG_GEOMETRY_H
#define SHG_GEOMETRY_H

#include <ostream>
#include <vector>
#include <shg/utils.h>

namespace SHG {

/**
 * \defgroup geometric_functions Geometric functions
 *
 * Geometric functions.
 *
 * \par Tangent line to a curve.
 * If a curve is given by \f$F(x, y) = 0\f$, the equation of the
 * tangent at the point \f$(x_0, y_0)\f$ of that curve is
 * \cite bronsztejn-siemiendiajew-musiol-muhlig-2004, page 243,
 * \anchor tangent_for_implicit_curve
 * \f[
 *   \frac{\partial F}{\partial x}(x_0, y_0)(x - x_0) + \frac{\partial
 *     F}{\partial y}(x_0, y_0)(y - y_0) = 0.
 * \f]
 * \par
 * If the curve is given by \f$x = x(t)\f$, \f$y = y(t)\f$ and the
 * point by \f$x_0 = x(t_0)\f$, \f$y_0 = y(t_0)\f$, the equation is
 * \cite bronsztejn-siemiendiajew-musiol-muhlig-2004, page 243,
 * \anchor tangent_for_parametric_curve
 * \f[
 *   x'(t_0)(y - y_0) = y'(t_0)(x - x_0)
 * \f].
 *
 * \{
 */

/**
 * Converts radians to degrees. \returns \f$ \frac{180}{\pi} \cdot
 * \var{radians}\f$.
 */
double radians_to_degrees(double radians) noexcept;

/**
 * Converts degrees to radians. \returns \f$ \frac{\pi}{180} \cdot
 * \var{degrees}\f$.
 */
double degrees_to_radians(double degrees) noexcept;

class Point;
using Vecpoint = std::vector<Point>;

/**
 * Abstract base class for geometric shapes.
 */
class Shape {
public:
     virtual ~Shape() = default;

     /** Move this shape to the point \f$p\f$. */
     void move_to(Point const& p) { do_move_to(p); }

     /** Move this shape by the vector \f$(a, b)\f$. */
     void move_by(double a, double b) { do_move_by(a, b); }

     /** Rotate this shape counter-clockwise by the angle
         \f$\varphi\f$ about the point \f$p\f$. */
     void rotate(double phi, Point const& p) { do_rotate(phi, p); }

private:
     virtual void do_move_to(Point const& p) = 0;
     virtual void do_move_by(double a, double b) = 0;
     virtual void do_rotate(double phi, Point const& p) = 0;
};

/**
 * Representation of a point on the plane. A point \f$p\f$ is
 * represented as a pair of coordinates \f$(x, y) \in \realn^2\f$.
 */
class Point : public Shape {
public:
     Point() = default;
     Point(double x, double y);

     double x() const { return x_; }
     double y() const { return y_; }
     void x(double x) { x_ = x; }
     void y(double y) { y_ = y; }

private:
     /** Assigns to this point coordinates of the point \f$p\f$. */
     void do_move_to(Point const& p) override;
     /** Moves this point to the point \f$(x + a, y+ b)\f$. */
     void do_move_by(double a, double b) override;
     // clang-format off
     /**
      * Rotates this point counter-clockwise by the angle \f$\varphi\f$
      * about the point \f$p\f$.
      *
      * \implementation Rotation of the point \f$(x, y)\f$ about the
      * point \f$(x_0, y_0)\f$ counter-clockwise by the angle
      * \f$\varphi\f$ converts it to the point \anchor gen_rot
      * \f{align*}{
      *   x' &= x_0 + (x - x_0) \cos \varphi - (y - y_0) \sin \varphi, \\
      *   y' &= y_0 + (x - x_0) \sin \varphi + (y - y_0) \cos \varphi.
      * \f}
      * Inverse rotation is the rotation by the angle \f$-\varphi\f$:
      * \f{align*}{
      *   x &= x_0 + (x' - x_0) \cos \varphi + (y' - y_0) \sin \varphi,\\
      *   y &= y_0 - (x' - x_0) \sin \varphi + (y' - y_0) \cos \varphi.
      * \f}
      */
     // clang-format on
     void do_rotate(double phi, Point const& p) override;

     double x_{0.0};
     double y_{0.0};
};

/**
 * Distance between two points.
 *
 * \implementation Distance between the points \f$(x_1, y_1)\f$,
 * \f$(x_2, y_2)\f$ is calculated as \f$\sqrt{(x_1 - x_2)^2 + (y_1 -
 * y_2)^2}\f$.
 */
double distance(Point const& p1, Point const& p2);

/**
 * Returns \e true if two points are approximately equal.
 *
 * The points \f$(x_1, y_1)\f$, \f$(x_2, y_2)\f$ are approximately
 * equal if \f$|x_1 - x_2| \leq \epsilon\f$ and \f$|y_1 - y_2| \leq
 * \epsilon\f$.
 */
bool equal(Point const& p1, Point const& p2, double eps = tolerance);

/**
 * Returns \e true if the first point is lexicographically less than
 * the second.
 *
 * The point \f$(x_1, y_1)\f$ is lexicographically less than the point
 * \f$(x_2, y_2)\f$ if and only if \f$x_1 < x_2 \lor (x_1 = x_2 \land
 * y_1 < y_2)\f$.
 */
bool is_less(Point const& p1, Point const& p2);

/**
 * Outputs a point to a stream.
 */
std::ostream& operator<<(std::ostream& stream, Point const& p);

/**
 * Representation of a straight line on the plane. A line \f$l\f$ is
 * represented by the three parameters of the equation \f$Ax + By + C
 * = 0\f$, provided that \f$A \neq 0\f$ or \f$B \neq 0\f$.
 */
class Line : public Shape {
public:
     /** Constructs the line \f$y = 0\f$. */
     Line() = default;

     /**
      * Constructs the line \f$Ax + By + C = 0\f$.
      * \throws std::invalid_argument if \f$A = B = 0.0\f$.
      */
     Line(double A, double B, double C);

     /**
      * Constructs the line passing through two points.
      * Uses the formulas:
      * \f{align*}{
      *   A &= y_2 - y_1 \\
      *   B &= x_1 - x_2 \\
      *   C &= y_1x_2 - x_1y_2
      * \f}
      */
     Line(Point const& p1, Point const& p2);

     double A() const { return A_; }
     double B() const { return B_; }
     double C() const { return C_; }

     /**
      * Returns the line perpendicular to this line.
      * Uses the formulas:
      * \f{align*}{
      *   A_1 &= -B \\
      *   B_1 &= A \\
      *   C_1 &= Bx - Ay
      * \f}
      * The point \f$p\f$ does not have to lie on this line.
      */
     Line perpendicular(Point const& p) const;

private:
     /**
      * Returns \e true if \f$A, B, C\f$ are finite numbers and \f$A
      * \neq 0 \lor B \neq 0\f$.
      */
     static bool parameters_correct(double A, double B, double C);

     /**
      * Moves the line parallelly so that it passes through the point
      * \e p. Uses the formula \f$C_1 = -Ap.x() - Bp.y()\f$.
      */
     void do_move_to(Point const& p) override;

     /**
      * Moves the line parallelly. The line will have the equation
      * \f$Ax + By + (C - Aa - Bb)\f$.
      *
      * \implementation If \f$A \neq 0\f$, we take the point \f$(-C /
      * A, 0)\f$ and move it by \f$(a, b)\f$. Then we find parallel to
      * the line passing through \f$(-C / A + a, b)\f$. Its equation
      * is \f$Ax + By + (C - Aa - Bb)\f$. The same equation is for
      * \f$B \neq 0\f$.
      */
     void do_move_by(double a, double b) override;

     /**
      * Rotates this line counter-clockwise by the angle \f$\varphi\f$
      * about the point \f$p\f$.
      *
      * \implementation Rotation of the line \f$Ax + By + C = 0\f$
      * about the point \f$(x_0, y_0)\f$ counter-clockwise by the
      * angle \f$\varphi\f$ converts it to the line \f$A_1x + B_1y +
      * C_1 = 0\f$, where
      *
      * \f{align*}{
      *   A_1 &= A \cos \varphi - B \sin \varphi, \\
      *   B_1 &= A \sin \varphi + B \cos \varphi, \\
      *   C_1 &= C + (A - A_1)x_0 + (B - B_1)y_0.
      * \f}
      */
     void do_rotate(double phi, Point const& p) override;

     double A_{0.0};
     double B_{1.0};
     double C_{0.0};
};

/**
 * Returns \e true if an only if two lines approximately overlap.
 * \see mutual_position().
 */
bool equal(Line const& line1, Line const& line2,
           double tol = tolerance);

/**
 * Mutual position of two straight lines.
 */
enum class Mutual_position {
     overlap,        ///< lines overlap
     parallel,       ///< lines have no common point
     perpendicular,  ///< lines are perpendicular
     intersect       ///< lines intersect, but are not perpendicular
};

/**
 * Returns mutual position of two straight lines.
 *
 * Let \f$A_1x + B_1y + C_1 = 0\f$ and \f$A_2x + B_2y + C_2 = 0\f$ be
 * two lines.
 *
 * If \f$A_1B_2 \neq A_2B_1\f$, the lines intersect. Then, if
 * \f$A_1A_2 = -B_1B_2\f$, they are perpendicular.
 *
 * If \f$A_1B_2 = A_2B_1\f$, then if \f$A_1C_2 = A_2C_1\f$ and
 * \f$B_1C_2 = B_2C_1\f$, the lines overlap, otherwise they are
 * parallel. The parameter \f$\var{tol}\f$ is used for comparison
 * accuracy.
 */
Mutual_position mutual_position(Line const& line1, Line const& line2,
                                double tol = tolerance);

/**
 * Returns distance between two lines.
 *
 * If the lines overlap or crosses each other, the function returns 0.
 * If the lines are parallel, the distance is calculated as
 * \f[\frac{|kC_2 - C_1|}{\sqrt{A_1^2 + B_1^2}},\f] where
 * \f[
 *   k = \begin{cases}
 *   A_1 / A_2 & \text{if $A_2 \neq 0$}, \\
 *   B_1 / B_2 & \text{if $A_2 = 0$}.
 *   \end{cases}
 * \f]
 * The parameter \f$\var{tol}\f$ is used for comparison accuracy in a
 * call to mutual_position().
 */
double distance(Line const& line1, Line const& line2,
                double tol = tolerance);

/**
 * Returns the point of intersection of two lines.
 *
 * The point of intersection is given as
 * \f{align*}{
 * x &= \frac{B_1C_2 - C_1B_2}{A_1B_2 - B_1A_2} \\
 * y &= \frac{C_1A_2 - A_1C_2}{A_1B_2 - B_1A_2}.
 * \f}
 *
 * \throws std::invalid_argument if \f$|A_1B_2 - B_1A_2| \leq
 * \var{tol}\f$
 */

Point intersection_point(Line const& line1, Line const& line2,
                         double tol = tolerance);

/**
 * Outputs a line to a stream.
 */
std::ostream& operator<<(std::ostream& stream, Line const& line);

// clang-format off
/**
 * Returns the angle between the line \f$Ax + By = 0\f$ and
 * \f$x\f$-axis.
 *
 * The angle is in \f$[0, \pi)\f$ according to the following table.
 *
 * %Line           | Angle
 * ----------------| -------------------------
 * \f$y = 0\f$     | \f$0\f$
 * \f$y = 0.1x\f$  | \f$\arctan(0.1)\f$
 * \f$y = x\f$     | \f$\pi / 4\f$
 * \f$x = 0\f$     | \f$\pi / 2\f$
 * \f$y = -x\f$    | \f$(3 / 4) \pi\f$
 * \f$y = -0.1x\f$ | \f$\pi + \arctan(-0.1)\f$
 *
 * \throws std::invalid_argument if both A and B are 0.
 *
 * \implementation See \cite bronsztejn-siemiendiajew-musiol-muhlig-2004,
 * page 213, formula 3.311e.
 */
// clang-format on
double angle_to_x_axis(double A, double B);

/**
 * Representation of a rectangle on the plane.
 *
 * A rectangle \f$R=R(a, b, \varphi, h, k)\f$ is represented with five
 * parameters: \f$a \geq b > 0\f$ are the lengths of the sides of the
 * rectangle, \f$\varphi \in [0, \pi)\f$ is the angle between the
 * longer side and the x-axis, and \f$(h, k) \in \realn^2\f$ are
 * coordinates of the intersection point of diagonals.
 */
class Rectangle : public Shape {
public:
     /**
      * Constructs \f$R(1, 1, 0, 0, 0)\f$.
      */
     Rectangle() = default;

     /**
      * Constructs \f$R=R(a, b, \varphi, h, k)\f$.
      *
      * \throws std::invalid_argument if \f$a < b\f$ or \f$b \leq 0\f$
      * or \f$\varphi < 0\f$ or \f$\varphi \geq \pi\f$
      */
     Rectangle(double a, double b, double phi, double h, double k);

     /**
      * Constructs rectangle with given vertices.
      *
      * The parameter \f$\var{tol}\f$ is used for comparison accuracy
      * in calls to mutual_position().
      *
      * \throws std::invalid_argument if the given points are not
      * vertices of a rectangle
      * \throws std::runtime_error in case of numerical inaccuracy
      */
     Rectangle(Point const& p1, Point const& p2, Point const& p3,
               Point const& p4, double tol = tolerance);
     double a() const { return a_; }
     double b() const { return b_; }
     double phi() const { return phi_; }
     double h() const { return h_; }
     double k() const { return k_; }

     double area() const { return a_ * b_; }

     /**
      * Returns four vertices of this rectangle.
      *
      * \implementation Let \f$R=R(a, b, \varphi, h, k)\f$ be this
      * rectangle. A straight line inclined to the \f$x\f$-axis at an
      * angle \f$\varphi\f$ and passing through the point \f$(h, k)\f$
      * is
      * \f[
      * \sin \varphi \ x - \cos \varphi \ y + (-h \sin \varphi + k
      * \cos \varphi) = 0.
      * \f]
      * Two lines parallel to this line and distant from it by \f$b /
      * 2\f$, which are longer sides of the rectangle, are (see
      * distance(Line const&, Line const&, double))
      * \f{align*}{
      * \sin \varphi \ x - \cos \varphi \ y + (-h \sin \varphi + k
      * \cos \varphi + \frac{b}{2}) &= 0, \\
      * \sin \varphi \ x - \cos \varphi \ y + (-h \sin \varphi + k
      * \cos \varphi - \frac{b}{2}) &= 0.
      * \f}
      * A straight line parallel to shorter sides of the rectangle and
      * passing through the point \f$(h, k)\f$ is (see
      * \cite bronsztejn-siemiendiajew-musiol-muhlig-2004, page 211,
      * formula 3.305)
      * \f[
      * \cos \varphi \ x + \sin \varphi \ y + (-h \cos \varphi - k
      * \sin \varphi) = 0.
      * \f]
      * Two lines parallel to this line and distant from it by \f$a /
      * 2\f$, which are shorter sides of the rectangle, are
      * \f{align*}{
      * \cos \varphi \ x + \sin \varphi \ y + (-h \cos \varphi - k
      * \sin \varphi + \frac{a}{2}) &= 0, \\
      * \cos \varphi \ x + \sin \varphi \ y + (-h \cos \varphi - k
      * \sin \varphi - \frac{a}{2}) &= 0.
      * \f}
      * Solving four systems of two equations gives the four vertices:
      * \f{align*}{
      * x &= h + \frac{1}{2} (a \cos \varphi + b \sin \varphi) &
      * y &= k + \frac{1}{2} (a \sin \varphi - b \cos \varphi) \\
      * x &= h - \frac{1}{2} (a \cos \varphi - b \sin \varphi) &
      * y &= k - \frac{1}{2} (a \sin \varphi + b \cos \varphi) \\
      * x &= h + \frac{1}{2} (a \cos \varphi - b \sin \varphi) &
      * y &= k + \frac{1}{2} (a \sin \varphi + b \cos \varphi) \\
      * x &= h - \frac{1}{2} (a \cos \varphi + b \sin \varphi) &
      * y &= k - \frac{1}{2} (a \sin \varphi - b \cos \varphi)
      * \f}
      * \returns A vector containg above points, in this order.
      */
     Vecpoint vertices() const;

private:
     /**
      * Reduces \c phi_ to \f$[0, \pi)\f$.
      * \todo In case if this is a square, it should reduce to
      * \f$[0.0, \pi / 2)\f$.
      */
     void reduce();

     /**
      * Moves this rectangle so that its center is at the point \p p.
      * \f$h \leftarrow p.x(), \quad k \leftarrow p.y()\f$.
      */
     void do_move_to(Point const& p) override;

     /**
      * Moves this rectangle so that its center is at the point
      * <tt>(h + a, k + b)</tt>.
      */
     void do_move_by(double a, double b) override;

     /**
      * Rotates this rectangle counter-clockwise by the angle
      * \f$\varphi\f$ about the point \f$p\f$.
      * \implementation See Ellipse::do_rotate().
      */
     void do_rotate(double phi, Point const& p) override;

     double a_{1.0};
     double b_{1.0};
     double phi_{0.0};
     double h_{0.0};
     double k_{0.0};
     double sin_phi_{0.0};
     double cos_phi_{1.0};
};

/**
 * Returns \e true if and only if the two rectangles approximately
 * overlap. This is true if and only if \f$|a_1 - a_2| \leq
 * \var{tol}\f$, \f$|b_1 - b_2| \leq \var{tol}\f$, \f$|\varphi_1 -
 * \varphi_2| \leq \var{tol}\f$, \f$|h_1 - h_2| \leq \var{tol}\f$,
 * \f$|k_1 - k_2| \leq \var{tol}\f$.
 *
 * Squares are not compared correctly by this function. When \f$a =
 * b\f$, \f$\varphi\f$ should be in \f$[0, \pi / 2)\f$.
 */
bool equal(Rectangle const& r1, Rectangle const& r2,
           double tol = tolerance);

/**
 * Outputs a rectangle to a stream.
 */
std::ostream& operator<<(std::ostream& stream, Rectangle const& r);

/**
 * Returns \e true if and only if the equation \f$ax^2 + 2bxy + cy^2 +
 * 2dx + 2ey + f = 0\f$ represents an ellipse.
 *
 * The equation represents an ellipse if and only if
 * \cite bronsztejn-siemiendiajew-musiol-muhlig-2004, pages 221--222,
 * \f$\delta > 0\f$ and \f$\Delta S < 0\f$, where
 * \f[
 *   \Delta = \left| \begin{array}{ccc}
 *   a & b & d \\
 *   b & c & e \\
 *   d & e & f \end{array} \right|,
 *   \ \delta = \left| \begin{array}{cc}
 *   a & b \\ b & c \end{array} \right|,
 *   \ S = a + c.
 * \f]
 * The function tests the inequalities \f$\delta > \epsilon\f$ and
 * \f$\Delta S < -\epsilon\f$.
 */
bool is_ellipse(double a, double b, double c, double d, double e,
                double f, double eps = 0.0);

/**
 * Representation of an ellipse on the plane.
 *
 * Let \f[ \frac{x^2}{a^2} + \frac{y^2}{b^2} = 1, \quad a \geq b > 0,
 * \f] be the canonical equation of an ellipse with its centre of
 * symmetry in the origin of the coordinate system and axes
 * overlapping axes of coordinate system. A rotation about the origin
 * through an angle \f$0 \leq \varphi < \pi\f$ and translation of its
 * centre of symmetry to a point \f$(h, k)\f$ transform the ellipse to
 * the ellipse \anchor equation_of_ellipse
 * \f[
 * \frac{[(x - h) \cos \varphi + (y - k) \sin \varphi]^2}{a^2} +
 * \frac{[-(x - h) \sin \varphi + (y - k) \cos \varphi]^2}{b^2} = 1.
 * \f]
 * Transformed ellipse will be denoted by \f$E(a, b, \varphi, h,
 * k)\f$, where \f$a \geq b > 0\f$, \f$0 \leq \varphi < \pi\f$, \f$(h,
 * k) \in \mathbf{R}^2\f$. If \f$a = b\f$, we accept \f$\varphi =
 * 0\f$. Each ellipse on the plane has an unequivocal representation
 * in this form. Parameters: \f$a\f$ is the semi-major axis, \f$b\f$
 * is the semi-minor axis, \f$\phi\f$ is the angle between the
 * semi-major axis and the line \f$y = 0\f$, \f$(h, k)\f$ are
 * coordinates of the centre of symmetry of the ellipse.
 */
class Ellipse : public Shape {
public:
     /**
      * Position of a point relative to an ellipse.
      */
     enum class Relative_position { interior, boundary, exterior };

     /**
      * Constructs \f$E(1, 1, 0, 0, 0)\f$.
      */
     Ellipse() = default;
     /**
      * Constructs \f$E(a, b, \varphi, h, k)\f$.
      * \throws std::invalid_argument if \f$a < b\f$ or \f$b \leq 0\f$
      * or \f$\varphi < 0\f$ or \f$\varphi \geq \pi\f$
      */
     Ellipse(double a, double b, double phi, double h, double k);
     double a() const { return a_; }
     double b() const { return b_; }
     double phi() const { return phi_; }
     double h() const { return h_; }
     double k() const { return k_; }

     /**
      * Returns position of the point \f$p\f$ relative to this
      * ellipse. The function calculates the left hand side of \ref
      * equation_of_ellipse "equation of ellipse" and compares the
      * result with 1 using tolerance \f$\var{tol}\f$.
      */
     Relative_position relative_position(
          Point const& p, double tol = tolerance) const;

     /**
      * Returns point of this ellipse for trigonometric parameter. The
      * ellipse \f$E(a, b, \varphi, h, k)\f$ has the parametric
      * equation \anchor ellipse_trigonometric_equation
      * \f{align*}{
      *   x &= h + a \cos \varphi \cos t - b \sin \varphi \sin t, \\
      *   y &= k + a \sin \varphi \cos t + b \cos \varphi \sin t,
      * \f}
      * \f$0 \leq t < 2\pi\f$.
      *
      * \param [in] t the parameter \f$t \in \realn\f$
      * \returns the point \f$(x, y)\f$
      */
     Point trigonometric(double t) const;

     /**
      * Returns point of this ellipse for rational parameter. The
      * ellipse \f$E(a, b, \varphi, h, k)\f$ has the parametric
      * equation \anchor ellipse_rational_equation
      * \cite cox-little-oshea-2007, pages 18--19,
      * \f{align*}{
      *   x &= h + a \cos \varphi c_t - b \sin \varphi s_t, \\
      *   y &= k + a \sin \varphi c_t + b \cos \varphi s_t,
      * \f}
      * where
      * \f{align*}{
      *   c_t &= \frac{b^2 - t^2}{b^2 + t^2}, &
      *     s_t = \frac{2bt}{b^2 + t^2}, \\
      *   c_t' &= -d_t s_t, &
      *     s_t' = d_t c_t, \\
      *   d_t &= \frac{2b}{b^2 + t^2},
      * \f}
      * \f$t \in (-\infty, +\infty)\f$. This parametrization does not
      * include the point \anchor not_parametrized
      * \f{align*}{
      *   x &= h - a \cos \varphi, \\
      *   y &= k - a \sin \varphi.
      * \f}
      *
      * \param [in] t the parameter \f$t \in \realn\f$
      * \returns the point \f$(x, y)\f$
      */
     Point rational(double t) const;

     /**
      * Tangent line to this ellipse at the point \p p.
      * \param [in] p the point on the ellipse
      * \returns tangent line to the ellipse at the point \p p
      *
      * \warning The point \p p must lie on the ellipse, but it is not
      * checked.
      *
      * \implementation We begin with
      * \f{align*}
      *   F(x, y) &= \frac{[(x - h) \cos \varphi + (y - k) \sin
      *     \varphi]^2}{a^2} \\
      *   &+ \frac{[-(x - h) \sin \varphi + (y - k) \cos
      *     \varphi]^2}{b^2} - 1.
      * \f}
      * Denoting \f$c = \cos \varphi\f$, \f$s = \sin \varphi\f$,
      * \anchor eq_partial_x_y
      * \f{align*}{
      *   \frac{\partial F}{\partial x} &= 2\frac{(x - h) c + (y - k)
      *   s}{a^2} c - 2\frac{-(x - h) s + (y - k) c}{b^2} s \\
      *   \frac{\partial F}{\partial y} &= 2\frac{(x - h) c + (y - k)
      *   s}{a^2} s + 2\frac{-(x - h) s + (y - k) c}{b^2} c
      * \f}
      * Putting the derivatives in \ref tangent_for_implicit_curve
      * "general equation of the tangent" we can see that the tangent
      * line has equation \f$Ax + By + C = 0\f$, where
      * \anchor eq_partial_A_B_C_x_y
      * \f{align*}{
      *   A &= \frac{\partial F}{\partial x}(x_0, y_0) \\
      *   B &= \frac{\partial F}{\partial y}(x_0, y_0) \\
      *   C &= -Ax_0 - By_0
      * \f}
      * The algorithm can be as follows:
      * \f{align*}{
      *   x_1 &= x_0 - h \\
      *   y_1 &= y_0 - k \\
      *   c &= \cos \varphi \\
      *   s &= \sin \varphi \\
      *   \alpha &= \frac{x_1 c + y_1 s}{a^2} \\
      *   \beta &= \frac{-x_1 s + y_1 c}{b^2} \\
      *   A &= \alpha c - \beta s \\
      *   B &= \alpha s + \beta c \\
      *   C &= -(A x_0 + B y_0)
      * \f}
      */
     Line tangent(Point const& p) const;

     /**
      * Tangent to this ellipse at the point determined by
      * trigonometric parameter.
      *
      * \param [in] t the parameter \f$t \in \realn\f$
      *
      * \returns tangent line to the ellipse at the point determined
      * by the parameter \p t
      *
      * \see #trigonometric(double) const
      *
      * \implementation From \ref ellipse_trigonometric_equation
      * "trigonometric parametrization" we have
      * \f{align*}{
      *   x'(t_0) &= -a c \sin t_0 - b s \cos t_0 \\
      *   y'(t_0) &= -a s \sin t_0 + b c \cos t_0
      * \f}
      * where \f$c = \cos \varphi\f$, \f$s = \sin \varphi\f$. Using
      * \ref tangent_for_parametric_curve
      * "tangent for parametric equation",
      * in the tangent \f$Ax + By + C = 0\f$ we have
      * \f{align*}{
      *   A &= y'(t_0) \\
      *   B &= -x'(t_0) \\
      *   C &= -Ax_0 - By_0
      * \f}
      * The algorithm could be as follows:
      * \f{align*}{
      *   c &= \cos \varphi \\
      *   s &= \sin \varphi \\
      *   c_0 &= \cos t_0 \\
      *   s_0 &= \sin t_0 \\
      *   A &= b c c_0 - a s s_0 \\
      *   B &= a c s_0 + b s c_0 \\
      *   C &= -Ax_0 - By_0
      * \f}
      */
     Line tangent_trigonometric(double t) const;

     /**
      * Tangent to this ellipse at the point determined by rational
      * parameter.
      *
      * \param [in] t the parameter \f$t \in \realn\f$
      *
      * \returns tangent line to the ellipse at the point determined
      * by the parameter \p t
      *
      * \see #rational(double) const
      *
      * \implementation From \ref ellipse_rational_equation
      * "rational parametrization" we have
      * \f{align*}{
      *   x'(t) &= -acd_ts_t - bsd_tc_t \\
      *   y'(t) &= -asd_ts_t + bcd_tc_t
      * \f}
      * where \f$c = \cos \varphi\f$, \f$s = \sin \varphi\f$. Using
      * \ref tangent_for_parametric_curve
      * "tangent for parametric equation",
      * in the tangent \f$Ax + By + C = 0\f$ we have
      * \f{align*}{
      *   A &= y'(t_0) \\
      *   B &= -x'(t_0) \\
      *   C &= -Ax_0 - By_0
      * \f}
      */
     Line tangent_rational(double t) const;

     /**
      * Returns two lines tangent to this ellipse and parallel to a
      * line.
      *
      * \implementation We are searching for tangent lines to ellipse
      * \f$E(a, b, \varphi, h, k)\f$ which are parallel to a given
      * line \f$Ax + By = 0\f$. There are always two such tangent
      * lines: \f$Ax + By + C_1 = 0\f$ and \f$Ax + By + C_2 = 0\f$,
      * \f$C_1 < C_2\f$. Denote the tangent point by \f$(x, y)\f$,
      * \f$c = \cos \varphi\f$, \f$s = \sin \varphi\f$, \f$x_1 = x -
      * h\f$, \f$y_1 = y - k\f$. The tangent point may be calculated
      * from the system
      * \f{gather*}{
      *   A(x_1 + h) + B(y_1 + k) + C = 0 \\
      *   \frac{(x_1c + y_1s)^2}{a^2} + \frac{(-x_1s + y_1c)^2}{b^2} =
      *   1
      * \f}
      * Assume that \f$B \neq 0\f$, denote \f$y_1 = \alpha + \beta
      * x_1\f$, where \f$\alpha = -\frac{D}{B}\f$, \f$\beta =
      * -\frac{A}{B}\f$, \f$D = Ah + Bk + C\f$, and put it into the
      * last equation:
      * \f{multline*}{
      *   [b^2(c + s \beta)^2 + a^2(s - c \beta)^2]x_1^2 \\
      *   + 2\alpha[\beta(b^2s^2 + a^2c^2) + cs(b^2 - a^2)]x \\
      *   + \alpha^2(b^2s^2 + a^2c^2) - a^2b^2 = 0.
      * \f}
      * This equation may have only one root as the line may have only
      * one point with the ellipse. The discriminant is
      * \f[
      *   4a^2b^2[a^2(s - \beta c)^2 + b^2(c + \beta s)^2 - \alpha^2]
      * \f]
      * so it may be zero if and only if
      * \f[
      *   \alpha^2 = a^2(s - \beta c)^2 + b^2(c + \beta s)^2
      * \f]
      * Putting \f$s - \beta c = s + \frac{A}{B}c\f$, \f$c + \beta s =
      * c - \frac{A}{B}s\f$, we obtain \anchor D_squared
      * \f[
      *   D^2 = a^2(Ac + Bs)^2 + b^2(As - Bc)^2
      * \f]
      * The same equation for \f$D^2\f$ can be obtained assuming \f$A
      * \neq 0\f$. The coefficients of the tangent lines are:
      * \f{align*}{
      *   C_1 &= -D - (Ah + Bk) \\
      *   C_2 &= D - (Ah + Bk)
      * \f}
      * For \f$C \in (C_1, C_2)\f$ the line \f$Ax + By + C = 0\f$
      * crosses the ellipse in two points. For \f$C < C_1\f$ and \f$C>
      * C_2\f$ the line has no common points with the ellipse.
      */
     std::pair<Line, Line> tangent(Line const& line) const;

     /**
      * Returns ellipse tangent to a given line at a given point.
      *
      * For given line \f$l\f$, point \f$p\f$ on this line, semi-axes
      * \f$a \geq b > 0\f$ and the angle \f$0 \leq \varphi < \pi\f$,
      * the function returns ellipse \f$E(a, b, \varphi, h, k)\f$ with
      * such \f$h\f$, \f$k\f$ that the ellipse is tangent to the line
      * \f$l\f$ at the point \f$p\f$. Because there are always two
      * such ellipses, if negative is true, the ellipse for which
      * \f$Ah + Bk + C < 0\f$ is returned, otherwise the one for which
      * \f$Ah + Bk + C > 0\f$.
      *
      * \note This function was created when solving the following
      * abandoned problem. Let be given an ellipse \f$E_0\f$ and a
      * point \f$(x_0, y_0)\f$ on this ellipse. Find all ellipses
      * \f$E(a, b, \varphi, h, k)\f$ with given semi-axes \f$a \geq b
      * > 0\f$, which are tangent to \f$E_0\f$ at \f$(x_0, y_0)\f$.
      * For each \f$0 \leq \varphi < \pi\f$, there exist exactly two
      * such ellipses, each on one side of the tangent line.

      * \implementation We begin with finding ellipses tangent to a
      * line \f$Ax + By + C = 0\f$ at the point \f$(x_0, y_0)\f$ on
      * this line. Let any \f$0 \leq \varphi < \pi\f$ be fixed, \f$c =
      * \cos \varphi\f$, \f$s = \sin \varphi\f$. The coefficients of
      * tangent line to \f$E\f$ at \f$(x_0, y_0)\f$ are proportional
      * to \f$A\f$, \f$B\f$ with proportion ratio \f$\gamma \neq 0\f$.
      * Denoting \f$x_1 = x_0 - h\f$, \f$y_1 = y_0 - k\f$, from \ref
      * eq_partial_x_y "formulas for partial derivatives" and \ref
      * eq_partial_A_B_C_x_y "formulas for coefficients of the line"
      * it follows:
      * \f{align*}{
      *   \frac{x_1c + y_1s}{a^2}c - \frac{-x_1s + y_1c}{b^2}s &=
      *   \frac{1}{2}\gamma A \\
      *   \frac{x_1c + y_1s}{a^2}s + \frac{-x_1s + y_1c}{b^2}c &=
      *   \frac{1}{2}\gamma B
      * \f}
      * Solving this linear system, we get
      * \f{align*}{
      *   x_1 &= \frac{1}{2}\gamma (Ab^2s^2 + Aa^2c^2 - Bb^2cs +
      *   Ba^2cs) \\
      *   y_1 &= \frac{1}{2}\gamma (Bb^2c^2 + Ba^2s^2 - Ab^2cs +
      *   Aa^2cs)
      * \f}
      * Putting \f$x_1\f$ and \f$y_1\f$ into \ref equation_of_ellipse
      * "equation of ellipse", we determine
      * \f[
      *   \gamma^2 = \frac{4}{[a(Ac + Bs)]^2 + [b(Bc - As)]^2}
      * \f]
      * Two tangent ellipses are:
      * \f{align*}{
      *   0 &\leq \varphi < \pi \\
      *   \gamma &= \frac{2}{\sqrt{[a(Ac + Bs)]^2 + [b(Bc - As)]^2}}
      *   \\
      *   h_1 &= x_0 - \frac{1}{2} \gamma (Ab^2s^2 + Aa^2c^2 - Bb^2cs
      *   + Ba^2cs) \\
      *   k_1 &= y_0 - \frac{1}{2} \gamma (Bb^2c^2 + Ba^2s^2 - Ab^2cs
      *   + Aa^2cs) \\
      *   h_2 &= x_0 + \frac{1}{2} \gamma (Ab^2s^2 + Aa^2c^2 - Bb^2cs
      *   + Ba^2cs) \\
      *   k_2 &= y_0 + \frac{1}{2} \gamma (Bb^2c^2 + Ba^2s^2 - Ab^2cs
      *   + Aa^2cs)
      * \f}
      * The values \f$h_1, k_1\f$ correspond to the ellipse located on
      * the negative side of the line \f$Ax + By + C = 0\f$ (ie. for
      * \f$Ax + By + C < 0\f$) and the values \f$h_2, k_2\f$
      * correspond to the one located on the positive side of the
      * line. This can be proved, for example, by putting \f$(h_2,
      * k_2)\f$ into \f$Ax + By + C\f$:
      * \f{multline*}{
      *   Ah_2 + Bk_2 + C = Ax_0 + By_0 + C \\
      *   + \frac{1}{2} \gamma A (Ab^2s^2 + Aa^2c^2 - Bb^2cs + Ba^2cs)
      *   \\
      *   + \frac{1}{2} \gamma B (Bb^2c^2 + Ba^2s^2 - Ab^2cs + Aa^2cs)
      *   \\
      *   = \frac{1}{2} \gamma (A^2b^2s^2 + A^2a^2c^2 + B^2b^2c^2 +
      *   B^2a^2s^2 + 2ABa^2cs - 2ABb^2cs) \\
      *   = \frac{1}{2} \gamma [(Abs - Bbc)^2 + (Aac + Bas)^2] \geq 0
      * \f}
      */
     static Ellipse tangent(Line const& l, Point const& p, double a,
                            double b, double phi, bool negative,
                            double tol = tolerance);

private:
     /**
      * Reduces phi_ to [0, pi).
      *
      * \todo In case if a circle, it should reduce to 0.0.
      */
     void reduce();

     /**
      * Moves this ellipse so that its center is at the point \p p.
      * \f$h \leftarrow p.x(), \quad k \leftarrow p.y()\f$.
      */
     void do_move_to(Point const& p) override;

     /**
      * Moves this ellipse so that its center is at the point <tt>(h +
      * a, k + b)</tt>.
      */
     void do_move_by(double a, double b) override;

     /**
      * Rotates this ellipse counter-clockwise by the angle
      * \f$\varphi\f$ about the point \f$p\f$.
      *
      * \implementation After rotating the ellipse \f$E(a, b, \varphi,
      * h, k)\f$ about the point \f$(x_0, y_0)\f$ counter-clockwise by
      * the angle \f$\theta\f$, the new ellipse will be \f$E(a, b,
      * \alpha, h', k')\f$, where by \ref gen_rot "rotation"
      * \f{align*}{
      *   h' &= x_0 + (h - x_0) \cos \theta - (k - y_0) \sin \theta,
      * \\ k' &= y_0 + (h - x_0) \sin \theta + (k - y_0) \cos \theta.
      * \f}
      * The parameter \f$\alpha\f$ must be calculated. Denote \f[ L =
      * (x' - h') \cos \alpha + (y' - k') \sin \alpha \f] (numerator
      * of the fraction with \f$a^2\f$ in the equation of \f$E(a, b,
      * \alpha, h', k')\f$). By \ref gen_rot "rotation",
      * \f{align*}{
      *   (x' - h') &= (x - h) \cos \theta - (y - k) \sin \theta \\
      *   (y' - k') &= (x - h) \sin \theta + (y - k) \cos \theta
      * \f}
      * so
      * \f{multline*}{
      *   L = (x - h) [\cos \theta \cos \alpha + \sin \theta \sin
      *   \alpha] + (y - k) [\cos \theta \sin \alpha - \sin \theta
      *   \cos \alpha] \\ = (x - h) \cos(\alpha - \theta) + (y - k)
      *   \sin(\alpha - \theta).
      * \f}
      * Consequently, \f$\alpha - \theta = \varphi\f$, so \f$\alpha =
      * \varphi + \theta\f$.
      */
     void do_rotate(double phi, Point const& p) override;

     double a_{1.0};
     double b_{1.0};
     double phi_{0.0};
     double h_{0.0};
     double k_{0.0};
     double sin_phi_{0.0};
     double cos_phi_{1.0};
};

/**
 * Returns \e true if and only if the two ellipses approximately
 * overlap. This is true if and only if \f$|a_1 - a_2| \leq
 * \var{tol}\f$, \f$|b_1 - b_2| \leq \var{tol}\f$, \f$|\varphi_1 -
 * \varphi_2| \leq \var{tol}\f$, \f$|h_1 - h_2| \leq \var{tol}\f$,
 * \f$|k_1 - k_2| \leq \var{tol}\f$.
 *
 * Circles are not compared correctly by this function. When \f$a =
 * b\f$, \f$\varphi\f$ should be 0.
 */
bool equal(Ellipse const& e1, Ellipse const& e2,
           double tol = tolerance);

/**
 * Outputs an ellipse to a stream.
 */
std::ostream& operator<<(std::ostream& stream, Ellipse const& e);

/**
 * Coefficients of polynomial for finding common points of two
 * ellipses. The function returns a vector \f$a\f$ of five
 * coefficients of the polynomial whose real roots are used to find
 * common points of two ellipse. The equation to find common points is
 * \f[ a_4t^4 + a_3t^3 + a_2t^2 + a_1t + a_0 = 0. \f]
 *
 * \implementation Let be given two ellipses:
 * \f{align*}{
 *   E_1 &= E(a_1, b_1, \varphi_1, h_1, k_1), \\
 *   E_2 &= E(a_2, b_2, \varphi_2, h_2, k_2).
 * \f}
 * Let \f$E_1\f$ has \ref ellipse_rational_equation
 * "parametric equations"
 * \f{align*}{
 *   x &= h_1 + a_1 \cos \varphi_1 c_t - b_1 \sin \varphi_1 s_t, \\
 *   y &= k_1 + a_1 \sin \varphi_1 c_t + b_1 \cos \varphi_1 s_t.
 * \f}
 * These two expressions we put into equation of \f$E_2\f$:
 * \f[
 *   b_2^2\left[(x - h_2) \cos \varphi_2 + (y - k_2) \sin \varphi_2
 *   \right]^2 + a_2^2 \left[-(x - h_2) \sin \varphi_2 + (y - k_2)
 *   \cos \varphi_2 \right]^2 - a_2^2b_2^2 = 0.
 * \f]
 * We have
 * \f{align*}{
 *   x - h_2 &= A_x + B_x c_t - C_x s_t \\
 *   y - k_2 &= A_y + B_y c_t + C_y s_t
 * \f}
 * where
 * \f{align*}{
 *   A_x &= h_1 - h_2 & B_x &= a_1 \cos \varphi_1 & C_x &= b_1 \sin
 *   \varphi_1 \\
 *   A_y &= k_1 - k_2 & B_y &= a_1 \sin \varphi_1 & C_y &= b_1 \cos
 *   \varphi_1
 * \f}
 * Substituting these in the above equation of \f$E_2\f$:
 * \f{align*}{
 *   b_2^2 \bigl[
 *     & A_x \cos \varphi_2
 *     + B_x \cos \varphi_2 c_t
 *     - C_x \cos \varphi_2 s_t
 *     + A_y \sin \varphi_2
 *     + B_y \sin \varphi_2 c_t
 *     + C_y \sin \varphi_2 s_t \bigr]^2 \\
 *   + a_2^2 \bigl[
 *     - & A_x \sin \varphi_2
 *     - B_x \sin \varphi_2 c_t
 *     + C_x \sin \varphi_2 s_t
 *     + A_y \cos \varphi_2
 *     + B_y \cos \varphi_2 c_t
 *     + C_y \cos \varphi_2 s_t \bigr]^2 - a_2^2b_2^2 = 0.
 * \f}
 * Denoting:
 * \f{align*}{
 *   \alpha_1 &= b_2 (A_x \cos \varphi_2 + A_y \sin \varphi_2) \\
 *   \alpha_2 &= b_2 (B_x \cos \varphi_2 + B_y \sin \varphi_2) \\
 *   \alpha_3 &= b_2 (-C_x \cos \varphi_2 + C_y \sin \varphi_2) \\
 *   \beta_1  &= a_2 (-A_x \sin \varphi_2 + A_y \cos \varphi_2) \\
 *   \beta_2  &= a_2 (-B_x \sin \varphi_2 + B_y \cos \varphi_2) \\
 *   \beta_3  &= a_2 (C_x \sin \varphi_2 + C_y \cos \varphi_2)
 * \f}
 * we get
 * \f[
 *   (\alpha_1 + \alpha_2 c_t + \alpha_3 s_t)^2
 *   + (\beta_1 + \beta_2 c_t + \beta_3 s_t)^2 - a_2^2 b_2^2 = 0
 * \f]
 * After expanding squares and denoting
 * \f{align*}{
 *   A &= \alpha_2^2 + \beta_2^2 \\
 *   B &= 2(\alpha_2 \alpha_3 + \beta_2 \beta_3) \\
 *   C &= \alpha_3^2 + \beta_3^2 \\
 *   D &= 2(\alpha_1 \alpha_2 + \beta_1 \beta_2) \\
 *   E &= 2(\alpha_1 \alpha_3 + \beta_1 \beta_3) \\
 *   F &= \alpha_1^2 + \beta_1^2 - a_2^2 b_2^2
 * \f}
 * we get
 * \f[
 *   A c_t^2 + B c_t s_t + C s_t^2 + D c_t + E s_t + F = 0
 * \f]
 * Multiplying both sides by \f$(b_1^2 + t^2)^2\f$, we get the
 * equation
 * \f[
 *   a_4 t^4 + a_3 t^3 + a_2 t^2 + a_1 t + a_0 = 0,
 * \f]
 * where
 * \f{align*}{
 *   a_4 &= A - D + F \\
 *   a_3 &= 2 b_1 (E - B) \\
 *   a_2 &= 2 b_1^2 (F - A + 2C) \\
 *   a_1 &= 2 b_1^3 (E + B) \\
 *   a_0 &= b_1^4 (A + D + F)
 * \f}
 */
Vecdouble common_points_polynomial(Ellipse const& e1,
                                   Ellipse const& e2);

/**
 * Finds common points of ellipses. The common points are returned in
 * \p p and the number of the points is \p p.size().
 * \returns 0 success
 * \returns 1 the ellipses everlap
 * \returns 2 the polynomial returned by common_points_polynomial() is
 * of degree 0
 * \returns 3 the procedure of finding roots failed
 */
int common_points(Ellipse const& e1, Ellipse const& e2, Vecpoint& p);

/**
 * Coefficients of polynomial for finding distance between point and
 * ellipse. The function returns a vector \f$a\f$ of five coefficients
 * of the polynomial whose real roots are used to find distance from
 * the point \p p to the ellipse \p e. The equation to find the
 * closest point on the ellipse is \f[ a_4t^4 + a_3t^3 + a_2t^2 + a_1t
 * + a_0 = 0. \f]
 *
 * \implementation To find the distance between the point \f$(x_0,
 * y_0)\f$ and the ellipse given by \ref ellipse_rational_equation
 * "parametric equations", the function \f[ f(t) = (x_t - x_0)^2 +
 * (y_t - y_0)^2 \f] should be minimized and the distance from
 * \f$(x_0, y_0)\f$ to the point \ref not_parametrized
 * "not parametrized" should be taken into account.
 * As
 * \f{align*}{
 *   x'_t &= d_t(-a \cos \varphi s_t - b \sin \varphi c_t) \\
 *   y'_t &= d_t(-a \sin \varphi s_t + b \cos \varphi c_t),
 * \f}
 * we have
 * \f{align*}{
 *   \frac{1}{2d_t}f'(t) &= (A_x + B_xc_t - C_xs_t)(-B_xs_t-C_xc_t) \\
 *   &+ (A_y + B_y c_t + C_y s_t)(-B_y s_t + C_y c_t),
 * \f}
 * where
 * \f{align*}{
 *   A_x &= h - x_0 & B_x &= a \cos \varphi & C_x &= b \sin \varphi \\
 *   A_y &= k - y_0 & B_y &= a \sin \varphi & C_y &= b \cos \varphi
 * \f}
 * After expanding and denoting
 * \f{align*}{
 *   A &= -B_x C_x + B_y C_y = 0 \\
 *   B &= B_x C_x - B_y C_y = 0 \\
 *   C &= -B_x^2 + C_x^2 - B_y^2 + C_y^2 = b^2 - a^2 \\
 *   D &= -A_x C_x + A_y C_y \\
 *   E &= -A_xB_x - A_y B_y
 * \f}
 * we get
 * \f[
 *   A c_t^2 + B s_t^2 + C c_t s_t + D c_t + E s_t = 0.
 * \f]
 * Multiplying both sides by \f$(b^2 + t^2)^2\f$, we get
 * \f[
 *   a_4 t^4 + a_3 t^3 + a_2 t^2 + a_1 t + a_0 = 0,
 * \f]
 * where
 * \f{align*}{
 *   a_4 &= -D \\
 *   a_3 &= 2 b (E - C) \\
 *   a_2 &= 0 \\
 *   a_1 &= 2 b^3 (E + C) \\
 *   a_0 &= b^4 D
 * \f}
 */
Vecdouble closest_points_polynomial(Point const& p, Ellipse const& e);

/**
 * Calculates distance between a point and an ellipse.
 * \param [in] p point
 * \param [in] e ellipse
 * \param [out] d distance
 * \param [out] q one or two closest points
 */
void distance(Point const& p, Ellipse const& e, double& d,
              Vecpoint& q);

/**
 * Checks if a point is a tangent point of two ellipses.
 *
 * \returns \e true if and only if the point \p p is a tangent point
 * of two ellipses \p e1 and \p e2, ie. if there exists common tangent
 * line to both ellipses passing through the point \p p
 *
 * \throws std::invalid_argument if \p p does not belong to boundaries
 * of both ellipses
 */
bool is_tangent_point(Ellipse const& e1, Ellipse const& e2,
                      Point const& p, double tol = tolerance);

/** \} */ /* end of group geometric_functions */

}  // namespace SHG

#endif
