// Andrew Naplavkov

// http://trac.osgeo.org/proj/

#ifndef BRIG_PROJ_DETAIL_LIB_HPP
#define BRIG_PROJ_DETAIL_LIB_HPP

#include <brig/detail/dynamic_loading.hpp>
#include <proj_api.h>

namespace brig { namespace proj { namespace detail {

class lib {
  lib();

public:
  decltype(pj_free) *p_pj_free;
  decltype(pj_get_release) *p_pj_get_release;
  decltype(pj_init_plus) *p_pj_init_plus;
  decltype(pj_is_latlong) *p_pj_is_latlong;
  decltype(pj_transform) *p_pj_transform;

  bool empty() const  { return p_pj_transform == 0; }
  static lib& singleton()  { static lib s; return s; }
}; // lib

inline lib::lib() : p_pj_transform(0)
{
  auto handle = BRIG_DL_LIBRARY("proj.dll", "lib.so.0");
  if (  handle
    && (p_pj_free = BRIG_DL_FUNCTION(handle, pj_free))
    && (p_pj_get_release = BRIG_DL_FUNCTION(handle, pj_get_release))
    && (p_pj_is_latlong = BRIG_DL_FUNCTION(handle, pj_is_latlong))
    && (p_pj_init_plus = BRIG_DL_FUNCTION(handle, pj_init_plus))
     )  p_pj_transform = BRIG_DL_FUNCTION(handle, pj_transform);
} // lib::

} } } // brig::proj::detail

#endif // BRIG_PROJ_DETAIL_LIB_HPP
