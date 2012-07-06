// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_VERSION_HPP
#define BRIG_DATABASE_ORACLE_VERSION_HPP

#include <brig/database/oracle/detail/lib.hpp>
#include <locale>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace oracle {

inline std::string oci_version()
{
  if (detail::lib::singleton().empty()) return "";
  sword major_version(0), minor_version(0), update_num(0), patch_num(0), port_update_num(0);
  detail::lib::singleton().p_OCIClientVersion(&major_version, &minor_version, &update_num, &patch_num, &port_update_num);
  std::ostringstream stream; stream.imbue(std::locale::classic());
  stream << major_version << "." << minor_version << "." << update_num;
  return stream.str();
}

} } } // brig::database::oracle

#endif // BRIG_DATABASE_ORACLE_VERSION_HPP
