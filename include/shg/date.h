/* date.h: calendrical calculations */

/**
 * \file include/shg/date.h
 * Calendrical calculations.
 * \date Created on 15 April 2009.
 */

#ifndef SHG_DATE_H
#define SHG_DATE_H

#include <cstdint>
#include <iostream>
#include <string>
#include "shg/except.h"

/**
 * The only namespace which contains all the library.
 */
namespace SHG {

/**
 * \defgroup calendrical_calculations Calendrical calculations
 *
 * The class for date and related functions.
 *
 * \internal
 * Implementation
 * ==============
 *
 * Let us extend Gregorian calendar backwards. 1 Jan. 0001 (Monday) is
 * given the number 1, 2 Jan. 0001 (Tuesday) is given the number 2 and
 * so on. In general, 1 Jan. of the \f$n\f$-th year, \f$n > 0\f$, is
 * given the number \f[ f(n) = 1 + 365(n - 1) + \lfloor \frac{n -
 * 1}{4} \rfloor - \lfloor \frac{n - 1}{100} \rfloor + \lfloor \frac{n
 * - 1}{400} \rfloor, \f] and the day of the week of the \f$d\f$-th
 * day may be calculated as \f$d - 7 \lfloor d / 7 \rfloor \f$ (0 ---
 * Sun., 1 --- Mon. and so on). Employing both inequalities \f$x - 1 <
 * \lfloor x \rfloor \leq x\f$ to each component of the definition of
 * \f$f\f$, the following approximation can be found: \f[ c(n - 1) - 1
 * < f(n) < c(n - 1) + 2, \; n = 1, 2, \ldots \f] where \f$c = 146097
 * / 400 = 365.2425\f$. Since \f$f(n+1) - f(n) > c - 3 > 0\f$, \f$f\f$
 * is a strictly increasing function, and for each \f$m = 1, 2, \ldots
 * \f$ there exists the greatest natural number \f$n\f$ such that
 * \f$f(n) \leq m < f(n+1)\f$. For \f$m = 1\f$ this number is 1. For
 * \f$m > 1\f$, the greatest natural \f$n\f$ satisfying the inequality
 * \f$c(n-1) + 2 \leq m\f$ is \f$N = 1 + \lfloor (m - 2) / c
 * \rfloor.\f$ On the right-hand of the above inequality we have
 * \f$f(N) < m.\f$ Since \f[ m < cN + 2 < cN + (c-1) = c(N+1) - 1 <
 * f(N+2),\f] we have \f$f(N) < m < f(N + 2)\f$. Hence, the one and
 * only number satisfying \f$f(n) \leq m < f(n + 1)\f$ is one of the
 * numbers \f$N\f$ and \f$N + 1\f$. For Julian calendar, the reasoning
 * is analogous.
 *
 * The reform of Julian calendar in 1582, which introduced Gregorian
 * calendar, consisted in modifying the definition of a leap year and
 * correcting the ten-day error by accepting that Thursday 4 Oct. 1582
 * (Julian) is followed immediately by Friday 15 Oct. 1582
 * (Gregorian). The following table illustrates the way of numbering
 * dates:
 *
 * Number |  Gregorian calend. |  Julian calendar
 * ------:|  ----------------- |  -----------------
 *      1 |  Mon. 01 Jan. 0001 |  Mon. 03 Jan. 0001
 * ...... |  ................. |  .................
 * 577735 |  Thu. 14 Oct. 1582 |  Thu. 04 Oct. 1582
 * 577736 |  Fri. 15 Oct. 1582 |  Fri. 05 Oct. 1582
 * ...... |  ................. |  .................
 * 710347 |  Mon. 12 Nov. 1945 |  Mon. 30 Oct. 1945
 * ...... |  ................. |  .................
 * 729755 |  Fri. 01 Jan. 1999 |  Fri. 19 Dec. 1998
 *
 *
 * The function Date::easter(int) is based on the following program in
 * Basic, quoted in "Informatyka" 2/1984 after "PPC Calculator
 * Journal":
 * \code
 * 10 rem Program podaje date Wielkanocy wg kalendarza
 * 20 rem gregorianskiego czyli od roku 1583 wlacznie.
 * 30 rem Dla dat wczesniejszych (kalendarz julianski)
 * 40 rem nalezy podstawic:
 * 50 rem m = 15 oraz p = 6 (patrz linie 130 oraz 150).
 * 60 input "rok"; y
 * 80 a = y - int(y / 19) * 19
 * 90 b = y - int(y / 4) * 4
 * 100 c = y - int(y / 7) * 7
 * 110 t = int(y / 100)
 * 120 p = 4 + t - int(t / 4)
 * 130 m = p - int(p / 7) * 7
 * 140 p = 15 + t - int(t / 4) - int((8 * t + 13) / 25)
 * 150 p = p - int(p / 30) * 30
 * 160 p = 19 * a + p
 * 170 d = p - int(p / 30) * 30
 * 180 if d < 28 then 220
 * 190 if d = 29 then 210
 * 200 if a <= 10 then 220
 * 210 d = d - 1
 * 220 p = 2 * b + 4 * c - d + m
 * 230 p = p - int(p / 7) * 7
 * 240 d = d + p
 * 250 if d > 9 then 290
 * 260 d = d + 22
 * 270 print "Wielkanoc: "; d; "marca "; y
 * 280 stop
 * 290 d = d - 9
 * 300 print "Wielkanoc: "; d; "kwietnia "; y
 * 310 end
 * \endcode
 *
 * Bibliography
 * ============
 *
 * \cite dershowitz-reingold-1990.
 *
 * Jakub Tatarkiewicz, Wielkanoc, Informatyka 2/1984, p. 19.
 *
 * \{
 */

/**
 * A class to store a calendrical date. The class is capable of
 * storing dates from 1 January 1583 to 31 December 9999.
 *
 * \par Date formats.
 *
 * Four formats of date are supported. They are listed below with
 * examples of the date 22 January 1901.
 *
 * - \a full "22 stycznia 1901"
 * - \a Roman "22 I 1901"
 * - \a common "22.01.1901"
 * - \a computer "19010122"
 *
 * The full format returns the month name in Polish in genitive case
 * in UTF-8 encoding.
 */
class Date {
public:
     enum Day {sun, mon, tue, wed, thu, fri, sat};
     enum Month {jan = 1, feb, mar, apr, may, jun,
                 jul, aug, sep, oct, nov, dec};

