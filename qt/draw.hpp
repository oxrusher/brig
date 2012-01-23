// Andrew Naplavkov

#ifndef BRIG_QT_DRAW_HPP
#define BRIG_QT_DRAW_HPP

#include <brig/qt/detail/draw_geom.hpp>
#include <brig/qt/frame.hpp>
#include <QPainter>

namespace brig { namespace qt {

template <typename InputIterator>
void draw(InputIterator& wkb, const frame& fr, QPainter& painter)
{
  detail::draw_geom(wkb, fr, painter);
}

} } // brig::qt

#endif // BRIG_QT_DRAW_HPP
