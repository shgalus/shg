#include <shg/neuralnet.h>
#include <cmath>
#include <sstream>
#include <shg/utils.h>
#include <shg/mzt.h>
#include "testing.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(neuralnet_test)

using namespace SHG::Neural_networks;

BOOST_AUTO_TEST_CASE(error_exception_test) {
     try {
          throw Error();
     } catch (Error const& e) {
          BOOST_CHECK(strcmp(e.what(), "neural network error") == 0);
     }
}

BOOST_AUTO_TEST_CASE(identity_test) {
     Vecreal const x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal const y0(make_vector({-1.0, 0.0, 1.0}));
     Vecreal const y = identity(x);
     BOOST_CHECK(facmp(y, y0, 1e-15));
}

BOOST_AUTO_TEST_CASE(sign_test) {
     Vecreal const x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal const y0(make_vector({-1.0, 0.0, 1.0}));
     Vecreal const y = sign(x);
     BOOST_CHECK(facmp(y, y0, 1e-15));
}

BOOST_AUTO_TEST_CASE(sigmoid_test) {
     Vecreal const x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal y0(x.size());
     for (Vecreal::size_type i = 0; i < x.size(); i++)
          y0[i] = 1.0 / (1.0 + std::exp(-x[i]));
     Vecreal const y = sigmoid(x);
     BOOST_CHECK(facmp(y, y0, 1e-15));
}

BOOST_AUTO_TEST_CASE(tgh_test) {
     Vecreal const x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal y0(x.size());
     for (Vecreal::size_type i = 0; i < x.size(); i++)
          y0[i] = std::tanh(x[i]);
     Vecreal const y = tgh(x);
     BOOST_CHECK(facmp(y, y0, 1e-15));
}

BOOST_AUTO_TEST_CASE(relu_test) {
     Vecreal const x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal const y0(make_vector({0.0, 0.0, 1.0}));
     Vecreal const y = relu(x);
     BOOST_CHECK(facmp(y, y0, 1e-15));
}

BOOST_AUTO_TEST_CASE(hardtanh_test) {
     Vecreal const x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal const y0(make_vector({-1.0, 0.0, 1.0}));
     Vecreal const y = hardtanh(x);
     BOOST_CHECK(facmp(y, y0, 1e-15));
}

Vecreal simple_softmax(Vecreal const& x) {
     Vecreal y(x.size());
     Real s = 0.0;
     for (Vecreal::size_type i = 0; i < x.size(); i++) {
          y[i] = std::exp(x[i]);
          s += y[i];
     }
     for (auto& yi : y)
          yi /= s;
     return y;
}

BOOST_AUTO_TEST_CASE(softmax_test) {
     Vecreal const x(make_vector({-2.0, -1.0, 0.0, 1.0, 2.0}));
     Vecreal const y0 = simple_softmax(x);
     Vecreal const y = softmax(x);
     BOOST_CHECK(facmp(y, y0, 1e-15));
}

BOOST_AUTO_TEST_CASE(didentity_test) {
     Vecreal const x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal const f = identity(x);
     Matreal const df0(make_matrix(
          3, 3, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));
     Matreal const df = didentity(x, f);
     BOOST_CHECK(facmp(df, df0, 1e-15));
}

BOOST_AUTO_TEST_CASE(dsign_test) {
     Vecreal x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal f = sign(x);
     BOOST_CHECK_THROW(dsign(x, f), Error);
     x = make_vector({-1.0, 0.01, 1.0});
     f = sign(x);
     Matreal const df0(make_matrix(
          3, 3, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}));
     Matreal const df = dsign(x, f);
     BOOST_CHECK(facmp(df, df0, 1e-15));
}

BOOST_AUTO_TEST_CASE(dsigmoid_test) {
     Vecreal const x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal const f = sigmoid(x);
     Matreal df0(3, 3);
     for (Matreal::size_type i = 0; i < df0.size1(); i++)
          for (Matreal::size_type j = 0; j < df0.size2(); j++)
               if (i == j) {
                    Real const e = std::exp(-x[i]);
                    df0(i, j) = e / sqr(1.0 + e);
               } else {
                    df0(i, j) = 0.0;
               }
     Matreal const df = dsigmoid(x, f);
     BOOST_CHECK(facmp(df, df0, 1e-15));
}

