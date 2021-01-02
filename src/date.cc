/**
 * \file src/date.cc
 * Calendrical calculations.
 * \date Created on 15 April 2009.
 */

#include <shg/date.h>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>

namespace SHG {

using std::istream;
using std::ostream;
using std::string;

Date::Bad_date::Bad_date() : Exception("bad date") {}

Date::Date() : abs() {}

Date::Date(int d, int m, int y) : abs() {
     if (correct(d, m, y))
          abs = pack(d, m, y);
     else
          throw Bad_date();
}

Date::Date(const string& s) : abs() {
     *this = Date(s.c_str());
}

Date::Date(const char* s) : abs() {
     /* The longest name of the month in polish_month[] and
        roman_month[] is 13 characters long. */
     static const int lnom = 13;

     if (const char* const sep = strchr(s, ' ')) {
          /* "22 stycznia 1901" or "22 I 1901" */
          int m, d;
          switch (sep - s) {
               case 1:
                    d = a1toi(s);
                    break;
               case 2:
                    if (s[0] == '0')
                         throw Bad_date();
                    d = a2toi(s);
                    break;
               default:
                    throw Bad_date();
          }
          const char* sep1 = strchr(sep + 1, ' ');
          if (sep1 == 0 || sep1 == sep + 1)
               throw Bad_date();
          const int len = sep1 - (sep + 1);
          if (len > lnom)
               throw Bad_date();
          char mnth[lnom + 1];
          strncpy(mnth, sep + 1, len)[len] = 0;
          for (m = 1; m <= 12; m++)
               if (strcmp(mnth, polish_month[m]) == 0 ||
                   strcmp(mnth, roman_month[m]) == 0)
                    break;
          if (m > 12 || strlen(++sep1) != 4)
               throw Bad_date();
          *this = Date(d, m, a4toi(sep1));
     } else if (strchr(s, '.')) {
          /*  "22.01.1901" */
          if (strlen(s) != 10)
               throw Bad_date();
          if (s[2] != '.' || s[5] != '.')
               throw Bad_date();
          *this = Date(a2toi(s), a2toi(s + 3), a4toi(s + 6));
     } else {
          /* "19010122" */
          if (strlen(s) != 8)
               throw Bad_date();
          *this = Date(a2toi(s + 6), a2toi(s + 4), a4toi(s));
     }
}

Date::Date(int n, int d, int m, int y) : abs() {
     if (n == 0 || n > 5 || n < -5 || d < sun || d > sat ||
         !correct(1, m, y))
          throw Bad_date();
     if (n > 0) {
          const Date d1(1, m, y);
          const int dow = d1.dayofweek();
          const int z = d < dow ? d + (7 - dow) : d - dow;
          *this = Date(d1 + z + (n - 1) * 7);
     } else {
          const bool leap = Date::leap(y);
          const int last = daytab[leap][m];
          const Date d1(last, m, y);
          const int dow = d1.dayofweek();
          const int z = d > dow ? d - (7 + dow) : d - dow;
          *this = Date(d1 + z + (n + 1) * 7);
     }
     if (month() != m)
          throw Bad_date();
}

int Date::day() const {
     int d, m, y;
     this->split(d, m, y);
     return d;
}

int Date::month() const {
     int d, m, y;
     this->split(d, m, y);
     return m;
}

int Date::year() const {
     int d, m, y;
     this->split(d, m, y);
     return y;
}

void Date::split(int& d, int& m, int& y) const {
     int32 x = abs;

     if (x == 1) {
          y = 1;
     } else {
          y = 1 + (x - 2) * 400 / 146097;
          if (jan1(y + 1) <= x)
               ++y;
     }
     ++x;
     x -= jan1(y);
     bool leap = Date::leap(y);
     for (m = 1; x > daytab2[leap][m]; ++m)
          ;
     d = x - daytab2[leap][m - 1];
}

Date::Day Date::dayofweek() const {
     return Day(abs % 7);
}

string Date::full() const {
     char buf[21];
     return full(buf);
}

string Date::roman() const {
     char buf[13];
     return roman(buf);
}

string Date::common() const {
     char buf[11];
     return common(buf);
}

string Date::computer() const {
     char buf[9];
     return computer(buf);
}

char* Date::full(char* s) const {
     int d, m, y;
     split(d, m, y);
     sprintf(s, "%d %s %d", d, polish_month[m], y);
     return s;
}

char* Date::roman(char* s) const {
     int d, m, y;
     split(d, m, y);
     sprintf(s, "%d %s %d", d, roman_month[m], y);
     return s;
}

char* Date::common(char* s) const {
     int d, m, y;
     split(d, m, y);
     sprintf(s, "%02d.%02d.%04d", d, m, y);
     return s;
}

char* Date::computer(char* s) const {
     int d, m, y;
     split(d, m, y);
     sprintf(s, "%04d%02d%02d", y, m, d);
     return s;
}

Date Date::min() {
     static const Date min(1, jan, 1583);
     return min;
}

Date Date::max() {
     static const Date max(31, dec, 9999);
     return max;
}

Date Date::today() {
     using std::localtime;
     using std::time;
     using std::time_t;
     using std::tm;
     const time_t rawtime = time(nullptr);
     if (rawtime == static_cast<time_t>(-1))
          throw Bad_date();
     const tm* const lt = localtime(&rawtime);
     return Date(lt->tm_mday, lt->tm_mon + 1, 1900 + lt->tm_year);
}

bool Date::correct(int d, int m, int y) {
     return y >= 1583 && y <= 9999 && m >= 1 && m <= 12 && d >= 1 &&
            d <= daytab[leap(y)][m];
}

bool Date::correct(const string& s) {
     try {
          Date d(s);
     } catch (const Bad_date&) {
          return false;
     }
     return true;
}

bool Date::leap(int y) {
     return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

int Date::lastdom(int m, int y) {
     if (correct(1, m, y))
          return daytab[leap(y)][m];
     throw Bad_date();
}

Date Date::easter(int y) {
     int p = y / 100;
     int q = 4 + p - p / 4;
     int r = q % 7;
     q = 15 + p - p / 4 - (8 * p + 13) / 25;
     int s = (19 * (y % 19) + q % 30) % 30;
     if (s > 27 && (s == 29 || y % 19 > 10))
          --s;
     q = (2 * (y % 4) + 4 * (y % 7) - s + r) % 7;
     while (q < 0)
          q += 7;
     return (s += q) > 9 ? Date(s - 9, 4, y) : Date(s + 22, 3, y);
}

Date& Date::operator++() {
     ++abs;
     return *this;
}

Date& Date::operator--() {
     --abs;
     return *this;
}

Date Date::operator++(int) {
     Date old(*this);
     ++*this;
     return old;
}

Date Date::operator--(int) {
     Date old(*this);
     --*this;
     return old;
}

Date::Date(int32 a) : abs(a) {}

Date::int32 Date::jan1(int y) {
     --y;
     return 365 * y + y / 4 - y / 100 + y / 400 + 1;
}

Date::int32 Date::pack(int d, int m, int y) {
     return jan1(y) + daytab2[leap(y)][m - 1] + d - 1;
}

const int Date::daytab[2][13] = {
     {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
     {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

const int Date::daytab2[2][13] = {
     {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
     {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}};

const char* Date::roman_month[13] = {0,   "I",  "II",  "III",  "IV",
                                     "V", "VI", "VII", "VIII", "IX",
                                     "X", "XI", "XII"};

const char* Date::polish_month[13] = {
     0, "stycznia", "lutego", "marca", "kwietnia", "maja", "czerwca",
     /* UTF-8 used below. */
     "lipca", "sierpnia", "wrze\305\233nia", "pa\305\272dziernika",
     "listopada", "grudnia"};

Date operator+(Date d, int n) {
     return Date(d.abs + n);
}
Date operator+(int n, Date d) {
     return Date(n + d.abs);
}
Date operator-(Date d, int n) {
     return Date(d.abs - n);
}
int operator-(Date d1, Date d2) {
     return d1.abs - d2.abs;
}

bool operator==(Date d1, Date d2) {
     return d1.abs == d2.abs;
}
bool operator!=(Date d1, Date d2) {
     return d1.abs != d2.abs;
}
bool operator<=(Date d1, Date d2) {
     return d1.abs <= d2.abs;
}
bool operator>=(Date d1, Date d2) {
     return d1.abs >= d2.abs;
}
bool operator<(Date d1, Date d2) {
     return d1.abs < d2.abs;
}
bool operator>(Date d1, Date d2) {
     return d1.abs > d2.abs;
}

ostream& operator<<(ostream& stream, Date d) {
     stream << d.computer();
     return stream;
}

istream& operator>>(istream& stream, Date& d) {
     string s;
     stream >> s;
     d = Date(s);
     return stream;
}

}  // namespace SHG
