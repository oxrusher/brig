// Andrew Naplavkov

// http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames

#ifndef BRIG_OSM_DETAIL_TILE_HPP
#define BRIG_OSM_DETAIL_TILE_HPP

#include <brig/boost/geometry.hpp>
#include <cmath>

namespace brig { namespace osm { namespace detail {

class tile {
  boost::point get_point() const;
public:
  int x, y, z;
  tile(int x_, int y_, int z_) : x(x_), y(y_), z(z_)  {}
  boost::box get_box() const;
  bool is_valid() const;
}; // tile

inline boost::point tile::get_point() const
{
  using namespace std;
  static const double BRIG_OSM_PI = 3.1415926535897932384;
  static const double BRIG_OSM_RMAJOR = 6378137.;
  static const double BRIG_OSM_RMINOR = 6356752.3142;
  static const double BRIG_OSM_RATIO = BRIG_OSM_RMINOR / BRIG_OSM_RMAJOR;
  static const double BRIG_OSM_ECCENT = sqrt(1. - (BRIG_OSM_RATIO * BRIG_OSM_RATIO));
  static const double BRIG_OSM_COM = 0.5 * BRIG_OSM_ECCENT;

  const double n(1 << z); // 2^z

  const double lon_deg(double(x) / n * 360. - 180.);
  const double lon_rad(lon_deg * BRIG_OSM_PI / 180.);
  const double merc_x(BRIG_OSM_RMAJOR * lon_rad);
  
  const double lat_rad(atan(sinh(BRIG_OSM_PI * (1. - 2. * double(y) / n))));
  const double sinphi(sin(lat_rad));
  double con(BRIG_OSM_ECCENT * sinphi);
  con = pow((1.0 - con) / (1.0 + con), BRIG_OSM_COM);
  const double ts(tan(0.5 * (BRIG_OSM_PI * 0.5 - lat_rad)) / con);
  const double merc_y(0 - BRIG_OSM_RMAJOR * log(ts));

  return boost::point(merc_x, merc_y);
}

inline boost::box tile::get_box() const
{
  using namespace brig::boost;
  auto a(get_point()), b(tile(x + 1, y + 1, z).get_point());
  return box(point(a.get<0>(), b.get<1>()), point(b.get<0>(), a.get<1>()));
}

inline bool tile::is_valid() const
{
  if ((x < 0) || (y < 0) || (z < 0)) return false;
  const int n(1 << z); // 2^z
  return (x < n) && (y < n);
} // tile::

} } } // brig::osm::detail

#endif // BRIG_OSM_DETAIL_TILE_HPP