BOOST_AUTO_TEST_CASE(dtgh_test) {
     Vecreal const x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal const f = tgh(x);
     Matreal df0(3, 3);
     for (Matreal::size_type i = 0; i < df0.size1(); i++)
          for (Matreal::size_type j = 0; j < df0.size2(); j++)
               df0(i, j) = i == j ? 1.0 - sqr(std::tanh(x(i))) : 0.0;
     Matreal const df = dtgh(x, f);
     BOOST_CHECK(facmp(df, df0, 1e-15));
}

BOOST_AUTO_TEST_CASE(drelu_test) {
     Vecreal x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal f = relu(x);
     BOOST_CHECK_THROW(drelu(x, f), Error);
     x = make_vector({-1.0, 0.01, 1.0});
     f = relu(x);
     Matreal const df0(make_matrix(
          3, 3, {0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));
     Matreal const df = drelu(x, f);
     BOOST_CHECK(facmp(df, df0, 1e-15));
}

BOOST_AUTO_TEST_CASE(dhardtanh_test) {
     Vecreal x(make_vector({-1.0, 0.0, 1.0}));
     Vecreal f = hardtanh(x);
     BOOST_CHECK_THROW(dhardtanh(x, f), Error);
     x = make_vector({-1.01, 0.0, 0.99});
     f = hardtanh(x);
     Matreal const df0(make_matrix(
          3, 3, {0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));
     Matreal const df = dhardtanh(x, f);
     BOOST_CHECK(facmp(df, df0, 1e-15));
}

BOOST_AUTO_TEST_CASE(dsoftmax_test) {
     Vecreal const x(make_vector({-2.0, -1.0, 0.0, 1.0, 2.0}));
     Vecreal const f = softmax(x);
     Matreal df0(5, 5);
     for (Matreal::size_type i = 0; i < df0.size1(); i++)
          for (Matreal::size_type j = 0; j < df0.size2(); j++)
               if (i == j)
                    df0(i, j) = f(i) * (1.0 - f(j));
               else
                    df0(i, j) = -f(i) * f(j);
     Matreal const df = dsoftmax(x, f);
     BOOST_CHECK(facmp(df, df0, 1e-15));
}

BOOST_AUTO_TEST_CASE(quadratic_test) {
     Vecreal const aL(make_vector({0.9, 2.0, 3.1}));
     Vecreal const y(make_vector({1.0, 2.0, 3.0}));
     Real const c = quadratic(aL, y);
     BOOST_CHECK(facmp(c, 0.01, 1e-15) == 0);
}

BOOST_AUTO_TEST_CASE(dquadratic_test) {
     Vecreal const aL(make_vector({0.9, 2.0, 3.1}));
     Vecreal const y(make_vector({1.0, 2.0, 3.0}));
     Vecreal const dc0 = make_vector({-0.1, 0.0, 0.1});
     Vecreal const dc = dquadratic(aL, y);
     BOOST_CHECK(facmp(dc, dc0, 1e-15));
}

BOOST_AUTO_TEST_CASE(cross_entropy_test) {
     Vecreal const aL(make_vector({0.2, 0.8, 0.3}));
     Vecreal const y(make_vector({0.0, 1.0, 0.0}));
     Real const c = cross_entropy(aL, y);
     BOOST_CHECK(facmp(c, -std::log(0.8), 1e-15) == 0);
}

BOOST_AUTO_TEST_CASE(dcross_entropy_test) {
     Vecreal const aL(make_vector({0.2, 0.8, 0.3}));
     Vecreal const y(make_vector({0.0, 1.0, 0.0}));
     Vecreal const dc0 = make_vector({0.0, -1.25, 0.0});
     Vecreal const dc = dcross_entropy(aL, y);
     BOOST_CHECK(facmp(dc, dc0, 1e-15));
}

BOOST_AUTO_TEST_CASE(is_standard_basis_vector_test) {
     Vecreal const v1(make_vector({1.0, 0.0, 0.0}));
     Vecreal const v2(make_vector({0.0, 1.0, 0.0}));
     Vecreal const v3(make_vector({0.0, 0.0, 1.0}));
     Vecreal const v4(make_vector({0.0, 0.0, 0.0}));
     Vecreal const v5(make_vector({1.0, 1.0, 0.0}));
     Vecreal const v6(make_vector({2.0, 0.0, 0.0}));
     BOOST_CHECK(is_standard_basis_vector(v1, 1e-15));
     BOOST_CHECK(is_standard_basis_vector(v2, 1e-15));
     BOOST_CHECK(is_standard_basis_vector(v3, 1e-15));
     BOOST_CHECK(!is_standard_basis_vector(v4, 1e-15));
     BOOST_CHECK(!is_standard_basis_vector(v5, 1e-15));
     BOOST_CHECK(!is_standard_basis_vector(v6, 1e-15));
}

BOOST_AUTO_TEST_CASE(mnn_basic_test) {
     MNN mnn;
     BOOST_CHECK(mnn.L() == 2);
     BOOST_CHECK(mnn.n().size() == 2);
     BOOST_CHECK(mnn.n()(0) == 1);
     BOOST_CHECK(mnn.n()(1) == 1);
     BOOST_CHECK(facmp(mnn.eta(), 0.1, 1e-15) == 0);
     BOOST_CHECK_THROW(mnn.phi(0), std::invalid_argument);
     BOOST_CHECK(mnn.phi(1) == Activation_function::sigmoid);
     BOOST_CHECK(mnn.C() == Cost_function::quadratic);
     Vecreal x(1);
     Vecreal z(1);
     Vecreal a;
     Vecreal aL;
     for (int i = 0; i <= 20; i++) {
          x(0) = -1.0 + 0.1 * i;
          z(0) = mnn.W()(1)(0, 0) * x(0) + mnn.b()(1)(0);
          a = sigmoid(z);
          aL = mnn.aL(x);
          BOOST_CHECK(facmp(aL, a, 1e-15));
     }
}

BOOST_AUTO_TEST_CASE(mnn_io_test) {
     Vecuint const n(make_vector({2, 2}));
     MNN mnn(n);
     mnn.phi(Activation_function::tgh, 1);
     mnn.eta(0.05);
     std::ostringstream oss(binout);
     mnn.write(oss);
     BOOST_REQUIRE(oss.good());
     std::istringstream iss(bininp);
     iss.str(oss.str());
     MNN mnn1;
     mnn1.read(iss);
     BOOST_REQUIRE(iss.good());
     BOOST_CHECK(facmp(mnn1, mnn, 1e-15));
}

BOOST_AUTO_TEST_CASE(mnn_train_test) {
     Vecuint const n(make_vector({2, 2}));
     MNN mnn(n);
     mnn.phi(Activation_function::identity, 1);
     // (x_1, x_2) --> (x_1 + x_2, x_1 - x_2)
     Vecreal const x(make_vector({1.0, 2.0}));
     Vecreal const y(make_vector({3.0, -1.0}));
     for (int i = 1; i <= 20; i++)
          mnn.train(x, y);
     BOOST_CHECK(facmp(quadratic(mnn.aL(x), y), 0.0, 1e-15) == 0);
}

struct Test_case {
     Vecreal x{};
     Vecreal y{};
};

/**
 * Prepares test set. x contains two coordinates of a random point in
 * [-1, 1] * [-1, 1] and y contains a vector pointing the quadrant of
 * coordinate system in which the point lies.
 */
SHG::Neural_networks::Vector<Test_case> test_set() {
     using boost::numeric::ublas::zero_vector;
     SHG::Neural_networks::Vector<Test_case> t(10000);
     MZT mzt;
     for (Uint i = 0; i < t.size(); i++) {
          auto& c = t(i);
          c.x.resize(2);
          c.x(0) = 2.0 * mzt() - 1.0;
          c.x(1) = 2.0 * mzt() - 1.0;
          c.y = zero_vector<Real>(4);
          if (c.x(0) < 0.0) {
               if (c.x(1) < 0)
                    c.y(2) = 1.0;
               else
                    c.y(1) = 1.0;
          } else {
               if (c.x(1) < 0)
                    c.y(3) = 1.0;
               else
                    c.y(0) = 1.0;
          }
     }
     return t;
}

BOOST_AUTO_TEST_CASE(classification_1_test) {
     auto const t{test_set()};
     Uint const N = 8 * t.size() / 10;
     Vecuint const n(make_vector({2, 4}));
     MNN mnn(n);
     mnn.phi(Activation_function::softmax, 1);
     mnn.C(Cost_function::cross_entropy);
     Uint nhits;
     MZT mzt;
     SHG::Vecint rs;

     for (int e = 0; e < 10; e++) {
          mzt.random_sample(N, N, rs);
          for (Uint i = 0; i < N; i++)
               mnn.train(t(rs(i)).x, t(rs(i)).y);
          nhits = 0;
          for (Uint i = N; i < t.size(); i++)
               if (mnn.is_hit(t(i).x, t(i).y, 1e-15))
                    nhits++;
     }
     BOOST_CHECK(nhits == 1978);
}

BOOST_AUTO_TEST_CASE(classification_2_test) {
     auto const t{test_set()};
     Uint const N = 8 * t.size() / 10;
     Vecuint const n(make_vector({2, 4, 4}));
     MNN mnn(n);
     mnn.phi(Activation_function::softmax, 2);
     mnn.C(Cost_function::cross_entropy);
     Uint nhits;
     MZT mzt;
     SHG::Vecint rs;

     for (int e = 0; e < 30; e++) {
          mzt.random_sample(N, N, rs);
          for (Uint i = 0; i < N; i++)
               mnn.train(t(rs(i)).x, t(rs(i)).y);
          nhits = 0;
          for (Uint i = N; i < t.size(); i++)
               if (mnn.is_hit(t(i).x, t(i).y, 1e-15))
                    nhits++;
     }
     BOOST_CHECK(nhits == 1982);
}

BOOST_AUTO_TEST_CASE(mnistdhd_data_test) {
     Mnistdhd v = mnistdhd(testdatadir, "t10k");
     BOOST_REQUIRE(v.size() == 10000);
     BOOST_CHECK(facmp(v(0).image(203), 185.0, 1e-15) == 0);
     BOOST_CHECK(facmp(v(0).label(7), 1.0, 1e-15) == 0);
     BOOST_CHECK(facmp(v(9999).image(597), 132.0, 1e-15) == 0);
     BOOST_CHECK(facmp(v(9999).label(6), 1.0, 1e-15) == 0);
     for (auto it = v.begin(); it != v.end(); ++it)
          BOOST_CHECK((is_standard_basis_vector((*it).label, 1e-15)));

     v = mnistdhd(testdatadir, "train");
     BOOST_REQUIRE(v.size() == 60000);
     BOOST_CHECK(facmp(v(0).image(676), 136.0, 1e-15) == 0);
     BOOST_CHECK(facmp(v(0).label(5), 1.0, 1e-15) == 0);
     BOOST_CHECK(facmp(v(59999).image(458), 130.0, 1e-15) == 0);
     BOOST_CHECK(facmp(v(59999).label(8), 1.0, 1e-15) == 0);
     for (auto it = v.begin(); it != v.end(); ++it)
          BOOST_CHECK((is_standard_basis_vector((*it).label, 1e-15)));
}

/**
 * Parity test. Input: 2-bit number, output: even or odd.
 */
BOOST_AUTO_TEST_CASE(parity_test) {
     using boost::numeric::ublas::zero_vector;
     SHG::Neural_networks::Vector<Test_case> t(4);
     t(0).x = make_vector({0.0, 0.0});
     t(0).y = make_vector({1.0, 0.0});  // 00 is even
     t(1).x = make_vector({0.0, 1.0});
     t(1).y = make_vector({0.0, 1.0});  // 01 is odd
     t(2).x = make_vector({1.0, 0.0});
     t(2).y = make_vector({1.0, 0.0});  // 10 is even
     t(3).x = make_vector({1.0, 1.0});
     t(3).y = make_vector({0.0, 1.0});  // 11 is odd

     Vecuint const n(make_vector({2, 2}));
     MNN mnn(n);
     mnn.phi(Activation_function::softmax, 1);
     mnn.C(Cost_function::cross_entropy);
     MZT mzt;
     SHG::Vecint rs;
     Uint const N = t.size();

     for (int e = 0; e < 100; e++) {
          mzt.random_sample(N, N, rs);
          for (Uint i = 0; i < N; i++)
               mnn.train(t(rs(i)).x, t(rs(i)).y);
     }
     Uint nhits = 0;
     for (Uint i = 0; i < N; i++)
          if (mnn.is_hit(t(i).x, t(i).y, 1e-15))
               nhits++;
     BOOST_CHECK(nhits == 4);
}

/**
 * This function was used to create the file mnistdhd.txt.
 */
void train_on_mnistdhd() {
     Mnistdhd const train_set = mnistdhd(testdatadir, "train");
     Mnistdhd const test_set = mnistdhd(testdatadir, "t10k");
     Vecuint const n(make_vector({784, 64, 16, 10}));
     MNN mnn(n);
     mnn.phi(Activation_function::softmax, 3);
     mnn.C(Cost_function::cross_entropy);
     mnn.eta(0.00005);

     Uint nhits;
     MZT mzt;
     SHG::Vecint rs;

     for (int e = 0; e < 100; e++) {
          mzt.random_sample(train_set.size(), train_set.size(), rs);
          for (Uint i = 0; i < train_set.size(); i++)
               mnn.train(train_set(rs(i)).image,
                         train_set(rs(i)).label);
          mnn.write("mnistdhd.txt");
          nhits = 0;
          for (Uint i = 0; i < train_set.size(); i++)
               if (mnn.is_hit(train_set(i).image, train_set(i).label,
                              1e-15))
                    nhits++;
          std::cout << "e = " << e << " training set nhits = "
                    << 100.0 * nhits / train_set.size() << '\n';
          nhits = 0;
          for (Uint i = 0; i < test_set.size(); i++)
               if (mnn.is_hit(test_set(i).image, test_set(i).label,
                              1e-15))
                    nhits++;
          std::cout << "e = " << e << " test set nhits = "
                    << 100.0 * nhits / test_set.size() << '\n';
     }
}

BOOST_AUTO_TEST_CASE(mnistdhd_test) {
     Mnistdhd const test_set = mnistdhd(testdatadir, "t10k");
     MNN mnn;
     std::string fname{testdatadir};
     fname += "mnistdhd.txt";
     mnn.read(fname.c_str());
     Uint nhits{0};
     for (Uint i = 0; i < test_set.size(); i++)
          if (mnn.is_hit(test_set(i).image, test_set(i).label, 1e-15))
               nhits++;
     BOOST_CHECK(nhits == 8337);
}

/**
 * This function was used to create the file mnistdhd2.txt.
 */
void train_on_mnistdhd2() {
     Mnistdhd train_set = mnistdhd(testdatadir, "train");
     Mnistdhd test_set = mnistdhd(testdatadir, "t10k");

     // Normalize byte data to [0, 1) to avoid overflow in cross
     // entropy cost function.
     for (Uint i = 0; i < train_set.size(); i++) {
          for (Uint j = 0; j < 784; j++) {
               Real z = train_set(i).image(j);
               BOOST_REQUIRE(z >= 0.0 && z < 256.0);
               train_set(i).image(j) /= 256.0;
          }
     }
     for (Uint i = 0; i < test_set.size(); i++) {
          for (Uint j = 0; j < 784; j++) {
               Real z = test_set(i).image(j);
               BOOST_REQUIRE(z >= 0.0 && z < 256.0);
               test_set(i).image(j) /= 256.0;
          }
     }

     Vecuint const n(make_vector({784, 10}));
     MNN mnn(n);
     mnn.phi(Activation_function::softmax, 1);
     mnn.C(Cost_function::cross_entropy);
     mnn.eta(0.005);

     Uint nhits;
     MZT mzt;
     SHG::Vecint rs;

     for (int t = 0; t <= 1; t++) {
          for (int e = 0; e < 100; e++) {
               mzt.random_sample(train_set.size(), train_set.size(),
                                 rs);
               for (Uint i = 0; i < train_set.size(); i++)
                    mnn.train(train_set(rs(i)).image,
                              train_set(rs(i)).label);
               mnn.write("mnistdhd2.txt");
               nhits = 0;
               for (Uint i = 0; i < train_set.size(); i++)
                    if (mnn.is_hit(train_set(i).image,
                                   train_set(i).label, 1e-15))
                         nhits++;
               std::cout << "e = " << e << " training set nhits = "
                         << 100.0 * nhits / train_set.size() << '\n';
               nhits = 0;
               for (Uint i = 0; i < test_set.size(); i++)
                    if (mnn.is_hit(test_set(i).image,
                                   test_set(i).label, 1e-15))
                         nhits++;
               std::cout << "e = " << e << " test set nhits = "
                         << 100.0 * nhits / test_set.size() << '\n';
          }
          mnn.eta(mnn.eta() * 0.3);
     }
}

BOOST_AUTO_TEST_CASE(mnistdhd2_test) {
     Mnistdhd const test_set = mnistdhd(testdatadir, "t10k");
     MNN mnn;
     std::string fname{testdatadir};
     fname += "mnistdhd2.txt";
     mnn.read(fname.c_str());
     Uint nhits{0};
     for (Uint i = 0; i < test_set.size(); i++)
          if (mnn.is_hit(test_set(i).image, test_set(i).label, 1e-15))
               nhits++;
     BOOST_CHECK(nhits == 8625);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
