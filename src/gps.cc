/**
 * \file src/gps.cc
 * Measurements using GPS.
 */

#include <shg/gps.h>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#ifdef HAVE_PUGIXML
#include <exception>
#include <fstream>
#include <sstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#endif
#include <shg/utils.h>
#include <shg/geometry.h>

namespace SHG::GPS {

namespace {

constexpr double eccentricity_squared =
     (sqr(semi_major_axis) - sqr(semi_minor_axis)) /
     sqr(semi_major_axis);
constexpr double one_minus_eccentricity_squared =
     1.0 - eccentricity_squared;

}  // anonymous namespace

void convert(const Geogr_coord& g, Cart_coord& p) {
     const double phi = degrees_to_radians(g.phi);
     const double lambda = degrees_to_radians(g.lambda);
     const double sin_phi = std::sin(phi);
     const double cos_phi = std::cos(phi);
     const double c =
          semi_major_axis /
          std::sqrt(1.0 - eccentricity_squared * sin_phi * sin_phi);
     const double d = c + g.h;
     p.x = d * cos_phi * std::cos(lambda);
     p.y = d * cos_phi * std::sin(lambda);
     p.z = (one_minus_eccentricity_squared * c + g.h) * sin_phi;
}

double distance(const Cart_coord& p, const Cart_coord& q) {
     return std::hypot(p.x - q.x, p.y - q.y, p.z - q.z);
}

#ifdef HAVE_PUGIXML

void GPX_data::parse(std::istream& f) {
     using std::runtime_error;

     state_ = State::error;

     pugi::xml_document doc;
     pugi::xml_parse_result const result = doc.load(f);
     if (!result)
          throw runtime_error(result.description());
     parse(doc);
}

void GPX_data::parse(char const* fname) {
     using std::runtime_error;

     state_ = State::error;

     pugi::xml_document doc;
     pugi::xml_parse_result const result = doc.load_file(fname);
     if (!result)
          throw runtime_error(result.description());
     parse(doc);
}

double GPX_data::distance() const {
     check_state();
     return distance_;
}

double GPX_data::distance_on_ellipsoid() const {
     check_state();
     return distance_on_ellipsoid_;
}

double GPX_data::uphill() const {
     check_state();
     return uphill_;
}

double GPX_data::downhill() const {
     check_state();
     return downhill_;
}

std::string GPX_data::start_time() const {
     check_state();
     return boost::posix_time::to_iso_extended_string(start_time_);
}

std::string GPX_data::end_time() const {
     check_state();
     return boost::posix_time::to_iso_extended_string(end_time_);
}

std::string GPX_data::elapsed_time() const {
     check_state();
     boost::posix_time::time_duration td = end_time_ - start_time_;
     return boost::posix_time::to_simple_string(td);
}

boost::int64_t GPX_data::elapsed_seconds() const {
     check_state();
     boost::posix_time::time_duration td = end_time_ - start_time_;
     return td.total_seconds();
}

double GPX_data::speedms() const {
     check_state();
     return distance_ / (end_time_ - start_time_).total_seconds();
}

double GPX_data::speedkmh() const {
     return speedms() * 3.6;
}

void GPX_data::parse(pugi::xml_document const& doc) {
     using std::runtime_error;
     using boost::posix_time::ptime;
     using boost::posix_time::from_iso_extended_string;

     distance_ = 0.0;
     distance_on_ellipsoid_ = 0.0;
     uphill_ = 0.0;
     downhill_ = 0.0;
     bool first = true;
     Geogr_coord gcp;   // previous
     Geogr_coord gcc;   // current
     Geogr_coord gcep;  // previous on ellipsoid
     Geogr_coord gcec;  // current on ellipsoid
     Cart_coord ccp;    // previous
     Cart_coord ccc;    // current
     Cart_coord ccep;   // previous on ellipsoid
     Cart_coord ccec;   // current on ellipsoid
     char* str_end;
     char tmp[20];
     boost::posix_time::ptime ctime;
     unsigned ntrkpts = 0;  // number of track points

     gcp = {0.0, 0.0, 0.0};  // to shut up warnings
     gcec.h = 0.0;
     pugi::xml_node ns =
          doc.child("gpx").child("trk").child("trkseg");
     for (pugi::xml_node p = ns.first_child(); p;
          p = p.next_sibling()) {
          char const* v = p.attribute("lat").value();
          if (!*v)
               throw runtime_error("missing latitude");
          gcec.phi = gcc.phi = std::strtod(v, &str_end);
          if (*str_end != '\0' || !std::isfinite(gcc.phi))
               throw runtime_error("invalid latitude");
          if (gcc.phi < -90.0 || gcc.phi > 90.0)
               throw runtime_error("latitude out of range");

          v = p.attribute("lon").value();
          if (!*v)
               throw runtime_error("missing longitude");
          gcec.lambda = gcc.lambda = std::strtod(v, &str_end);
          if (*str_end != '\0' || !std::isfinite(gcc.lambda))
               throw runtime_error("invalid longitude");
          if (gcc.lambda < -180.0 || gcc.lambda >= 180.0)
               throw runtime_error("longitude out of range");

          v = p.child_value("ele");
          if (!*v)
               throw runtime_error("missing elevation");
          gcc.h = std::strtod(v, &str_end);
          if (*str_end != '\0' || !std::isfinite(gcc.h))
               throw runtime_error("invalid elevation");

          // "2021-06-11T16:01:53Z" --> "2021-06-11T16:01:53"
          strncpy(tmp, p.child_value("time"), 19);
          tmp[19] = '\0';
          if (!*tmp)
               throw runtime_error("missing timestamp");
          try {
               ctime = ptime(from_iso_extended_string(tmp));
          } catch (std::exception const&) {
               throw runtime_error("invalid timestamp");
          }

          convert(gcc, ccc);
          convert(gcec, ccec);

          if (first) {
               start_time_ = end_time_ = ctime;
               first = false;
          } else {
               distance_ += SHG::GPS::distance(ccp, ccc);
               distance_on_ellipsoid_ +=
                    SHG::GPS::distance(ccep, ccec);
               if (gcc.h > gcp.h)
                    uphill_ += gcc.h - gcp.h;
               else if (gcc.h < gcp.h)
                    downhill_ += gcp.h - gcc.h;
               if (ctime < end_time_)
                    throw runtime_error("timestamp mismatch");
               end_time_ = ctime;
          }
          gcp = gcc;
          ccp = ccc;
          gcep = gcec;
          ccep = ccec;
          ntrkpts++;
     }
     ns = ns.next_sibling();
     if (ns)
          throw runtime_error("more then one track segment found");
     if (ntrkpts <= 1)
          throw runtime_error("not enough track points");
     if ((end_time_ - start_time_).total_seconds() < 1)
          throw runtime_error("elapsed time is zero seconds");
     state_ = State::ok;
}

void GPX_data::check_state() const {
     if (state_ != State::ok)
          throw std::logic_error(
               "successfully call the method parse() first");
}

void Activity_statistics::run(std::filesystem::path const& dir) {
     namespace fs = std::filesystem;
     namespace io = boost::iostreams;
     using std::ios_base;
     GPX_data d;

     for (auto const& p : fs::directory_iterator(dir)) {
          if (!fs::is_regular_file(p.path()))
               continue;
          io::filtering_streambuf<io::input> inbuf;

          if (p.path().extension() == ".gz" &&
              p.path().stem().extension() == ".gpx") {
               inbuf.push(io::gzip_decompressor());
          } else if (p.path().extension() != ".gpx") {
               continue;
          }
          Result r;
          r.fname = p.path().filename().generic_string();
          r.status = "failed";

          try {
               std::ifstream f(p.path(),
                               ios_base::in | ios_base::binary);
               inbuf.push(f);
               std::istream instream(&inbuf);
               // When not using buf and calling
               // d.parse(instream), on Windows an exception "no
               // random access: iostream stream error" is
               // thrown.
               std::stringstream buf(ios_base::in | ios_base::out |
                                     ios_base::binary);
               buf << instream.rdbuf();
               d.parse(buf);
               if (d.state() == GPX_data::State::ok) {
                    r.status = "ok";
                    r.distance = d.distance();
                    r.distance_on_ellipsoid =
                         d.distance_on_ellipsoid();
                    r.uphill = d.uphill();
                    r.downhill = d.downhill();
                    r.start_time = d.start_time();
                    r.end_time = d.end_time();
                    r.elapsed_time = d.elapsed_time();
                    r.elapsed_seconds = d.elapsed_seconds();
                    r.speedms = d.speedms();
                    r.speedkmh = d.speedkmh();
               }
          } catch (std::exception const& e) {
               r.status += ": ";
               r.status += e.what();
          }
          results_.push_back(r);
     }
}

#endif

}  // namespace SHG::GPS
