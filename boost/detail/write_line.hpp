// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_WRITE_LINE_HPP
#define BRIG_BOOST_DETAIL_WRITE_LINE_HPP

#include <brig/detail/ogc.hpp>
#include <brig/boost/detail/write_point.hpp>
#include <brig/boost/geometry.hpp>
#include <cstdint>

namespace brig { namespace boost { namespace detail {

template <typename OutputIterator, typename Line>
void write_raw(OutputIterator& itr, const Line& line)
{
  brig::detail::ogc::write<uint32_t>(itr, static_cast<uint32_t>(line.size()));
  for (size_t i(0); i < line.size(); ++i)
    write_raw<>(itr, line[i]);
}

template <typename OutputIterator>
void write(OutputIterator& itr, const linestring& line)
{
  using namespace brig::detail::ogc;
  write_byte_order(itr);
  brig::detail::ogc::write<uint32_t>(itr, uint32_t(LineString));
  write_raw(itr, line);
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_WRITE_LINE_HPP
