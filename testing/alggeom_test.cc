#include <shg/alggeom.h>
#include <cmath>
#include <numeric>
#include <sstream>
#include <boost/math/special_functions/binomial.hpp>
#include <shg/utils.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(alggeom_test)

namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(is_prime_test, bdata::xrange(1000), xr) {
     using SHG::ALGGEOM::is_prime;
     const int n = xr;
     bool is_prime1 = n > 1;
     if (is_prime1) {
          for (int i = 2; i * i <= n; i++)
               if (n % i == 0) {
                    is_prime1 = false;
                    break;
               }
     }
     BOOST_CHECK(is_prime1 == is_prime(n));
}

/**
 * Basic test. See \cite knuth-2002b, section 4.5.2, page 367.
 */
BOOST_AUTO_TEST_CASE(extended_gcd_basic_test) {
     using SHG::ALGGEOM::Extended_gcd;
     {
          Extended_gcd<int> xgcd;
          xgcd.calculate(40902, 24140);
          BOOST_CHECK(xgcd.u1 == 337 && xgcd.u2 == -571 &&
                      xgcd.u3 == 34);
     }
     {
          Extended_gcd xgcd(40902, 24140);
          BOOST_CHECK(xgcd.u1 == 337 && xgcd.u2 == -571 &&
                      xgcd.u3 == 34);
     }
}

BOOST_DATA_TEST_CASE(extended_gcd_extended_test,
                     bdata::xrange(20) * bdata::xrange(20), xr1,
                     xr2) {
     using SHG::ALGGEOM::Extended_gcd;
     using std::gcd;
     Extended_gcd<int> xgcd(xr1, xr2);
     BOOST_CHECK(xr1 * xgcd.u1 + xr2 * xgcd.u2 == xgcd.u3);
     BOOST_CHECK(xgcd.u3 == gcd(xr1, xr2));
}

template <typename T>
T ipow(T x, int n) {
     T y = T(1);
     for (int i = 1; i <= n; i++)
          y *= x;
     return y;
}

BOOST_AUTO_TEST_CASE(pow_test) {
     using SHG::ALGGEOM::pow;
     for (int x = -10; x <= 10; x++) {
          for (int n = 0; n <= 5; n++) {
               const int p = pow(x, n);
               const int p2 = ipow(x, n);
               BOOST_CHECK(p == p2);
          }
     }
     for (int x = -3; x <= 3; x++) {
          for (int n = 0; n <= 18; n++) {
               const int p = pow(x, n);
               const int p2 = ipow(x, n);
               BOOST_CHECK(p == p2);
          }
     }
     for (int x = -2; x <= 2; x++) {
          for (int n = 0; n <= 31; n++) {
               const int p = pow(x, n);
               const int p2 = ipow(x, n);
               BOOST_CHECK(p == p2);
          }
     }
     const double x = 1.001;
     for (int n = 0; n <= 1000; n++) {
          const double p = pow(x, n);
          const double p2 = std::pow(x, n);
          BOOST_CHECK(faeq(p, p2, 8e-14));
     }
}

