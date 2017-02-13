/* stock.cc: processing stock data */

/**
 * \file src/stock.cc
 * Processing stock data.
 * \date Created on 11 September 2009.
 */

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include "shg/except.h"
#include "shg/utils.h"
#include "shg/stock.h"

using std::abs;
using std::left;
using std::right;
using std::setw;
using std::size_t;
using std::string;
using std::vector;
using SHG::Date;
using SHG::Matdouble;

namespace SHG {
namespace Stock {

Stock_error::Stock_error(const string& what)
     : SHG::Exception(what) {}

Invalid_input_file::Invalid_input_file()
     : Stock_error("invalid_input_file") {}

namespace {

class Rawdata {                 // reads in a file
public:
     explicit Rawdata(const char* fname);
     Rawdata(const Rawdata&) = delete;
     Rawdata& operator=(const Rawdata&) = delete;
     virtual ~Rawdata();
protected:
     SHG::Vecchar vc;           // file buffer
     char* const buf;           // pointer to file buffer
     const size_t bufsize;      // buffer size
     string fname_;             // file name
     static bool isvaliddate(const char* s);
};

class Rawprices : public Rawdata {      // file of prices
public:
     explicit Rawprices(const char* fname);
     void report(std::ostream& f = std::cout) const;
     void add(Rawprices& d, std::ostream& f = std::cout);
     void fill(int& nassets, int& nobs, vector<string>& name,
               vector<Date>& date, Matdouble& price) const;
private:
     struct Report {
          Report();
          const char* first_date;
          const char* last_date;
          size_t nobs;
     };
     int nrec;                  // the number of records
     static const int reclen = 33;
     static bool isvalidprice(const char* s);
};

class Rawsplits : public Rawdata {      // file of splits
public:
     explicit Rawsplits(const char* fname);
     void adjust(const vector<string>& name,
                 const vector<Date>& date,
                 Matdouble& price) const;
private:
     int nrec;                  // the number of records
     static const int reclen = 30;
     static bool isvalidint(const char* s);
};

Rawdata::Rawdata(const char* fname)
     : vc(SHG::wfread(fname)), buf(vc.c_vec()),
       bufsize(vc.size()), fname_(fname) {
     if (bufsize <= 0)
          throw Stock_error("could not read file");
     if (buf[bufsize - 1] != '\n')
          throw Stock_error("the last character not newline");
     buf[bufsize - 1] = 0;
     const char* s = buf;
     while (*s++) {/* VOID */}
     if (s != buf + bufsize)
          throw Stock_error("file contains null characters");
     buf[bufsize - 1] = '\n';
}

Rawdata::~Rawdata() {}

bool Rawdata::isvaliddate(const char* s) {
     Date dt;
     try {dt = Date(string(s));}
     catch (Date::Bad_date) {return false;}
     Date::Day dof = dt.dayofweek();
     if (dof == Date::sat || dof == Date::sun)
          return false;
     int d, m, y;
     dt.split(d, m, y);
     if (m == 12 && (d == 25 || d == 26))
          return false;
     if (m == 8 && d == 15)
          return false;
     Date easter = Date::easter(y);
     if (dt == easter || dt == easter + 1)
          return false;
     return true;
}

/*
 * The record in file of prices is as follows:
 *
 *  0         1         2         3      32
 *  01234567890123456789012345678901
 * "19910416 EXBUD         149000.00" + '\n'
 * "dddddddd aaaaaaaaaaaa ddddddd.dd"
 */
Rawprices::Rawprices(const char* fname)
     : Rawdata(fname), nrec(bufsize / reclen) {
     char *s;
     const char* t;
     int i, j;

     if (static_cast<size_t>(nrec) * static_cast<size_t>(reclen)
         != bufsize)
          throw Stock_error("invalid number of records");

     for (i = 0, s = buf; i < nrec; i++, s += reclen)
          if (*(s + 8) != ' ' || *(s + 21) != ' ' || *(s + 32) != '\n')
               throw Invalid_input_file();
          else
               *(s + 32) = ' ';

     buf[bufsize - 1] = 0;
     for (s = buf; *s; s++)
          if (isspace(*s)) {
               if (*s != ' ')
                    throw Invalid_input_file();
               *s = 0;
          }

     t = buf;                   // Dummy initialization for compiler.
     for (i = 0, s = buf; i < nrec; i++, s += reclen) {
          if (!*(s + 9))
               throw Invalid_input_file();
          for (j = 22; j < 32 && !s[j]; j++) ;
          if (j >= 32)
               throw Invalid_input_file();
          if (!isvalidprice(s + j))
               throw Invalid_input_file();
          if (i) {
               if (strcmp(s, t) && !isvaliddate(s))
                    throw Invalid_input_file();
               if (memcmp(s, t, 21) <= 0)
                    throw Stock_error("unsorted input file");
          } else {
               if (!isvaliddate(s))
                    throw Invalid_input_file();
          }
          t = s;
     }
}

void Rawprices::report(std::ostream& f) const {
     std::map<string, Report> rep;
     const char *s = buf;
     for (int i = 0; i < nrec; i++, s += reclen) {
          Report& r = rep[s + 9];
          if (strcmp(s, r.first_date) < 0)
               r.first_date = s;
          if (strcmp(s, r.last_date) > 0)
               r.last_date = s;
          // Only positive prices should increase the number of
          // observations.
          const char* t = s + 21;
          while (!*++t) {/* VOID */}
          if (atof(t) > 0.0)
               r.nobs++;
     }
     int lp = 0;

     f << " Nr      Name     Period of quotations  N. of obs.\n"
       << "---- ------------ --------------------- -----------\n";
     for (std::map<string, Report>::const_iterator p = rep.begin();
          p != rep.end(); p++) {
          f << setw(4) << ++lp << ' '
            << setw(12) << left << p->first << right << ' '
            << Date(p->second.first_date).common() << '-'
            << Date(p->second.last_date).common() << ' '
            << setw(11) << p->second.nobs << "\n";
     }
}

void Rawprices::add(Rawprices& d, std::ostream& f) {
     // date of last day from previous set
     const char* dt = buf + (nrec - 1) * reclen;
     const char* dt1 = d.buf;   // date from new dataset
     if (strcmp(dt, dt1) >= 0)
          throw Stock_error("date of new data invalid");
     int i;
     const char* s;
     for (i = 0, s = d.buf; i < d.nrec; i++, s += reclen)
          if (strcmp(dt1, s))
               throw Invalid_input_file();
     // names of stocks quoted previously and names from new dataset
     std::map<string, bool> oldnam, newnam;
     s = buf + nrec * reclen;
     while ((s -= reclen) >= buf && !strcmp(s, dt))
          oldnam[s + 9];
     s = d.buf + d.nrec * reclen;
     while ((s -= reclen) >= d.buf)
          newnam[s + 9];

     // Are there discontinued or new stocks?
     std::map<string, bool>::iterator p;
     int ndis = 0, nnew = 0;
     for (p = oldnam.begin(); p != oldnam.end(); p++)
          if ((p->second = newnam.find(p->first) == newnam.end()))
               ndis++;
     for (p = newnam.begin(); p != newnam.end(); p++)
          if ((p->second = oldnam.find(p->first) == oldnam.end()))
               nnew++;
     if (ndis > 0) {
          f << ndis << " discontinued stock(s):";
          for (p = oldnam.begin(); p != oldnam.end(); p++)
               if (p->second)
                    f << ' ' << p->first;
          f << '\n';
     }
     if (nnew > 0) {
          f << nnew << " new stock(s):";
          for (p = newnam.begin(); p != newnam.end(); p++)
               if (p->second)
                    f << ' ' << p->first;
          f << '\n';
     }

     // Restore d1.buf to original state before appending.
     for (size_t i = 0; i < d.bufsize; i++)
          if (!d.buf[i])
               d.buf[i] = ' ';
     char* t = d.buf + 32;
     for (i = 0; i < d.nrec; i++, t += reclen)
          *t = '\n';

     // Append new data to the main data set.
     std::ofstream g(fname_.c_str(), std::ios::app);
     if (!g)
          throw Stock_error("could not open output file");
     g.write(d.buf, d.bufsize);
     if (g.bad())
          throw Stock_error("error writing output file");
}

void Rawprices::fill(int& nassets, int& nobs, vector<string>& name,
                     vector<Date>& date, Matdouble& price) const {
     const char *s, *t;
     int i;

     date.push_back(Date(buf));
     for (i = 1, s = buf + reclen, t = buf; i < nrec; i++, s += reclen)
          if (strcmp(s, t)) {
               date.push_back(Date(s));
               t = s;
          }
     std::set<string> names;
     for (i = 0, s = buf; i < nrec; i++, s += reclen)
          names.insert(s + 9);
     std::set<string>::const_iterator p;
     for (p = names.begin(); p != names.end(); p++)
          name.push_back(*p);
     nassets = name.size();
     nobs = date.size();
     price.assign(nassets, nobs, -1.00);
     int j = 0, k = -1, l;
     double q;
     for (i = 0, s = buf, t = buf; i < nrec; i++, s += reclen) {
          if (strcmp(s, t)) {
               t = s;
               j++;
               k = -1;
               if (static_cast<size_t>(j) >= date.size())
                    throw Invalid_input_file();
          }
          while (strcmp(s + 9, name[++k].c_str())) {/* VOID */}
          l = 21;
          while (!s[++l]) {/* VOID */}
          q = atof(s + l);
          if (q > 1e-5)
               price[k][j] = q;
     }
     if (static_cast<size_t>(j) + 1 != date.size())
          throw Invalid_input_file();
}

Rawprices::Report::Report()
     : first_date("99991231"), last_date("15830101"), nobs(0) {}

/*
 * A valid price must be of the form "-1.00" or d{d}.dd, where d is a
 * decimal digit. At least one digit must be different from 0.
 */
bool Rawprices::isvalidprice(const char* s) {
     if (*s == '-')
          return *++s == '1' && *++s == '.' &&
               *++s == '0' && *++s == '0' && !*++s;
     if (!isdigit(*s))
          return false;
     bool nonzero = *s != '0';
     while (isdigit(*++s))
          if (*s != '0')
               nonzero = true;
     if (*s != '.')
          return false;
     if (!isdigit(*++s))
          return false;
     if (*s != '0')
          nonzero = true;
     if (!isdigit(*++s))
          return false;
     if (*s != '0')
          nonzero = true;
     return !*++s && nonzero;
}

/* The record in file of splits is as follows:
 *
 *  0         1         2             29
 *  01234567890123456789012345678
 * "19930614 MOSTALEXP      1   2" + '\n'
 * "dddddddd aaaaaaaaaaaa ddd ddd"
 */
Rawsplits::Rawsplits(const char* fname)
     : Rawdata(fname), nrec(bufsize / reclen) {
     char *s;
     const char* t;
     int i, j;

     if (static_cast<size_t>(nrec) * static_cast<size_t>(reclen)
         != bufsize)
          throw Stock_error("invalid number of records");

     for (i = 0, s = buf; i < nrec; i++, s += reclen)
          if (*(s + 8) != ' ' || *(s + 21) != ' ' || *(s + 25) != ' '
              || *(s + 29) != '\n')
               throw Invalid_input_file();
          else
               *(s + 29) = ' ';

     buf[bufsize - 1] = 0;
     for (s = buf; *s; s++)
          if (isspace(*s)) {
               if (*s != ' ')
                    throw Invalid_input_file();
               *s = 0;
          }

     t = buf;                   // Dummy initialization for compiler.
     for (i = 0, s = buf; i < nrec; i++, s += reclen) {
          if (!*(s + 9))
               throw Invalid_input_file();
          for (j = 22; j < 25 && !s[j]; j++) ;
          if (j >= 25)
               throw Stock_error("missing numerator in splits");
          if (!isvalidint(s + j))
               throw Stock_error("invalid numerator in splits");

          for (j = 26; j < 29 && !s[j]; j++) ;
          if (j >= 29)
               throw Stock_error("missing denominator in splits");
          if (!isvalidint(s + j))
               throw Stock_error("invalid denominator in splits");

          if (i) {
               if (strcmp(s, t) && !isvaliddate(s))
                    throw Invalid_input_file();
               if (memcmp(s, t, 21) <= 0)
                    throw Invalid_input_file();
          } else {
               if (!isvaliddate(s))
                    throw Invalid_input_file();
          }
          t = s;
     }
}

void Rawsplits::adjust(const vector<string>& name,
                       const vector<Date>& date,
                       Matdouble& price) const {
     // Splits are sorted according to date.
     for (int nr = 0; nr < nrec; nr++) {
          const char* s = buf + nr * reclen, *t = s + 21;
          while (!*++t) {/* VOID */}
          const int num = atoi(t);
          if (num <= 0)
               throw Invalid_input_file();
          t = s + 25;
          while (!*++t) {/* VOID */}
          const int denom = atoi(t);
          if (denom <= 0)
               throw Invalid_input_file();
          vector<string>::size_type si;
          string q = s + 9;
          if (!vbsearch(q, name, si))
               throw Stock_error("asset from splits not found in data");
          const int i = si;
          vector<Date>::size_type sj;
          const Date d(s);
          if (!vbsearch(d, date, sj))
               throw Stock_error(
                    "first quotation after split not found in data");
          const int j1 = sj;
          const double rate = static_cast<double>(num) /
               static_cast<double>(denom);
          for (int j = 0; j < j1; j++)
               if (price[i][j] > 0.0)
                    price[i][j] *= rate;
     }
}

// A valid int must be of the form d, dd or ddd, the first digit may not
// be 0.
bool Rawsplits::isvalidint(const char* s) {
     size_t i, len = strlen(s);

     if (len < 1 || len > 3)
          return false;
     for (i = 0; i < len; i++)
          if (!isdigit(s[i]))
               return false;
     return *s != '0';
}

}  // anonymous namespace

Data::Data(const char* datafn, const char* splitfn)
     : nassets_(), nobs_(), name_(), date_(), price_() {
     Rawprices(datafn).fill(nassets_, nobs_, name_, date_, price_);
     if (splitfn)
          Rawsplits(splitfn).adjust(name_, date_, price_);
}

Data::~Data() {}

void Data::missing(const char* fn, int n) {
     if (n <= 0)
          throw Stock_error("length of sequence must be positive");
     std::ofstream f(fn);
     if (!f)
          throw Stock_error("could not open output file");
     const int nc = price_.ncols();
     int lp = 0;
     for (int i = 0; i < nassets_; i++) {
          int j, p;

          // Set p to the position of the first positive price.
          for (p = 0; p < nc && price_[i][p] < 0.0; p++) ;
          if (p == nc)
               throw Stock_error("a stock found with no observations");
          // Set q to the first position after the last positive price.
          j = nc;
          while (price_[i][--j] < 0.0) {/* VOID */}
          const int q = j + 1;
          bool positive = true;

          while (++p < q)
               if (positive) {
                    if (price_[i][p] < 0.0) {
                         j = p;
                         positive = false;
                    }
               } else {
                    if (price_[i][p] >= 0.0) {
                         positive = true;
                         if (p - j >= n) {
                              f << setw(4) << ++lp << ' '
                                << setw(12) << left << name_[i]
                                << right << ' '
                                << date_[j].common() << '-'
                                << date_[p - 1].common() << ' '
                                << setw(11) << p - j << '\n';
                         }
                    }
               }
     }

     // The second part.
     f << "\nPart II\n\n";
     lp = 0;
     const double M = 0.2;
     for (int i = 0; i < nassets_; i++) {
          int j = 0;
          double z = -1.0;

          for (; j < nc; j++)
               if (price_[i][j] > 0.0) {
                    z = price_[i][j];
                    break;
               }
          if (z < 0.0)
               continue;
          j++;
          for (; j < nc; j++)
               if (price_[i][j] > 0.0) {
                    const double v = price_[i][j];
                    const double r = (v - z) / z;
                    z = v;
                    if (abs(r) > M)
                         f << setw(4) << ++lp << ' '
                           << setw(12) << left << name_[i]
                           << right << ' '
                           << date_[j].common() << ' '
                           << r << '\n';
               }
     }

     if (f.bad())
          throw Stock_error("error writing output file");
}

GPW_data::GPW_data(const char* datafn, const char* splitfn)
     : Data(datafn) {
     static const Date dd(1, 1, 1995);  // Date of denomination.

     for (int i = 0; i < nassets_; i++)
          for (int j = 0; date_[j] < dd; j++)
               if (price_[i][j] > -0.5)
                    price_[i][j] /= 10000.0;

     Rawsplits(splitfn).adjust(name_, date_, price_);
}

void msample(const Data& d, Sample_data& sd) {
     const int nassets_ = d.nassets();
     const vector<Date>& date_ = d.date();
     const Matdouble& price_ = d.price();

     /*
      * Find the smallest jd0 and the greatest jd1 such that
      *
      * sd.d - (sd.frequency + sd.reserve) <= date_[jd0] < date_[jd1]
      * < sd.d.
      *
      * The period from date_[jd0] to date_[jd1] will be a true sample
      * period.
      */
     vector<Date>::size_type jd0, jd1, jd, jdf, j, jtf;
     int i, nobs;
     Date d0, d1, tf;

     sd.nassets = 0;
     if (!vbsearch(sd.d, date_, jd) ||
         !vbsearch(sd.d + sd.frequency, date_, jdf))
          return;
     if (jd <= 1)
          return;
     jd0 = jd1 = jd - 1;

     d0 = sd.d - (sd.frequency + sd.reserve);
     while (jd0-- > 0 && date_[jd0] > d0) {/* VOID */}
     if (date_[jd0] < d0)
          jd0++;
     SHG_ASSERT(d0 <= date_[jd0] &&
                date_[jd0] < date_[jd1] &&
                date_[jd1] < sd.d);

     d0 = date_[jd0];
     d1 = date_[jd1];

     // Collect assets quoted on each session from jd0 to jd1, on the
     // day jd and on the day jdf.
     sd.a.clear();
     for (i = 0; i < nassets_; i++) {
          for (j = jd0; j <= jd1; j++)
               if (price_[i][j] <= 0.0)
                    break;
          if (j > jd1 && price_[i][jd] > 0.0 && price_[i][jdf] > 0.0)
               sd.a.push_back(i);
     }
     if ((sd.nassets = sd.a.size()) == 0)
          return;

     // Calculate the number of observations.
     nobs = 0;
     for (j = jd0; j <= jd1; j++) {
          tf = date_[j] + sd.frequency;
          if (tf > d1)
               break;
          if (vbsearch(tf, date_, jtf)) {
               SHG_ASSERT(jtf <= jd1);
               nobs++;
          }
     }

     if (nobs < 1) {
          sd.nassets = 0;
          return;
     }

     sd.nobs = nobs;
     sd.r.resize(sd.nassets, sd.nobs);

     nobs = 0;
     for (j = jd0; j <= jd1; j++) {
          tf = date_[j] + sd.frequency;
          if (tf > d1)
               break;
          if (vbsearch(tf, date_, jtf)) {
               SHG_ASSERT(jtf <= jd1);
               for (i = 0; i < sd.nassets; i++) {
                    const double pt = price_[sd.a[i]][j];
                    const double ptf = price_[sd.a[i]][jtf];
                    SHG_ASSERT(pt > 0.0 && ptf > 0.0);
                    sd.r[i][nobs] = (ptf - pt) / pt;
               }
               nobs++;
          }
     }
     SHG_ASSERT(sd.nobs == nobs);
}

void check(const char* datafn) {
     Rawprices p(datafn);
}

void report(const char* datafn) {
     Rawprices(datafn).report();
}

void add_data(const char* datafn, const char *newdatafn) {
     Rawprices d(newdatafn);
     Rawprices(datafn).add(d);
}

Select::Select()
     : nseries(), last_date(), max_missing(), replace_exact_0(true),
       rng(0), dfn(), sfn(), ofn(), lfn(), v() {}

void Select::select() {
     SHG_ASSERT(nseries > 0 && max_missing >= 0);
     GPW_data d(dfn.c_str(), sfn.c_str());
     const int q = last_date_ind(d.date());

     v.clear();

     // Select all series with not more than max_missing missing
     // observations. Perform interpolation on missing observations,
     // calculate rates of return and update v.
     for (int i = 0; i < d.nassets(); i++) {
          if (d.price()[i][q] <= 0.0)
               continue;
          Series s(d.name()[i]);
          for (int j = 0; j <= q; j++)
               if (d.price()[i][j] > 0.0) {
                    s.first = j;
                    break;
               }
          for (int j = s.first; j <= q; j++)
               if (d.price()[i][j] > 0.0)
                    s.nobs++;
          s.nmissing = q - s.first + 1 - s.nobs;
          if (s.nmissing > max_missing)
               continue;
          s.r.resize(s.nobs + s.nmissing);
          for (int j = s.first, k = 0; j <= q; j++, k++)
               s.r[k] = d.price()[i][j];
          interpolate(s.r);
          for (size_t k = s.r.size() - 1; k > 0; k--)
               s.r[k] = s.r[k] / s.r[k - 1] - 1.0;
          s.r.erase(s.r.begin());
          v.push_back(s);
     }
     // Are there enough series?
     SHG_ASSERT(v.size() >= static_cast<size_t>(nseries));

     // Limit to nseries series with the maximum number of
     // observations.
     sort(v.begin(), v.end(), numobs);
     v.erase(v.begin() + nseries, v.end());
     sort(v.begin(), v.end(), alpha);

     // Replace exact zeroes if required.
     if (replace_exact_0) {
          SHG_ASSERT(rng != 0);
          for (size_t i = 0; i < v.size(); i++)
               v[i].nreplaced = replace_zeros(v[i].r);
     }

     // Output data.
     std::ofstream f(ofn.c_str());
     SHG_ASSERT(bool(f));
     f << std::fixed << std::setprecision(7);
     for (size_t i = 0; i < v.size(); i++) {
          const Series& s = v[i];
          f << setw(12) << left << s.name << right << '\n';
          for (size_t j = 0; j < s.r.size(); j++)
               f << d.date()[s.first + j + 1] << ' '
                 << setw(12) << s.r[j] << '\n';
          f << '\n';
     }
     SHG_ASSERT(!f.bad());
     f.close();

     f.open(lfn.c_str());
     SHG_ASSERT(bool(f));
     f << std::fixed << std::setprecision(1);
     const string ld = d.date()[q].common();
     f << " 1.       2.          3.         4.       5.    6. "
          "   7.    8.\n"
          "---- ------------ ---------- ---------- ----- -----"
          " ----- -----\n";
     for (size_t i = 0; i < v.size(); i++) {
          const Series& s = v[i];
          f << setw(3) << i + 1 << ". "
            << setw(12) << left << s.name << right << ' '
            << d.date()[s.first].common() << ' '
            << ld << ' '
            << setw(5) << s.r.size() << ' '
            << setw(5) << s.nmissing << ' '
            << setw(5) << s.nreplaced << ' '
            << setw(5) << (100.0 * static_cast<double>(s.nreplaced)) /
               static_cast<double>(s.r.size()) << '\n';
     }
     SHG_ASSERT(!f.bad());
}

Select::Series::Series(const string& name)
     : name(name), first(-1), nobs(0), nmissing(0), nreplaced(0), r() {
}

int Select::last_date_ind(const std::vector<Date>& d) const {
     size_t i = d.size();
     while (i-- > 0)
          if (d[i] <= last_date)
               return i;
     throw Stock_error("last date not found");
}

void Select::interpolate(std::vector<double>& v) {
     for (size_t i = 0; i < v.size(); i++)
          if (v[i] <= 0.0) {
               size_t j = i;
               // the stock is guaranteed to be quoted on the last day:
               while (v[++j] <= 0.0) {/* VOID */}
               const double d = (v[j] - v[i - 1]) / (j - i + 1);
               for (; i < j; i++)
                    v[i] = v[i - 1] + d;
          }
}

bool Select::numobs(const Series& d1, const Series& d2) {
     return d1.nobs > d2.nobs;
}

bool Select::alpha(const Series& d1, const Series& d2) {
     return d1.name < d2.name;
}

int Select::replace_zeros(vector<double>& x) {
     int nreplaced = 0;
     double minpos = std::numeric_limits<double>::max(),
          maxneg = -minpos;
     for (size_t i = 0; i < x.size(); i++)
          if (x[i] > 0.0) {
               if (x[i] < minpos)
                    minpos = x[i];
          } else if (x[i] < 0.0) {
               if (x[i] > maxneg)
                    maxneg = x[i];
          }
     maxneg = -maxneg;
     const double s = (maxneg < minpos ? maxneg : minpos) / 3.0;
     for (size_t i = 0; i < x.size(); i++)
          if (x[i] == 0.0) {
               x[i] = rng->normal() * s;
               nreplaced++;
          }
     return nreplaced;
}

Closing_price::Closing_price() : date(), price() {}

int sp500(const char* fname, vector<Closing_price>& data, int& line) {
     line = 0;
     std::ifstream f(fname);
     if (!f)
          return 1;
     string s;
     Date prev;
     Closing_price cp;
     vector<string> flds;
     data.clear();
     if (!std::getline(f, s))
          return 2;
     ++line;

     while (std::getline(f, s)) {
          ++line;
          flds = SHG::split_string(s, ",");
          if (flds.size() != 7)
               return 3;
          string& dt = flds[0];
          if (dt.length() < 10)
               return 3;
          dt.erase(4, 1);
          dt.erase(6, 1);
          try {cp.date = Date(dt);}
          catch (SHG::Date::Bad_date) {return 3;}
          {
               const Date::Day dow = cp.date.dayofweek();
               int d, m, y;
               cp.date.split(d, m, y);
               const Date easter = Date::easter(y);
               if (dow == Date::sun || dow == Date::sat ||
                   (d == 1 && m == Date::jan) ||
                   (d == 4 && m == Date::jul) ||
                   (d == 25 && m == Date::dec) ||
                   (cp.date == easter) ||
                   (cp.date == easter - 2))
                    return 3;
          }
          if (line > 2 && cp.date >= prev)
               return 3;
          prev = cp.date;
          cp.price = atof(flds[4].c_str());
          if (cp.price < 0.01)
               return 3;
          data.insert(data.begin(), cp);
     }
     if (f.bad() || !f.eof())
          return 2;
     return 0;
}

int wig(const char* fname, vector<Closing_price>& data, int& line) {
     line = 0;
     std::ifstream f(fname);
     if (!f)
          return 1;
     string s;
     Date prev;
     Closing_price cp;
     vector<string> flds;
     data.clear();
     if (!std::getline(f, s))
          return 2;
     ++line;

     while (std::getline(f, s)) {
          ++line;
          flds = SHG::split_string(s, ",");
          // Normally, there are 6 fields, but if the field number 5
          // is missing, the volume is 0 and is not given.
          if (flds.size() < 5)
               return 3;
          string& dt = flds[0];
          if (dt.length() < 10)
               return 3;
          dt.erase(4, 1);
          dt.erase(6, 1);
          try {cp.date = Date(dt);}
          catch (SHG::Date::Bad_date) {return 3;}
          {
               const Date::Day dow = cp.date.dayofweek();
               int d, m, y;
               cp.date.split(d, m, y);
               const Date easter = Date::easter(y);
               if (dow == Date::sun || dow == Date::sat ||
                   (d == 1 && m == Date::jan) ||
                   (d == 25 && m == Date::dec) ||
                   (d == 26 && m == Date::dec) ||
                   (cp.date == easter) ||
                   (cp.date == easter + 1))
                    return 3;
          }
          if (line > 2 && cp.date <= prev)
               return 3;
          prev = cp.date;
          cp.price = atof(flds[4].c_str());
          if (cp.price < 0.01)
               return 3;
          data.insert(data.begin(), cp);
     }
     if (f.bad() || !f.eof())
          return 2;

     return 0;
}

}       // namespace Stock
}       // namespace SHG
