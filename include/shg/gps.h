/**
 * \file include/shg/gps.h
 * Measurements using GPS.
 */

#ifndef SHG_GPS_H
#define SHG_GPS_H

#ifdef HAVE_PUGIXML
#include <istream>
#include <string>
#include <filesystem>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <pugixml.hpp>
#endif

/**
 * GPS.
 */
namespace SHG::GPS {

/**
 * \defgroup gps GPS
 *
 * Classes and functions related to measurements using %GPS.
 *
 * Documentation for this group is in \ref additional_documentation,
 * section %GPS.
 *
 * See also [Elevation for Your
 * Activity](https://support.strava.com/hc/en-us/articles/216919447-Elevation-for-Your-Activity)
 * and [How Distance is
 * Calculated](https://support.strava.com/hc/en-us/articles/216919487-How-Distance-is-Calculated).
 *
 * \{
 */

constexpr double semi_major_axis = 6378137.0;  // in meters
constexpr double inverse_of_flattening = 298.257223563;
constexpr double semi_minor_axis =
     semi_major_axis - semi_major_axis / inverse_of_flattening;

struct Geogr_coord {
     double phi;     // lattitude in degrees
     double lambda;  // longitude in degrees
     double
          h;  // distance from the surface of the ellipsoid in meters
};

struct Cart_coord {
     double x;
     double y;
     double z;
};

/**
 * Converts geographical coordinates to Cartesian coordinates.
 */
void convert(const Geogr_coord& g, Cart_coord& p);

/**
 * Returns distance between two points by a straight line.
 */
double distance(const Cart_coord& p, const Cart_coord& q);

#ifdef HAVE_PUGIXML

class GPX_data {
public:
     /**
      * Current state of the object:
      */
     enum class State {
          initialized,  ///< the method parse() has not been called
          error,        ///< the method parse() failed
          ok            ///< the method parse() has been successful
     };

     /** Current state of the object. */
     State state() const { return state_; }

     /** Get data from GPX stream. */
     void parse(std::istream& f);
     /** Get data from named GPX file. */
     void parse(char const* fname);
     /** Distance in meters. */
     double distance() const;
     /** Distance in meters on ellipsoid. */
     double distance_on_ellipsoid() const;
     /** Distance vertically uphill in  meters. */
     double uphill() const;
     /** Distance vertically downhill in  meters. */
     double downhill() const;
     /** Start time as "yyyy-mm-ddThh:mm:ss". */
     std::string start_time() const;
     /** End time as "yyyy-mm-ddThh:mm:ss". */
     std::string end_time() const;
     /** Elapsed time as string. */
     std::string elapsed_time() const;
     /** Elapsed time in seconds. */
     boost::int64_t elapsed_seconds() const;
     /** Average speed in meters per second. */
     double speedms() const;
     /** Average speed in kilometers per hour. */
     double speedkmh() const;

private:
     void parse(pugi::xml_document const& doc);
     void check_state() const;

     State state_{State::initialized};
     double distance_{};
     double distance_on_ellipsoid_{};
     double uphill_{};
     double downhill_{};
     boost::posix_time::ptime start_time_{};
     boost::posix_time::ptime end_time_{};
};

class Activity_statistics {
public:
     struct Result {
          std::string fname{};
          std::string status{};
          double distance{0.0};
          double distance_on_ellipsoid{0.0};
          double uphill{0.0};
          double downhill{0.0};
          std::string start_time{};
          std::string end_time{};
          std::string elapsed_time{};
          boost::int64_t elapsed_seconds{0};
          double speedms{0.0};
          double speedkmh{0.0};
     };

     /** Collect data from files in directory named path. */
     void run(std::filesystem::path const& dir);
     /** Get vector of results. */
     std::vector<Result> const& results() const { return results_; }
     /** Clear results. */
     void init() { results_.clear(); }

private:
     std::vector<Result> results_{};
};

#endif

/** \} */ /* end of group gps */

}  // namespace SHG::GPS

#endif
