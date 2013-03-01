// Andrew Naplavkov

#ifndef BRIG_PROJ_TRANSFORM_HPP
#define BRIG_PROJ_TRANSFORM_HPP

#include <brig/proj/detail/lib.hpp>
#include <stdexcept>

namespace brig { namespace proj {

inline void transform(double* point_xy_begin, long point_xy_count, projPJ in_pj, projPJ out_pj)
{
  using namespace std;
  using namespace detail;

  if (!in_pj || !out_pj)
    throw runtime_error("projection error");

  if (lib::singleton().p_pj_is_latlong(in_pj))
    for (long i(0), count(2*point_xy_count); i < count; ++i)
      point_xy_begin[i] *= DEG_TO_RAD;

  if (0 != lib::singleton().p_pj_transform(in_pj, out_pj, point_xy_count, 2, point_xy_begin, point_xy_begin + 1, 0))
    throw runtime_error("projection error");

  if (lib::singleton().p_pj_is_latlong(out_pj))
    for (long i(0), count(2*point_xy_count); i < count; ++i)
      point_xy_begin[i] *= RAD_TO_DEG;
}

} } // brig::proj

#endif // BRIG_PROJ_TRANSFORM_HPP
