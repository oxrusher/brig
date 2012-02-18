// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_LIMIT_HPP
#define BRIG_DATABASE_DETAIL_SQL_LIMIT_HPP

#include <brig/database/global.hpp>
#include <brig/detail/string_cast.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline void sql_limit(DBMS sys, int rows, std::string& sql_prefix, std::string& sql_infix, std::string& sql_suffix, std::string& sql_postfix)
{
  if (rows < 0) return;
  const std::string sql_rows(brig::detail::string_cast<char>(rows));
  switch (sys)
  {
  default: break;
  case DB2: sql_suffix = "FETCH FIRST " + sql_rows + " ROWS ONLY OPTIMIZE FOR " + sql_rows + " ROWS"; break;
  case MS_SQL: sql_infix = "TOP (" + sql_rows + ')'; break;
  case MySQL:
  case SQLite: sql_suffix = "LIMIT " + sql_rows; break;
  case Oracle:
    sql_prefix = "SELECT * FROM (";
    sql_infix = "/*+ FIRST_ROWS(" + sql_rows + ") */";
    sql_postfix = ") WHERE ROWNUM <= " + sql_rows;
    break;
  case Postgres: sql_suffix = "FETCH FIRST " + sql_rows + " ROWS ONLY"; break; // SQL:2008
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_LIMIT_HPP
