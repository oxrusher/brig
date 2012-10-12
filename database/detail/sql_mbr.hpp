// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_MBR_HPP
#define BRIG_DATABASE_DETAIL_SQL_MBR_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/detail/is_geodetic_type.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/global.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_mbr(const DBMS sys, const identifier& tbl, const column_definition& col)
{
  // 1. metadata
  // 2. geodetic (no sql)
  // 3. aggregate

  if (Postgres == sys)
  {
    if (VoidColumn == col.type) throw std::runtime_error("SQL error");
    else if (col.dbms_type_lcase.name.compare("raster") == 0) return "SELECT ST_XMin(t.r), ST_YMin(t.r), ST_XMax(t.r), ST_YMax(t.r) FROM (SELECT ST_Envelope(extent) r FROM raster_columns WHERE r_table_schema = '" + tbl.schema + "' AND r_table_name = '" + tbl.name + "' AND r_raster_column = '" + col.name + "') t";
    else if (col.dbms_type_lcase.name.compare("geography") == 0) return "";
    else if (col.dbms_type_lcase.name.compare("geometry") == 0) return "SELECT ST_XMin(t.r), ST_YMin(t.r), ST_XMax(t.r), ST_YMax(t.r) FROM (SELECT ST_Extent(" + sql_identifier(sys, col.name) + ") r FROM " + sql_identifier(sys, tbl) + ") t";
    else throw std::runtime_error("SQL error");
  }

  if (Geometry != col.type)
    throw std::runtime_error("SQL error");

  switch (sys)
  {
  default:
    throw std::runtime_error("SQL error");
  case DB2:
    if (is_geodetic_type(sys, col)) return "";
    else return "SELECT Min(DB2GSE.ST_MinX(" + sql_identifier(sys, col.name) + ")), Min(DB2GSE.ST_MinY(" + sql_identifier(sys, col.name) + ")), Max(DB2GSE.ST_MaxX(" + sql_identifier(sys, col.name) + ")), Max(DB2GSE.ST_MaxY(" + sql_identifier(sys, col.name) + ")) FROM " + sql_identifier(sys, tbl);
  case Informix:
    return "SELECT Min(SE_Xmin(" + sql_identifier(sys, col.name) + ")), Min(SE_Ymin(" + sql_identifier(sys, col.name) + ")), Max(SE_Xmax(" + sql_identifier(sys, col.name) + ")), Max(SE_Ymax(" + sql_identifier(sys, col.name) + ")) FROM " + sql_identifier(sys, tbl);
  case Ingres:
    return "SELECT X(PointN(t.r, 1)), Y(PointN(t.r, 1)), X(PointN(t.r, 3)), Y(PointN(t.r, 3)) FROM (SELECT ExteriorRing(Extent(" + sql_identifier(sys, col.name) + ")) r FROM " + sql_identifier(sys, tbl) + ") t";
  case MS_SQL:
    if (is_geodetic_type(sys, col)) return "";
    else return "SELECT a.bounding_box_xmin, a.bounding_box_ymin, a.bounding_box_xmax, a.bounding_box_ymax FROM sys.spatial_index_tessellations a, sys.index_columns b WHERE a.object_id = OBJECT_ID('" + sql_identifier(sys, tbl) + "') AND COL_NAME(a.object_id, b.column_id) = '" + col.name + "' AND a.index_id = b.index_id";
  case MySQL:
    return "SELECT Min(X(PointN(t.r, 1))), Min(Y(PointN(t.r, 1))), Max(X(PointN(t.r, 3))), Max(Y(PointN(t.r, 3))) FROM (SELECT ExteriorRing(Envelope(" + sql_identifier(sys, col.name) + ")) r FROM " + sql_identifier(sys, tbl) + ") t";
  case Oracle:
    {
    const std::string t("SELECT rownum n, d.sdo_lb l, d.sdo_ub u FROM (SELECT * FROM all_sdo_geom_metadata WHERE owner = '" + tbl.schema + "' AND table_name = '" + tbl.name + "' AND column_name = '" + col.name + "') m, TABLE(m.diminfo) d");
    return "SELECT x.l, y.l, x.u, y.u FROM (SELECT * FROM (" + t + ") WHERE n = 1) x, (SELECT * FROM (" + t + ") WHERE n = 2) y";
    }
  case SQLite:
    return "SELECT Min(MbrMinX(" + sql_identifier(sys, col.name) + ")), Min(MbrMinY(" + sql_identifier(sys, col.name) + ")), Max(MbrMaxX(" + sql_identifier(sys, col.name) + ")), Max(MbrMaxY(" + sql_identifier(sys, col.name) + ")) FROM " + sql_identifier(sys, tbl);
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_MBR_HPP
