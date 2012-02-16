// Andrew Naplavkov

#ifndef BRIG_QT_DRAW_HPP
#define BRIG_QT_DRAW_HPP

#include <brig/blob_t.hpp>
#include <brig/qt/detail/draw_geometry.hpp>
#include <brig/qt/frame.hpp>
#include <QPainter>

namespace brig { namespace qt {

inline void draw(const blob_t& wkb, const frame& fr, QPainter& painter)
{
  auto ptr = wkb.data();
  detail::draw_geometry(ptr, fr, painter);
}

} } // brig::qt

#endif // BRIG_QT_DRAW_HPP