     /**
      * An exception class thrown in case of incorrect date. The
      * what() function returns the text "bad date".
      */
     struct Bad_date : public virtual Exception {
          Bad_date();
     };

     /**
      * Constructs an unitialized Date.
      */
     Date();
     /**
      * Constructs initialized date.
      *
      * \param d the number of the day in the month
      * \param m the number of the month in the year
      * \param y the number of the year
      *
      * For example, the code Date(22, 1, 1901) constructs the date of
      * 22 January 1901.
      *
      * \exception Bad_date if the date is incorrect
      */
     Date(int d, int m, int y);

     /**
      * \copybrief Date(int, int, int)
      *
      * This constructor accepts all four formats named in Date. In \a
      * full and \a Roman formats the number of the day must not start
      * from 0. In \a common and \a computer formats the number of the
      * day and the number of the month must have two digits. In all
      * formats the number of the year must have 4 digits. Leading and
      * trailing white space is not allowed.
      *
      * Examples:
      *
      * - Date("22 stycznia 1901") --- OK
      * - Date("22 I 1901") --- OK
      * - Date("22.01.1901") --- OK, length must be 10
      * - Date("19010122") --- OK, length must be 8
      * - Date("02 stycznia 1901") --- will fail
      * - Date("02 I 1901") --- will fail
      * - Date("22.1.1901") --- will fail
      *
      * \exception Bad_date if the date is incorrect
      */
     explicit Date(const std::string& s);
     /**
      * \copydoc Date(const std::string&)
      */
     explicit Date(const char *s);

     /**
      * \copybrief Date(int, int, int)
      *
      * Constructs date in the following way:
      *
      * 1. if \a n > 0 this is the \a n-th day of week \a d in month
      * \a m in year \a y
      *
      * 2. if \a n < 0 this is the -\a n-th day of week starting from
      * the last day in the month \a m.
      *
      * For example, to get date of the next to the last Friday in
      * November 2011 call Date(-2, fri, nov, 2011).
      *
      * \exception Bad_date on incorrect input
      */
     Date(int n, int d, int m, int y);

