// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_READ_POINT_HPP
#define BRIG_BOOST_DETAIL_READ_POINT_HPP

#include <brig/detail/ogc.hpp>
#include <brig/boost/geometry.hpp>
#include <cstdint>

namespace brig { namespace boost { namespace detail {

template <typename InputIterator>
void read_point(uint8_t byte_order, InputIterator& itr, point& pt)
{
  using namespace brig::detail::ogc;
  pt.set<0>( read<double>(byte_order, itr) );
  pt.set<1>( read<double>(byte_order, itr) );
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_READ_POINT_HPP
