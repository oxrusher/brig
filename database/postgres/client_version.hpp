// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_CLIENT_VERSION_HPP
#define BRIG_DATABASE_POSTGRES_CLIENT_VERSION_HPP

#include <brig/database/postgres/detail/lib.hpp>
#include <locale>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace postgres {

inline std::string client_version()
{
  if (detail::lib::singleton().empty()) return "";
  const int version(detail::lib::singleton().p_PQlibVersion());
  const int major_version(version / 10000);
  const int minor_version((version % 10000) / 100);
  const int revision(version % 100);
  std::ostringstream stream; stream.imbue(std::locale::classic());
  stream << major_version << "." << minor_version << "." << revision;
  return stream.str();
}

} } } // brig::database::postgres

#endif // BRIG_DATABASE_POSTGRES_CLIENT_VERSION_HPP
