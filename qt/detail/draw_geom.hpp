// Andrew Naplavkov

#ifndef BRIG_QT_DETAIL_DRAW_GEOM_HPP
#define BRIG_QT_DETAIL_DRAW_GEOM_HPP

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
void draw_geom(InputIterator& iter, const frame& fr, QPainter& painter)
{
  using namespace brig::detail::ogc;
  uint8_t byte_order(get_byte_order(iter)), i(0), count(0);
  switch (get<uint32_t>(byte_order, iter))
  {
  default: throw std::runtime_error("WKB error");
  case Point: draw_point(byte_order, iter, fr, painter); break;
  case LineString: draw_line(byte_order, iter, fr, painter); break;
  case Polygon: draw_polygon(byte_order, iter, fr, painter); break;

  case MultiPoint:
    for (i = 0, count = get<uint32_t>(byte_order, iter); i < count; ++i)
    {
      byte_order = get_byte_order(iter);
      if (Point != get<uint32_t>(byte_order, iter)) throw std::runtime_error("WKB error");
      draw_point(byte_order, iter, fr, painter);
    }
    break;

  case MultiLineString:
    for (i = 0, count = get<uint32_t>(byte_order, iter); i < count; ++i)
    {
      byte_order = get_byte_order(iter);
      if (LineString != get<uint32_t>(byte_order, iter)) throw std::runtime_error("WKB error");
      draw_line(byte_order, iter, fr, painter);
    }
    break;

  case MultiPolygon:
    for (i = 0, count = get<uint32_t>(byte_order, iter); i < count; ++i)
    {
      byte_order = get_byte_order(iter);
      if (Polygon != get<uint32_t>(byte_order, iter)) throw std::runtime_error("WKB error");
      draw_polygon(byte_order, iter, fr, painter);
    }
    break;

  case GeometryCollection:
    for (i = 0, count = get<uint32_t>(byte_order, iter); i < count; ++i)
      draw_geom(iter, fr, painter);
    break;
  }
}

} } } // brig::qt::detail

#endif // BRIG_QT_DETAIL_DRAW_GEOM_HPP
