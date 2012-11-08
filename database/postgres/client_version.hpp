// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_CLIENT_VERSION_HPP
#define BRIG_DATABASE_POSTGRES_CLIENT_VERSION_HPP

#include <brig/database/postgres/detail/lib.hpp>
#include <brig/string_cast.hpp>
#include <string>

namespace brig { namespace database { namespace postgres {

inline std::string client_version()
{
  if (detail::lib::singleton().empty()) return "";
  const int version(detail::lib::singleton().p_PQlibVersion());
  const int major_version(version / 10000);
  const int minor_version((version % 10000) / 100);
  const int revision(version % 100);
  return string_cast<char>(major_version) + "." + string_cast<char>(minor_version) + "." + string_cast<char>(revision);
}

} } } // brig::database::postgres

#endif // BRIG_DATABASE_POSTGRES_CLIENT_VERSION_HPP
