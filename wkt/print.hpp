// Andrew Naplavkov

#ifndef BRIG_WKT_PRINT_HPP
#define BRIG_WKT_PRINT_HPP

#include <brig/blob_t.hpp>
#include <brig/wkt/detail/print_geometry.hpp>
#include <locale>
#include <sstream>
#include <string>

namespace brig { namespace wkt {

inline void print(const blob_t& in_wkb, std::string& out_wkt)
{
  auto in_ptr = in_wkb.data();
  std::ostringstream out_stream;
  out_stream.imbue(std::locale::classic());
  detail::print_geometry(in_ptr, out_stream);
  out_wkt = out_stream.str();
}

} } // brig::wkt

#endif // BRIG_WKT_PRINT_HPP