using SHG::ALGGEOM::F_p;
using SHG::ALGGEOM::Z_n;
using z_n_test_types =
     boost::mpl::list<Z_n<2>, Z_n<3>, Z_n<4>, Z_n<5>, Z_n<6>, Z_n<7>,
                      Z_n<8>, Z_n<9>, Z_n<10>, F_p<2>, F_p<3>, F_p<5>,
                      F_p<7>, F_p<11>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(zn_template_test, T, z_n_test_types) {
     for (int i = -T::N_ - 3; i <= T::N_ + 3; i++)
          if (i >= 0) {
               BOOST_CHECK_NO_THROW(T w(i));
               T z;
               BOOST_CHECK_NO_THROW(z = i);
          } else {
               BOOST_CHECK_THROW(T w(i), std::invalid_argument);
               T z;
               BOOST_CHECK_THROW(z = i, std::invalid_argument);
          }
     for (int i = 0; i < T::N_; i++) {
          const T z(i);
          {
               T u, w, w1;
               w = i;
               w1 = i == T::N_ - 1 ? 0 : i + 1;
               u = z;
               BOOST_CHECK(u == w);
               u++;
               BOOST_CHECK(u == w1);
               u--;
               BOOST_CHECK(u == w);
               BOOST_CHECK(++u == w1);
               BOOST_CHECK(--u == w);
               u = +w;
               BOOST_CHECK(u == w);
               u = -w;
               u = -u;
               BOOST_CHECK(u == w);
          }
          for (int j = 0; j < T::N_; j++) {
               const T w(j);
               T r = T((i + j) % T::N_);
               T u = z;
               u += w;
               BOOST_CHECK(u == r);
               BOOST_CHECK(z + w == r);
               u = z;
               u -= w;
               BOOST_CHECK(u + w == z);
               u = z - w;
               BOOST_CHECK(u + w == z);
               r = T((i * j) % T::N_);
               u = z;
               u *= w;
               BOOST_CHECK(u == r);
               BOOST_CHECK(z * w == r);
          }
          for (int j = -T::N_ - 3; j <= T::N_ + 3; j++) {
               const T w1 = j * z;
               const T w2 = z * j;
               const T r =
                    j < 0 ? -T((i * -j) % T::N_) : T((i * j) % T::N_);
               BOOST_CHECK(w1 == r);
               BOOST_CHECK(w2 == r);
          }
          for (int j = 0; j <= T::N_ + 10; j++) {
               const T w = pow(z, j);
               T r(1);
               for (int k = 0; k < j; k++)
                    r *= z;
               BOOST_CHECK(w == r);
          }
          {
               std::stringstream ss;
               T w;
               ss << z;
               ss >> w;
               BOOST_REQUIRE(ss);
               BOOST_CHECK(w == z);
          }
     }
}

using f_p_test_types =
     boost::mpl::list<F_p<2>, F_p<3>, F_p<5>, F_p<7>, F_p<11>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(f_p_template_test, T, f_p_test_types) {
     using SHG::ALGGEOM::Invalid_F_p;
     constexpr int N = T::N_;
     {
          Z_n<N> w(N - 1);
          F_p<N> u(w);
          BOOST_CHECK(u == F_p<N>(N - 1));
     }
     {
          Z_n<N> w(N - 1);
          F_p<N> u;
          u = w;
          BOOST_CHECK(u == F_p<N>(N - 1));
     }
     {
          constexpr int N2 = 2 * N;
          Z_n<N2> w(N2 - 2);
          BOOST_CHECK_THROW(F_p<N2> u, Invalid_F_p);
          BOOST_CHECK_THROW(F_p<N2> u(w), Invalid_F_p);
          BOOST_CHECK_THROW(F_p<N2> u{w}, Invalid_F_p);
     }
     for (int i = 0; i < N; i++) {
          const F_p<N> z{i};
          if (i == 0)
               BOOST_CHECK_THROW(z.inv(), std::invalid_argument);
          else
               BOOST_CHECK(z * z.inv() == F_p<N>(1));
          for (int j = 0; j < N; j++) {
               const F_p<N> w(j);
               if (j == 0)
                    BOOST_CHECK_THROW(z / w, std::invalid_argument);
               else
                    BOOST_CHECK(w * (z / w) == z);
          }
     }
}

