/* stock.h: processing stock data */

/**
 * \file include/shg/stock.h
 * Processing stock data.
 * \date Created on 11 September 2009.
 */

#ifndef SHG_STOCK_H
#define SHG_STOCK_H

#include <string>
#include <vector>
#include "shg/date.h"
#include "shg/except.h"
#include "shg/matrix.h"
#include "shg/rng.h"
#include "shg/utils.h"

namespace SHG {

/**
 * Processing stock data.
 */
namespace Stock {

/**
 * \defgroup stock Processing stock data
 *
 * Processing stock data.
 *
 * \{
 */

struct Stock_error : public virtual SHG::Exception {
     explicit Stock_error(const std::string& what = std::string());
};

struct Invalid_input_file : public Stock_error {
     Invalid_input_file();
};

/**
 * Class Data provides an array of prices of assets. Layout of data is
 * as follows:
 *
 * <pre>
 * |-----------------------------------------------------------|
 * |                       j |        0        1 ... nobs_ - 1 |
 * |                date_[j] | 19910416 19910417 ...  20090414 |
 * |      i         name_[i] |                                 |
 * |-------------------------+---------------------------------|
 * |      0       | 01NFI    |                                 |
 * |      1       | 04PRO    |                                 |
 * |     ...      | ...      |       price_[i][j]              |
 * | nassets_ - 1 | ZYWIEC   |                                 |
 * |-----------------------------------------------------------|
 * </pre>
 *
 * Data::nassets_ is the number of stocks, Data::nobs_ is the number
 * of trading days in the data set. The vectors Data::name_ and
 * Data::date_ are sorted: name_[0] < ... < name_[nassets_ - 1],
 * date_[0] < ... < date_[nobs_ - 1]. The matrix Data::price_ is a
 * matrix of Data::nassets_ rows and Data::nobs_ columns of double.
 * Data::price_[i][j] is a price of the stock Data::name_[i] quoted at
 * Data::date_[j]. For missing values Data::price_[i][j] == -1.
 *
 * \implementation Justification of the method. Within the period from
 * 16 April 1991 to 15 October 2010, 521 stocks were quoted. Each
 * quotation requires 33 bytes. On a 32-bit system there should be no
 * difficulty to allocate 2^31 - 1 = 2147483647 bytes. If the number
 * of stocks were four times greater (2084) and the file were
 * collected 100 years 260 trading days a year, the file would contain
 * 2084 x 260 x 100 x 33 = 1788072000 bytes, that is 83% of 2^31 - 1.
 */
class Data {
public:
     /**
      * Constructs the set.
      *
      * \param[in] datafn path to the file which contains data on
      * prices
      *
      * \param[in] splitfn path to the file which contains data on
      * splits
      */
     Data(const char* datafn, const char* splitfn = 0);
     virtual ~Data();
     /**
      * Returns the number of assets in the set.
      */
     int nassets() const {return nassets_;}
     /**
      * Returns the number of trading days in the set.
      */
     int nobs() const {return nobs_;}
     /**
      * Returns the vector with names of assets
      */
     const std::vector<std::string>& name() const {return name_;}
     /**
      * Returns the vector with dates of trading days.
      */
     const std::vector<SHG::Date>& date() const {return date_;}
     /**
      * Returns the matrix with prices. price()(i, j) is a price of
      * the stock name()[i] quoted at date date()[j]. If price()(i, j)
      * is equal to -1, the observation is missing.
      */
     const SHG::Matdouble& price() const {return price_;}

