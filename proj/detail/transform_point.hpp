// Andrew Naplavkov

#ifndef BRIG_PROJ_DETAIL_TRANSFORM_POINT_HPP
#define BRIG_PROJ_DETAIL_TRANSFORM_POINT_HPP

#include <brig/detail/ogc.hpp>
#include <brig/proj/detail/lib.hpp>
#include <brig/proj/transform.hpp>
#include <cstdint>

namespace brig { namespace proj { namespace detail {

template <typename InputIterator, typename OutputIterator>
void transform_point(uint8_t byte_order, InputIterator& in_itr, OutputIterator& out_itr, projPJ in_pj, projPJ out_pj)
{
  using namespace brig::detail::ogc;
  double point_xy[2];
  point_xy[0] = read<double>(byte_order, in_itr);
  point_xy[1] = read<double>(byte_order, in_itr);
  transform(point_xy, 1, in_pj, out_pj);
  write<double>(out_itr, point_xy[0]);
  write<double>(out_itr, point_xy[1]);
}

} } } // brig::proj::detail

#endif // BRIG_PROJ_DETAIL_TRANSFORM_POINT_HPP
