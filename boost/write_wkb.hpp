// Andrew Naplavkov

#ifndef BRIG_BOOST_WRITE_WKB_HPP
#define BRIG_BOOST_WRITE_WKB_HPP

#include <brig/blob_t.hpp>
#include <brig/detail/back_insert_iterator.hpp>
#include <brig/boost/detail/write_geometry.hpp>
#include <brig/boost/geometry.hpp>

namespace brig { namespace boost {

inline void write_wkb(const geometry& in_geom, blob_t& out_wkb)
{
  out_wkb.clear();
  auto out_iter = brig::detail::back_inserter(out_wkb);
  detail::write<>(out_iter, in_geom);
}

} } // brig::boost

#endif // BRIG_BOOST_WRITE_WKB_HPP
