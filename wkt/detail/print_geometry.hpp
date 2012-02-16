// Andrew Naplavkov

#ifndef BRIG_WKT_DETAIL_PRINT_GEOMETRY_HPP
#define BRIG_WKT_DETAIL_PRINT_GEOMETRY_HPP

#include <brig/detail/ogc.hpp>
#include <brig/wkt/detail/print_line.hpp>
#include <brig/wkt/detail/print_point.hpp>
#include <brig/wkt/detail/print_polygon.hpp>
#include <cstdint>
#include <stdexcept>
#include <sstream>

namespace brig { namespace wkt { namespace detail {

template <typename InputIterator>
void print_geometry(InputIterator& in_iter, std::ostringstream& out_stream)
{
  using namespace brig::detail::ogc;
  uint8_t byte_order(read_byte_order(in_iter));
  uint32_t type(read<uint32_t>(byte_order, in_iter)), i(0), count(0);
  switch (type)
  {
  default: throw std::runtime_error("WKB error");
  case Point:
    out_stream << "POINT ";
    print_point(byte_order, in_iter, out_stream);
    break;

  case LineString:
    out_stream << "LINESTRING ";
    print_line(byte_order, in_iter, out_stream);
    break;

  case Polygon:
    out_stream << "POLYGON ";
    print_polygon(byte_order, in_iter, out_stream);
    break;

  case MultiPoint:
    out_stream << "MULTIPOINT (";
    for (i = 0, count = read<uint32_t>(byte_order, in_iter); i < count; ++i)
    {
      if (i > 0) out_stream << ", ";
      byte_order = read_byte_order(in_iter);
      type = read<uint32_t>(byte_order, in_iter);
      if (Point != type) throw std::runtime_error("WKB error");
      print_point(byte_order, in_iter, out_stream);
    }
    out_stream << ')';
    break;

  case MultiLineString:
    out_stream << "MULTILINESTRING (";
    for (i = 0, count = read<uint32_t>(byte_order, in_iter); i < count; ++i)
    {
      if (i > 0) out_stream << ", ";
      byte_order = read_byte_order(in_iter);
      type = read<uint32_t>(byte_order, in_iter);
      if (LineString != type) throw std::runtime_error("WKB error");
      print_line(byte_order, in_iter, out_stream);
    }
    out_stream << ')';
    break;

  case MultiPolygon:
    out_stream << "MULTIPOLYGON (";
    for (i = 0, count = read<uint32_t>(byte_order, in_iter); i < count; ++i)
    {
      if (i > 0) out_stream << ", ";
      byte_order = read_byte_order(in_iter);
      type = read<uint32_t>(byte_order, in_iter);
      if (Polygon != type) throw std::runtime_error("WKB error");
      print_polygon(byte_order, in_iter, out_stream);
    }
    out_stream << ')';
    break;

  case GeometryCollection:
    out_stream << "GEOMETRYCOLLECTION (";
    for (i = 0, count = read<uint32_t>(byte_order, in_iter); i < count; ++i)
    {
      if (i > 0) out_stream << ", ";
      print_geometry(in_iter, out_stream);
    }
    out_stream << ')';
    break;
  }
}

} } } // brig::wkt::detail

#endif // BRIG_WKT_DETAIL_PRINT_GEOMETRY_HPP