BOOST_AUTO_TEST_CASE(monomial_test) {
     using SHG::ALGGEOM::Monomial;
     using SHG::ALGGEOM::Negative_degree;

     Monomial m;
     BOOST_CHECK(m.dim() == 0);
     BOOST_CHECK(m.total_degree() == 0);
     Monomial m1{m};
     BOOST_CHECK(m == m1);

     for (int n = 0; n < 5; n++) {
          std::vector<int> d;
          for (int i = 0; i < n; i++)
               d.push_back(i + 1);
          const int td = std::accumulate(d.cbegin(), d.cend(), 0);
          for (int i = n; i < 10; i++) {
               Monomial m{d};
               BOOST_CHECK(m.dim() == n);
               for (int j = 0; j < m.dim(); j++) {
                    BOOST_CHECK(m[j] == d[j]);
                    BOOST_CHECK(m.degree(j) == d[j]);
               }
               BOOST_CHECK(m.total_degree() == td);
               Monomial m1{m};
               BOOST_CHECK(m == m1);
               BOOST_CHECK(!(m != m1));
               d.push_back(0);
          }
     }
     BOOST_CHECK_THROW(Monomial({-1}), Negative_degree);
     BOOST_CHECK_THROW(Monomial({0, -1}), Negative_degree);
     BOOST_CHECK_THROW(Monomial({-1}), Negative_degree);
     BOOST_CHECK_THROW(Monomial({-1, 0}), Negative_degree);
     m = {2, 1};
     m += {0, 2, 1};
     m1 = {2, 3, 1};
     BOOST_CHECK(m == m1);
     m = {0, 2, 1};
     m += {2, 1};
     BOOST_CHECK(m == m1);
     m = {};
     m += {0, 2, 1};
     m1 = {0, 2, 1};
     BOOST_CHECK(m == m1);
}

BOOST_AUTO_TEST_CASE(monomial_ordering_test) {
     using SHG::ALGGEOM::Monomial;

     SHG::ALGGEOM::Lex_less lex;
     SHG::ALGGEOM::Grlex_less grlex;
     SHG::ALGGEOM::Grevlex_less grevlex;

     // \cite cox-little-oshea-2007, page 56.
     BOOST_CHECK(lex({0, 3, 4}, {1, 2, 0}));
     BOOST_CHECK(lex({3, 2, 1}, {3, 2, 4}));
     BOOST_CHECK(lex({0, 0, 1}, {0, 1, 0}));
     BOOST_CHECK(lex({0, 1, 0}, {1, 0, 0}));

     // \cite cox-little-oshea-2007, page 58.
     BOOST_CHECK(grlex({3, 2, 0}, {1, 2, 3}));
     BOOST_CHECK(grlex({1, 1, 5}, {1, 2, 4}));
     BOOST_CHECK(grlex({0, 0, 1}, {0, 1, 0}));
     BOOST_CHECK(grlex({0, 1, 0}, {1, 0, 0}));

     // \cite cox-little-oshea-2007, page 58.
     BOOST_CHECK(grevlex({4, 2, 3}, {4, 7, 1}));
     BOOST_CHECK(grevlex({4, 1, 3}, {1, 5, 2}));
     BOOST_CHECK(grevlex({0, 0, 1}, {0, 1, 0}));
     BOOST_CHECK(grevlex({0, 1, 0}, {1, 0, 0}));

     // \cite cox-little-oshea-2007, pages 58-59.
     BOOST_CHECK(lex({4, 1, 2}, {5, 1, 1}));
     BOOST_CHECK(grlex({4, 1, 2}, {5, 1, 1}));
     BOOST_CHECK(grevlex({4, 1, 2}, {5, 1, 1}));

     // \cite cox-little-oshea-2007, page 59.
     const Monomial a{1, 2, 1};
     const Monomial b{0, 0, 2};
     const Monomial c{3, 0, 0};
     const Monomial d{2, 0, 2};
     BOOST_CHECK(lex(b, a));
     BOOST_CHECK(lex(a, d));
     BOOST_CHECK(lex(d, c));
     BOOST_CHECK(grlex(b, c));
     BOOST_CHECK(grlex(c, a));
     BOOST_CHECK(grlex(a, d));
     BOOST_CHECK(grevlex(b, c));
     BOOST_CHECK(grevlex(c, d));
     BOOST_CHECK(grevlex(d, a));

     // All monomials of the second degree.
     {
          const Monomial xx{2, 0, 0};
          const Monomial yy{0, 2, 0};
          const Monomial zz{0, 0, 2};
          const Monomial xy{1, 1, 0};
          const Monomial xz{1, 0, 1};
          const Monomial yz{0, 1, 1};
          BOOST_CHECK(lex(zz, yz));
          BOOST_CHECK(lex(yz, yy));
          BOOST_CHECK(lex(yy, xz));
          BOOST_CHECK(lex(xz, xy));
          BOOST_CHECK(lex(xy, xx));
          BOOST_CHECK(grlex(zz, yz));
          BOOST_CHECK(grlex(yz, yy));
          BOOST_CHECK(grlex(yy, xz));
          BOOST_CHECK(grlex(xz, xy));
          BOOST_CHECK(grlex(xy, xx));
          BOOST_CHECK(grevlex(zz, yz));
          BOOST_CHECK(grevlex(yz, xz));
          BOOST_CHECK(grevlex(xz, yy));
          BOOST_CHECK(grevlex(yy, xy));
          BOOST_CHECK(grevlex(xy, xx));
     }

     // All monomials of the second degree with trailing zeros
     // removed.
     {
          const Monomial xx{2};
          const Monomial yy{0, 2};
          const Monomial zz{0, 0, 2};
          const Monomial xy{1, 1};
          const Monomial xz{1, 0, 1};
          const Monomial yz{0, 1, 1};
          BOOST_CHECK(lex(zz, yz));
          BOOST_CHECK(lex(yz, yy));
          BOOST_CHECK(lex(yy, xz));
          BOOST_CHECK(lex(xz, xy));
          BOOST_CHECK(lex(xy, xx));
          BOOST_CHECK(grlex(zz, yz));
          BOOST_CHECK(grlex(yz, yy));
          BOOST_CHECK(grlex(yy, xz));
          BOOST_CHECK(grlex(xz, xy));
          BOOST_CHECK(grlex(xy, xx));
          BOOST_CHECK(grevlex(zz, yz));
          BOOST_CHECK(grevlex(yz, xz));
          BOOST_CHECK(grevlex(xz, yy));
          BOOST_CHECK(grevlex(yy, xy));
          BOOST_CHECK(grevlex(xy, xx));
     }
}

