// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_SRID_HPP
#define BRIG_DATABASE_DETAIL_SQL_SRID_HPP

#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_object.hpp>
#include <brig/database/global.hpp>
#include <brig/database/object.hpp>
#include <locale>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_srid(const DBMS sys, const object& tbl, const column_detail& col)
{
  using namespace boost::algorithm;

  auto loc = std::locale::classic();
  if (is_geometry_type(sys, col))
    switch (sys)
    {
    default: throw std::runtime_error("SQL error");
    case DB2: return "SELECT c.SRS_ID, (CASE s.ORGANIZATION WHEN 'EPSG' THEN s.ORGANIZATION_COORDSYS_ID ELSE NULL END) epsg FROM (SELECT SRS_ID FROM DB2GSE.ST_GEOMETRY_COLUMNS WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' AND COLUMN_NAME = '" + col.name + "') c LEFT JOIN DB2GSE.ST_SPATIAL_REFERENCE_SYSTEMS s ON c.SRS_ID = s.SRS_ID";
    case MS_SQL: return "SELECT TOP 1 " + sql_identifier(sys, col.name) + ".STSrid FROM " + sql_object(sys, tbl);
    case MySQL: return "SELECT SRID(" + sql_identifier(sys, col.name) + ") FROM " + sql_object(sys, tbl) + " LIMIT 1";
    case Oracle: return "SELECT c.SRID, (CASE s.DATA_SOURCE WHEN 'EPSG' THEN s.SRID ELSE NULL END) epsg, s.COORD_REF_SYS_KIND FROM (SELECT * FROM MDSYS.ALL_SDO_GEOM_METADATA WHERE OWNER = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' AND COLUMN_NAME = '" + col.name + "') c LEFT JOIN MDSYS.SDO_COORD_REF_SYS s ON c.SRID = s.SRID";
    case Postgres:
      {
      const bool geography(iequals(col.type.name, "GEOGRAPHY", loc));
      std::string str;
      str += "SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'EPSG' THEN s.AUTH_SRID ELSE NULL END) epsg, c.TYPE FROM (SELECT SRID, TYPE FROM PUBLIC.";
      str += (geography? "GEOGRAPHY_COLUMNS": "GEOMETRY_COLUMNS");
      str += " WHERE F_TABLE_SCHEMA = '" + tbl.schema + "' AND F_TABLE_NAME = '" + tbl.name + "' AND ";
      str += (geography? "F_GEOGRAPHY_COLUMN": "F_GEOMETRY_COLUMN");
      str += " = '" + col.name + "') c LEFT JOIN PUBLIC.SPATIAL_REF_SYS s ON c.SRID = s.SRID";
      return std::move(str);
      }
    }

  else if (Postgres == sys && iequals(col.type.schema, "USER-DEFINED", loc) && iequals(col.type.name, "RASTER", loc))
    return "SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'EPSG' THEN s.AUTH_SRID ELSE NULL END) epsg FROM (SELECT SRID FROM PUBLIC.RASTER_COLUMNS WHERE R_TABLE_SCHEMA = '" + tbl.schema + "' AND R_TABLE_NAME = '" + tbl.name + "' AND R_RASTER_COLUMN = '" + col.name + "') c LEFT JOIN PUBLIC.SPATIAL_REF_SYS s ON c.SRID = s.SRID";

  return "";
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_SRID_HPP
