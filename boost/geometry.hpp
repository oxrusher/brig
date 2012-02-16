// Andrew Naplavkov

#ifndef BRIG_BOOST_GEOMETRY_HPP
#define BRIG_BOOST_GEOMETRY_HPP

#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/ring.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
#include <vector>

namespace brig { namespace boost {

typedef ::boost::geometry::model::d2::point_xy<double> point;
typedef ::boost::geometry::model::box<point> box;
typedef ::boost::geometry::model::ring<point> linearring;
typedef ::boost::geometry::model::linestring<point> linestring;
typedef ::boost::geometry::model::polygon<point> polygon;
typedef std::vector<point> multipoint;
typedef std::vector<linestring> multilinestring;
typedef std::vector<polygon> multipolygon;
struct geometrycollection;
typedef ::boost::variant<
  point,
  linestring,
  polygon,
  multipoint,
  multilinestring,
  multipolygon,
  ::boost::recursive_wrapper<geometrycollection>
> geometry;
struct geometrycollection : std::vector<geometry>  {};

} } // brig::boost

#endif // BRIG_BOOST_GEOMETRY_HPP
