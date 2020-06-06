#include "shg/smc.h"
#include <boost/test/unit_test.hpp>
#include "shg/mzt.h"

namespace SHG::BTesting {

BOOST_AUTO_TEST_SUITE(smc_test)

BOOST_AUTO_TEST_CASE(std_test) {
     MZT g;
     std::unique_ptr<SMC::STD> a;
     int s = 0;

     Vecint x(3);
     Vecdouble p(3);
     x[0] = 1;
     x[1] = 2;
     x[2] = 3;
     p[0] = 0.5;
     p[1] = 1.0 / 3.0;
     p[2] = 1.0 / 6.0;

     a.reset(new SMC::Finite(x, p));
     for (int i = 0; i < 1000; i++) {
          const int x = a->generate(g);
          BOOST_CHECK(x > 0);
          s += x;
     }

     a.reset(new SMC::Geometric(0.5));
     for (int i = 0; i < 1000; i++) {
          const int x = a->generate(g);
          BOOST_CHECK(x > 0);
          s += x;
     }

     a.reset(new SMC::Negative_binomial(0.5, 0.5));
     for (int i = 0; i < 1000; i++) {
          const int x = a->generate(g);
          BOOST_CHECK(x > 0);
          s += x;
     }

     a.reset(new SMC::Logarithmic(0.5));
     for (int i = 0; i < 1000; i++) {
          const int x = a->generate(g);
          BOOST_CHECK(x > 0);
          s += x;
     }

     a.reset(new SMC::Poisson(0.5));
     for (int i = 0; i < 1000; i++) {
          const int x = a->generate(g);
          BOOST_CHECK(x > 0);
          s += x;
     }

     BOOST_CHECK(s == 8134);
}

BOOST_AUTO_TEST_CASE(smc_test) {
     struct Result {
          int x, s, j;
     };
     const Vector<Result> ex1xsj{
          {1, 0, 2}, {4, 1, 3}, {3, 5, 4}, {1, 8, 0}};
     const Vecint ex1z{2, 3, 3, 3, 3, 4, 4, 4, 0};
     const Vector<Result> ex2xsj{
          {1, 0, 2}, {4, 1, 3}, {3, 5, 4}, {2, 8, 0}};
     const Vecint ex2z{2, 3, 3, 3, 3, 4, 4, 4, 0, 0};

     const int nstates = 5;
     SMC smc(nstates);
     for (int i = 0; i < nstates; i++) {
          smc.alpha_[i] = 1.0 / nstates;
          for (int j = 0; j < nstates; j++)
               smc.P_(i, j) = 1.0 / (nstates - 1);
          smc.P_(i, i) = 0.0;
     }
     Vecint x(4);
     Vecdouble p(4);
     x[0] = 1;
     p[0] = 0.4;
     x[1] = 2;
     p[1] = 0.3;
     x[2] = 3;
     p[2] = 0.2;
     x[3] = 4;
     p[3] = 0.1;
     smc.std_ = new SMC::Finite(x, p);

     smc.g_ = new MZT;
     BOOST_CHECK(smc.generate(9, true) == 0);
     BOOST_CHECK(smc.X.size() == 4 && smc.S.size() == 4 &&
                 smc.J.size() == 4);
     BOOST_CHECK(smc.Z.size() == 9);
     for (Vector<Result>::size_type i = 0; i < ex1xsj.size(); i++) {
          BOOST_CHECK(smc.X[i] == ex1xsj[i].x);
          BOOST_CHECK(smc.S[i] == ex1xsj[i].s);
          BOOST_CHECK(smc.J[i] == ex1xsj[i].j);
     }
     for (Vecint::size_type i = 0; i < ex1z.size(); i++)
          BOOST_CHECK(smc.Z[i] == ex1z[i]);

     delete smc.g_;
     smc.g_ = 0;

     smc.g_ = new MZT;
     BOOST_CHECK(smc.generate(9, false) == 0);
     BOOST_CHECK(smc.X.size() == 4 && smc.S.size() == 4 &&
                 smc.J.size() == 4);
     BOOST_CHECK(smc.Z.size() == 10);
     for (Vector<Result>::size_type i = 0; i < ex2xsj.size(); i++) {
          BOOST_CHECK(smc.X[i] == ex2xsj[i].x);
          BOOST_CHECK(smc.S[i] == ex2xsj[i].s);
          BOOST_CHECK(smc.J[i] == ex2xsj[i].j);
     }
     for (Vecint::size_type i = 0; i < ex2z.size(); i++)
          BOOST_CHECK(smc.Z[i] == ex2z[i]);
}

BOOST_AUTO_TEST_CASE(unideggaumix_test) {
     MZT g;
     Unideggaumix u(5000, 5);

     u.pi(0) = 0.1;
     u.mu(0) = -0.08;
     u.sigma(0) = 0.02;
     u.pi(1) = 0.2;
     u.mu(1) = -0.01;
     u.sigma(1) = 0.005;
     u.pi(2) = 0.3;
     u.mu(2) = 0.01;
     u.sigma(2) = 0.005;
     u.pi(3) = 0.2;
     u.mu(3) = 0.08;
     u.sigma(3) = 0.02;
     u.pi(4) = 0.2;
     u.x0 = 0;
     u.fx0 = 1e10;

     int k = 0;
     for (int i = 0; i < 4; i++) {
          const int n = iceil(u.n * u.pi(i));
          const double mu = u.mu(i);
          const double sigma = u.sigma(i);
          for (int j = 0; j < n; j++)
               u.x(k++) = mu + sigma * g.normal();
     }

     while (k < u.n)
          u.x(k++) = u.x0;
     u.eps = 1e-5;
     u.maxit = 1000;
     u.estimate();

     BOOST_CHECK(u.get_status() == 0);
     BOOST_CHECK(u.iter == 13);
     double s = 0.0;

     const Vecdouble p{1.000090989306e-01, 2.005154391418e-01,
                       2.991848410989e-01, 2.002906213288e-01,
                       1.999999994999e-01};

     const Vecdouble mu{-8.047728616669e-02, -1.016764381500e-02,
                        1.016802041740e-02, 8.021842431891e-02};

     const Vecdouble sigma{1.918811543256e-02, 5.006600877334e-03,
                           4.972330263071e-03, 2.086705495178e-02};

     const Vecdouble::size_type K = u.K;
     const Vecdouble::size_type K1 = u.K1;

     BOOST_CHECK(K == 5 && K1 == 4);
     BOOST_CHECK(K == p.size());
     BOOST_CHECK(K1 == mu.size());
     BOOST_CHECK(K1 == sigma.size());

     for (Vecdouble::size_type k = 0; k < K; k++) {
          BOOST_CHECK(std::abs(u.pi(k) - p[k]) < 1e-12);
          s += u.pi(k);
          if (k < K1) {
               BOOST_CHECK(std::abs(u.mu(k) - mu[k]) < 1e-12);
               BOOST_CHECK(std::abs(u.sigma(k) - sigma[k]) < 1e-12);
          }
     }
     BOOST_CHECK(std::abs(s - 1.0) < 1e-12);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting
