// Andrew Naplavkov

#ifndef BRIG_BOOST_AS_BINARY_HPP
#define BRIG_BOOST_AS_BINARY_HPP

#include <boost/geometry/geometry.hpp>
#include <brig/blob_t.hpp>
#include <brig/detail/back_insert_iterator.hpp>
#include <brig/boost/detail/write_geometry.hpp>
#include <brig/boost/geometry.hpp>

namespace brig { namespace boost {

inline blob_t as_binary(const geometry& g)
{
  blob_t wkb;
  auto iter(brig::detail::back_inserter(wkb));
  detail::write<>(iter, g);
  return wkb;
}

inline blob_t as_binary(const box& b)
{
  polygon poly;
  ::boost::geometry::convert(b, poly);
  return as_binary(geometry(poly));
}

} } // brig::boost

#endif // BRIG_BOOST_AS_BINARY_HPP
