// Andrew Naplavkov

#ifndef BRIG_QT_DRAW_WKB_HPP
#define BRIG_QT_DRAW_WKB_HPP

#include <brig/qt/detail/draw_geom.hpp>
#include <brig/qt/frame.hpp>
#include <QPainter>

namespace brig { namespace qt {

template <typename InputIterator>
void draw_wkb(InputIterator& itr, const frame& fr, QPainter& painter)
{
  detail::draw_geom(itr, fr, painter);
}

} } // brig::qt

#endif // BRIG_QT_DRAW_WKB_HPP
