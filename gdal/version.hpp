// Andrew Naplavkov

#ifndef BRIG_GDAL_VERSION_HPP
#define BRIG_GDAL_VERSION_HPP

#include <brig/gdal/detail/lib.hpp>
#include <string>

namespace brig { namespace gdal {

inline std::string version()
{
  using namespace detail;
  return lib::singleton().empty()? "": lib::singleton().p_GDALVersionInfo("RELEASE_NAME");
}

} } // brig::gdal

#endif // BRIG_GDAL_VERSION_HPP
