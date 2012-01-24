// Andrew Naplavkov

#ifndef BRIG_QT_DRAW_HPP
#define BRIG_QT_DRAW_HPP

#include <brig/qt/detail/draw_geom.hpp>
#include <brig/qt/frame.hpp>
#include <QPainter>
#include <vector>

namespace brig { namespace qt {

template <typename T>
void draw(const std::vector<T>& wkb, const frame& fr, QPainter& painter)
{
  const T* ptr(wkb.data());
  detail::draw_geom(ptr, fr, painter);
}

} } // brig::qt

#endif // BRIG_QT_DRAW_HPP
