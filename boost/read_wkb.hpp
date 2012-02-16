// Andrew Naplavkov

#ifndef BRIG_BOOST_READ_WKB_HPP
#define BRIG_BOOST_READ_WKB_HPP

#include <brig/blob_t.hpp>
#include <brig/boost/detail/read_geometry.hpp>
#include <brig/boost/geometry.hpp>

namespace brig { namespace boost {

inline void read_wkb(const blob_t& in_wkb, geometry& out_geom)
{
  auto in_ptr = in_wkb.data();
  detail::read_geometry(in_ptr, out_geom);
}

} } // brig::boost

#endif // BRIG_BOOST_READ_WKB_HPP