using SHG::ALGGEOM::Monomial;
using SHG::ALGGEOM::Poly;
using SHG::ALGGEOM::Rational;

/**
 * Returns \f$\binom{n}{k}\f$ as an element of the field \f$k\f$.
 */
template <typename K>
K binom(int n, int k) {
     using boost::math::binomial_coefficient;
     auto b = narrow_cast<int, double>(
          binomial_coefficient<double>(n, k));
     return K(b);
}

/**
 * Returns \f$(x^2y + xy^2)^n\f$ for \f$n \geq 0\f$.
 * \implementation \f$ (x^2y + xy^2)^n = \sum_{k = 0}^n \binom{n}{k}
 * x^{n + k} y^{2n - k} \f$.
 */
template <typename K, typename Compare>
Poly<K, Compare> test_poly(int n) {
     Poly<K, Compare> p(2);
     for (int k = 0; k <= n; k++) {
          const K b = binom<K>(n, k);
          if (b != K(0))
               p += {b, {n + k, 2 * n - k}};
     }
     return p;
}

/**
 * Returns \f$(ax^2yz + bxy^2z + cxyz^2)^n\f$ for \f$n \geq 0\f$.
 * \implementation \f[ (ax^2yz + bxy^2z + cxyz^2)^n = \sum_{k_1 = 0}^n
 * \binom{n}{k_1}a^{k_1} \sum_{k_2 = 0}^{n - k_1} \binom{n - k_1}{k_2}
 * b^{k_2}c^{n - k_1 - k_2} x^{n + k_1} y^{n + k_2} z^{2n - k_1 - k_2}
 * \f]
 */
