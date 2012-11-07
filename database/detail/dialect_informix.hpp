// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_INFORMIX_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_INFORMIX_HPP

#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_iso_type.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/database/global.hpp>
#include <brig/string_cast.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

struct dialect_informix : dialect {
  std::string sql_tables() override;
  std::string sql_geometries() override;

  std::string sql_columns(const identifier& tbl) override;
  std::string sql_indexed_columns(const identifier& tbl) override;
  std::string sql_spatial_detail(const table_definition& tbl, const std::string& col) override;
  column_type get_type(const identifier& dbms_type_lcase, int scale) override;

  std::string sql_mbr(const table_definition& tbl, const std::string& col) override;

  std::string sql_schema() override;
  column_definition fit_column(const column_definition& col) override;
  std::string sql_srid(int epsg) override;

  void sql_register_spatial_column(const table_definition& tbl, const std::string& col, std::vector<std::string>& sql) override;
  void sql_unregister_spatial_column(const identifier& layer, std::vector<std::string>& sql) override;
  std::string sql_create_spatial_index(const table_definition& tbl, const std::string& col) override;

  std::string sql_parameter(const command_traits& trs, const column_definition& param, size_t order) override;
  std::string sql_column(const command_traits& trs, const column_definition& col) override;
  void sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix) override;
  std::string sql_intersect(const table_definition& tbl, const std::string& col, const boost::box& box) override;

  static std::string sql_catalog();
}; // dialect_informix

inline std::string dialect_informix::sql_catalog()
{
  return "(SELECT RTRIM(ODB_DBName) DB FROM sysmaster:SysOpenDB WHERE CAST(ODB_SessionID AS INT) = CAST(DBINFO('sessionid') AS INT) AND ODB_IsCurrent = 'Y')";
}

inline std::string dialect_informix::sql_tables()
{
  return "SELECT RTRIM(owner) scm, tabname tbl FROM systables WHERE RTRIM(owner) <> 'sde' AND tabtype = 'T' AND tabname NOT LIKE 'sys%'";
}

inline std::string dialect_informix::sql_geometries()
{
  return "\
SELECT g.scm scm, g.tbl tbl, g.col col \
FROM (SELECT f_table_schema scm, f_table_name tbl, f_geometry_column col FROM sde.geometry_columns) g \
JOIN (SELECT owner scm, tabname tbl, colname col FROM systables JOIN syscolumns ON systables.tabid = syscolumns.tabid) c \
ON g.scm = c.scm AND g.tbl = c.tbl AND g.col = c.col";
}

inline std::string dialect_informix::sql_columns(const identifier& tbl)
{
  return "\
SELECT colname, '', CASE WHEN (syscolumns.extended_id == 0) THEN CASE MOD(coltype, 256) \
  WHEN 0 THEN 'CHAR' \
  WHEN 1 THEN 'SMALLINT' \
  WHEN 2 THEN 'INTEGER' \
  WHEN 3 THEN 'FLOAT' \
  WHEN 4 THEN 'SMALLFLOAT' \
  WHEN 5 THEN 'DECIMAL' \
  WHEN 6 THEN 'SERIAL' \
  WHEN 7 THEN 'DATE' \
  WHEN 8 THEN 'MONEY' \
  WHEN 10 THEN 'DATETIME' \
  WHEN 11 THEN 'BYTE' \
  WHEN 12 THEN 'TEXT' \
  WHEN 13 THEN 'VARCHAR' \
  WHEN 14 THEN 'INTERVAL' \
  WHEN 15 THEN 'NCHAR' \
  WHEN 16 THEN 'VNCHAR' \
  WHEN 17 THEN 'INT8' \
  WHEN 18 THEN 'SERIAL8' \
  WHEN 19 THEN 'SET' \
  WHEN 20 THEN 'MULTISET' \
  WHEN 21 THEN 'LIST' \
  WHEN 22 THEN 'ROW' \
  WHEN 23 THEN 'COLLECTION' \
  WHEN 24 THEN 'ROWREF' \
  WHEN 40 THEN 'UDTVAR' \
  WHEN 41 THEN 'UDTFIXED' \
  WHEN 42 THEN 'REFSER8' \
  WHEN 52 THEN 'BIGINT' \
  WHEN 53 THEN 'BIGSERIAL' \
  ELSE NULL \
END ELSE name END AS t, CASE MOD(coltype, 256) \
  WHEN  0 THEN collength \
  WHEN 12 THEN collength \
  WHEN 13 THEN (collength - (trunc(collength / 256))*256) \
  WHEN 15 THEN collength \
  WHEN 16 THEN (collength - (trunc(collength / 256))*256) \
  ELSE NULL \
END AS l, CASE MOD(coltype, 256) \
  WHEN  5 THEN (collength - ((trunc(collength / 256))*256)) \
  WHEN  8 THEN (collength - ((trunc(collength / 256))*256)) \
  ELSE NULL \
END AS s, CASE \
  WHEN (coltype-256) < 0 THEN 0 \
  ELSE 1 \
END AS nn \
FROM systables \
JOIN syscolumns ON systables.tabid = syscolumns.tabid \
LEFT JOIN sysxtdtypes ON syscolumns.extended_id = sysxtdtypes.extended_id WHERE systables.owner = '" + tbl.schema + "' AND tabname = '" + tbl.name + "' \
ORDER BY colno";
}

