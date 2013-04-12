// Andrew Naplavkov

#ifndef BRIG_PROJ_DETAIL_LIB_HPP
#define BRIG_PROJ_DETAIL_LIB_HPP

#include <brig/detail/dynamic_loading.hpp>
#include <proj_api.h>

namespace brig { namespace proj { namespace detail {

class lib {
  lib();

public:
  decltype(pj_ctx_alloc) *p_pj_ctx_alloc;
  decltype(pj_ctx_free) *p_pj_ctx_free;
  decltype(pj_free) *p_pj_free;
  decltype(pj_get_def) *p_pj_get_def;
  decltype(pj_get_release) *p_pj_get_release;
  decltype(pj_init_plus_ctx) *p_pj_init_plus_ctx;
  decltype(pj_is_latlong) *p_pj_is_latlong;
  decltype(pj_transform) *p_pj_transform;

  bool empty() const  { return p_pj_transform == 0; }
  static lib& singleton()  { static lib s; return s; }
}; // lib

inline lib::lib() : p_pj_transform(0)
{
  auto handle(BRIG_DL_LIBRARY("libproj-0.dll", "libproj.so.0"));
  if (  handle
    && (p_pj_ctx_alloc = BRIG_DL_FUNCTION(handle, pj_ctx_alloc))
    && (p_pj_ctx_free = BRIG_DL_FUNCTION(handle, pj_ctx_free))
    && (p_pj_free = BRIG_DL_FUNCTION(handle, pj_free))
    && (p_pj_get_def = BRIG_DL_FUNCTION(handle, pj_get_def))
    && (p_pj_get_release = BRIG_DL_FUNCTION(handle, pj_get_release))
    && (p_pj_init_plus_ctx = BRIG_DL_FUNCTION(handle, pj_init_plus_ctx))
    && (p_pj_is_latlong = BRIG_DL_FUNCTION(handle, pj_is_latlong))
     )  p_pj_transform = BRIG_DL_FUNCTION(handle, pj_transform);
} // lib::

} } } // brig::proj::detail

#endif // BRIG_PROJ_DETAIL_LIB_HPP