template <typename K, typename Compare>
Poly<K, Compare> test_poly(int a, int b, int c, int n) {
     using SHG::ALGGEOM::pow;
     Poly<K, Compare> p(3);

     for (int k1 = 0; k1 <= n; k1++) {
          const K bin1 = binom<K>(n, k1);
          if (bin1 == K(0))
               continue;
          const K ak1 = pow(K(a), k1);
          if (ak1 == K(0))
               continue;
          for (int k2 = 0; k2 <= n - k1; k2++) {
               const K bin2 = binom<K>(n - k1, k2);
               if (bin2 == K(0))
                    continue;
               const K bk2 = pow(K(b), k2);
               if (bk2 == K(0))
                    continue;
               const K ck3 = pow(K(c), n - k1 - k2);
               if (ck3 == K(0))
                    continue;
               const K coeff = bin1 * ak1 * bin2 * bk2 * ck3;
               p += {coeff, {n + k1, n + k2, 2 * n - k1 - k2}};
          }
     }
     return p;
}

using poly_basic_test_types =
     boost::mpl::list<Poly<Rational<int>, std::less<Monomial>>,
                      Poly<F_p<5>, std::less<Monomial>>,
                      Poly<F_p<7>, std::less<Monomial>>,
                      Poly<F_p<11>, std::less<Monomial>>,
                      Poly<F_p<13>, std::less<Monomial>>,
                      Poly<Rational<int>, SHG::ALGGEOM::Grlex_less>,
                      Poly<F_p<5>, SHG::ALGGEOM::Grlex_less>,
                      Poly<F_p<7>, SHG::ALGGEOM::Grlex_less>,
                      Poly<F_p<11>, SHG::ALGGEOM::Grlex_less>,
                      Poly<F_p<13>, SHG::ALGGEOM::Grlex_less>,
                      Poly<Rational<int>, SHG::ALGGEOM::Grevlex_less>,
                      Poly<F_p<5>, SHG::ALGGEOM::Grevlex_less>,
                      Poly<F_p<7>, SHG::ALGGEOM::Grevlex_less>,
                      Poly<F_p<11>, SHG::ALGGEOM::Grevlex_less>,
                      Poly<F_p<13>, SHG::ALGGEOM::Grevlex_less>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(poly_basic_test, T,
                              poly_basic_test_types) {
     using F = typename T::Field;
     using Comp = typename T::Comp;
     using Term = SHG::ALGGEOM::Term<F>;
     using SHG::ALGGEOM::pow;
     {
          T p(3);
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.degree() == -1);
          BOOST_CHECK(p.terms().size() == 0);
          p = F(0);
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.degree() == -1);
          BOOST_CHECK(p.terms().size() == 0);
     }
     {
          T p(3, 0);
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.degree() == -1);
          BOOST_CHECK(p.terms().size() == 0);
     }
     for (int i = 1; i < 5; i++) {
          const F a{i};
          T p(3, a);
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.degree() == 0);
          const auto& t = p.terms();
          BOOST_CHECK(t.size() == 1);
          const auto it = t.find({});
          BOOST_CHECK(it != t.cend());
          BOOST_CHECK(it->second == a);
     }
     {
          T p(3);
          p = F(0);
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.degree() == -1);
          BOOST_CHECK(p.terms().size() == 0);
     }
     for (int i = 1; i < 5; i++) {
          const F a{i};
          T p(3);
          p = a;
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.degree() == 0);
          const auto& t = p.terms();
          BOOST_CHECK(t.size() == 1);
          const auto it = t.find({});
          BOOST_CHECK(it != t.cend());
          BOOST_CHECK(it->second == a);
     }
     {
          // x^2yz + 2xy^2z + 3xyz^2
          T p(3);
          p += {1, {2, 1, 1}};
          p += {2, {1, 2, 1}};
          p += {3, {1, 1, 2}};
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.degree() == 4);
          BOOST_CHECK(p.terms().size() == 3);
          bool result = p == test_poly<F, Comp>(1, 2, 3, 1);
          BOOST_CHECK(result);
          const SHG::ALGGEOM::Term<F> t2{3, {1, 2, 3, 4}};
          BOOST_CHECK_THROW(p += t2, std::invalid_argument);
          p -= {1, {2, 1, 1}};
          p -= {2, {1, 2, 1}};
          p -= {3, {1, 1, 2}};
          BOOST_CHECK(p.dim() == 3);
          BOOST_CHECK(p.degree() == -1);
          BOOST_CHECK(p.terms().size() == 0);
     }
     {
          T p(3), q(3), r(3);
          p += {1, {2, 1, 1}};
          T p0(3);
          p0 = p;
          q += {2, {1, 2, 1}};
          r += {1, {2, 1, 1}};
          r += {2, {1, 2, 1}};
          p += q;
          BOOST_CHECK(p == r);
          p -= q;
          BOOST_CHECK(p == p0);
     }
     {
          // (x^2y + yz) * (x + y) = x^3y + xyz + x^2y^2 + y^2z
          T p(3), q(3), r(3);
          p += Term(1, {2, 1});
          p += Term(1, {0, 1, 1});
          q += Term(1, {1});
          q += Term(1, {0, 1});
          r += Term(1, {3, 1});
          r += Term(1, {1, 1, 1});
          r += Term(1, {2, 2});
          r += Term(1, {0, 2, 1});
          p *= q;
          BOOST_CHECK(p == r);
     }
     {
          // x^2yz + 2xy^2z + 3xyz^2
          T p(3);
          p += {1, {2, 1, 1}};
          p += {2, {1, 2, 1}};
          p += {3, {1, 1, 2}};
          // For x = t, y = 2t, z = 3t the polynomial has the value
          // 84t^4.
          for (int t = 0; t < 10; t++)
               BOOST_CHECK(p({t, 2 * t, 3 * t}) == 84 * pow(F(t), 4));
     }
     {
          auto getq = []() {
               T q(3);
               q += {2, {1, 2, 1}};
               return q;
          };
          // x^2yz + 2xy^2z + 3xyz^2
          T p(3);
          p += {1, {2, 1, 1}};
          p += {2, {1, 2, 1}};
          p += {3, {1, 1, 2}};

          const T zero(3);
          const T one(3, 1);

          const auto p0 = p;
          const auto q = getq();

          BOOST_CHECK(zero == +zero);
          BOOST_CHECK(zero == -zero);
          BOOST_CHECK(one == +one);
          BOOST_CHECK(one == -(-one));

          p = +p;
          BOOST_CHECK(p == p0);

          p = p0;
          p -= q;
          p = -p;
          p -= q;
          p = -p;
          BOOST_CHECK(p == p0);

          p = p0;
          p = p - q;
          p = -p;
          p = p - q;
          p = -p;
          BOOST_CHECK(p == p0);

          p = p0;
          p += q;
          p = -p;
          p += q;
          p = -p;
          BOOST_CHECK(p == p0);

          p = p0;
          p = p + q;
          p = -p;
          p = p + q;
          p = -p;
          BOOST_CHECK(p == p0);
     }
}

