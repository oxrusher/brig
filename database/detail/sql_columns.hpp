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
  // xpg4_is.sql file in the $INFORMIXDIR/etc directory
  case Informix: return
"SELECT colname, '', CASE WHEN (syscolumns.extended_id == 0) THEN CASE MOD(coltype, 256)\
  WHEN 0 THEN 'CHAR'\
  WHEN 1 THEN 'SMALLINT'\
  WHEN 2 THEN 'INTEGER'\
  WHEN 3 THEN 'FLOAT'\
  WHEN 4 THEN 'SMALLFLOAT'\
  WHEN 5 THEN 'DECIMAL'\
  WHEN 6 THEN 'SERIAL'\
  WHEN 7 THEN 'DATE'\
  WHEN 8 THEN 'MONEY'\
  WHEN 10 THEN 'DATETIME'\
  WHEN 11 THEN 'BYTE'\
  WHEN 12 THEN 'TEXT'\
  WHEN 13 THEN 'VARCHAR'\
  WHEN 14 THEN 'INTERVAL'\
  WHEN 15 THEN 'NCHAR'\
  WHEN 16 THEN 'VNCHAR'\
  WHEN 17 THEN 'INT8'\
  WHEN 18 THEN 'SERIAL8'\
  WHEN 19 THEN 'SET'\
  WHEN 20 THEN 'MULTISET'\
  WHEN 21 THEN 'LIST'\
  WHEN 22 THEN 'ROW'\
  WHEN 23 THEN 'COLLECTION'\
  WHEN 24 THEN 'ROWREF'\
  WHEN 40 THEN 'UDTVAR'\
  WHEN 41 THEN 'UDTFIXED'\
  WHEN 42 THEN 'REFSER8'\
  WHEN 52 THEN 'BIGINT'\
  WHEN 53 THEN 'BIGSERIAL'\
  ELSE NULL \
END ELSE name END AS t, CASE MOD(coltype, 256)\
  WHEN  0 THEN collength\
  WHEN 12 THEN collength\
  WHEN 13 THEN (collength - (trunc(collength / 256))*256)\
  WHEN 15 THEN collength\
  WHEN 16 THEN (collength - (trunc(collength / 256))*256)\
  ELSE NULL \
END AS l, CASE MOD(coltype, 256)\
  WHEN  5 THEN trunc(collength / 256)\
  WHEN  8 THEN trunc(collength / 256)\
  ELSE NULL \
END AS p, CASE MOD(coltype, 256)\
  WHEN  5 THEN (collength - ((trunc(collength / 256))*256))\
  WHEN  8 THEN (collength - ((trunc(collength / 256))*256))\
  ELSE NULL \
END AS s, CASE\
  WHEN (coltype-256) < 0 THEN 0\
  ELSE 1 \
END AS nn FROM systables JOIN syscolumns ON systables.tabid = syscolumns.tabid LEFT JOIN sysxtdtypes ON syscolumns.extended_id = sysxtdtypes.extended_id WHERE systables.owner = '" + tbl.schema + "' AND tabname = '" + tbl.name + "' ORDER BY colno";
  case MS_SQL:
  case MySQL: return "SELECT COLUMN_NAME, '', DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, NUMERIC_PRECISION, NUMERIC_SCALE, (CASE IS_NULLABLE WHEN 'NO' THEN 1 ELSE 0 END) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' ORDER BY ORDINAL_POSITION";
  case Oracle: return "SELECT COLUMN_NAME, DATA_TYPE_OWNER, DATA_TYPE, CHAR_LENGTH, DATA_PRECISION, DATA_SCALE, (CASE NULLABLE WHEN 'N' THEN 1 ELSE 0 END) FROM ALL_TAB_COLUMNS WHERE OWNER = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' ORDER BY COLUMN_ID";
  case Postgres: return "SELECT COLUMN_NAME, (CASE DATA_TYPE WHEN 'USER-DEFINED' THEN DATA_TYPE ELSE '' END), (CASE DATA_TYPE WHEN 'USER-DEFINED' THEN UDT_NAME ELSE DATA_TYPE END), CHARACTER_MAXIMUM_LENGTH, NUMERIC_PRECISION, NUMERIC_SCALE, (CASE IS_NULLABLE WHEN 'NO' THEN 1 ELSE 0 END) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' ORDER BY ORDINAL_POSITION";
  }
  return "";
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_COLUMNS_HPP
