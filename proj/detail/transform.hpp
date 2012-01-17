// Andrew Naplavkov

#ifndef BRIG_PROJ_DETAIL_TRANSFORM_HPP
#define BRIG_PROJ_DETAIL_TRANSFORM_HPP

#include <brig/proj/detail/lib.hpp>
#include <cstdint>
#include <stdexcept>

namespace brig { namespace proj { namespace detail {

inline void transform(double* point_xy_begin, uint32_t point_xy_count, projPJ in_pj, projPJ out_pj)
{
  if (lib::singleton().p_pj_is_latlong(in_pj))
    for (uint32_t i(0), count(2*point_xy_count); i < count; ++i)
      point_xy_begin[i] *= DEG_TO_RAD;

  if (0 != lib::singleton().p_pj_transform(in_pj, out_pj, point_xy_count, 2, point_xy_begin, point_xy_begin + 1, 0))
    throw std::runtime_error("projection error");

  if (lib::singleton().p_pj_is_latlong(out_pj))
    for (uint32_t i(0), count(2*point_xy_count); i < count; ++i)
      point_xy_begin[i] *= RAD_TO_DEG;
}

} } } // brig::proj::detail

#endif // BRIG_PROJ_DETAIL_TRANSFORM_HPP
