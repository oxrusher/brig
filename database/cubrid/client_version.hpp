// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_CLIENT_VERSION_HPP
#define BRIG_DATABASE_CUBRID_CLIENT_VERSION_HPP

#include <brig/database/cubrid/detail/lib.hpp>
#include <brig/string_cast.hpp>
#include <string>

namespace brig { namespace database { namespace cubrid {

inline std::string client_version()
{
  using namespace detail;

  int major_version(0), minor_version(0), patch(0);
  if ( lib::singleton().p_cci_get_version == 0
    || lib::error(lib::singleton().p_cci_get_version(&major_version, &minor_version, &patch))
     ) return "";
  return string_cast<char>(major_version) + "." + string_cast<char>(minor_version) + "." + string_cast<char>(patch);
}

} } } // brig::database::cubrid

#endif // BRIG_DATABASE_CUBRID_CLIENT_VERSION_HPP
