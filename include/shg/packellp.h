/**
 * \file include/shg/packellp.h
 * Packing ellipses.
 */

#ifndef SHG_PACKELLP_H
#define SHG_PACKELLP_H

#include <shg/geometry.h>

/**
 * Packing ellipses.
 */
namespace SHG::PACKING {

/**
 * \defgroup packing_ellipses Packing ellipses
 *
 * Packing ellipses.
 *
 * \par Minimum rectangle containing an ellipse.
 * For each \f$0 \leq \alpha < \pi / 2\f$ take a straight line
 * inclined to the \f$x\f$-axis at the angle \f$\alpha\f$
 * \anchor line1_tmp
 * \f[
 *   \sin \alpha \ x - \cos \alpha \ y = 0,
 * \f]
 * and a straight line perpendicular to this line \anchor line2_tmp
 * \f[
 *   \cos \alpha \ x + \sin \alpha \ y = 0.
 * \f]
 * For the ellipse \f$E(a, b, \varphi, h, k)\f$, tangent lines
 * parallel to these lines, respectively, have the values of \ref
 * D_squared "D"
 * \f{align*}{
 *   D_1 &= \sqrt{a^2 \sin^2 \theta + b^2 \cos^2 \theta} \\
 *   D_2 &= \sqrt{a^2 \cos^2 \theta + b^2 \sin^2 \theta}
 * \f}
 * where \f$\theta = \alpha - \varphi\f$. The sides of the rectangle
 * created by these two lines are \f$2D_1\f$ and \f$2D_2\f$,
 * respectively, and the area of this rectangle is
 * \f[
 *   S = 4 D_1 D_2
 *     = 4 \sqrt{a^2b^2 + (a^2 - b^2)^2 \sin^2 \theta \cos^2 \theta}
 * \f]
 * For \f$0 \leq \alpha < \pi / 2\f$ and \f$0 \leq \varphi < \pi /
 * 2\f$ the area is minimum when \f$\alpha = \phi\f$ and is equal to
 * \f$4ab\f$, as expected.
 *
 * \par Coordinates of an ellipse tangent to two tangent ellipses.
 * Let \f$a \geq b > 0\f$ and \f$E_1(a, b, 0, a, 0)\f$, \f$E_2(a, b,
 * 0, 3a, 0)\f$ be two ellipses. Find \f$k > 0\f$ such that the
 * ellipse \f$E_3(a, b, 0, 2a, k)\f$ is tangent to \f$E_1\f$ and
 * \f$E_2\f$. Let \f$(x_0, y_0)\f$ be the tangent point of \f$E_1\f$
 * and \f$E_3\f$. There must be \f$a < x_0 < 2a\f$, \f$0 < y_0 < b\f$,
 * \f$b < k < 2b\f$. Let
 * \f{align*}{
 *   A_1x + B_1y + C_1 &= 0 \\
 *   A_3x + B_3y + C_3 &= 0
 * \f}
 * be tangent lines to \f$E_1\f$, \f$E_3\f$, respectively.
 * From equations in Ellipse::tangent(const Point&) const it follows
 * that
 * \f{align*}{
 *   A_1 &= \frac{2(x_0 - a)}{a^2} & B_1
 *       &= \frac{2y_0}{b^2} & C_1 &= -A_1x_0 - B_1y_0  \\
 *   A_3 &= \frac{2(x_0 - 2a)}{a^2} & B_3
 *       &= \frac{2(y_0 - k)}{b^2} & C_3 &= -A_3x_0 - B_3y_0.
 * \f}
 * The two lines must overlap, so \f$A_1B_3 = A_3B_1\f$. From this,
 * \f$y_0\f$ and \f$y_0 - k\f$ can be determined as
 * \f[
 *   y_0 = \frac{k(x_0 - a)}{a}, \quad y_0 - k = k \frac{x_0 - 2a}{a}.
 * \f]
 * The point \f$(x_0, y_0)\f$ lies on both \f$E_1\f$ and \f$E_3\f$, so
 * putting \f$y_0\f$ and \f$y_0 - k\f$ into the system
 * \f{gather*}{
 *   \frac{(x_0 - a)^2}{a^2} + \frac{y_0^2}{b^2} = 1 \\
 *   \frac{(x_0 - 2a)^2}{a^2} + \frac{(y_0 - k)^2}{b^2} = 1
 * \f}
 * we get \anchor distance_between_rows
 * \f{gather*}{
 *   k = b \sqrt{3} \\
 *   x_0 = \frac{3}{2}a, \quad y_0 = b \frac{\sqrt{3}}{2}
 * \f}
 *
 * \{
 */

/**
 * Returns the minimum alpha-rectangle for the set of ellipses.
 * \returns the minimum rectangle containing all ellipses given in
 * \f$v\f$ and whose longer side is inclined to the \f$x\f$-axis at
 * the angle \f$\alpha\f$.
 * \implementation For each ellipse the function
 * Ellipse::tangent(const Line&) const is called and minimum and
 * maximum values of the free terms are collected. For the line
 * \f$\sin \alpha \ x - \cos \alpha \ y = 0\f$ (see \ref line1_tmp
 * "inclined")
 * \f{align*}{
 *   D_{1i} &= \sqrt{a_i^2 \sin^2(\alpha - \varphi_i) + b_i^2
 *     \cos^2(\alpha - \varphi_i)} \\
 *   C^{11}_i &= -D_{1i} - h_i \sin \alpha + k_i \cos \alpha \\
 *   C^{12}_i &= D_{1i} - h_i \sin \alpha + k_i \cos \alpha
 * \f}
 * and for the line \f$\cos \alpha \ x + \sin \alpha \ y = 0\f$ (see
 * \ref line2_tmp "perpendicular")
 * \f{align*}{
 *   D_{2i} &= \sqrt{a_i^2 \cos^2(\alpha - \varphi_i) + b_i^2
 *     \sin^2(\alpha - \varphi_i)} \\
 *   C^{21}_i &= -D_{2i} - h_i \cos \alpha - k_i \sin \alpha \\
 *   C^{22}_i &= D_{2i} - h_i \cos \alpha - k_i \sin \alpha
 * \f}
 * Then
 * \f{align*}{
 *   C^1_{\mathit{min}} &= \min C^{11}_i \\
 *   C^1_{\mathit{max}} &= \max C^{12}_i \\
 *   C^2_{\mathit{min}} &= \min C^{21}_i \\
 *   C^2_{\mathit{max}} &= \max C^{22}_i
 * \f}
 * The minimum rectangle is determined by the cross points of the
 * lines
 * \f{align*}{
 *   \sin \alpha \ x - \cos \alpha \ y + C^1_{\mathit{min}} &= 0 \\
 *   \sin \alpha \ x - \cos \alpha \ y + C^1_{\mathit{max}} &= 0 \\
 *   \cos \alpha \ x + \sin \alpha \ y + C^2_{\mathit{min}} &= 0 \\
 *   \cos \alpha \ x + \sin \alpha \ y + C^2_{\mathit{max}} &= 0
 * \f}
 */
Rectangle min_alpha_rectangle(const std::vector<Ellipse>& v,
                              double alpha, double tol = tolerance);

/**
 * Returns \e true if and only if positions of ellipses in \p v are
 * feasible. The positions are feasisible if interiors of ellipses are
 * disjoint.
 */
bool is_feasible_packing(const std::vector<Ellipse>& v,
                         double tol = tolerance);

/**
 * Returns the minimum rectangle for the set of ellipses. Uses Brent
 * minimization.
 */
Rectangle min_rectangle(const std::vector<Ellipse>& v,
                        double tol = tolerance);

/**
 * Class to represent semi-axes of ellipse.
 */
class Semiaxes {
public:
     Semiaxes(double major, double minor);
     double major() const { return major_; }
     double minor() const { return minor_; }

private:
     double major_;
     double minor_;
};

/**
 * Returns the minimum rectangle for movable ellipses. Uses Nelder and
 * Mead algorithm.
 */
Rectangle min_rectangle(const std::vector<Semiaxes>& semiaxes,
                        std::vector<Ellipse>& ellipses,
                        double tol = tolerance);

/**
 * A class for investigating minimum rectangle for congruent ellipses
 * arranged regularly.
 * We arrange on the plane \f$n \geq 1\f$ cogruent ellipses with
 * semiaxes \f$a \geq b > 0\f$. Each ellipse is placed so that its
 * major axis is parallel to the \f$x\f$-axis and its center is in one
 * of the points
 * \f[
 *   R_{ij} = \biggl( (2j - i + 2 \lfloor i / 2 \rfloor) a, \; b + (i
 *   - 1)K \biggr), \quad i, j \in \naturaln.
 * \f]
 * \f$K = b \sqrt{3}\f$ denotes here the \ref distance_between_rows
 * "distance between rows".
 *
 * The method run() generates all possible arrangements (ordered
 * partitions).
 */
class Congruent_regular {
public:
     Congruent_regular(std::size_t n, double a, double b);
     void operator()(int k, const std::vector<int>& a);
     void run();
     std::size_t n() const;
     double a() const;
     double b() const;
     const std::vector<int>& p() const;
     double area() const;

private:
     const std::size_t n_;
     const double a_;
     const double b_;
     const double K_;
     std::vector<int> p_{};  // best partition
     double area_{};         // minimum area
     bool first_{true};
     std::vector<int> w_{};  // work partition
};

/** \} */ /* end of group packing_ellipses */

}  // namespace SHG::PACKING

#endif
