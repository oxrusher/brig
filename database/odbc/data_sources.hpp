// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DATA_SOURCES_HPP
#define BRIG_DATABASE_ODBC_DATA_SOURCES_HPP

#include <brig/database/odbc/detail/lib.hpp>
#include <brig/unicode/transform.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace odbc {

inline void data_sources(std::vector<std::string>& dsns)
{
  SQLHANDLE env(SQL_NULL_HANDLE);
  if ( detail::lib::singleton().empty()
    || !SQL_SUCCEEDED(detail::lib::singleton().p_SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env)))
    return;

  if (SQL_SUCCEEDED(detail::lib::singleton().p_SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, SQLPOINTER(SQL_OV_ODBC3), 0)))
  {
    SQLWCHAR dsn[SQL_MAX_MESSAGE_LENGTH], desc[SQL_MAX_MESSAGE_LENGTH];
    SQLSMALLINT dsn_size(0), desc_size(0);
    while (SQL_SUCCEEDED(detail::lib::singleton().p_SQLDataSourcesW(env, SQL_FETCH_NEXT
      , dsn, SQL_MAX_MESSAGE_LENGTH, &dsn_size
      , desc, SQL_MAX_MESSAGE_LENGTH, &desc_size
      )))
      dsns.push_back(brig::unicode::transform<std::string>(dsn));
  }

  detail::lib::singleton().p_SQLFreeHandle(SQL_HANDLE_ENV, env);
}

} } } // brig::database::odbc

#endif // BRIG_DATABASE_ODBC_DATA_SOURCES_HPP
