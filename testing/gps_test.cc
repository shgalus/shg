#include <shg/gps.h>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <ios>
#include <sstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include "testing.h"
#include "gpsdata.h"

namespace SHG::Testing {

BOOST_AUTO_TEST_SUITE(gps_test)

namespace bdata = boost::unit_test::data;
namespace but = boost::unit_test;

using namespace SHG::GPS;

BOOST_AUTO_TEST_CASE(basic_test) {
     Geogr_coord gc;
     Cart_coord cc;

     gc.phi = 0.0;
     gc.lambda = 0.0;
     gc.h = 0.0;
     convert(gc, cc);
     BOOST_CHECK(facmp(cc.x, semi_major_axis, 1e-15) == 0);
     BOOST_CHECK(facmp(cc.y, 0.0, 1e-15) == 0);
     BOOST_CHECK(facmp(cc.z, 0.0, 1e-15) == 0);
     gc.phi = 90.0;
     convert(gc, cc);
     BOOST_CHECK(facmp(cc.x, 0.0, 4e-10) == 0);
     BOOST_CHECK(facmp(cc.y, 0.0, 1e-15) == 0);
     BOOST_CHECK(facmp(cc.z, semi_minor_axis, 1e-15) == 0);
     gc.phi = -90.0;
     convert(gc, cc);
     BOOST_CHECK(facmp(cc.x, 0.0, 4e-10) == 0);
     BOOST_CHECK(facmp(cc.y, 0.0, 1e-15) == 0);
     BOOST_CHECK(facmp(cc.z, -semi_minor_axis, 1e-15) == 0);
}

BOOST_DATA_TEST_CASE(latitude_circle_test, bdata::xrange(19), xr) {
     Geogr_coord gc;
     Cart_coord cc;

     const int n = xr - 9;
     gc.phi = 10.0 * n;
     gc.lambda = 0.0;
     gc.h = 0.0;
     convert(gc, cc);
     const double z = cc.z;

     for (int i = -18; i <= 18; i++) {
          gc.lambda = 10.0 * i;
          convert(gc, cc);
          BOOST_CHECK(facmp(cc.z, z, min_dbl) == 0);
     }
}

BOOST_AUTO_TEST_CASE(distance_basic_test) {
     Geogr_coord gc;
     Cart_coord p, q;
     double d;

     gc.phi = 0;
     gc.lambda = 0;
     gc.h = 0;
     convert(gc, p);
     gc.lambda = 180.0;
     convert(gc, q);
     d = distance(p, q);
     BOOST_CHECK(facmp(d, 2.0 * semi_major_axis, min_dbl) == 0);

     gc.phi = 90.0;
     convert(gc, p);
     gc.phi = -90.0;
     convert(gc, q);
     d = distance(p, q);
     BOOST_CHECK(facmp(d, 2.0 * semi_minor_axis, min_dbl) == 0);
}

constexpr char const* const gpx1 = R"(
<?xml version="1.0" encoding="UTF-8"?>
<gpx creator="StravaGPX Android" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd" version="1.1" xmlns="http://www.topografix.com/GPX/1/1">
 <metadata>
  <time>2021-06-11T16:01:53Z</time>
 </metadata>
 <trk>
  <name>Evening Ride</name>
  <type>1</type>
  <trkseg>
   <trkpt lat="54.3809460" lon="18.6134350">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:53Z</time>
   </trkpt>
   <trkpt lat="54.3809430" lon="18.6134320">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:55Z</time>
   </trkpt>
   <trkpt lat="54.3809320" lon="18.6133820">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:56Z</time>
   </trkpt>
   <trkpt lat="54.3809130" lon="18.6132500">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:57Z</time>
   </trkpt>
   <trkpt lat="54.3808990" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:58Z</time>
   </trkpt>
)";

constexpr char const* const gpx2 = R"(
   <trkpt lat="54.3809080" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)";

constexpr char const* const gpx3 = R"(
   <trkpt lat="54.3809240" lon="18.6131980">
    <ele>13.0</ele>
    <time>2021-06-11T16:02:00Z</time>
   </trkpt>
   <trkpt lat="54.3809360" lon="18.6132410">
    <ele>13.1</ele>
    <time>2021-06-11T16:02:01Z</time>
   </trkpt>
   <trkpt lat="54.3809470" lon="18.6133380">
    <ele>13.1</ele>
    <time>2021-06-11T16:02:02Z</time>
   </trkpt>
   <trkpt lat="54.3809510" lon="18.6134050">
    <ele>13.0</ele>
    <time>2021-06-11T16:02:03Z</time>
   </trkpt>
  </trkseg>
 </trk>
</gpx>
)";

