// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_TABLES_HPP
#define BRIG_DATABASE_DETAIL_SQL_TABLES_HPP

#include <brig/database/detail/sql_schema_filter.hpp>
#include <brig/database/global.hpp>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_tables(DBMS sys, const std::string& tbl = std::string())  // TABLE_SCHEMA, TABLE_NAME
{
  std::ostringstream stream; stream.imbue(std::locale::classic());
  switch (sys)
  {
  default: throw std::runtime_error("SQL error");

  case DB2:
    stream << "SELECT RTRIM(TABSCHEMA), TABNAME FROM SYSCAT.TABLES WHERE TYPE = 'T' AND ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "TABSCHEMA");
    else stream << " lower(TABNAME) = lower('" << tbl << "')";
    stream << " ORDER BY TABSCHEMA, TABNAME";
    break;

  case MS_SQL: 
  case MySQL:
  case Postgres:
    stream << "SELECT TABLE_SCHEMA, TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "TABLE_SCHEMA");
    else stream << " lower(TABLE_NAME) = lower('" << tbl << "')";
    stream << " ORDER BY TABLE_SCHEMA, TABLE_NAME";
    break;

  case Oracle:
    stream << "SELECT OWNER, TABLE_NAME FROM ALL_TABLES WHERE TABLE_NAME NOT LIKE '%$%' AND ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "OWNER");
    else stream << " lower(TABLE_NAME) = lower('" << tbl << "')";
    stream << " ORDER BY OWNER, TABLE_NAME";
    break;

  case SQLite:
    stream << "SELECT '', NAME FROM SQLITE_MASTER WHERE TYPE = 'table' AND ";
    if (tbl.empty()) stream << " NAME NOT LIKE 'sqlite!_%' ESCAPE '!'";
    else stream << " lower(NAME) = lower('" << tbl << "')";
    stream << " ORDER BY NAME";
    break;
  }
  return stream.str();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_TABLES_HPP
