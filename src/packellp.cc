/**
 * \file src/packellp.cc
 * Packing ellipses.
 */

#include <shg/packellp.h>
#include <shg/fcmp.h>
#include <shg/ipart.h>
#include <shg/mconsts.h>
#include <shg/numalg.h>

namespace SHG::PACKING {

using std::cos;
using std::sin;

Rectangle min_alpha_rectangle(std::vector<Ellipse> const& v,
                              double alpha, double tol) {
     if (alpha < 0.0 || alpha >= 0.5 * Constants::pi<double>)
          throw std::invalid_argument(
               "invalid angle in min_alpha_rectangle");
     double const sa = sin(alpha);
     double const ca = cos(alpha);
     Line const line1(sa, -ca, 0.0);
     Line const line2(ca, sa, 0.0);
     auto it = v.cbegin();
     if (it == v.cend())
          throw std::invalid_argument(
               "no ellipses given in min_alpha_rectangle");
     auto p = it->tangent(line1);
     double minC1 = p.first.C();
     double maxC1 = p.second.C();
     p = v[0].tangent(line2);
     double minC2 = p.first.C();
     double maxC2 = p.second.C();

     while (++it != v.cend()) {
          p = it->tangent(line1);
          if (p.first.C() < minC1)
               minC1 = p.first.C();
          if (p.second.C() > maxC1)
               maxC1 = p.second.C();
          p = it->tangent(line2);
          if (p.first.C() < minC2)
               minC2 = p.first.C();
          if (p.second.C() > maxC2)
               maxC2 = p.second.C();
     }

     minC1 = -minC1;
     maxC1 = -maxC1;
     minC2 = -minC2;
     maxC2 = -maxC2;

     Point const p1(minC1 * sa + maxC2 * ca, maxC2 * sa - minC1 * ca);
     Point const p2(maxC1 * sa + maxC2 * ca, maxC2 * sa - maxC1 * ca);
     Point const p3(maxC1 * sa + minC2 * ca, minC2 * sa - maxC1 * ca);
     Point const p4(minC1 * sa + minC2 * ca, minC2 * sa - minC1 * ca);

     return Rectangle(p1, p2, p3, p4, tol);
}

bool is_feasible_packing(std::vector<Ellipse> const& v, double tol) {
     using sztp = std::vector<Ellipse>::size_type;

     for (sztp i = 0; i < v.size(); i++) {
          Ellipse const& e1 = v[i];
          for (sztp j = i + 1; j < v.size(); j++) {
               Ellipse const& e2 = v[j];
               double const d =
                    distance({e1.h(), e1.k()}, {e2.h(), e2.k()});
               if (d < e1.b() + e2.b())
                    return false;
               if (d >= e1.a() + e2.a())
                    continue;
               if (e1.relative_position({e2.h(), e2.k()}, tol) !=
                   Ellipse::Relative_position::exterior)
                    return false;  // Center of e1 belongs to Int(e2)
                                   // or Fr(e2).
               if (e2.relative_position({e1.h(), e1.k()}, tol) !=
                   Ellipse::Relative_position::exterior)
                    return false;  // Center of e2 belongs to Int(e1)
                                   // or Fr(e1).
               std::vector<Point> p;
               int const r = common_points(e1, e2, p);
               if (r != 0)
                    throw std::runtime_error(__func__);
               if (p.size() > 1)
                    return false;  // Fr(e1) and Fr(e2) have two or
                                   // more common points.
          }
     }
     return true;
}

namespace {

struct fn2_params {
     std::vector<Ellipse> const& v;
     double const tol;
};

double fn2(double alpha, void* p) {
     fn2_params* params = static_cast<fn2_params*>(p);
     Rectangle r = min_alpha_rectangle(params->v, alpha, params->tol);
     return r.area();
}

}  // anonymous namespace

Rectangle min_rectangle(std::vector<Ellipse> const& v, double tol) {
     tol = 1e-7;
     auto f = [&v, tol](double alpha) {
          Rectangle r = min_alpha_rectangle(v, alpha, tol);
          return r.area();
     };
     Uniform_search_for_minimum smin;
     smin.search(f, 0.0, 0.5 * Constants::pi<double> - tol, 0.01);
     if (smin.result.empty())
          return min_alpha_rectangle(v, 0.0, tol);

     Minimizer m(gsl_min_fminimizer_brent);
     gsl_function F;
     fn2_params params = {v, tol};
     F.function = &fn2;
     F.params = &params;

     Rectangle rectangle;

     auto it = smin.result.cbegin();
     while (it != smin.result.cend()) {
          m.set(&F, it->x_minimum, it->x_lower, it->x_upper);
          int const status = m.iterate(100, tol, tol);
          if (status != 0)
               throw std::runtime_error("min_rectangle: status != 0");
          auto r = min_alpha_rectangle(v, it->x_minimum, tol);
          if (it == smin.result.cbegin())
               rectangle = r;
          else if (r.area() < rectangle.area())
               rectangle = r;
          ++it;
     }
     return rectangle;
}

Semiaxes::Semiaxes(double major, double minor)
     : major_(major), minor_(minor) {
     if (minor > major || minor <= 0.0)
          throw std::invalid_argument(
               "semi-major axis less than semi-minor axis");
}

struct Parameters {
     std::size_t const n;
     std::vector<Semiaxes> const& semiaxes;
     double const big;
     double const tol;
};

double obj(gsl_vector const* x, void* params) {
     Parameters* p = static_cast<Parameters*>(params);
     std::vector<Ellipse> v(p->n);

     v[0] = {p->semiaxes[0].major(), p->semiaxes[0].minor(), 0.0,
             p->semiaxes[0].major(), 0.0};
     std::size_t j = 0;
     for (std::vector<Ellipse>::size_type i = 1; i < p->n; i++) {
          double phi = gsl_vector_get(x, j++);
          if (phi < 0.0 && phi > -p->tol)
               phi = 0.0;
          while (phi < 0.0)
               phi += Constants::pi<double>;
          double const h = gsl_vector_get(x, j++);
          double const k = gsl_vector_get(x, j++);
          v[i] = {p->semiaxes[i].major(), p->semiaxes[i].minor(), phi,
                  h, k};
     }
     bool const feas = is_feasible_packing(v, p->tol);
     if (!feas)
          return p->big;
     auto r = min_rectangle(v, p->tol);
     return r.area();
}

Rectangle min_rectangle(std::vector<Semiaxes> const& semiaxes,
                        std::vector<Ellipse>& ellipses, double tol) {
     if (semiaxes.size() < 2 || tol <= 0.0)
          throw std::invalid_argument(__func__);

     std::vector<double> x(3 * (semiaxes.size() - 1));
     std::vector<double>::size_type k = 0;
     std::vector<Semiaxes>::size_type j;
     double a = 0.0, b = 0.0;
     for (j = 0; j < semiaxes.size(); j++) {
          a += semiaxes[j].major();
          if (j > 0) {
               x[k++] = 0.0;  // phi
               x[k++] = a;    // h
               x[k++] = 0.0;  // k
          }
          a += semiaxes[j].major();
          if (semiaxes[j].minor() > b)
               b = semiaxes[j].minor();
     }
     assert(k == x.size());

     Parameters parameters{semiaxes.size(), semiaxes, a * b + 100.0,
                           tol};
     gsl_multimin_function F;
     F.n = 3 * (semiaxes.size() - 1);
     F.f = obj;
     F.params = &parameters;
     std::vector<double> step(3 * (semiaxes.size() - 1), 1.0);
     Multimin_fminimizer m{gsl_multimin_fminimizer_nmsimplex2,
                           3 * (semiaxes.size() - 1)};
     m.set(&F, x, step);
     int status = m.iterate(100, 1e-2);
     if (status != 0)
          status = m.iterate(100, 1e-2);
     if (status != 0)
          throw std::runtime_error("method did not converged");

     ellipses.resize(semiaxes.size());
     ellipses[0] = {semiaxes[0].major(), semiaxes[0].minor(), 0.0,
                    semiaxes[0].major(), 0.0};
     std::vector<double> const& x_minimum = m.x_minimum();
     k = 0;
     for (std::vector<Ellipse>::size_type i = 1; i < ellipses.size();
          i++) {
          double phi = x_minimum[k++];
          if (phi < 0.0 && phi > -2e-5)
               phi = 0.0;
          double const h = x_minimum[k++];
          double const k1 = x_minimum[k++];
          ellipses[i] = {semiaxes[i].major(), semiaxes[i].minor(),
                         phi, h, k1};
     }
     auto r = min_rectangle(ellipses, tol);
     return r;
}

Congruent_regular::Congruent_regular(std::size_t n, double a,
                                     double b)
     : n_(n), a_(a), b_(b), K_(b * std::sqrt(3.0)) {
     if (n < 1 || a < b || b <= 0.0)
          throw std::invalid_argument(__func__);
}

void Congruent_regular::operator()(int k, std::vector<int> const& a) {
     w_.assign(a.cbegin(), a.cbegin() + k);
     int const m = *std::max_element(w_.cbegin(), w_.cend());
     bool ext = false;
     for (std::vector<int>::size_type i = 0; i < w_.size(); i++)
          if (w_[i] == m && i % 2 == 1) {
               ext = true;
               break;
          }
     double const height = 2.0 * b_ + (w_.size() - 1) * K_;
     double area = 2.0 * a_ * m * height;
     if (ext)
          area += a_ * height;
     if (first_) {
          p_ = w_;
          area_ = area;
          first_ = false;
     } else if (area < area_) {
          p_ = w_;
          area_ = area;
     }
}

void Congruent_regular::run() {
     Ordered_partitions_generator g(n_, *this);
     g.generate();
}

std::size_t Congruent_regular::n() const {
     return n_;
}

double Congruent_regular::a() const {
     return a_;
}

double Congruent_regular::b() const {
     return b_;
}

std::vector<int> const& Congruent_regular::p() const {
     return p_;
}

double Congruent_regular::area() const {
     return area_;
}

}  // namespace SHG::PACKING
