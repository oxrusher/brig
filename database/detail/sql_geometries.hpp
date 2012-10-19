// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_GEOMETRIES_HPP
#define BRIG_DATABASE_DETAIL_SQL_GEOMETRIES_HPP

#include <brig/database/global.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_geometries(DBMS sys, bool order_by)
{
  std::string sql;
  switch (sys)
  {
  case VoidSystem:
  case CUBRID: throw std::runtime_error("DBMS error");
  case DB2: sql = "SELECT TABLE_SCHEMA scm, TABLE_NAME tbl, COLUMN_NAME col FROM DB2GSE.ST_GEOMETRY_COLUMNS"; break;
  case Informix: sql = "SELECT g.scm, g.tbl, g.col FROM (SELECT f_table_schema scm, f_table_name tbl, f_geometry_column col FROM sde.geometry_columns) g JOIN (SELECT owner scm, tabname tbl, colname col FROM systables JOIN syscolumns ON systables.tabid = syscolumns.tabid) c ON g.scm = c.scm AND g.tbl = c.tbl AND g.col = c.col"; break;
  case Ingres: sql = "SELECT g.scm, g.tbl, g.col FROM (SELECT f_table_schema scm, f_table_name tbl, f_geometry_column col FROM geometry_columns) g JOIN (SELECT table_owner scm, table_name tbl, column_name col FROM iicolumns) c ON g.scm = c.scm AND g.tbl = c.tbl AND g.col = c.col"; break;
  case MS_SQL: sql = "SELECT TABLE_SCHEMA scm, TABLE_NAME tbl, COLUMN_NAME col FROM INFORMATION_SCHEMA.COLUMNS WHERE DATA_TYPE IN ('geometry', 'geography')"; break;
  case MySQL: sql = "SELECT TABLE_SCHEMA scm, TABLE_NAME tbl, COLUMN_NAME col FROM INFORMATION_SCHEMA.COLUMNS WHERE DATA_TYPE IN ('geometry', 'point', 'curve', 'linestring', 'surface', 'polygon', 'geometrycollection', 'multipoint', 'multicurve', 'multilinestring', 'multisurface', 'multipolygon')"; break;
  case Oracle: sql = "SELECT g.scm, g.tbl, g.col FROM (SELECT OWNER scm, TABLE_NAME tbl, COLUMN_NAME col FROM MDSYS.ALL_SDO_GEOM_METADATA) g JOIN (SELECT OWNER scm, TABLE_NAME tbl, COLUMN_NAME col FROM ALL_TAB_COLUMNS) c ON g.scm = c.scm AND g.tbl = c.tbl AND g.col = c.col"; break;
  case Postgres: sql = "SELECT g.scm, g.tbl, g.col FROM (SELECT f_table_schema scm, f_table_name tbl, f_geometry_column col FROM geometry_columns UNION ALL SELECT f_table_schema scm, f_table_name tbl, f_geography_column col FROM geography_columns) g JOIN (SELECT table_schema scm, table_name tbl, column_name col FROM INFORMATION_SCHEMA.COLUMNS) c ON g.scm = c.scm AND g.tbl = c.tbl AND g.col = c.col"; break;
  case SQLite: sql = "SELECT '' scm, F_TABLE_NAME tbl, F_GEOMETRY_COLUMN col FROM GEOMETRY_COLUMNS"; break;
  }
  if (order_by) sql += " ORDER BY scm, tbl, col";
  return sql;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_GEOMETRIES_HPP