     int day() const;   /**< Returns the day of the month. */
     int month() const; /**< Returns the month of the year. */
     int year() const;  /**< Returns the year. */
     /**
      * Returns the day, month and the year.
      */
     void split(int& d, int& m, int& y) const;
     /**
      * Returns day of week.
      */
     Day dayofweek() const;
     std::string full() const;     /**< Returns \a full format. */
     std::string roman() const;    /**< Returns \a Roman format. */
     std::string common() const;   /**< Returns \a common format. */
     std::string computer() const; /**< Returns \a computer format. */
     /**
      * \copybrief full() \details \a s must be at least 21 characters
      * long. \return \a s
      */
     char* full(char* s) const;
     /**
      * \copybrief roman() \details \a s must be at least 13
      * characters long. \return \a s
      */
     char* roman(char* s) const;
     /**
      * \copybrief common() \details \a s must be at least 11
      * characters long. \return \a s
      */
     char* common(char* s) const;
     /**
      * \copybrief computer() \details \a s must be at least 9
      * characters long. \return \a s
      */
     char* computer(char* s) const;
     /**
      * Returns minimum date: 1 January 1583.
      */
     static Date min();
     /**
      * Returns maximum date: 31 December 9999.
      */
     static Date max();
     /**
      * Returns current date.
      * \exception Bad_date if the date is unavailable
      */
     static Date today();
     /**
      * Returns true if the date \a dmy is a correct date. The date
      * must be between Date::min and Date::max.
      */
     static bool correct(int d, int m, int y);
     /**
      * Returns true if the date written in \a computer format is a
      * correct date. The date must be between Date::min and
      * Date::max.
      */
     static bool correct(const std::string& s);
     /**
      * Returns true if the year \a y is leap.
      */
     static bool leap(int y);
     /**
      * Returns thne number of the last day in the month.
      * \exception Bad_date if the month or the year is incorrect
      */
     static int lastdom(int m, int y);
     /**
      * Returns date of Easter in the year \a y.
      */
     static Date easter(int y);

     /**
      * \name Incrementation and decrementation by one day.
      * \warning There is no range control.
      * \{
      */
     Date& operator++();
     Date& operator--();
     Date operator++(int);
     Date operator--(int);
     /**\}*/

     friend Date operator+(Date d, int n);
     friend Date operator+(int n, Date d);
     friend Date operator-(Date d, int n);
     friend int operator-(Date d1, Date d2);

     friend bool operator==(Date d1, Date d2);
     friend bool operator!=(Date d1, Date d2);
     friend bool operator<=(Date d1, Date d2);
     friend bool operator>=(Date d1, Date d2);
     friend bool operator<(Date d1, Date d2);
     friend bool operator>(Date d1, Date d2);

private:
     typedef std::int_fast32_t int32;

     int32 abs;                 /**< Internal representation. */

     /**
      * Constructor from internal representation.
      */
     explicit Date(int32 a);
     /**
      * Returns internal representation of the 1 January of the year
      * \a y.
      */
     static int32 jan1(int y);
     /**
      * Returns internal representation of the date \a dmy.
      */
     static int32 pack(int d, int m, int y);

     static int a4toi(const char* s) {
          if (!isdigit(s[0]) || !isdigit(s[1]) ||
              !isdigit(s[2]) || !isdigit(s[3]))
               throw Bad_date();
          return
               (s[0] - '0') * 1000 +
               (s[1] - '0') *  100 +
               (s[2] - '0') *   10 +
               (s[3] - '0');
     }

     static int a2toi(const char* s) {
          if (!isdigit(s[0]) || !isdigit(s[1]))
               throw Bad_date();
          return (s[0] - '0') * 10 + (s[1] - '0');
     }

     static int a1toi(const char* s) {
          if (!isdigit(s[0]))
               throw Bad_date();
          return s[0] - '0';
     }

     /**
      * Numbers of days in months.
      */
     static const int daytab[2][13];
     /**
      * Numbers of days since 1 January to the last day in the month.
      */
     static const int daytab2[2][13];
     /**
      * Roman numbers.
      */
     static const char* roman_month[13];
     /**
      * Polish name of months in genitive case, UTF-8 encoded.
      */
     static const char* polish_month[13];
};

/**
 * Adds \a n days to \a d. \warning There is no range control.
 */
Date operator+(Date d, int n);
/**
 * \copydoc operator+(Date, int)
 */
Date operator+(int n, Date d);
/**
 * Subtracts \a n days from the date \a d. \warning There is no range
 * control.
 */
Date operator-(Date d, int n);
/**
 * Returns \a d1 - \a d2 in days.
 */
int operator-(Date d1, Date d2);

bool operator==(Date d1, Date d2);
bool operator!=(Date d1, Date d2);
bool operator<=(Date d1, Date d2);
bool operator>=(Date d1, Date d2);
bool operator<(Date d1, Date d2);
bool operator>(Date d1, Date d2);

/**
 * Outputs date in \a computer format.
 */
std::ostream& operator<<(std::ostream& stream, Date d);
/**
 * Inputs date from any of the four formats.
 * \exception Bad_date on incorrect input
 */
std::istream& operator>>(std::istream& stream, Date& d);

/** \} */ /* end of group calendrical_calculations */

}       // namespace SHG

#endif