BOOST_AUTO_TEST_CASE(poly_output_test) {
     // Expected result for F_p<5>, F_p<7>, F_p<11> and F_p<13>.
     const char* const er =
          "3 x1^1 x2^1 x3^2 + 2 x1^1 x2^2 x3^1 + 1 x1^2 x2^1 x3^1";
     std::stringstream ss;
     {
          using T = Rational<int>;
          using Comp = Poly<T>::Comp;
          Poly<T> p = test_poly<T, Comp>(1, 2, 3, 1);
          ss.str("");
          ss << p;
          BOOST_CHECK(
               ss.str() ==
               "3/1 x1^1 x2^1 x3^2 + 2/1 x1^1 x2^2 x3^1 + 1/1 x1^2 "
               "x2^1 x3^1");
     }
     {
          using T = F_p<3>;
          using Comp = Poly<T>::Comp;
          Poly<T> p = test_poly<T, Comp>(1, 2, 3, 1);
          ss.str("");
          ss << p;
          BOOST_CHECK(ss.str() ==
                      "2 x1^1 x2^2 x3^1 + 1 x1^2 x2^1 x3^1");
     }
     {
          using T = F_p<5>;
          using Comp = Poly<T>::Comp;
          Poly<T> p = test_poly<T, Comp>(1, 2, 3, 1);
          ss.str("");
          ss << p;
          BOOST_CHECK(ss.str() == er);
     }
     {
          using T = F_p<7>;
          using Comp = Poly<T>::Comp;
          Poly<T> p = test_poly<T, Comp>(1, 2, 3, 1);
          ss.str("");
          ss << p;
          BOOST_CHECK(ss.str() == er);
     }
     {
          using T = F_p<11>;
          using Comp = Poly<T>::Comp;
          Poly<T> p = test_poly<T, Comp>(1, 2, 3, 1);
          ss.str("");
          ss << p;
          BOOST_CHECK(ss.str() == er);
     }
     {
          using T = F_p<13>;
          using Comp = Poly<T>::Comp;
          Poly<T> p = test_poly<T, Comp>(1, 2, 3, 1);
          ss.str("");
          ss << p;
          BOOST_CHECK(ss.str() == er);
     }
}

