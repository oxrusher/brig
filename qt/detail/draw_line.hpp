// Andrew Naplavkov

#ifndef BRIG_QT_DETAIL_DRAW_LINE_HPP
#define BRIG_QT_DETAIL_DRAW_LINE_HPP

#include <brig/detail/ogc.hpp>
#include <brig/qt/detail/draw_point.hpp>
#include <brig/qt/frame.hpp>
#include <cstdint>
#include <QPainter>
#include <QPointF>
#include <QPolygonF>

namespace brig { namespace qt { namespace detail {

using namespace brig::detail::ogc;

template <typename InputIterator>
QPolygonF get_line(uint8_t byte_order, InputIterator& iter, const frame& fr)
{
  const uint32_t count(get<uint32_t>(byte_order, iter));
  QPolygonF line; line.reserve(count);
  for (uint32_t i(0); i < count; ++i)
    line.push_back( get_point(byte_order, iter, fr) );
  return line;
}

template <typename InputIterator>
void draw_line(uint8_t byte_order, InputIterator& iter, const frame& fr, QPainter& painter)
{
  painter.drawPolyline( get_line(byte_order, iter, fr) );
}

} } } // brig::qt::detail

#endif // BRIG_QT_DETAIL_DRAW_LINE_HPP