inline std::string dialect_informix::sql_indexed_columns(const identifier& tbl)
{
  using namespace std;

  ostringstream stream; stream.imbue(locale::classic());
  for (int i(0); i < 16; ++i)
  {
    if (i > 0) stream << " UNION ALL ";
    stream << "\
SELECT RTRIM(i.owner), i.idxname, (CASE cs.constrtype WHEN 'P' THEN 1 ELSE 0 END) pri, (CASE i.idxtype WHEN 'U' THEN 1 ELSE 0 END) unq, 0 sp, c.colname, i.dsc, " << i << " keyno \
FROM (SELECT idxname, owner, tabid, idxtype, ABS(ikeyextractcolno(indexkeys, " << i << ")) colno, (CASE WHEN ikeyextractcolno(indexkeys, " << i << ") < 0 THEN 1 ELSE 0 END) dsc FROM sysindices) i \
JOIN (SELECT tabid FROM systables WHERE owner ='" << tbl.schema << "' AND tabname = '" << tbl.name << "') t ON i.tabid = t.tabid \
JOIN (SELECT tabid, colno, colname FROM syscolumns) c ON (i.tabid = c.tabid AND i.colno = c.colno) \
LEFT JOIN (SELECT tabid, idxname, constrtype FROM sysconstraints) cs ON (i.tabid = cs.tabid AND i.idxname = cs.idxname)";
  }
  stream << " UNION ALL ";
  stream << "\
SELECT RTRIM(i.owner), i.idxname, 0 pri, 0 unq, 1 sp, c.colname, 0 dsc, 0 keyno \
FROM \
  (SELECT idxname, owner, tabid, CAST(SUBSTRING(CAST(indexkeys AS lvarchar) FROM 1 FOR 2) AS SMALLINT) colno, amid FROM sysindices) i \
, (SELECT tabid FROM systables WHERE owner = 'informix' AND tabname = 't') t \
, (SELECT am_id FROM sysams WHERE am_name = 'rtree') m \
, syscolumns c \
WHERE i.tabid = t.tabid AND i.amid = m.am_id AND i.tabid = c.tabid AND i.colno = c.colno";
  stream << " ORDER BY 3 DESC, 1, 2, 7";
  return stream.str();
}

inline std::string dialect_informix::sql_spatial_detail(const table_definition& tbl, const std::string& col)
{
  return "\
SELECT c.srid, (CASE s.auth_name WHEN 'EPSG' THEN s.auth_srid ELSE NULL END) epsg \
FROM (SELECT srid FROM sde.geometry_columns WHERE f_table_schema = '" + tbl.id.schema + "' AND f_table_name = '" + tbl.id.name + "' AND f_geometry_column = '" + col + "') c \
LEFT JOIN sde.spatial_references s ON c.srid = s.srid";
}

inline column_type dialect_informix::get_type(const identifier& dbms_type_lcase, int scale)
{
  if (!dbms_type_lcase.schema.empty()) return VoidColumn;
  if (is_ogc_type(dbms_type_lcase.name)) return Geometry;
  if (dbms_type_lcase.name.find("serial") != std::string::npos) return Integer;
  if (dbms_type_lcase.name.compare("byte") == 0) return Blob;
  return get_iso_type(dbms_type_lcase.name, scale);
}

inline std::string dialect_informix::sql_mbr(const table_definition& tbl, const std::string& col)
{
  const std::string c(sql_identifier(col));
  const std::string t(sql_identifier(tbl.id));
  return "SELECT Min(SE_Xmin(" + c + ")), Min(SE_Ymin(" + c + ")), Max(SE_Xmax(" + c + ")), Max(SE_Ymax(" + c + ")) FROM " + t;
}

inline std::string dialect_informix::sql_schema()
{
  return "SELECT RTRIM(USER) FROM sysmaster:systables WHERE tabid = 1";
}

inline column_definition dialect_informix::fit_column(const column_definition& col)
{
  using namespace std;

  column_definition res;
  res.name = fit_identifier(col.name);
  res.type = col.type;
  switch (res.type)
  {
  case VoidColumn: throw runtime_error("datatype error");
  case Blob: res.dbms_type_lcase.name = "byte"; break;
  case Double: res.dbms_type_lcase.name = "double precision"; break;
  case Geometry:
    res.dbms_type_lcase.name = "st_geometry";
    res.epsg = col.epsg;
    break;
  case Integer: res.dbms_type_lcase.name = "int8"; break;
  case String:
    res.chars = (col.chars > 0 && col.chars < CharsLimit)? col.chars: CharsLimit;
    res.dbms_type_lcase.name = "varchar(" + string_cast<char>(res.chars) + ")";
    break;
  }
  if (col.not_null) res.not_null = true;
  return res;
}

