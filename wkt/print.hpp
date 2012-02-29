// Andrew Naplavkov

#ifndef BRIG_WKT_PRINT_HPP
#define BRIG_WKT_PRINT_HPP

#include <brig/blob_t.hpp>
#include <brig/wkt/detail/print_geometry.hpp>
#include <locale>
#include <sstream>
#include <string>

namespace brig { namespace wkt {

inline std::string print(const blob_t& wkb)
{
  auto ptr = wkb.data();
  std::ostringstream stream; stream.imbue(std::locale::classic());
  detail::print_geometry(ptr, stream);
  return stream.str();
}

} } // brig::wkt

#endif // BRIG_WKT_PRINT_HPP
