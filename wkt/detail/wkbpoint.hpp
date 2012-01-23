// Andrew Naplavkov

#ifndef BRIG_WKT_DETAIL_WKBPOINT_HPP
#define BRIG_WKT_DETAIL_WKBPOINT_HPP

#include <brig/detail/ogc.hpp>
#include <brig/wkt/detail/point_t.hpp>
#include <cstdint>

namespace brig { namespace wkt { namespace detail {

struct wkbpoint  { point_t point; };

template <typename OutputIterator>
void set(OutputIterator& out_iter, const wkbpoint& point)
{
  using namespace brig::detail;
  ogc::set_byte_order(out_iter);
  ogc::set<uint32_t>(out_iter, ogc::Point);
  set<>(out_iter, point.point);
}

} } } // brig::wkt::detail

#endif // BRIG_WKT_DETAIL_WKBPOINT_HPP
