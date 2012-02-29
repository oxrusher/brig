// Andrew Naplavkov

#ifndef BRIG_BOOST_AS_BINARY_HPP
#define BRIG_BOOST_AS_BINARY_HPP

#include <brig/blob_t.hpp>
#include <brig/detail/back_insert_iterator.hpp>
#include <brig/boost/detail/write_geometry.hpp>
#include <brig/boost/geometry.hpp>

namespace brig { namespace boost {

inline blob_t as_binary(const geometry& geom)
{
  blob_t wkb;
  auto iter = brig::detail::back_inserter(wkb);
  detail::write<>(iter, geom);
  return wkb;
}

} } // brig::boost

#endif // BRIG_BOOST_AS_BINARY_HPP
