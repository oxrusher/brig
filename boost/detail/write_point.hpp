// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_WRITE_POINT_HPP
#define BRIG_BOOST_DETAIL_WRITE_POINT_HPP

#include <brig/detail/ogc.hpp>
#include <brig/boost/geometry.hpp>
#include <cstdint>

namespace brig { namespace boost { namespace detail {

template <typename OutputIterator>
void write_raw(OutputIterator& itr, const point& pt)
{
  using namespace brig::detail;
  ogc::write<double>(itr, pt.get<0>());
  ogc::write<double>(itr, pt.get<1>());
}

template <typename OutputIterator>
void write(OutputIterator& itr, const point& pt)
{
  using namespace brig::detail;
  using namespace brig::detail::ogc;
  write_byte_order(itr);
  ogc::write<uint32_t>(itr, uint32_t(Point));
  write_raw<>(itr, pt);
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_WRITE_POINT_HPP
