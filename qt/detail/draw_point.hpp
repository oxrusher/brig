// Andrew Naplavkov

#ifndef BRIG_QT_DETAIL_DRAW_POINT_HPP
#define BRIG_QT_DETAIL_DRAW_POINT_HPP

#include <brig/detail/ogc.hpp>
#include <brig/qt/frame.hpp>
#include <cstdint>
#include <QPainter>
#include <QPointF>

namespace brig { namespace qt { namespace detail {

template <typename InputIterator>
QPointF read_point(uint8_t byte_order, InputIterator& iter, const frame& fr)
{
  using namespace brig::detail::ogc;
  QPointF point;
  point.setX(read<double>(byte_order, iter));
  point.setY(read<double>(byte_order, iter));
  return fr.proj_to_pixel(point);
}

template <typename InputIterator>
void draw_point(uint8_t byte_order, InputIterator& iter, const frame& fr, QPainter& painter)
{
  const QPointF point( read_point(byte_order, iter, fr) );
  painter.drawRect(point.x(), point.y(), 3, 3);
}

} } } // brig::qt::detail

#endif // BRIG_QT_DETAIL_DRAW_POINT_HPP
