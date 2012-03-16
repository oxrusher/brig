// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_READ_GEOMETRY_HPP
#define BRIG_BOOST_DETAIL_READ_GEOMETRY_HPP

#include <brig/detail/ogc.hpp>
#include <brig/boost/detail/read_line.hpp>
#include <brig/boost/detail/read_point.hpp>
#include <brig/boost/detail/read_polygon.hpp>
#include <brig/boost/geometry.hpp>
#include <cstdint>
#include <stdexcept>

namespace brig { namespace boost { namespace detail {

template <typename InputIterator>
void read_geometry(InputIterator& iter, geometry& geom)
{
  using namespace ::boost;
  using namespace brig::detail::ogc;
  uint8_t byte_order(read_byte_order(iter));
  switch (read<uint32_t>(byte_order, iter))
  {
  default: throw std::runtime_error("WKB error");
  case Point:
    {
    geom = point();
    point& pt = get<point>(geom);
    read_point(byte_order, iter, pt);
    }
    break;

  case LineString:
    {
    geom = linestring();
    linestring& line = get<linestring>(geom);
    read_line(byte_order, iter, line);
    }
    break;

  case Polygon:
    {
    geom = polygon();
    polygon& poly = get<polygon>(geom);
    read_polygon(byte_order, iter, poly);
    }
    break;

  case MultiPoint:
    {
    geom = multi_point();
    multi_point& mpt = get<multi_point>(geom);
    const uint32_t count(read<uint32_t>(byte_order, iter));
    mpt.resize(count);
    for (uint32_t i = 0; i < count; ++i)
    {
      byte_order = read_byte_order(iter);
      if (Point != read<uint32_t>(byte_order, iter)) throw std::runtime_error("WKB error");
      read_point(byte_order, iter, mpt[i]);
    }
    }
    break;

  case MultiLineString:
    {
    geom = multi_linestring();
    multi_linestring& mline = get<multi_linestring>(geom);
    const uint32_t count(read<uint32_t>(byte_order, iter));
    mline.resize(count);
    for (uint32_t i = 0; i < count; ++i)
    {
      byte_order = read_byte_order(iter);
      if (LineString != read<uint32_t>(byte_order, iter)) throw std::runtime_error("WKB error");
      read_line(byte_order, iter, mline[i]);
    }
    }
    break;

  case MultiPolygon:
    {
    geom = multi_polygon();
    multi_polygon& mpoly = get<multi_polygon>(geom);
    const uint32_t count(read<uint32_t>(byte_order, iter));
    mpoly.resize(count);
    for (uint32_t i = 0; i < count; ++i)
    {
      byte_order = read_byte_order(iter);
      if (Polygon != read<uint32_t>(byte_order, iter)) throw std::runtime_error("WKB error");
      read_polygon(byte_order, iter, mpoly[i]);
    }
    }
    break;

  case GeometryCollection:
    {
    geom = geometry_collection();
    geometry_collection& coll = get<geometry_collection>(geom);
    const uint32_t count(read<uint32_t>(byte_order, iter));
    coll.resize(count);
    for (uint32_t i = 0; i < count; ++i)
      read_geometry(iter, coll[i]);
    }
    break;
  }
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_READ_GEOMETRY_HPP
