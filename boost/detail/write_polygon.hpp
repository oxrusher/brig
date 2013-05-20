// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_WRITE_POLYGON_HPP
#define BRIG_BOOST_DETAIL_WRITE_POLYGON_HPP

#include <brig/detail/ogc.hpp>
#include <brig/boost/detail/write_line.hpp>
#include <brig/boost/geometry.hpp>
#include <cstdint>

namespace brig { namespace boost { namespace detail {

template <typename OutputIterator>
void write(OutputIterator& itr, const polygon& poly)
{
  using namespace brig::detail;
  using namespace brig::detail::ogc;
  write_byte_order(itr);
  ogc::write<uint32_t>(itr, Polygon);
  ogc::write<uint32_t>(itr, uint32_t(1 + poly.inners().size()));
  write_raw<>(itr, poly.outer());
  for (size_t i(0); i < poly.inners().size(); ++i)
    write_raw<>(itr, poly.inners()[i]);
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_WRITE_POLYGON_HPP
