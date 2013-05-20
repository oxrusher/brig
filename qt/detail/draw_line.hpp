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

template <typename InputIterator>
QPolygonF read_line(uint8_t byte_order, InputIterator& itr, const frame& fr)
{
  const uint32_t count(brig::detail::ogc::read<uint32_t>(byte_order, itr));
  QPolygonF line; line.reserve(count);
  for (uint32_t i(0); i < count; ++i)
    line.push_back( read_point(byte_order, itr, fr) );
  return line;
}

template <typename InputIterator>
void draw_line(uint8_t byte_order, InputIterator& itr, const frame& fr, QPainter& painter)
{
  painter.drawPolyline( read_line(byte_order, itr, fr) );
}

} } } // brig::qt::detail

#endif // BRIG_QT_DETAIL_DRAW_LINE_HPP
