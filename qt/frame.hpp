// Andrew Naplavkov

#ifndef BRIG_QT_FRAME_HPP
#define BRIG_QT_FRAME_HPP

#include <cmath>
#include <QPointF>
#include <QSize>
#include <QSizeF>
#include <QtGlobal>
#include <stdexcept>

namespace brig { namespace qt {

class frame {
  QPointF m_center;
  qreal m_scale;
  QSize m_size;
  QSizeF m_half_size;

  static bool huge(qreal val)  { return !(-HUGE_VAL < val && val < HUGE_VAL); }

public:
  frame(): m_scale(1)  {}
  frame(const QPointF& center, qreal scale, const QSize& size);
  bool operator==(const frame& r) const;
  QPointF proj_to_pixel(const QPointF& point) const;
  QPointF pixel_to_proj(const QPointF& point) const;
}; // frame

inline frame::frame(const QPointF& center, qreal scale, const QSize& size)
{
  m_center = center;
  if (qFuzzyCompare(scale + 1., 1.)) throw std::runtime_error("scale error");
  m_scale = scale;
  m_size = size;
  m_half_size = QSizeF(size) / 2.;
}

inline bool frame::operator==(const frame& r) const
{
  return m_center == r.m_center && qFuzzyCompare(m_scale, r.m_scale) && m_size == r.m_size;
}

inline QPointF frame::proj_to_pixel(const QPointF& point) const
{
  if (huge(point.x()) || huge(point.y())) throw std::runtime_error("huge val error");
  return QPointF(m_half_size.width() + (point.x() - m_center.x()) / m_scale, m_half_size.height() + (m_center.y() - point.y()) / m_scale);
}

inline QPointF frame::pixel_to_proj(const QPointF& point) const
{
  if (huge(point.x()) || huge(point.y())) throw std::runtime_error("huge val error");
  return QPointF((point.x() - m_half_size.width()) * m_scale + m_center.x(), (m_half_size.height() - point.y()) * m_scale + m_center.y());
}

} } // brig::qt

#endif // BRIG_QT_FRAME_HPP
