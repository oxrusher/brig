// Andrew Naplavkov

#ifndef BRIG_WKT_DETAIL_PRINT_POINT_HPP
#define BRIG_WKT_DETAIL_PRINT_POINT_HPP

#include <brig/detail/ogc.hpp>
#include <cstdint>
#include <sstream>

namespace brig { namespace wkt { namespace detail {

template <typename InputIterator>
void print_point(uint8_t byte_order, InputIterator& in_iter, std::ostringstream& out_stream)
{
  using namespace brig::detail::ogc;
  out_stream << get<double>(byte_order, in_iter);
  out_stream << ' ';
  out_stream << get<double>(byte_order, in_iter);
}

template <typename InputIterator>
void print_point_text(uint8_t byte_order, InputIterator& in_iter, std::ostringstream& out_stream)
{
  out_stream << '(';
  print_point(byte_order, in_iter, out_stream);
  out_stream << ')';
}

} } } // brig::wkt::detail

#endif // BRIG_WKT_DETAIL_PRINT_POINT_HPP
