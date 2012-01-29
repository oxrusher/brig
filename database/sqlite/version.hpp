// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_VERSION_HPP
#define BRIG_DATABASE_SQLITE_VERSION_HPP

#include <brig/database/sqlite/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace sqlite {

inline std::string sqlite3_libversion()
{
  return detail::lib::singleton().empty()? "": detail::lib::singleton().p_sqlite3_libversion();
}

inline std::string spatialite_version()
{
  return detail::lib::singleton().p_spatialite_version == 0? "": detail::lib::singleton().p_spatialite_version();
}

} } } // brig::database::sqlite

#endif // BRIG_DATABASE_SQLITE_VERSION_HPP
