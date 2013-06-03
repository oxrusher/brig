// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_DECODE_POINT_HPP
#define BRIG_BOOST_DETAIL_DECODE_POINT_HPP

#include <brig/detail/ogc.hpp>
#include <cstdint>
#include <sstream>

namespace brig { namespace boost { namespace detail {

template <typename InputIterator>
void decode_point_raw(uint8_t byte_order, InputIterator& in_itr, std::ostringstream& out_stream)
{
  using namespace brig::detail::ogc;
  out_stream << read<double>(byte_order, in_itr);
  out_stream << " ";
  out_stream << read<double>(byte_order, in_itr);
}

template <typename InputIterator>
void decode_point(uint8_t byte_order, InputIterator& in_itr, std::ostringstream& out_stream)
{
  out_stream << "(";
  decode_point_raw(byte_order, in_itr, out_stream);
  out_stream << ")";
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_DECODE_POINT_HPP
