#include "shg/date.h"
#include <sstream>
#include <string>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include "shg/date.h"
#include "shg/vector.h"

namespace bdata = boost::unit_test::data;

namespace SHG::BTesting {

BOOST_AUTO_TEST_SUITE(date_test)

BOOST_AUTO_TEST_CASE(all_dates_test) {
     Vecint c(7);
     for (int y = Date::min().year(); y <= Date::max().year(); y++) {
          for (int m = 1; m <= 12; m++) {
               const int last = Date::lastdom(m, y);
               c = 0;
               for (int d = 1; d <= last; d++) {
                    const Date dt(d, m, y);
                    const int dow = dt.dayofweek();
                    c[dow]++;
                    Date dt1(c[dow], dow, m, y);
                    BOOST_CHECK(dt1 == dt);
               }
               c = 0;
               for (int d = last; d >= 1; d--) {
                    const Date dt(d, m, y);
                    const int dow = dt.dayofweek();
                    c[dow]++;
                    Date dt1(-c[dow], dow, m, y);
                    BOOST_CHECK(dt1 == dt);
               }
          }
     }
     BOOST_CHECK_THROW(Date d(5, Date::fri, Date::nov, 2011),
                       Date::Bad_date);
}

namespace {

constexpr const char* const incorrect_input[] = {
     "02 stycznia 1901", "02 I 1901", "2.1.1901", "02.1.1901",
     "2.01.1901"};

constexpr const char* const correct_input[] = {
     "19010122", "22.01.1901", "22 stycznia 1901", "22 I 1901",
     "19010102", "02.01.1901", "2 stycznia 1901",  "2 I 1901"};

}  // anonymous namespace

BOOST_DATA_TEST_CASE(incorrect_constructor_input_test,
                     bdata::make(incorrect_input), s) {
     BOOST_CHECK_THROW(Date d(s), Date::Bad_date);
}

BOOST_DATA_TEST_CASE(correct_constructor_input_test,
                     bdata::make(correct_input), s) {
     BOOST_CHECK_NO_THROW(Date d(s));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting

namespace {

using SHG::Date;

class Date_dataset {
public:
     using sample = Date;
     enum { arity = 1 };

     struct iterator {
          iterator() : interval(1), current(first1) {}

          Date operator*() const { return current; }
          iterator& operator++() {
               ++current;
               switch (interval) {
                    case 1:
                         if (current > last1) {
                              interval = 2;
                              current = first2;
                         }
                         break;
                    case 2:
                         if (current > last2) {
                              interval = 3;
                              current = first3;
                         }
                         break;
               }
               return *this;
          }
          static const Date first1;
          static const Date last1;
          static const Date first2;
          static const Date last2;
          static const Date first3;
          static const Date last3;

     private:
          int interval;
          Date current;
     };
     Date_dataset() {}
     bdata::size_t size() const {
          return (iterator::last1 - iterator::first1 + 1) +
                 (iterator::last2 - iterator::first2 + 1) +
                 (iterator::last3 - iterator::first3 + 1);
     }
     iterator begin() const {
          return iterator();
     }
};

const Date Date_dataset::iterator::first1 = Date::min();
const Date Date_dataset::iterator::last1 = Date::min()+ 40000;
const Date Date_dataset::iterator::first2 = Date(1, Date::jan, 1899);
const Date Date_dataset::iterator::last2 = Date(31, Date::dec, 2400);
const Date Date_dataset::iterator::first3 = Date::max() - 40000;
const Date Date_dataset::iterator::last3 = Date::max();

}  // anonymous namespace

namespace boost::unit_test::data::monomorphic {

template <>
struct is_dataset<Date_dataset> : boost::mpl::true_ {};

}  // namespace boost::unit_test::data::monomorphic

namespace SHG::BTesting {

BOOST_AUTO_TEST_SUITE(date_test)

BOOST_DATA_TEST_CASE(operator_test, Date_dataset()) {
     const Date d(sample);
     Date b = d;
     b--; BOOST_CHECK(b < d && d != b);
     b++; BOOST_CHECK(b <= d && b >= d);
     --b; BOOST_CHECK(d > b && b != d);
     ++b; BOOST_CHECK(b == d);
     int day, month, year;
     d.split(day, month, year);
     BOOST_CHECK(day == d.day() && month == d.month() &&
                year == d.year());
     BOOST_CHECK(Date::correct(day, month, year));
     std::string s = d.computer();
     Date d1(s);
     BOOST_CHECK(Date::correct(s));
     BOOST_CHECK(d1 == d);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::BTesting
