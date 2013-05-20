// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_WRITE_SEQUENCE_HPP
#define BRIG_BOOST_DETAIL_WRITE_SEQUENCE_HPP

#include <brig/detail/ogc.hpp>
#include <brig/boost/detail/write_line.hpp>
#include <brig/boost/detail/write_point.hpp>
#include <brig/boost/detail/write_polygon.hpp>
#include <brig/boost/geometry.hpp>
#include <cstdint>

namespace brig { namespace boost { namespace detail {

template <typename Geometry> struct traits;
template <> struct traits<multi_linestring>  { static const uint32_t ogc_type = brig::detail::ogc::MultiLineString; };
template <> struct traits<multi_point>  { static const uint32_t ogc_type = brig::detail::ogc::MultiPoint; };
template <> struct traits<multi_polygon>  { static const uint32_t ogc_type = brig::detail::ogc::MultiPolygon; };

template <typename OutputIterator, typename Sequence>
void write(OutputIterator& itr, const Sequence& seq)
{
  using namespace brig::detail;
  using namespace brig::detail::ogc;
  write_byte_order(itr);
  ogc::write<uint32_t>(itr, traits<Sequence>::ogc_type);
  ogc::write<uint32_t>(itr, uint32_t(seq.size()));
  for (size_t i(0); i < seq.size(); ++i)
    write<>(itr, seq[i]);
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_WRITE_SEQUENCE_HPP
