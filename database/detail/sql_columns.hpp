// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_COLUMNS_HPP
#define BRIG_DATABASE_DETAIL_SQL_COLUMNS_HPP

#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_columns(DBMS sys, const identifier& tbl)
{
  switch (sys)
  {
  default: throw std::runtime_error("SQL error");
  case DB2: return "SELECT COLNAME, RTRIM(TYPESCHEMA), TYPENAME, LENGTH, LENGTH, SCALE FROM SYSCAT.COLUMNS WHERE TABSCHEMA = '" + tbl.schema + "' AND TABNAME = '" + tbl.name + "' ORDER BY COLNO";
  case MS_SQL:
  case MySQL: return "SELECT COLUMN_NAME, '', DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, NUMERIC_PRECISION, NUMERIC_SCALE FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' ORDER BY ORDINAL_POSITION";
  case Oracle: return "SELECT COLUMN_NAME, DATA_TYPE_OWNER, DATA_TYPE, CHAR_LENGTH, DATA_PRECISION, DATA_SCALE FROM ALL_TAB_COLUMNS WHERE OWNER = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' ORDER BY COLUMN_ID";
  case Postgres: return "SELECT COLUMN_NAME, DATA_TYPE, UDT_NAME, CHARACTER_MAXIMUM_LENGTH, NUMERIC_PRECISION, NUMERIC_SCALE FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' ORDER BY ORDINAL_POSITION";
  }
  return "";
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_COLUMNS_HPP
