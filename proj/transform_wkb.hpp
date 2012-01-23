// Andrew Naplavkov

#ifndef BRIG_PROJ_TRANSFORM_WKB_HPP
#define BRIG_PROJ_TRANSFORM_WKB_HPP

#include <brig/proj/detail/lib.hpp>
#include <brig/proj/detail/transform_geom.hpp>

namespace brig { namespace proj {

template <typename InputIterator, typename OutputIterator>
void transform_wkb(InputIterator& in_iter, OutputIterator& out_iter, projPJ in_pj, projPJ out_pj)
{
  detail::transform_geom(in_iter, out_iter, in_pj, out_pj);
}

} } // brig::proj

#endif // BRIG_PROJ_TRANSFORM_WKB_HPP
