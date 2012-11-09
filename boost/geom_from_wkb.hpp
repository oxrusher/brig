// Andrew Naplavkov

#ifndef BRIG_BOOST_GEOM_FROM_WKB_HPP
#define BRIG_BOOST_GEOM_FROM_WKB_HPP

#include <brig/blob_t.hpp>
#include <brig/boost/detail/read_geometry.hpp>
#include <brig/boost/geometry.hpp>

namespace brig { namespace boost {

inline geometry geom_from_wkb(const blob_t& wkb)
{
  auto ptr(wkb.data());
  geometry geom;
  detail::read_geometry(ptr, geom);
  return geom;
}

} } // brig::boost

#endif // BRIG_BOOST_GEOM_FROM_WKB_HPP