     /**
      * Writes to the file fn missing observations that form sequences
      * at least n > 0 trading days long.
      */
     void missing(const char* fn, int n);

protected:
     int nassets_;                      ///< number of stocks
     int nobs_;                         ///< number of trading days
     std::vector<std::string> name_;    ///< names of stocks
     std::vector<SHG::Date> date_;      ///< dates of trading days
     SHG::Matdouble price_;             ///< prices
};

/**
 * The class provides data on the Warsaw Stock Exchange.
 */
class GPW_data : public Data {
public:
     GPW_data(const char* datafn, const char* splitfn);
};

/**
 * Struct for passing data to and from msample().
 */
struct Sample_data {
     /** \name Input. */
     /** \{ */
     SHG::Date d;       ///< when investment starts
     int frequency;     ///< investment period in days
     int reserve;       ///< see comment for msample()
     /** \} */
     /** \name Output. */
     /** \{ */
     int nassets;       ///< number of assets in sample
     int nobs;          ///< number of observations in sample
     std::vector<int>
     a;                 ///< indexes in name() of selected assets
     SHG::Matdouble r;  ///< matrix of rates of return
     /** \} */
};

/**
 * Gets sample of historical observations on rates of return for
 * experiment consisting in buying assets on day d and selling them on
 * day d + frequency. We examine the period
 *
 * (d - frequency - reserve) (d - frequency - reserve + 1) ... (d - 1)
 *
 * Selected assets must be quoted on day d and on day d + frequency.
 *
 * The return is valid if nassets >= 1. Then nobs >= 1, a contains,
 * indexes of assets from name() and r contains sample rates of
 * return, r[i][j], 0 <= j < nobs, being observations on rates of
 * return on assets named name()[a[i]], i <= 0 < nassets. If on exit
 * nassets == 0, no sample has been found, and values of nobs, a and r
 * are indefinite.
 *
 * The variable reserve is a reserve of days. The greater reserve, the
 * more observations will be used to calculate moments. For example,
 * if reserve = 90 and frequency = 1, we calculate rates of return
 * using data from three months just before start of investment.
 */
void msample(const Data& d, Sample_data& sd);

/**
 * Checks if the data file is correct
 */
void check(const char* datafn);

/**
 * Prints a report on data file.
 */
void report(const char* datafn);

/**
 * Adds new data to data file.
 */
void add_data(const char* datafn, const char *newdatafn);

/**
 * This class selects nseries series of rates of return. The last data
 * on prices may be from last_date. First, the assets which have more
 * than max_missing missing observations are removed. Second, the
 * remaining assets are sorted by the number of observations and the
 * first nseries is selected with the greatest number of observations.
 * These assets are than sorted alphabetically. The missing
 * observations on prices are linearly interpolated, then rates of
 * return are calculated. If replace_exact_0 is true, all rates of
 * return that are equal to 0.0 are replaced by random normal numbers
 * with mean 0.0 and small variance using rng generator (if
 * replace_exact_0 is false, rng is not used and may be 0). Then data
 * is output to the file ofn. A summary is printed to the file lfn.
 * The summary contains:
 *
 * 1. Number.
 * 2. Name of the stock.
 * 3. %Date of the first observed rate of return.
 * 4. %Date of the last observed rate of return.
 * 5. The number of observations in the series of rates of return.
 * 6. The number of observations missing in the series of prices. The
 * series of prices has one observation more than the series of rates
 * of return. These observations were linearly interpolated.
 * 7. The number of rates of return which were exactly equal to zero
 * and were replaced by random normal numbers.
 * 8. Percentage of replaced observations.
 */
struct Select {
     Select();
     Select(const Select&) = delete;
     Select& operator=(const Select&) = delete;

     int nseries;               ///< how many series to pick
     Date last_date;            ///< date of the last observation
     int max_missing;           ///< max. number missing observations
     bool replace_exact_0;      ///< if true, replace exact 0 with rnd
     RNG* rng;                  ///< RNG to use when replacing zeros
     std::string dfn;           ///< path to data file
     std::string sfn;           ///< path to file of splits
     std::string ofn;           ///< path to output file
     std::string lfn;           ///< path to summary file
     void select();             ///< do selection

private:
     struct Series {
          explicit Series(const std::string& name);
          std::string name;     ///< name
          int first;            ///< first quotation
          int nobs;             ///< number of observations
          int nmissing;         ///< number of missing observations
          int nreplaced;        ///< number of replaced zeros
          std::vector<double>
          r;                    ///< rates of return
     };

     std::vector<Series> v;     ///< working vector of series

     /**
      * Returns the index of last_date in d.
      */
     int last_date_ind(const std::vector<Date>& d) const;

     /**
      * Linear interpolation on vector of prices.
      */
     static void interpolate(std::vector<double>& v);

     /**
      * For sorting assets according to the number of observations.
      */
     static bool numobs(const Series& d1, const Series& d2);

     /**
      * For sorting assets alphabetically.
      */
     static bool alpha(const Series& d1, const Series& d2);

     /**
      * Returns the number of replaced zeros.
      */
     int replace_zeros(std::vector<double>& x);
};

/**
 * Struct used to return closing prices from sp500() and wig().
 */
struct Closing_price {
     Closing_price();
     SHG::Date date;            ///< date
     double price;              ///< price
};

/**
 * Reads the file with historical prices of S&P 500 index. The file
 * may come from
 * http://finance.yahoo.com/q/hp?s=%5EGSPC+Historical+Prices.
 *
 * \param [in] fname path to the file with prices
 * \param [out] data vector with closing prices
 * \param [out] line number of lines read
 *
 * \retval 0 ok
 * \retval 1 cannot open data file
 * \retval 2 error reading file
 * \retval 3 error in data file
 */
int sp500(const char* fname, std::vector<Closing_price>& data,
          int& line);

/**
 * Reads the file with historical prices of WIG index. The file
 * may come from http://stooq.pl/q/d/?s=wig.
 *
 * \param [in] fname path to the file with prices
 * \param [out] data vector with closing prices
 * \param [out] line number of lines read
 *
 * \retval 0 ok
 * \retval 1 cannot open data file
 * \retval 2 error reading file
 * \retval 3 error in data file
 */
int wig(const char* fname, std::vector<Closing_price>& data, int& line);

/** \} */ /* end of group stock */

}       // namespace Stock
}       // namespace SHG

#endif
