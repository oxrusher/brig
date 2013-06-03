// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_DECODE_GEOMETRY_HPP
#define BRIG_BOOST_DETAIL_DECODE_GEOMETRY_HPP

#include <brig/boost/detail/decode_line.hpp>
#include <brig/boost/detail/decode_point.hpp>
#include <brig/boost/detail/decode_polygon.hpp>
#include <brig/detail/ogc.hpp>
#include <cstdint>
#include <stdexcept>
#include <sstream>

namespace brig { namespace boost { namespace detail {

template <typename InputIterator>
void decode_geometry(InputIterator& in_itr, std::ostringstream& out_stream)
{
  using namespace std;
  using namespace brig::detail::ogc;

  uint8_t byte_order(read_byte_order(in_itr));
  uint32_t type(read<uint32_t>(byte_order, in_itr)), i(0), count(0);
  switch (type)
  {
  default: throw runtime_error("WKB error");
  case Point:
    out_stream << "POINT";
    decode_point(byte_order, in_itr, out_stream);
    break;

  case LineString:
    out_stream << "LINESTRING";
    decode_line(byte_order, in_itr, out_stream);
    break;

  case Polygon:
    out_stream << "POLYGON";
    decode_polygon(byte_order, in_itr, out_stream);
    break;

  case MultiPoint:
    out_stream << "MULTIPOINT(";
    for (i = 0, count = read<uint32_t>(byte_order, in_itr); i < count; ++i)
    {
      if (i > 0) out_stream << ", ";
      byte_order = read_byte_order(in_itr);
      type = read<uint32_t>(byte_order, in_itr);
      if (Point != type) throw runtime_error("WKB error");
      decode_point(byte_order, in_itr, out_stream);
    }
    out_stream << ")";
    break;

  case MultiLineString:
    out_stream << "MULTILINESTRING(";
    for (i = 0, count = read<uint32_t>(byte_order, in_itr); i < count; ++i)
    {
      if (i > 0) out_stream << ", ";
      byte_order = read_byte_order(in_itr);
      type = read<uint32_t>(byte_order, in_itr);
      if (LineString != type) throw runtime_error("WKB error");
      decode_line(byte_order, in_itr, out_stream);
    }
    out_stream << ")";
    break;

  case MultiPolygon:
    out_stream << "MULTIPOLYGON(";
    for (i = 0, count = read<uint32_t>(byte_order, in_itr); i < count; ++i)
    {
      if (i > 0) out_stream << ", ";
      byte_order = read_byte_order(in_itr);
      type = read<uint32_t>(byte_order, in_itr);
      if (Polygon != type) throw runtime_error("WKB error");
      decode_polygon(byte_order, in_itr, out_stream);
    }
    out_stream << ")";
    break;

  case GeometryCollection:
    out_stream << "GEOMETRYCOLLECTION(";
    for (i = 0, count = read<uint32_t>(byte_order, in_itr); i < count; ++i)
    {
      if (i > 0) out_stream << ", ";
      decode_geometry(in_itr, out_stream);
    }
    out_stream << ")";
    break;
  }
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_DECODE_GEOMETRY_HPP
