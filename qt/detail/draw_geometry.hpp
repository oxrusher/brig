// Andrew Naplavkov

#ifndef BRIG_QT_DETAIL_DRAW_GEOMETRY_HPP
#define BRIG_QT_DETAIL_DRAW_GEOMETRY_HPP

#include <brig/detail/ogc.hpp>
#include <brig/qt/detail/draw_line.hpp>
#include <brig/qt/detail/draw_point.hpp>
#include <brig/qt/detail/draw_polygon.hpp>
#include <brig/qt/frame.hpp>
#include <cstdint>
#include <QPainter>
#include <stdexcept>

namespace brig { namespace qt { namespace detail {

template <typename InputIterator>
void draw_geometry(InputIterator& itr, const frame& fr, QPainter& painter)
{
  using namespace brig::detail::ogc;
  uint8_t byte_order(read_byte_order(itr));
  switch (read<uint32_t>(byte_order, itr))
  {
  default: throw std::runtime_error("WKB error");
  case Point: draw_point(byte_order, itr, fr, painter); break;
  case LineString: draw_line(byte_order, itr, fr, painter); break;
  case Polygon: draw_polygon(byte_order, itr, fr, painter); break;

  case MultiPoint:
    for (uint32_t i = 0, count = read<uint32_t>(byte_order, itr); i < count; ++i)
    {
      byte_order = read_byte_order(itr);
      if (Point != read<uint32_t>(byte_order, itr)) throw std::runtime_error("WKB error");
      draw_point(byte_order, itr, fr, painter);
    }
    break;

  case MultiLineString:
    for (uint32_t i = 0, count = read<uint32_t>(byte_order, itr); i < count; ++i)
    {
      byte_order = read_byte_order(itr);
      if (LineString != read<uint32_t>(byte_order, itr)) throw std::runtime_error("WKB error");
      draw_line(byte_order, itr, fr, painter);
    }
    break;

  case MultiPolygon:
    for (uint32_t i = 0, count = read<uint32_t>(byte_order, itr); i < count; ++i)
    {
      byte_order = read_byte_order(itr);
      if (Polygon != read<uint32_t>(byte_order, itr)) throw std::runtime_error("WKB error");
      draw_polygon(byte_order, itr, fr, painter);
    }
    break;

  case GeometryCollection:
    for (uint32_t i = 0, count = read<uint32_t>(byte_order, itr); i < count; ++i)
      draw_geometry(itr, fr, painter);
    break;
  }
}

} } } // brig::qt::detail

#endif // BRIG_QT_DETAIL_DRAW_GEOMETRY_HPP
