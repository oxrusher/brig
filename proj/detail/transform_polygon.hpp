// Andrew Naplavkov

#ifndef BRIG_PROJ_DETAIL_TRANSFORM_POLYGON_HPP
#define BRIG_PROJ_DETAIL_TRANSFORM_POLYGON_HPP

#include <brig/detail/ogc.hpp>
#include <brig/proj/detail/lib.hpp>
#include <brig/proj/detail/transform_line.hpp>
#include <cstdint>

namespace brig { namespace proj { namespace detail {

template <typename InputIterator, typename OutputIterator>
void transform_polygon(uint8_t byte_order, InputIterator& in_itr, OutputIterator& out_itr, projPJ in_pj, projPJ out_pj)
{
  using namespace brig::detail::ogc;
  const uint32_t count(read<uint32_t>(byte_order, in_itr)); write<uint32_t>(out_itr, count);
  for (uint32_t i(0); i < count; ++i)
    transform_line(byte_order, in_itr, out_itr, in_pj, out_pj);
}

} } } // brig::proj::detail

#endif // BRIG_PROJ_DETAIL_TRANSFORM_POLYGON_HPP