/**
 * Non-zero polynomial equal to zero for all values.
 * \cite cox-little-oshea-2007, exercise 2, page 5.
 */
BOOST_AUTO_TEST_CASE(exercise2_test) {
     using SHG::ALGGEOM::Point;
     using SHG::ALGGEOM::Poly;
     using T = F_p<2>;

     Poly<T> p(2);
     p += {1, {2, 1}};
     p += {1, {1, 2}};
     Point<T> x(2);

     for (int i = 0; i < 2; i++) {
          x[0] = i;
          for (int j = 0; j < 2; j++) {
               x[1] = j;
               BOOST_CHECK(p(x) == T(0));
          }
     }
}

constexpr const char* const result = R"(
 000
 001 010 100
 002 011 020 101 110 200
 003 012 021 030 102 111 120 201 210 300
 004 013 022 031 040 103 112 121 130 202 211 220 301 310 400
)";

BOOST_AUTO_TEST_CASE(recursive_monomial_generator_test) {
     std::ostringstream buf;
     auto fun = [&buf](const std::vector<int>& a) {
          buf << ' ';
          for (std::vector<int>::size_type i = 0; i < a.size(); i++)
               buf << a[i];
     };
     const int n = 3;

     buf << '\n';
     for (int k = 0; k < 5; k++) {
          SHG::ALGGEOM::Recursive_monomial_generator g(n, k, fun);
          g.generate();
          buf << '\n';
     }
     BOOST_CHECK(buf.str() == result);
}

BOOST_AUTO_TEST_CASE(iterative_monomial_generator_test) {
     using SHG::ALGGEOM::Iterative_monomial_generator;
     BOOST_CHECK_THROW(Iterative_monomial_generator g(0, 2),
                       Subdomain_error<int>);
     BOOST_CHECK_THROW(Iterative_monomial_generator g(3, -1),
                       Subdomain_error<int>);

     std::ostringstream buf;
     const int n = 3;

     buf << '\n';
     for (int k = 0; k < 5; k++) {
          Iterative_monomial_generator g(n, k);
          do {
               buf << ' ';
               const std::vector<int>& a = g.get();
               for (std::vector<int>::size_type i = 0; i < a.size();
                    i++)
                    buf << a[i];
          } while (g.next());
          buf << '\n';
     }
     BOOST_CHECK(buf.str() == result);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
