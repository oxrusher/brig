// Andrew Naplavkov

#ifndef BRIG_QT_DETAIL_DRAW_POLYGON_HPP
#define BRIG_QT_DETAIL_DRAW_POLYGON_HPP

#include <brig/detail/ogc.hpp>
#include <brig/qt/detail/draw_line.hpp>
#include <brig/qt/frame.hpp>
#include <cstdint>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>

namespace brig { namespace qt { namespace detail {

template <typename InputIterator>
void draw_polygon(uint8_t byte_order, InputIterator& iter, const frame& fr, QPainter& painter)
{
  using namespace brig::detail::ogc;
  QPainterPath path;
  for (uint32_t i(0), count(get<uint32_t>(byte_order, iter)); i < count; ++i)
    path.addPolygon( get_line(byte_order, iter, fr) );
  painter.drawPath(path);
}

} } } // brig::qt::detail

#endif // BRIG_QT_DETAIL_DRAW_POLYGON_HPP
