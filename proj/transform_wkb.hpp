// Andrew Naplavkov

#ifndef BRIG_PROJ_TRANSFORM_WKB_HPP
#define BRIG_PROJ_TRANSFORM_WKB_HPP

#include <brig/proj/detail/lib.hpp>
#include <brig/proj/detail/transform_geom.hpp>
#include <vector>

namespace brig { namespace proj {

template <typename T>
void transform_wkb(std::vector<T>& wkb, projPJ in_pj, projPJ out_pj)
{
  const T* in_ptr = wkb.data();
  T* out_ptr = (T*)wkb.data();
  detail::transform_geom(in_ptr, out_ptr, in_pj, out_pj);
}

} } // brig::proj

#endif // BRIG_PROJ_TRANSFORM_WKB_HPP
