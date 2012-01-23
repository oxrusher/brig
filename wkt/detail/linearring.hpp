// Andrew Naplavkov

#ifndef BRIG_WKT_DETAIL_LINEARRING_HPP
#define BRIG_WKT_DETAIL_LINEARRING_HPP

#include <brig/detail/ogc.hpp>
#include <brig/wkt/detail/point_t.hpp>
#include <cstdint>
#include <vector>

namespace brig { namespace wkt { namespace detail {

struct linearring  { std::vector<point_t> points; };

template <typename OutputIterator>
void set(OutputIterator& out_iter, const linearring& line)
{
  brig::detail::ogc::set<uint32_t>(out_iter, static_cast<uint32_t>(line.points.size()));
  for (size_t i(0); i < line.points.size(); ++i)
    set<>(out_iter, line.points[i]);
}

} } } // brig::wkt::detail

#endif // BRIG_WKT_DETAIL_LINEARRING_HPP
