// Andrew Naplavkov

#ifndef BRIG_DATABASE_MYSQL_CLIENT_VERSION_HPP
#define BRIG_DATABASE_MYSQL_CLIENT_VERSION_HPP

#include <brig/database/mysql/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace mysql {

inline std::string client_version()
{
  return detail::lib::singleton().empty()? "": detail::lib::singleton().p_mysql_get_client_info();
}

} } } // brig::database::mysql

#endif // BRIG_DATABASE_MYSQL_CLIENT_VERSION_HPP
