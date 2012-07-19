// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_SRID_HPP
#define BRIG_DATABASE_DETAIL_SQL_SRID_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_srid(const DBMS sys, const identifier& tbl, const column_definition & col)
{
  if (Postgres == sys)
  {
    if (VoidColumn == col.type) return "";
    else if (col.dbms_type_lcase.name.compare("raster") == 0) return "SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'EPSG' THEN s.AUTH_SRID ELSE NULL END) epsg FROM (SELECT SRID FROM PUBLIC.RASTER_COLUMNS WHERE R_TABLE_SCHEMA = '" + tbl.schema + "' AND R_TABLE_NAME = '" + tbl.name + "' AND R_RASTER_COLUMN = '" + col.name + "') c LEFT JOIN PUBLIC.SPATIAL_REF_SYS s ON c.SRID = s.SRID";
    else if (col.dbms_type_lcase.name.compare("geography") == 0) return "SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'EPSG' THEN s.AUTH_SRID ELSE NULL END) epsg, c.TYPE FROM (SELECT SRID, TYPE FROM PUBLIC.GEOGRAPHY_COLUMNS WHERE F_TABLE_SCHEMA = '" + tbl.schema + "' AND F_TABLE_NAME = '" + tbl.name + "' AND F_GEOGRAPHY_COLUMN = '" + col.name + "') c LEFT JOIN PUBLIC.SPATIAL_REF_SYS s ON c.SRID = s.SRID";
    else if (col.dbms_type_lcase.name.compare("geometry") == 0) return "SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'EPSG' THEN s.AUTH_SRID ELSE NULL END) epsg, c.TYPE FROM (SELECT SRID, TYPE FROM PUBLIC.GEOMETRY_COLUMNS WHERE F_TABLE_SCHEMA = '" + tbl.schema + "' AND F_TABLE_NAME = '" + tbl.name + "' AND F_GEOMETRY_COLUMN = '" + col.name + "') c LEFT JOIN PUBLIC.SPATIAL_REF_SYS s ON c.SRID = s.SRID";
    else return "";
  }

  if (Geometry != col.type) return "";

  switch (sys)
  {
  default: throw std::runtime_error("SQL error");
  case DB2: return "SELECT c.SRS_ID, (CASE s.ORGANIZATION WHEN 'EPSG' THEN s.ORGANIZATION_COORDSYS_ID ELSE NULL END) epsg FROM (SELECT SRS_ID FROM DB2GSE.ST_GEOMETRY_COLUMNS WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' AND COLUMN_NAME = '" + col.name + "') c LEFT JOIN DB2GSE.ST_SPATIAL_REFERENCE_SYSTEMS s ON c.SRS_ID = s.SRS_ID";
  case Informix: return "SELECT c.srid, (CASE s.auth_name WHEN 'EPSG' THEN s.auth_srid ELSE NULL END) epsg FROM (SELECT srid FROM sde.geometry_columns WHERE f_table_schema = '" + tbl.schema + "' AND f_table_name = '" + tbl.name + "' AND f_geometry_column = '" + col.name + "') c LEFT JOIN sde.spatial_references s ON c.srid = s.srid";
  case MS_SQL: return "SELECT TOP 1 " + sql_identifier(sys, col.name) + ".STSrid FROM " + sql_identifier(sys, tbl);
  case MySQL: return "SELECT SRID(" + sql_identifier(sys, col.name) + ") FROM " + sql_identifier(sys, tbl) + " LIMIT 1";
  case Oracle: return "SELECT c.SRID, (CASE s.DATA_SOURCE WHEN 'EPSG' THEN s.SRID ELSE NULL END) epsg, s.COORD_REF_SYS_KIND FROM (SELECT * FROM MDSYS.ALL_SDO_GEOM_METADATA WHERE OWNER = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' AND COLUMN_NAME = '" + col.name + "') c LEFT JOIN MDSYS.SDO_COORD_REF_SYS s ON c.SRID = s.SRID";
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_SRID_HPP
