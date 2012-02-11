// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_COLUMNS_HPP
#define BRIG_DATABASE_DETAIL_SQL_COLUMNS_HPP

#include <brig/database/global.hpp>
#include <brig/database/object.hpp>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_columns(DBMS sys, const object& tbl)
{
  std::ostringstream stream;
  stream.imbue(std::locale::classic());
  switch (sys)
  {
  default: throw std::runtime_error("SQL error");
  case DB2: stream << "SELECT COLNAME, RTRIM(TYPESCHEMA), TYPENAME, LENGTH, LENGTH, SCALE FROM SYSCAT.COLUMNS WHERE TABSCHEMA = '" << tbl.schema << "' AND TABNAME = '" << tbl.name << "' ORDER BY COLNO"; break;
  case MS_SQL:
  case MySQL: stream << "SELECT COLUMN_NAME, '', DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, NUMERIC_PRECISION, NUMERIC_SCALE FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = '" << tbl.schema << "' AND TABLE_NAME = '" << tbl.name << "' ORDER BY ORDINAL_POSITION"; break;
  case Oracle: stream << "SELECT COLUMN_NAME, DATA_TYPE_OWNER, DATA_TYPE, CHAR_LENGTH, DATA_PRECISION, DATA_SCALE FROM ALL_TAB_COLUMNS WHERE OWNER = '" << tbl.schema << "' AND TABLE_NAME = '" << tbl.name << "' ORDER BY COLUMN_ID"; break;
  case Postgres: stream << "SELECT COLUMN_NAME, DATA_TYPE, UDT_NAME, CHARACTER_MAXIMUM_LENGTH, NUMERIC_PRECISION, NUMERIC_SCALE FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = '" << tbl.schema << "' AND TABLE_NAME = '" << tbl.name << "' ORDER BY ORDINAL_POSITION"; break;
  }
  return stream.str();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_COLUMNS_HPP
