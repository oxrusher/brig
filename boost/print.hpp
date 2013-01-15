// Andrew Naplavkov

#ifndef BRIG_BOOST_PRINT_HPP
#define BRIG_BOOST_PRINT_HPP

#include <brig/blob_t.hpp>
#include <brig/boost/detail/print_geometry.hpp>
#include <locale>
#include <sstream>
#include <string>

namespace brig { namespace boost {

inline std::string print(const blob_t& wkb)
{
  auto ptr(wkb.data());
  std::ostringstream stream; stream.imbue(std::locale::classic());
  detail::print_geometry(ptr, stream);
  return stream.str();
}

} } // brig::boost

#endif // BRIG_BOOST_PRINT_HPP
