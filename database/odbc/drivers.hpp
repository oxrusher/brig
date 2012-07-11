// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DRIVERS_HPP
#define BRIG_DATABASE_ODBC_DRIVERS_HPP

#include <brig/database/odbc/detail/lib.hpp>
#include <brig/unicode/transform.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace odbc {

inline void drivers(std::vector<std::string>& drvs)
{
  SQLHANDLE env(SQL_NULL_HANDLE);
  if ( detail::lib::singleton().empty()
    || !SQL_SUCCEEDED(detail::lib::singleton().p_SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env)))
    return;

  if (SQL_SUCCEEDED(detail::lib::singleton().p_SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, SQLPOINTER(SQL_OV_ODBC3), 0)))
  {
    SQLWCHAR desc[SQL_MAX_MESSAGE_LENGTH], attr[SQL_MAX_MESSAGE_LENGTH];
    SQLSMALLINT desc_size(0), attr_size(0);
    while (SQL_SUCCEEDED(detail::lib::singleton().p_SQLDriversW(env, SQL_FETCH_NEXT
      , desc, SQL_MAX_MESSAGE_LENGTH, &desc_size
      , attr, SQL_MAX_MESSAGE_LENGTH, &attr_size
      )))
      drvs.push_back(brig::unicode::transform<std::string>(desc));
  }

  detail::lib::singleton().p_SQLFreeHandle(SQL_HANDLE_ENV, env);
}

} } } // brig::database::odbc

#endif // BRIG_DATABASE_ODBC_DRIVERS_HPP
