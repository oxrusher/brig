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
  case CUBRID: return "SELECT a.attr_name, '', t.type_name, d.prec, d.prec, d.scale, (CASE a.is_nullable WHEN 0 THEN 1 ELSE 0 END) FROM _db_class c, _db_attribute a, _db_domain d, _db_data_type t WHERE c.owner.name = '" + tbl.schema + "' AND c.class_name = '" + tbl.name + "' AND a.class_of = c AND d.object_of = a AND d.data_type = t.type_id ORDER BY a.def_order";
  case DB2: return "SELECT COLNAME, RTRIM(TYPESCHEMA), TYPENAME, LENGTH, LENGTH, SCALE, (CASE NULLS WHEN 'N' THEN 1 ELSE 0 END) FROM SYSCAT.COLUMNS WHERE TABSCHEMA = '" + tbl.schema + "' AND TABNAME = '" + tbl.name + "' ORDER BY COLNO";
  case MS_SQL:
  case MySQL: return "SELECT COLUMN_NAME, '', DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, NUMERIC_PRECISION, NUMERIC_SCALE, (CASE IS_NULLABLE WHEN 'NO' THEN 1 ELSE 0 END) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' ORDER BY ORDINAL_POSITION";
  case Oracle: return "SELECT COLUMN_NAME, DATA_TYPE_OWNER, DATA_TYPE, CHAR_LENGTH, DATA_PRECISION, DATA_SCALE, (CASE NULLABLE WHEN 'N' THEN 1 ELSE 0 END) FROM ALL_TAB_COLUMNS WHERE OWNER = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' ORDER BY COLUMN_ID";
  case Postgres: return "SELECT COLUMN_NAME, (CASE DATA_TYPE WHEN 'USER-DEFINED' THEN DATA_TYPE ELSE '' END), (CASE DATA_TYPE WHEN 'USER-DEFINED' THEN UDT_NAME ELSE DATA_TYPE END), CHARACTER_MAXIMUM_LENGTH, NUMERIC_PRECISION, NUMERIC_SCALE, (CASE IS_NULLABLE WHEN 'NO' THEN 1 ELSE 0 END) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' ORDER BY ORDINAL_POSITION";
  }
  return "";
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_COLUMNS_HPP
