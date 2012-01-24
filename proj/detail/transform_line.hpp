// Andrew Naplavkov

#ifndef BRIG_PROJ_DETAIL_TRANSFORM_LINE_HPP
#define BRIG_PROJ_DETAIL_TRANSFORM_LINE_HPP

#include <brig/detail/ogc.hpp>
#include <brig/proj/detail/lib.hpp>
#include <brig/proj/detail/transform_point.hpp>
#include <brig/proj/transform.hpp>
#include <cstdint>
#include <cstring>

namespace brig { namespace proj { namespace detail {

using namespace brig::detail::ogc;

template <typename InputIterator, typename OutputIterator>
void transform_line(uint8_t byte_order, InputIterator& in_iter, OutputIterator& out_iter, projPJ in_pj, projPJ out_pj)
{
  const uint32_t count(get<uint32_t>(byte_order, in_iter)); set<uint32_t>(out_iter, count);
  for (uint32_t i(0); i < count; ++i)
    transform_point(byte_order, in_iter, out_iter, in_pj, out_pj);
}

template <typename InputType, typename OutputType>
void transform_line(uint8_t byte_order, InputType*& in_ptr, OutputType*& out_ptr, projPJ in_pj, projPJ out_pj) // optimization
{
  static_assert(sizeof(InputType) == sizeof(uint8_t), "size error");
  static_assert(sizeof(OutputType) == sizeof(uint8_t), "size error");
  const uint32_t point_xy_count(get<uint32_t>(byte_order, in_ptr)); set<uint32_t>(out_ptr, point_xy_count);
  double* point_xy_begin((double*)out_ptr);
  if (system_byte_order() == byte_order)
  {
    const uint32_t bytes(point_xy_count * 2 * sizeof(double));
    if (in_ptr != out_ptr) memcpy(out_ptr, in_ptr, bytes);
    in_ptr += bytes;
    out_ptr += bytes;
  }
  else
    for (uint32_t i(0); i < point_xy_count; ++i)
    {
      set<double>(out_ptr, get<double>(byte_order, in_ptr));
      set<double>(out_ptr, get<double>(byte_order, in_ptr));
    }
  transform(point_xy_begin, point_xy_count, in_pj, out_pj);
}

} } } // brig::proj::detail

#endif // BRIG_PROJ_DETAIL_TRANSFORM_LINE_HPP