BOOST_AUTO_TEST_CASE(gpx_positive_test) {
     std::string gpx{gpx1};
     gpx += gpx2;
     gpx += gpx3;
     GPX_data d;
     BOOST_CHECK(d.state() == GPX_data::State::initialized);
     std::istringstream iss(gpx);
     BOOST_CHECK_NO_THROW(d.parse(iss));
     BOOST_CHECK(d.state() == GPX_data::State::ok);
     BOOST_CHECK(facmp(d.distance(), 37.27, 0.01) == 0);
     BOOST_CHECK(facmp(d.distance_on_ellipsoid(), 37.27, 0.01) == 0);
     BOOST_CHECK(facmp(d.uphill(), 0.1, 0.01) == 0);
     BOOST_CHECK(facmp(d.downhill(), 0.1, 0.01) == 0);
     BOOST_CHECK(d.start_time() == "2021-06-11T16:01:53");
     BOOST_CHECK(d.end_time() == "2021-06-11T16:02:03");
     BOOST_CHECK(d.elapsed_time() == "00:00:10");
     BOOST_CHECK(d.elapsed_seconds() == 10);
     BOOST_CHECK(facmp(d.speedms(), 3.73, 0.01) == 0);
     BOOST_CHECK(facmp(d.speedkmh(), 13.42, 0.01) == 0);
}

void exec_negative_test(char const* s) {
     GPX_data d;
     BOOST_CHECK(d.state() == GPX_data::State::initialized);
     std::istringstream iss(s);
     BOOST_CHECK_THROW(d.parse(iss), std::runtime_error);
     BOOST_CHECK(d.state() == GPX_data::State::error);
     BOOST_CHECK_THROW(d.distance(), std::logic_error);
     BOOST_CHECK_THROW(d.distance_on_ellipsoid(), std::logic_error);
     BOOST_CHECK_THROW(d.uphill(), std::logic_error);
     BOOST_CHECK_THROW(d.downhill(), std::logic_error);
     BOOST_CHECK_THROW(d.start_time(), std::logic_error);
     BOOST_CHECK_THROW(d.end_time(), std::logic_error);
     BOOST_CHECK_THROW(d.elapsed_time(), std::logic_error);
     BOOST_CHECK_THROW(d.elapsed_seconds(), std::logic_error);
     BOOST_CHECK_THROW(d.speedms(), std::logic_error);
     BOOST_CHECK_THROW(d.speedkmh(), std::logic_error);
}

constexpr char const* const two_track_segments = R"(
<?xml version="1.0" encoding="UTF-8"?>
<gpx creator="StravaGPX Android" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd" version="1.1" xmlns="http://www.topografix.com/GPX/1/1">
 <metadata>
  <time>2021-06-11T16:01:53Z</time>
 </metadata>
 <trk>
  <name>Evening Ride</name>
  <type>1</type>
  <trkseg>
   <trkpt lat="54.3809460" lon="18.6134350">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:53Z</time>
   </trkpt>
   <trkpt lat="54.3809430" lon="18.6134320">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:55Z</time>
   </trkpt>
   <trkpt lat="54.3809320" lon="18.6133820">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:56Z</time>
   </trkpt>
   <trkpt lat="54.3809130" lon="18.6132500">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:57Z</time>
   </trkpt>
   <trkpt lat="54.3808990" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:58Z</time>
   </trkpt>
   <trkpt lat="54.3809080" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
   <trkpt lat="54.3809240" lon="18.6131980">
    <ele>13.0</ele>
    <time>2021-06-11T16:02:00Z</time>
   </trkpt>
  </trkseg>
  <trkseg>
   <trkpt lat="54.3809360" lon="18.6132410">
    <ele>13.1</ele>
    <time>2021-06-11T16:02:01Z</time>
   </trkpt>
   <trkpt lat="54.3809470" lon="18.6133380">
    <ele>13.1</ele>
    <time>2021-06-11T16:02:02Z</time>
   </trkpt>
   <trkpt lat="54.3809510" lon="18.6134050">
    <ele>13.0</ele>
    <time>2021-06-11T16:02:03Z</time>
   </trkpt>
  </trkseg>
 </trk>
</gpx>
)";

BOOST_AUTO_TEST_CASE(gpx_two_track_segments_test) {
     exec_negative_test(two_track_segments);
}

constexpr char const* const no_trkpt = R"(
<?xml version="1.0" encoding="UTF-8"?>
<gpx creator="StravaGPX Android" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd" version="1.1" xmlns="http://www.topografix.com/GPX/1/1">
 <metadata>
  <time>2021-06-11T16:01:53Z</time>
 </metadata>
 <trk>
  <name>Evening Ride</name>
  <type>1</type>
  <trkseg>
  </trkseg>
 </trk>
</gpx>
)";

BOOST_AUTO_TEST_CASE(gpx_no_trkpt_test) {
     exec_negative_test(no_trkpt);
}