inline std::string dialect_informix::sql_srid(int epsg)
{
  return "SELECT FIRST 1 srid FROM sde.spatial_references WHERE auth_name = 'EPSG' AND auth_srid = " + string_cast<char>(epsg) + " ORDER BY srid";
}

inline void dialect_informix::sql_register_spatial_column(const table_definition& tbl, const std::string& col, std::vector<std::string>& sql)
{
  sql.push_back("\
DELETE FROM sde.geometry_columns \
WHERE f_table_catalog = " + sql_catalog() + " \
AND f_table_schema = RTRIM(USER) \
AND f_table_name = '" + tbl.id.name + "' \
AND f_geometry_column = '" + col + "'");
  sql.push_back("\
INSERT INTO sde.geometry_columns (f_table_catalog, f_table_schema, f_table_name, f_geometry_column, geometry_type, coord_dimension, srid) \
VALUES (" + sql_catalog() + ", RTRIM(USER), '" + tbl.id.name + "', '" + col + "', 0, 2, " + string_cast<char>(tbl[col]->srid) + ")");
}

inline void dialect_informix::sql_unregister_spatial_column(const identifier& layer, std::vector<std::string>& sql)
{
  sql.push_back("\
DELETE FROM sde.geometry_columns \
WHERE RTRIM(f_table_catalog) = " + sql_catalog() + " \
AND RTRIM(f_table_schema) = '" + layer.schema + "' \
AND f_table_name = '" + layer.name + "' \
AND f_geometry_column = '" + layer.qualifier + "'");
}

inline std::string dialect_informix::sql_create_spatial_index(const table_definition& tbl, const std::string& col)
{
  return "CREATE INDEX " + sql_identifier(tbl.rtree(col)->id.name) + " ON " + sql_identifier(tbl.id.name) + " (" + sql_identifier(col) + " ST_Geometry_Ops) USING RTREE";
}

inline std::string dialect_informix::sql_parameter(const command_traits& trs, const column_definition& param, size_t order)
{
  using namespace std;

  const string marker(trs.sql_parameter_marker(order));
  if (Geometry == param.type && !trs.writable_geometry)
  {
    const string suffix("(" + marker + ", " + string_cast<char>(param.srid) + ")");
         if ( param.dbms_type_lcase.name.compare("st_geometry") == 0
           || param.dbms_type_lcase.name.compare("st_geomcollection") == 0 ) return "ST_GeomFromWKB" + suffix;
    else if ( param.dbms_type_lcase.name.compare("st_point") == 0 ) return "ST_PointFromWKB" + suffix;
    else if ( param.dbms_type_lcase.name.compare("st_curve") == 0
           || param.dbms_type_lcase.name.compare("st_linestring") == 0 ) return "ST_LineFromWKB" + suffix;
    else if ( param.dbms_type_lcase.name.compare("st_surface") == 0
           || param.dbms_type_lcase.name.compare("st_polygon") == 0 ) return "ST_PolyFromWKB" + suffix;
    else if ( param.dbms_type_lcase.name.compare("st_multipoint") == 0 ) return "ST_MPointFromWKB" + suffix;
    else if ( param.dbms_type_lcase.name.compare("st_multicurve") == 0
           || param.dbms_type_lcase.name.compare("st_multilinestring") == 0 ) return "ST_MLineFromWKB" + suffix;
    else if ( param.dbms_type_lcase.name.compare("st_multisurface") == 0
           || param.dbms_type_lcase.name.compare("st_multipolygon") == 0 ) return "ST_MPolyFromWKB" + suffix;
    else throw runtime_error("datatype error");
  }
  return marker;
}

inline std::string dialect_informix::sql_column(const command_traits& trs, const column_definition& col)
{
  using namespace std;

  const string id(sql_identifier(col.name));
  if (!col.query_expression.empty()) return col.query_expression + " AS " + id;
  if (String == col.type && col.dbms_type_lcase.name.find("time") != string::npos) return "(TO_CHAR(" + id + ", '%Y-%m-%d') || 'T' || TO_CHAR(" + id + ", '%H:%M:%S')) AS " + id;
  if (String == col.type && col.dbms_type_lcase.name.find("date") != string::npos) return "TO_CHAR(" + id + ", '%Y-%m-%d') AS " + id;
  if (Geometry == col.type && !trs.readable_geometry) return "ST_AsBinary(" + id + ") AS " + id;
  return id;
}

inline void dialect_informix::sql_limit(int rows, std::string& sql_infix, std::string&, std::string&)
{
  sql_infix = "FIRST " + string_cast<char>(rows);
}

inline std::string dialect_informix::sql_intersect(const table_definition& tbl, const std::string& col, const boost::box& box)
{
  using namespace std;

  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
  stream << "SE_EnvelopesIntersect(" << sql_identifier(col)
    << ", ST_Envelope(ST_Union(ST_Point(" << xmin << ", " << ymin << ", " << tbl[col]->srid << "), ST_Point(" << xmax << ", " << ymax << ", " << tbl[col]->srid << "))))";
  return stream.str();
} // dialect_informix::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_INFORMIX_HPP
