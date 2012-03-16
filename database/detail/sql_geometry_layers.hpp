// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_GEOMETRY_LAYERS_HPP
#define BRIG_DATABASE_DETAIL_SQL_GEOMETRY_LAYERS_HPP

#include <brig/database/detail/sql_schema_filter.hpp>
#include <brig/database/global.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_geometry_layers(DBMS sys) // TABLE_SCHEMA, TABLE_NAME, COLUMN_NAME
{
  switch (sys)
  {
  default: throw std::runtime_error("SQL error");
  case DB2: return "SELECT DISTINCT i.TABSCHEMA scm, i.TABNAME tbl, c.COLNAME col FROM SYSCAT.INDEXES i, SYSCAT.INDEXCOLUSE c WHERE " + sql_schema_filter(sys, "i.TABSCHEMA") + " AND RTRIM(i.IESCHEMA) = 'DB2GSE' AND i.IENAME = 'SPATIAL_INDEX' AND i.INDSCHEMA = c.INDSCHEMA AND i.INDNAME = c.INDNAME ORDER BY scm, tbl, col";
  case MS_SQL: return "SELECT DISTINCT OBJECT_SCHEMA_NAME(i.object_id) scm, OBJECT_NAME(i.object_id) tbl, COL_NAME(c.object_id, c.column_id) col FROM sys.indexes i, sys.index_columns c WHERE " + sql_schema_filter(sys, "OBJECT_SCHEMA_NAME(i.object_id)") + " AND i.type = 4 AND i.object_id = c.object_id AND i.index_id = c.index_id ORDER BY scm, tbl, col";
  case MySQL: return "SELECT DISTINCT TABLE_SCHEMA scm, TABLE_NAME tbl, COLUMN_NAME col FROM INFORMATION_SCHEMA.STATISTICS WHERE " + sql_schema_filter(sys, "TABLE_SCHEMA") + " AND INDEX_TYPE = 'SPATIAL' ORDER BY scm, tbl, col";
  case Oracle: return "SELECT DISTINCT i.TABLE_OWNER scm, i.TABLE_NAME tbl, c.COLUMN_NAME col FROM (SELECT * FROM ALL_INDEXES WHERE " + sql_schema_filter(sys, "TABLE_OWNER") + " AND ITYP_OWNER = 'MDSYS' AND ITYP_NAME = 'SPATIAL_INDEX') i JOIN ALL_IND_COLUMNS c ON i.OWNER = c.INDEX_OWNER AND i.INDEX_NAME = c.INDEX_NAME ORDER BY scm, tbl, col";
  case Postgres: return "SELECT DISTINCT s.nspname scm, t.relname tbl, a.attname col FROM pg_catalog.pg_index i, pg_catalog.pg_class o, pg_catalog.pg_class t, pg_catalog.pg_namespace s, pg_catalog.pg_am m, pg_catalog.pg_attribute a WHERE i.indexrelid = o.oid AND o.relam = m.oid AND m.amname = 'gist' AND i.indrelid = t.oid AND t.relnamespace = s.oid AND " + sql_schema_filter(sys, "s.nspname") + " AND i.indrelid = a.attrelid AND i.indkey[array_lower(i.indkey, 1)] = a.attnum ORDER BY scm, tbl, col";
  case SQLite: return "SELECT '' scm, F_TABLE_NAME tbl, F_GEOMETRY_COLUMN col FROM GEOMETRY_COLUMNS WHERE SPATIAL_INDEX_ENABLED = 1 ORDER BY tbl, col";
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_GEOMETRY_LAYERS_HPP
