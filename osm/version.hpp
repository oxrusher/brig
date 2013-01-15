// Andrew Naplavkov

#ifndef BRIG_OSM_VERSION_HPP
#define BRIG_OSM_VERSION_HPP

#include <brig/osm/detail/lib.hpp>
#include <string>

namespace brig { namespace osm {

inline std::string curl_version()
{
  using namespace detail;
  return lib::singleton().empty()? "": lib::singleton().p_curl_version();
}

} } // brig::osm

#endif // BRIG_OSM_VERSION_HPP
