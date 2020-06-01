/* date.cc: testing calendrical calculations */

/**
 * \file testing/date.cc
 * Testing calendrical calculations.
 * \date Created on 4 April 2010.
 */

#include <sstream>
#include "shg/date.h"
#include "shg/vector.h"
#include "testshg.h"

namespace SHG {
namespace Testing {

using std::string;
using std::stringstream;
using SHG::Date;
using SHG::Vecint;

namespace {

/*
 * Returns day and month of Easter in Gregorian year y. This function is
 * written exactly on the algorithm given in Donald E. Knuth, Sztuka
 * programowania, t. I, Algorytmy podstawowe, p. 165-166, 538-540.
 */
void knuth(int y, int* day, int* month) {
     int g = y % 19 + 1;
     int c = y / 100 + 1;
     int x = (3 * c) / 4 - 12;
     int z = (8 * c + 5) / 25 - 5;
     int d = (5 * y) / 4 - x - 10;
     int e = (11 * g + 20 + z - x) % 30;
     if (y <= 1582) {
          *day = *month = -1;
          return;
     }
     if (e < 0)
          e += 30;
     if ((e == 25 && g > 11) || e == 24)
          e++;
     int n = 44 - e;
     if (n < 21)
          n += 30;
     n = n + 7 - ((d + n) % 7);
     if (n > 31) {
          *day = n - 31;
          *month = 4;
     } else {
          *day = n;
          *month = 3;
     }
}

void test_constructor() {
     SHG::Vecint c(7);
     for (int y = Date::min().year(); y <= Date::max().year(); y++) {
          for (int m = 1; m <= 12; m++) {
               const int last = Date::lastdom(m, y);
               c = 0;
               for (int d = 1; d <= last; d++) {
                    const Date dt(d, m, y);
                    const int dow = dt.dayofweek();
                    c[dow]++;
                    Date dt1(c[dow], dow, m, y);
                    SHG_ASSERT(dt1 == dt);
               }
               c = 0;
               for (int d = last; d >= 1; d--) {
                    const Date dt(d, m, y);
                    const int dow = dt.dayofweek();
                    c[dow]++;
                    Date dt1(-c[dow], dow, m, y);
                    SHG_ASSERT(dt1 == dt);
               }
          }
     }
     try {const Date d(5, Date::fri, Date::nov, 2011); SHG_ASSERT(0);}
     catch (const Date::Bad_date&) {}
}

void test_one_date(Date d) {
     Date b = d;
     b--; SHG_ASSERT(b < d && d != b);
     b++; SHG_ASSERT(b <= d && b >= d);
     --b; SHG_ASSERT(d > b && b != d);
     ++b; SHG_ASSERT(b == d);
     int day, month, year;
     d.split(day, month, year);
     SHG_ASSERT(day == d.day() && month == d.month() &&
                year == d.year());
     SHG_ASSERT(Date::correct(day, month, year));
     string s = d.computer();
     Date d1(s);
     SHG_ASSERT(Date::correct(s));
     SHG_ASSERT(d1 == d);
}

}       // anonymous namespace

void test_date() {
     static const char* const bad[] = {
          "02 stycznia 1901",
          "02 I 1901",
          "2.1.1901",
          "02.1.1901",
          "2.01.1901",
          0
     };
     static const struct {const char* s; int d, m, y;} good[] =  {
          {"19010122", 22, 1, 1901},
          {"22.01.1901", 22, 1, 1901},
          {"22 stycznia 1901", 22, 1, 1901},
          {"22 I 1901", 22, 1, 1901},
          {"19010102", 2, 1, 1901},
          {"02.01.1901", 2, 1, 1901},
          {"2 stycznia 1901", 2, 1, 1901},
          {"2 I 1901", 2, 1, 1901},
          {0, 0, 0, 0}
     };
     test_constructor();
     for (int i = 0; good[i].s; i++) {
          const Date d(good[i].s);
          SHG_ASSERT(d == Date(good[i].d, good[i].m, good[i].y));
     }

     for (int i = 0; bad[i]; i++) {
          try  {Date d(bad[i]);}
          catch (const Date::Bad_date&) {continue;}
          SHG_ASSERT(0);
     }

     Date last;
     last = Date::min() + 40000;
     for (Date d = last - 40000; d <= last; ++d)
          test_one_date(d);

     last = Date(31, Date::dec, 2400);
     for (Date d = Date(1, Date::jan, 1899); d <= last; ++d)
          test_one_date(d);

     last = Date::max();
     for (Date d = last - 40000; d <= last; ++d)
          test_one_date(d);

     /* Test the function easter against the Knuth function. */
     for (int y = Date::min().year(); y <= Date::max().year(); y++) {
          int d, m;
          knuth(y, &d, &m);
          SHG_ASSERT(Date::easter(y) == Date(d, m, y));
     }

     for (Date d = Date(31, Date::dec, 2008);
          d >= Date(1, Date::jan, 1945); --d) {
          SHG_ASSERT((d - 30) + 30 == d);
          SHG_ASSERT((30 + d) - 30 == d);
          SHG_ASSERT((d + 30) - d == 30);
          SHG_ASSERT(d - (30 + d) == -30);
     }

     try {Date d(31, Date::dec, 1582);}
     catch (const Date::Bad_date&) {goto next;}
     SHG_ASSERT(0);
next:
     stringstream ss;
     ss << Date::min();
     Date d;
     ss >> d;
     SHG_ASSERT(d == Date::min());
}

}       // namespace Testing
}       // namespace SHG