constexpr char const* const one_trkpt = R"(
<?xml version="1.0" encoding="UTF-8"?>
<gpx creator="StravaGPX Android" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd" version="1.1" xmlns="http://www.topografix.com/GPX/1/1">
 <metadata>
  <time>2021-06-11T16:01:53Z</time>
 </metadata>
 <trk>
  <name>Evening Ride</name>
  <type>1</type>
  <trkseg>
   <trkpt lat="54.3809460" lon="18.6134350">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:53Z</time>
   </trkpt>
  </trkseg>
 </trk>
</gpx>
)";

BOOST_AUTO_TEST_CASE(gpx_one_trkpt_test) {
     exec_negative_test(one_trkpt);
}

constexpr char const* const zero_elapsed_time = R"(
<?xml version="1.0" encoding="UTF-8"?>
<gpx creator="StravaGPX Android" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd" version="1.1" xmlns="http://www.topografix.com/GPX/1/1">
 <metadata>
  <time>2021-06-11T16:01:53Z</time>
 </metadata>
 <trk>
  <name>Evening Ride</name>
  <type>1</type>
  <trkseg>
   <trkpt lat="54.3809460" lon="18.6134350">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:53Z</time>
   </trkpt>
   <trkpt lat="54.3809460" lon="18.6134350">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:53Z</time>
   </trkpt>
  </trkseg>
 </trk>
</gpx>
)";

BOOST_AUTO_TEST_CASE(gpx_zero_elapsed_time_test) {
     exec_negative_test(zero_elapsed_time);
}

// Invalid <trkpt> ... </trkpt> nodes to replace gpx2.
constexpr char const* const trkpt[] = {
     // missing timestamp
     R"(
   <trkpt lat="54.3809080" lon="18.6131550">
    <ele>13.0</ele>
    <time></time>
   </trkpt>
)",
     // missing timestamp
     R"(
   <trkpt lat="54.3809080" lon="18.6131550">
    <ele>13.0</ele>
   </trkpt>
)",
     // invalid timestamp
     R"(
   <trkpt lat="54.3809080" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-02-29T16:01:59Z</time>
   </trkpt>
)",
     // invalid timestamp
     R"(
   <trkpt lat="54.3809080" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-0611T16:01:59Z</time>
   </trkpt>
)",
     // invalid timestamp
     R"(
   <trkpt lat="54.3809080" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-13-11T16:01:59Z</time>
   </trkpt>
)",
     // timestamp mismatch (earlier than previous timestamp)
     R"(
   <trkpt lat="54.3809080" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:57Z</time>
   </trkpt>
)",
     // missing latitude
     R"(
   <trkpt lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // missing latitude
     R"(
   <trkpt lat="" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // invalid latitude
     R"(
   <trkpt lat="54.380908a" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // latitude out of range
     R"(
   <trkpt lat="90.1" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // latitude out of range
     R"(
   <trkpt lat="-90.1" lon="18.6131550">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // missing longitude
     R"(
   <trkpt lat="54.3809080">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // missing longitude
     R"(
   <trkpt lat="54.3809080" lon="">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // invalid longitude
     R"(
   <trkpt lat="54.3809080" lon="INF">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // longitude out of range
     R"(
   <trkpt lat="54.3809080" lon="-180.1">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // longitude out of range
     R"(
   <trkpt lat="54.3809080" lon="180.0">
    <ele>13.0</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // missing elevation
     R"(
   <trkpt lat="54.3809080" lon="18.6131550">
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // missing elevation
     R"(
   <trkpt lat="54.3809080" lon="18.6131550">
    <ele></ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",
     // invalid elevation
     R"(
   <trkpt lat="54.3809080" lon="18.6131550">
    <ele>NAN</ele>
    <time>2021-06-11T16:01:59Z</time>
   </trkpt>
)",

};

BOOST_DATA_TEST_CASE(gpx_negative_test,
                     bdata::xrange(std::size(trkpt)), xr) {
     std::string gpx{gpx1};
     gpx += trkpt[xr];
     gpx += gpx3;
     exec_negative_test(gpx.c_str());
}

struct AS_data {
     unsigned char const* s;
     unsigned int const size;
     char const* fname;
     bool should_be_ungzipped;
};

AS_data const as_data[] = {
     {SHG::Testing::GPX::gpx_gz_5474850155,
      SHG::Testing::GPX::gpx_gz_5474850155_len, "5474850155.gpx.gz",
      true},
     {SHG::Testing::GPX::gpx_gz_5480723783,
      SHG::Testing::GPX::gpx_gz_5480723783_len, "5480723783.gpx.gz",
      true},
     {SHG::Testing::GPX::gpx_gz_5733570778,
      SHG::Testing::GPX::gpx_gz_5733570778_len, "5733570778.gpx.gz",
      false},
     {SHG::Testing::GPX::gpx_gz_afternoon_ride,
      SHG::Testing::GPX::gpx_gz_afternoon_ride_len,
      "Afternoon_Ride.gpx.gz", true},
};

