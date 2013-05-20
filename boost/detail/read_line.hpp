// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_READ_LINE_HPP
#define BRIG_BOOST_DETAIL_READ_LINE_HPP

#include <brig/detail/ogc.hpp>
#include <brig/boost/detail/read_point.hpp>
#include <cstdint>

namespace brig { namespace boost { namespace detail {

template <typename InputIterator, typename Line>
void read_line(uint8_t byte_order, InputIterator& itr, Line& line)
{
  const size_t count(brig::detail::ogc::read<uint32_t>(byte_order, itr));
  line.resize(count);
  for (size_t i(0); i < count; ++i)
    read_point(byte_order, itr, line[i]);
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_READ_LINE_HPP
