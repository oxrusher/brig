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

using namespace brig::detail::ogc;

template <typename InputIterator>
void draw_polygon(uint8_t byte_order, InputIterator& itr, const frame& fr, QPainter& painter)
{
  QPainterPath path;
  for (uint32_t i(0), count(get<uint32_t>(byte_order, itr)); i < count; ++i)
    path.addPolygon( get_line(byte_order, itr, fr) );
  painter.drawPath(path);
}

} } } // brig::qt::detail

#endif // BRIG_QT_DETAIL_DRAW_POLYGON_HPP