// Prepare files for testing in subdirectory of temporary directory.
void setup() {
     namespace fs = std::filesystem;
     using std::ios_base;
     bool ok = false;
     fs::path p;

     try {
          p = fs::temp_directory_path();
          p /= "shg";
          fs::remove_all(p);
          ok = fs::create_directory(p);
     } catch (std::exception const&) {
     }

     BOOST_REQUIRE(ok);

     std::ofstream f;
     fs::path q;

     for (std::size_t i = 0; i < std::size(as_data); i++) {
          AS_data const& a = as_data[i];
          char const* sp = reinterpret_cast<char const*>(a.s);
          std::istringstream iss(std::string(sp, a.size),
                                 ios_base::in | ios_base::binary);
          boost::iostreams::filtering_streambuf<
               boost::iostreams::input>
               inbuf;
          q = p / a.fname;
          if (a.should_be_ungzipped) {
               inbuf.push(boost::iostreams::gzip_decompressor());
               q.replace_extension("");
          }
          inbuf.push(iss);
          std::istream instream(&inbuf);
          f.open(q, ios_base::out | ios_base::binary);
          BOOST_REQUIRE(f);
          // << behaves as an unformatted output function.
          f << instream.rdbuf();
          BOOST_REQUIRE(f);
          f.close();
     }
}

// Remove prepared subdirectory of temporary directory.
void teardown() {
     namespace fs = std::filesystem;
     try {
          fs::path const p = fs::temp_directory_path() / "shg";
          fs::remove_all(p);
     } catch (std::exception const&) {
          BOOST_TEST_MESSAGE("Could not remove temporary directory.");
     }
}

const Activity_statistics::Result res[] = {
     {"Afternoon_Ride.gpx", "ok", 22496.047, 22493.700, 106.100,
      106.000, "2021-05-24T14:38:55", "2021-05-24T15:53:05",
      "01:14:10", 4450, 5.055, 18.199},
     {"5474850155.gpx", "ok", 11205.585, 11199.917, 79.300, 79.300,
      "2021-06-15T15:26:12", "2021-06-15T17:00:08", "01:33:56", 5636,
      1.988, 7.158},
     {"5480723783.gpx", "ok", 2908.656, 2906.439, 37.800, 12.800,
      "2021-06-16T16:40:35", "2021-06-16T17:12:23", "00:31:48", 1908,
      1.524, 5.488},
     {"5733570778.gpx.gz", "ok", 26718.040, 22493.281, 4778.281,
      4777.886, "2021-05-24T14:38:55", "2021-05-24T15:53:05",
      "01:14:10", 4450, 6.004, 21.615}};

BOOST_AUTO_TEST_CASE(activity_statistics_test,
                     *but::fixture(&setup, &teardown)) {
     namespace fs = std::filesystem;
     bool used[std::size(res)];

     for (auto& u : used)
          u = false;

     fs::path const p = fs::temp_directory_path() / "shg";
     Activity_statistics as;
     BOOST_REQUIRE(as.results().size() == 0);
     BOOST_REQUIRE_NO_THROW(as.run(p));
     BOOST_REQUIRE(as.results().size() == std::size(res));

     Activity_statistics::Result const* r;
     Activity_statistics::Result const* t;

     for (std::size_t i = 0; i < std::size(res); i++) {
          r = &as.results()[i];
          t = nullptr;
          for (std::size_t j = 0; j < std::size(res); j++)
               if (res[j].fname == r->fname) {
                    BOOST_REQUIRE(!used[j]);
                    used[j] = true;
                    t = &(res[j]);
               }
          BOOST_REQUIRE(t != nullptr);
          BOOST_CHECK(r->status == t->status);
          BOOST_CHECK(facmp(r->distance, t->distance, 0.01) == 0);
          BOOST_CHECK(facmp(r->distance_on_ellipsoid,
                            t->distance_on_ellipsoid, 0.01) == 0);
          BOOST_CHECK(facmp(r->uphill, t->uphill, 0.01) == 0);
          BOOST_CHECK(facmp(r->downhill, t->downhill, 0.01) == 0);
          BOOST_CHECK(r->start_time == t->start_time);
          BOOST_CHECK(r->end_time == t->end_time);
          BOOST_CHECK(r->elapsed_time == t->elapsed_time);
          BOOST_CHECK(r->elapsed_seconds == t->elapsed_seconds);
          BOOST_CHECK(facmp(r->speedms, t->speedms, 0.01) == 0);
          BOOST_CHECK(facmp(r->speedkmh, t->speedkmh, 0.01) == 0);
     }
     for (auto u : used)
          BOOST_CHECK(u);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace SHG::Testing
