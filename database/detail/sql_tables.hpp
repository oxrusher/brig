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

inline std::string sql_tables(DBMS sys, const std::string& tbl, bool order_by)
{
  std::ostringstream stream; stream.imbue(std::locale::classic());
  switch (sys)
  {
  default: throw std::runtime_error("SQL error");

  case CUBRID:
    stream << "SELECT owner.name scm, class_name tbl FROM _db_class WHERE class_type = 0 AND ";
    if (tbl.empty()) stream << "is_system_class = 0";
    else stream << "LOWER(class_name) = LOWER('" << tbl << "')";
    break;

  case DB2:
    stream << "SELECT RTRIM(TABSCHEMA) scm, TABNAME tbl FROM SYSCAT.TABLES WHERE TYPE = 'T' AND ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "TABSCHEMA");
    else stream << "LOWER(TABNAME) = LOWER('" << tbl << "')";
    break;

  case Informix:
    stream << "SELECT RTRIM(owner) scm, tabname tbl FROM systables WHERE tabtype = 'T' AND tabname NOT LIKE 'sys%' AND ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "owner");
    else stream << "LOWER(tabname) = LOWER('" << tbl << "')";
    break;

  case Ingres:
    stream << "SELECT RTRIM(table_owner) scm, RTRIM(table_name) tbl FROM iitables WHERE table_type = 'T' AND system_use = 'U' AND table_name NOT LIKE 'iietab_%' AND ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "table_owner");
    else stream << "LOWER(table_name) = LOWER('" << tbl << "')";
    break;

  case MS_SQL: 
  case MySQL:
  case Postgres:
    stream << "SELECT TABLE_SCHEMA scm, TABLE_NAME tbl FROM INFORMATION_SCHEMA.TABLES WHERE ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "TABLE_SCHEMA");
    else stream << "LOWER(TABLE_NAME) = LOWER('" << tbl << "')";
    break;

  case Oracle:
    stream << "SELECT OWNER scm, TABLE_NAME tbl FROM ALL_TABLES WHERE TABLE_NAME NOT LIKE '%$%' AND ";
    if (tbl.empty()) stream << sql_schema_filter(sys, "OWNER");
    else stream << "LOWER(TABLE_NAME) = LOWER('" << tbl << "')";
    break;

  case SQLite:
    stream << "SELECT '' scm, NAME tbl FROM SQLITE_MASTER WHERE TYPE = 'table' AND ";
    if (tbl.empty()) stream << "NAME NOT LIKE 'sqlite!_%' ESCAPE '!'";
    else stream << "LOWER(NAME) = LOWER('" << tbl << "')";
    break;
  }

  if (order_by) stream << " ORDER BY scm, tbl";
  return stream.str();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_TABLES_HPP
