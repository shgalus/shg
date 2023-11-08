#include <shg/fcmp.h>
#include "tests.h"

namespace TESTS {

BOOST_AUTO_TEST_SUITE(fcmp_test)

using test_types = boost::mpl::list<float, double, long double>;

BOOST_AUTO_TEST_CASE_TEMPLATE(ieee754_test, T, test_types) {
     using std::isnan;
     using nl = std::numeric_limits<T>;
     T const zero{static_cast<T>(0)};

     BOOST_CHECK(nl::min() < nl::max());
     BOOST_CHECK(zero < nl::max());
     BOOST_CHECK(zero < nl::min());

     BOOST_CHECK(!(nl::quiet_NaN() < nl::infinity()));
     BOOST_CHECK(!(nl::quiet_NaN() == nl::infinity()));
     BOOST_CHECK(!(nl::quiet_NaN() > nl::infinity()));

     BOOST_CHECK(!(nl::quiet_NaN() < zero));
     BOOST_CHECK(!(nl::quiet_NaN() == zero));
     BOOST_CHECK(!(nl::quiet_NaN() > zero));

     BOOST_CHECK(!(nl::signaling_NaN() < nl::infinity()));
     BOOST_CHECK(!(nl::signaling_NaN() == nl::infinity()));
     BOOST_CHECK(!(nl::signaling_NaN() > nl::infinity()));

     BOOST_CHECK(!(nl::signaling_NaN() < zero));
     BOOST_CHECK(!(nl::signaling_NaN() == zero));
     BOOST_CHECK(!(nl::signaling_NaN() > zero));

     BOOST_CHECK(nl::infinity() == nl::infinity());
     BOOST_CHECK(!(nl::infinity() < nl::infinity()));
     BOOST_CHECK(!(nl::infinity() > nl::infinity()));

     BOOST_CHECK(nl::infinity() + nl::infinity() == nl::infinity());
     BOOST_CHECK(-nl::infinity() - nl::infinity() == -nl::infinity());

     BOOST_CHECK(nl::infinity() * nl::infinity() == nl::infinity());
     BOOST_CHECK(-nl::infinity() * nl::infinity() == -nl::infinity());

     BOOST_CHECK(isnan(nl::infinity() - nl::infinity()));
     BOOST_CHECK(isnan(nl::infinity() / nl::infinity()));
     BOOST_CHECK(isnan(-nl::infinity() + nl::infinity()));
}

template <typename T, typename U, typename V, typename E>
int facmp(U u, V v, E eps) {
     return SHG::facmp<T>(static_cast<T>(u), static_cast<T>(v),
                          static_cast<T>(eps));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(facmp_test, T, test_types) {
     using std::invalid_argument;
     using nl = std::numeric_limits<T>;
     auto const max = nl::max();
     auto const min = nl::min();
     auto const inf = nl::infinity();
     auto const qnan = nl::quiet_NaN();
     auto const snan = nl::signaling_NaN();

     BOOST_CHECK_THROW(facmp<T>(1.0, 1.0, -min),
                       std::invalid_argument);
     BOOST_CHECK(facmp<T>(0.25, 0.25, max) == 0);
     BOOST_CHECK(facmp<T>(0.25, 0.25, 0.0) == 0);
     BOOST_CHECK(facmp<T>(0.25, 0.25001, 0.000005) == -1);
     BOOST_CHECK(facmp<T>(0.25001, 0.25, 0.000005) == 1);
     BOOST_CHECK(facmp<T>(0.25001, 0.25, 0.0002) == 0);

     BOOST_CHECK_THROW(facmp<T>(1.0, inf, 0.1), invalid_argument);
     BOOST_CHECK_THROW(facmp<T>(inf, 1.0, 0.1), invalid_argument);
     BOOST_CHECK_THROW(facmp<T>(1.0, -inf, 0.1), invalid_argument);
     BOOST_CHECK_THROW(facmp<T>(-inf, 1.0, 0.1), invalid_argument);

     BOOST_CHECK_THROW(facmp<T>(1.0, qnan, 0.1), invalid_argument);
     BOOST_CHECK_THROW(facmp<T>(qnan, 1.0, 0.1), invalid_argument);
     BOOST_CHECK_THROW(facmp<T>(1.0, -qnan, 0.1), invalid_argument);
     BOOST_CHECK_THROW(facmp<T>(-qnan, 1.0, 0.1), invalid_argument);

     BOOST_CHECK_THROW(facmp<T>(1.0, snan, 0.1), invalid_argument);
     BOOST_CHECK_THROW(facmp<T>(snan, 1.0, 0.1), invalid_argument);
     BOOST_CHECK_THROW(facmp<T>(1.0, -snan, 0.1), invalid_argument);
     BOOST_CHECK_THROW(facmp<T>(-snan, 1.0, 0.1), invalid_argument);

     BOOST_CHECK(facmp<T>(1.0, max, 0.1) < 0);
     BOOST_CHECK(facmp<T>(max, 1.0, 0.1) > 0);
     BOOST_CHECK(facmp<T>(max, max, 0.1) == 0);

     BOOST_CHECK(facmp<T>(max, -max, 0.1) > 0);
     BOOST_CHECK(facmp<T>(-max, max, 0.1) < 0);
}

template <typename T, typename U, typename V, typename E>
int frcmp(U u, V v, E eps) {
     return SHG::frcmp<T>(static_cast<T>(u), static_cast<T>(v),
                          static_cast<T>(eps));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(frcmp_test, T, test_types) {
     using std::invalid_argument;
     using std::ldexp;
     using nl = std::numeric_limits<T>;
     constexpr auto max = nl::max();
     constexpr auto min = nl::min();
     constexpr auto inf = nl::infinity();
     constexpr auto qnan = nl::quiet_NaN();
     constexpr auto snan = nl::signaling_NaN();

     BOOST_CHECK_THROW(frcmp<T>(1.0, 1.0, -min), invalid_argument);
     BOOST_CHECK_THROW(frcmp<T>(1.0, 1.0, std::nextafter(1.0, 2.0)),
                       invalid_argument);

     T const u = ldexp(T(1.0), -2);
     T const v = u + ldexp(T(1.0), -17);
     T const eps1 = ldexp(T(1.0), -16);
     T const eps2 = ldexp(T(1.0), -15);
     BOOST_CHECK(frcmp<T>(u, u, 0.0) == 0);
     BOOST_CHECK(frcmp<T>(v, v, 0.0) == 0);
     BOOST_CHECK(frcmp<T>(u, v, eps1) == -1);
     BOOST_CHECK(frcmp<T>(v, u, eps1) == 1);
     BOOST_CHECK(frcmp<T>(u, v, eps2) == 0);
     BOOST_CHECK(frcmp<T>(v, u, eps2) == 0);

     BOOST_CHECK_THROW(frcmp<T>(1.0, inf, 0.1), invalid_argument);
     BOOST_CHECK_THROW(frcmp<T>(inf, 1.0, 0.1), invalid_argument);
     BOOST_CHECK_THROW(frcmp<T>(1.0, -inf, 0.1), invalid_argument);
     BOOST_CHECK_THROW(frcmp<T>(-inf, 1.0, 0.1), invalid_argument);

     BOOST_CHECK_THROW(frcmp<T>(1.0, qnan, 0.1), invalid_argument);
     BOOST_CHECK_THROW(frcmp<T>(qnan, 1.0, 0.1), invalid_argument);
     BOOST_CHECK_THROW(frcmp<T>(1.0, -qnan, 0.1), invalid_argument);
     BOOST_CHECK_THROW(frcmp<T>(-qnan, 1.0, 0.1), invalid_argument);

     BOOST_CHECK_THROW(frcmp<T>(1.0, snan, 0.1), invalid_argument);
     BOOST_CHECK_THROW(frcmp<T>(snan, 1.0, 0.1), invalid_argument);
     BOOST_CHECK_THROW(frcmp<T>(1.0, -snan, 0.1), invalid_argument);
     BOOST_CHECK_THROW(frcmp<T>(-snan, 1.0, 0.1), invalid_argument);

     BOOST_CHECK(frcmp<T>(1.0, max, 0.1) < 0);
     BOOST_CHECK(frcmp<T>(max, 1.0, 0.1) > 0);
     BOOST_CHECK(frcmp<T>(max, max, 0.1) == 0);

     BOOST_CHECK(frcmp<T>(max, -max, 0.1) > 0);
     BOOST_CHECK(frcmp<T>(-max, max, 0.1) < 0);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(bool_absolute_functions_test, T,
                              test_types) {
     using SHG::faeq;
     using SHG::fane;
     using SHG::falt;
     using SHG::fale;
     using SHG::fagt;
     using SHG::fage;
     using std::ldexp;

     T eps;
     T u;
     T v;

     eps = ldexp(T(1.0), -8);
     u = 1.0;
     v = u + eps / 2;

     BOOST_CHECK(faeq(u, v, eps));
     BOOST_CHECK(!fane(u, v, eps));
     BOOST_CHECK(!falt(u, v, eps));
     BOOST_CHECK(fale(u, v, eps));
     BOOST_CHECK(!fagt(u, v, eps));
     BOOST_CHECK(fage(u, v, eps));

     std::swap(u, v);

     BOOST_CHECK(faeq(u, v, eps));
     BOOST_CHECK(!fane(u, v, eps));
     BOOST_CHECK(!falt(u, v, eps));
     BOOST_CHECK(fale(u, v, eps));
     BOOST_CHECK(!fagt(u, v, eps));
     BOOST_CHECK(fage(u, v, eps));

     u = 1.0;
     v = u + 2 * eps;

     BOOST_CHECK(!faeq(u, v, eps));
     BOOST_CHECK(fane(u, v, eps));
     BOOST_CHECK(falt(u, v, eps));
     BOOST_CHECK(fale(u, v, eps));
     BOOST_CHECK(!fagt(u, v, eps));
     BOOST_CHECK(!fage(u, v, eps));

     std::swap(u, v);

     BOOST_CHECK(!faeq(u, v, eps));
     BOOST_CHECK(fane(u, v, eps));
     BOOST_CHECK(!falt(u, v, eps));
     BOOST_CHECK(!fale(u, v, eps));
     BOOST_CHECK(fagt(u, v, eps));
     BOOST_CHECK(fage(u, v, eps));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(bool_relative_functions_test, T,
                              test_types) {
     using SHG::freq;
     using SHG::frne;
     using SHG::frlt;
     using SHG::frle;
     using SHG::frgt;
     using SHG::frge;
     using std::ldexp;

     T eps;
     T u;
     T v;

     eps = ldexp(T(1.0), -8);
     u = 1.0;
     v = u + eps / 2;

     BOOST_CHECK(freq(u, v, eps));
     BOOST_CHECK(!frne(u, v, eps));
     BOOST_CHECK(!frlt(u, v, eps));
     BOOST_CHECK(frle(u, v, eps));
     BOOST_CHECK(!frgt(u, v, eps));
     BOOST_CHECK(frge(u, v, eps));

     std::swap(u, v);

     BOOST_CHECK(freq(u, v, eps));
     BOOST_CHECK(!frne(u, v, eps));
     BOOST_CHECK(!frlt(u, v, eps));
     BOOST_CHECK(frle(u, v, eps));
     BOOST_CHECK(!frgt(u, v, eps));
     BOOST_CHECK(frge(u, v, eps));

     u = 1.0;
     v = u + 2 * eps;

     BOOST_CHECK(!freq(u, v, eps));
     BOOST_CHECK(frne(u, v, eps));
     BOOST_CHECK(frlt(u, v, eps));
     BOOST_CHECK(frle(u, v, eps));
     BOOST_CHECK(!frgt(u, v, eps));
     BOOST_CHECK(!frge(u, v, eps));

     std::swap(u, v);

     BOOST_CHECK(!freq(u, v, eps));
     BOOST_CHECK(frne(u, v, eps));
     BOOST_CHECK(!frlt(u, v, eps));
     BOOST_CHECK(!frle(u, v, eps));
     BOOST_CHECK(frgt(u, v, eps));
     BOOST_CHECK(frge(u, v, eps));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace TESTS
