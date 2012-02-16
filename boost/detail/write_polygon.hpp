// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_WRITE_POLYGON_HPP
#define BRIG_BOOST_DETAIL_WRITE_POLYGON_HPP

#include <brig/detail/ogc.hpp>
#include <brig/boost/detail/write_line.hpp>
#include <brig/boost/geometry.hpp>
#include <cstdint>

namespace brig { namespace boost { namespace detail {

template <typename OutputIterator>
void write(OutputIterator& iter, const polygon& poly)
{
  using namespace brig::detail;
  using namespace brig::detail::ogc;
  write_byte_order(iter);
  ogc::write<uint32_t>(iter, Polygon);
  ogc::write<uint32_t>(iter, uint32_t(1 + poly.inners().size()));
  write_raw<>(iter, poly.outer());
  for (size_t i(0); i < poly.inners().size(); ++i)
    write_raw<>(iter, poly.inners()[i]);
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_WRITE_POLYGON_HPP
