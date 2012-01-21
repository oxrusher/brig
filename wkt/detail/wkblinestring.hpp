// Andrew Naplavkov

#ifndef BRIG_WKT_DETAIL_WKBLINESTRING_HPP
#define BRIG_WKT_DETAIL_WKBLINESTRING_HPP

#include <brig/detail/ogc.hpp>
#include <brig/wkt/detail/line_t.hpp>
#include <cstdint>

namespace brig { namespace wkt { namespace detail {

struct wkblinestring
{
  line_t line;
  wkblinestring()  {}
  wkblinestring(const line_t& line_) : line(line_)  {}
};

template <typename OutputIterator>
void set(OutputIterator& out_iter, const wkblinestring& line)
{
  brig::detail::ogc::set_byte_order(out_iter);
  brig::detail::ogc::set<uint32_t>(out_iter, brig::detail::ogc::LineString);
  set<>(out_iter, line.line);
}

} } } // brig::wkt::detail

#endif // BRIG_WKT_DETAIL_WKBLINESTRING_HPP
