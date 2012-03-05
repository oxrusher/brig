// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_GET_SQL_TYPE_HPP
#define BRIG_DATABASE_ODBC_DETAIL_GET_SQL_TYPE_HPP

#include <brig/database/global.hpp>
#include <brig/database/odbc/detail/lib.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

inline SQLSMALLINT get_sql_type_blob(DBMS sys)  { return MS_SQL == sys? SQL_LONGVARBINARY: SQL_VARBINARY; }
inline SQLSMALLINT get_sql_type_string(DBMS sys)  { return MS_SQL == sys? SQL_WLONGVARCHAR: SQL_WVARCHAR; }

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_GET_SQL_TYPE_HPP
