// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_LIMIT_HPP
#define BRIG_DATABASE_DETAIL_SQL_LIMIT_HPP

#include <brig/database/global.hpp>
#include <brig/string_cast.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline void sql_limit(DBMS sys, int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix)
{
  if (rows < 0) return;
  const std::string sql_rows(string_cast<char>(rows));
  switch (sys)
  {
  default: break;
  case CUBRID:
  case MySQL:
  case SQLite: sql_suffix = "LIMIT " + sql_rows; break;
  case DB2: sql_suffix = "FETCH FIRST " + sql_rows + " ROWS ONLY OPTIMIZE FOR " + sql_rows + " ROWS"; break;
  case MS_SQL: sql_infix = "TOP (" + sql_rows + ')'; break;
  case Oracle:
    sql_infix = "/*+ FIRST_ROWS(" + sql_rows + ") */";
    sql_counter = "ROWNUM <= " + sql_rows;
    break;
  case Postgres: sql_suffix = "FETCH FIRST " + sql_rows + " ROWS ONLY"; break; // SQL:2008
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_LIMIT_HPP
