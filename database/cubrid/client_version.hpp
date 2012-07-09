// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_CLIENT_VERSION_HPP
#define BRIG_DATABASE_CUBRID_CLIENT_VERSION_HPP

#include <brig/database/cubrid/detail/lib.hpp>
#include <locale>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace cubrid {

inline std::string client_version()
{
  int major_version(0), minor_version(0), patch(0);
  if ( detail::lib::singleton().p_cci_get_version == 0
    || detail::lib::error(detail::lib::singleton().p_cci_get_version(&major_version, &minor_version, &patch))
     ) return "";
  
  std::ostringstream stream; stream.imbue(std::locale::classic());
  stream << major_version << "." << minor_version << "." << patch;
  return stream.str();
}

} } } // brig::database::cubrid

#endif // BRIG_DATABASE_CUBRID_CLIENT_VERSION_HPP
