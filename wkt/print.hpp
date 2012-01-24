// Andrew Naplavkov

#ifndef BRIG_WKT_PRINT_HPP
#define BRIG_WKT_PRINT_HPP

#include <brig/wkt/detail/print_geom.hpp>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

namespace brig { namespace wkt {

template <typename T>
void print(const std::vector<T>& in_wkb, std::string& out_wkt)
{
  const T* in_ptr(in_wkb.data());
  std::ostringstream out_stream;
  out_stream.imbue(std::locale::classic());
  detail::print_geom(in_ptr, out_stream);
  out_wkt = out_stream.str();
}

} } // brig::wkt

#endif // BRIG_WKT_PRINT_HPP
