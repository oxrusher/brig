// Andrew Naplavkov

#ifndef BRIG_PROJ_DETAIL_TRANSFORM_GEOMETRY_HPP
#define BRIG_PROJ_DETAIL_TRANSFORM_GEOMETRY_HPP

#include <brig/detail/ogc.hpp>
#include <brig/proj/detail/lib.hpp>
#include <brig/proj/detail/transform_line.hpp>
#include <brig/proj/detail/transform_point.hpp>
#include <brig/proj/detail/transform_polygon.hpp>
#include <cstdint>
#include <stdexcept>

namespace brig { namespace proj { namespace detail {

template <typename InputIterator, typename OutputIterator>
void transform_geometry(InputIterator& in_itr, OutputIterator& out_itr, projPJ in_pj, projPJ out_pj)
{
  using namespace brig::detail::ogc;
  uint8_t byte_order(read_byte_order(in_itr)); write_byte_order(out_itr);
  uint32_t type(read<uint32_t>(byte_order, in_itr)); write<uint32_t>(out_itr, type);
  uint32_t i(0), count(0);
  switch (type)
  {
  default: throw std::runtime_error("WKB error");
  case Point: transform_point(byte_order, in_itr, out_itr, in_pj, out_pj); break;
  case LineString: transform_line(byte_order, in_itr, out_itr, in_pj, out_pj); break;
  case Polygon: transform_polygon(byte_order, in_itr, out_itr, in_pj, out_pj); break;

  case MultiPoint:
    count = read<uint32_t>(byte_order, in_itr); write<uint32_t>(out_itr, count);
    for (i = 0; i < count; ++i)
    {
      byte_order = read_byte_order(in_itr); write_byte_order(out_itr);
      type = read<uint32_t>(byte_order, in_itr); write<uint32_t>(out_itr, type);
      if (Point != type) throw std::runtime_error("WKB error");
      transform_point(byte_order, in_itr, out_itr, in_pj, out_pj);
    }
    break;

  case MultiLineString:
    count = read<uint32_t>(byte_order, in_itr); write<uint32_t>(out_itr, count);
    for (i = 0; i < count; ++i)
    {
      byte_order = read_byte_order(in_itr); write_byte_order(out_itr);
      type = read<uint32_t>(byte_order, in_itr); write<uint32_t>(out_itr, type);
      if (LineString != type) throw std::runtime_error("WKB error");
      transform_line(byte_order, in_itr, out_itr, in_pj, out_pj);
    }
    break;

  case MultiPolygon:
    count = read<uint32_t>(byte_order, in_itr); write<uint32_t>(out_itr, count);
    for (i = 0; i < count; ++i)
    {
      byte_order = read_byte_order(in_itr); write_byte_order(out_itr);
      type = read<uint32_t>(byte_order, in_itr); write<uint32_t>(out_itr, type);
      if (Polygon != type) throw std::runtime_error("WKB error");
      transform_polygon(byte_order, in_itr, out_itr, in_pj, out_pj);
    }
    break;

  case GeometryCollection:
    count = read<uint32_t>(byte_order, in_itr); write<uint32_t>(out_itr, count);
    for (i = 0; i < count; ++i)
      transform_geometry(in_itr, out_itr, in_pj, out_pj);
    break;
  }
}

} } } // brig::proj::detail

#endif // BRIG_PROJ_DETAIL_TRANSFORM_GEOMETRY_HPP
