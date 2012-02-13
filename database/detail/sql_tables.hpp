// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_TABLES_HPP
#define BRIG_DATABASE_DETAIL_SQL_TABLES_HPP

#include <brig/database/detail/sql_schema_filter.hpp>
#include <brig/database/global.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_tables(DBMS sys, const std::string& tbl_filter = std::string())  // TABLE_SCHEMA, TABLE_NAME
{
  std::string str;
  switch (sys)
  {
  default:
    throw std::runtime_error("SQL error");

  case DB2:
    str += "SELECT RTRIM(TABSCHEMA), TABNAME FROM SYSCAT.TABLES WHERE TYPE = 'T' AND ";
    if (tbl_filter.empty()) str += sql_schema_filter(sys, "TABSCHEMA");
    else str += " TABNAME LIKE '" + tbl_filter + "'";
    str += " ORDER BY TABSCHEMA, TABNAME";
    break;

  case MS_SQL: 
  case MySQL:
  case Postgres:
    str += "SELECT TABLE_SCHEMA, TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE ";
    if (tbl_filter.empty()) str += sql_schema_filter(sys, "TABLE_SCHEMA");
    else str += " TABLE_NAME LIKE '" + tbl_filter + "'";
    str += " ORDER BY TABLE_SCHEMA, TABLE_NAME";
    break;

  case Oracle:
    str += "SELECT OWNER, TABLE_NAME FROM ALL_TABLES WHERE TABLE_NAME NOT LIKE '%$%' AND ";
    if (tbl_filter.empty()) str += sql_schema_filter(sys, "OWNER");
    else str += " TABLE_NAME LIKE '" + tbl_filter + "'";
    str += " ORDER BY OWNER, TABLE_NAME";
    break;

  case SQLite:
    str += "SELECT '', NAME FROM SQLITE_MASTER WHERE TYPE = 'table' AND NAME ";
    if (tbl_filter.empty()) str += " NOT LIKE 'sqlite!_%' ESCAPE '!'";
    else str += " LIKE '" + tbl_filter + "'";
    str += " ORDER BY NAME";
    break;
  }
  return std::move(str);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_TABLES_HPP
