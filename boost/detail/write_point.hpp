// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_WRITE_POINT_HPP
#define BRIG_BOOST_DETAIL_WRITE_POINT_HPP

#include <brig/detail/ogc.hpp>
#include <brig/boost/geometry.hpp>
#include <cstdint>

namespace brig { namespace boost { namespace detail {

template <typename OutputIterator>
void write_raw(OutputIterator& iter, const point& pt)
{
  using namespace brig::detail;
  ogc::write<double>(iter, pt.get<0>());
  ogc::write<double>(iter, pt.get<1>());
}

template <typename OutputIterator>
void write(OutputIterator& iter, const point& pt)
{
  using namespace brig::detail;
  using namespace brig::detail::ogc;
  write_byte_order(iter);
  ogc::write<uint32_t>(iter, uint32_t(Point));
  write_raw<>(iter, pt);
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_WRITE_POINT_HPP
