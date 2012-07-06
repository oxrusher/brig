// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_VERSION_HPP
#define BRIG_DATABASE_POSTGRES_VERSION_HPP

#include <brig/database/postgres/detail/lib.hpp>
#include <locale>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace postgres {

inline std::string libpq_version()
{
  if (detail::lib::singleton().empty()) return "";
  const int version(detail::lib::singleton().p_PQlibVersion());
  const int major(version / 10000);
  const int minor((version % 10000) / 100);
  const int revision(version % 100);
  std::ostringstream stream; stream.imbue(std::locale::classic());
  stream << major << "." << minor << "." << revision;
  return stream.str();
}

} } } // brig::database::postgres

#endif // BRIG_DATABASE_POSTGRES_VERSION_HPP
