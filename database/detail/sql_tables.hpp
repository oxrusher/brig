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

  case CUBRID:
    stream << "SELECT owner.name, class_name FROM _db_class WHERE class_type = 0 AND ";
    if (tbl.empty()) stream << "is_system_class = 0";
    else stream << "LOWER(class_name) = LOWER('" << tbl << "')";
    break;

  case DB2:
    stream << "SELECT RTRIM(TABSCHEMA), TABNAME FROM SYSCAT.TABLES WHERE TYPE = 'T' AND ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "TABSCHEMA");
    else stream << "LOWER(TABNAME) = LOWER('" << tbl << "')";
    break;

  case Informix:
    stream << "SELECT RTRIM(owner), tabname FROM systables WHERE tabtype = 'T' AND tabname NOT LIKE 'sys%' AND ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "owner");
    else stream << "LOWER(tabname) = LOWER('" << tbl << "')";
    break;

  case MS_SQL: 
  case MySQL:
  case Postgres:
    stream << "SELECT TABLE_SCHEMA, TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "TABLE_SCHEMA");
    else stream << "LOWER(TABLE_NAME) = LOWER('" << tbl << "')";
    break;

  case Oracle:
    stream << "SELECT OWNER, TABLE_NAME FROM ALL_TABLES WHERE TABLE_NAME NOT LIKE '%$%' AND ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "OWNER");
    else stream << "LOWER(TABLE_NAME) = LOWER('" << tbl << "')";
    break;

  case SQLite:
    stream << "SELECT '', NAME FROM SQLITE_MASTER WHERE TYPE = 'table' AND ";
    if (tbl.empty()) stream << "NAME NOT LIKE 'sqlite!_%' ESCAPE '!'";
    else stream << "LOWER(NAME) = LOWER('" << tbl << "')";
    break;
  }

  stream << " ORDER BY 1, 2";
  return stream.str();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_TABLES_HPP
