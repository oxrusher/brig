// Andrew Naplavkov

#ifndef BRIG_PROJ_TRANSFORM_WKB_HPP
#define BRIG_PROJ_TRANSFORM_WKB_HPP

#include <brig/blob_t.hpp>
#include <brig/proj/detail/lib.hpp>
#include <brig/proj/detail/transform_geometry.hpp>

namespace brig { namespace proj {

inline void transform_wkb(blob_t& wkb, projPJ in_pj, projPJ out_pj)
{
  const blob_t::value_type* in_ptr(wkb.data());
  blob_t::value_type* out_ptr((blob_t::value_type*)wkb.data());
  detail::transform_geometry(in_ptr, out_ptr, in_pj, out_pj);
}

} } // brig::proj

#endif // BRIG_PROJ_TRANSFORM_WKB_HPP
