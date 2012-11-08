// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_CLIENT_VERSION_HPP
#define BRIG_DATABASE_ORACLE_CLIENT_VERSION_HPP

#include <brig/database/oracle/detail/lib.hpp>
#include <brig/string_cast.hpp>
#include <string>

namespace brig { namespace database { namespace oracle {

inline std::string client_version()
{
  if (detail::lib::singleton().empty()) return "";
  sword major_version(0), minor_version(0), update_num(0), patch_num(0), port_update_num(0);
  detail::lib::singleton().p_OCIClientVersion(&major_version, &minor_version, &update_num, &patch_num, &port_update_num);
  return
    string_cast<char>(major_version) + "." +
    string_cast<char>(minor_version) + "." +
    string_cast<char>(update_num) + "." +
    string_cast<char>(patch_num) + "." +
    string_cast<char>(port_update_num);
}

} } } // brig::database::oracle

#endif // BRIG_DATABASE_ORACLE_CLIENT_VERSION_HPP
