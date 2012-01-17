// Andrew Naplavkov

#ifndef BRIG_QT_DETAIL_DRAW_POINT_HPP
#define BRIG_QT_DETAIL_DRAW_POINT_HPP

#include <brig/detail/ogc.hpp>
#include <brig/qt/frame.hpp>
#include <cstdint>
#include <QPainter>
#include <QPointF>

namespace brig { namespace qt { namespace detail {

using namespace brig::detail::ogc;

template <typename InputIterator>
QPointF get_point(uint8_t byte_order, InputIterator& itr, const frame& fr)
{
  QPointF point;
  point.setX(get<double>(byte_order, itr));
  point.setY(get<double>(byte_order, itr));
  return fr.proj_to_pixel(point);
}

template <typename InputIterator>
void draw_point(uint8_t byte_order, InputIterator& itr, const frame& fr, QPainter& painter)
{
  const QPointF point( get_point(byte_order, itr, fr) );
  painter.drawRect(point.x(), point.y(), 3, 3);
}

} } } // brig::qt::detail

#endif // BRIG_QT_DETAIL_DRAW_POINT_HPP
