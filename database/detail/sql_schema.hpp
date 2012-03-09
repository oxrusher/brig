// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_SCHEMA_HPP
#define BRIG_DATABASE_DETAIL_SQL_SCHEMA_HPP

#include <brig/database/global.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_schema(DBMS sys)
{
  switch (sys)
  {
  default: throw std::runtime_error("sql error");
  case DB2: return "VALUES CURRENT_SCHEMA";
  case MS_SQL: return "SELECT SCHEMA_NAME()";
  case MySQL: return "select schema()";
  case Oracle: return "SELECT SYS_CONTEXT('USERENV','SESSION_SCHEMA') FROM DUAL";
  case Postgres: return "select current_schema()";
  case SQLite: return "";
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_SCHEMA_HPP
