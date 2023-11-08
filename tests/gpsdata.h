#ifndef GPSDATA_H
#define GPSDATA_H

/**
 * Data for testing in gps_test. Tables are created by the command xxd
 * -i.
 */
namespace TESTS::GPX {

/**
 * The file 5474850155.gpx.gz. Source: the file
 * export_2486169.zip, exported from
 * [Strava](https://www.strava.com/) on 22 VI 2021. The original
 * file in the archive is not gzipped. Ride from 15 VI 2021 in
 * Gdansk.
 */
extern unsigned char const gpx_gz_5474850155[];
extern unsigned int const gpx_gz_5474850155_len;

/**
 * The file 5480723783.gpx.gz. Source: the file export_2486169.zip,
 * exported from [Strava](https://www.strava.com/) on 22 VI 2021. The
 * original file in the archive is not gzipped. Walk from 16 VI 2021
 * in Gdansk.
 */
extern unsigned char const gpx_gz_5480723783[];
extern unsigned int const gpx_gz_5480723783_len;

/**
 * The file 5733570778.gpx.gz. Source: the file export_2486169.zip,
 * exported from [Strava](https://www.strava.com/) on 22 VI 2021. The
 * original file in the archive is gzipped. Ride from 24 V 2021, Hel -
 * Jurata - Hel, recorded by Caynax Sports Tracker.
 */
extern unsigned char const gpx_gz_5733570778[];
extern unsigned int const gpx_gz_5733570778_len;

/**
 * The file Afternoon_Ride.gpx.gz. Source: downloaded from
 * [Strava](https://www.strava.com/) on 23 VI 2021. The original file
 * was not gzipped. Ride from 24 V 2021, Hel - Jurata - Hel.
 *
 * The original file gpx_gz_5733570778 was uploaded to Strava and then
 * downloaded again. There are differences in elevation data between
 * gpx_gz_5733570778 and gpx_gz_afternoon_ride.
 */
extern unsigned char const gpx_gz_afternoon_ride[];
extern unsigned int const gpx_gz_afternoon_ride_len;

}  // namespace TESTS::GPX

#endif
