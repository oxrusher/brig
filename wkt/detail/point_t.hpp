// Andrew Naplavkov

#ifndef BRIG_WKT_DETAIL_POINT_T_HPP
#define BRIG_WKT_DETAIL_POINT_T_HPP

#include <brig/detail/ogc.hpp>

namespace brig { namespace wkt { namespace detail {

struct point_t
{
  double x, y;
  point_t() : x(0), y(0)  {}
};

template <typename OutputIterator>
void set(OutputIterator& out_iter, const point_t& point)
{
  using namespace brig::detail;
  ogc::set<double>(out_iter, point.x);
  ogc::set<double>(out_iter, point.y);
}

} } } // brig::wkt::detail

#endif // BRIG_WKT_DETAIL_POINT_T_HPP
