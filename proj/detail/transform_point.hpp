// Andrew Naplavkov

#ifndef BRIG_PROJ_DETAIL_TRANSFORM_POINT_HPP
#define BRIG_PROJ_DETAIL_TRANSFORM_POINT_HPP

#include <brig/detail/ogc.hpp>
#include <brig/proj/detail/lib.hpp>
#include <brig/proj/transform.hpp>
#include <cstdint>

namespace brig { namespace proj { namespace detail {

using namespace brig::detail::ogc;

template <typename InputIterator, typename OutputIterator>
void transform_point(uint8_t byte_order, InputIterator& in_itr, OutputIterator& out_itr, projPJ in_pj, projPJ out_pj)
{
  double point_xy[2];
  point_xy[0] = get<double>(byte_order, in_itr);
  point_xy[1] = get<double>(byte_order, in_itr);
  transform(point_xy, 1, in_pj, out_pj);
  set<double>(out_itr, point_xy[0]);
  set<double>(out_itr, point_xy[1]);
}

} } } // brig::proj::detail

#endif // BRIG_PROJ_DETAIL_TRANSFORM_POINT_HPP
