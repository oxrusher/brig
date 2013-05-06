// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_POSTGRES_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_POSTGRES_HPP

#include <algorithm>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_iso_type.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/global.hpp>
#include <brig/string_cast.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

struct dialect_postgres : dialect {
  std::string sql_tables() override;
  std::string sql_geometries() override;
  std::string sql_test_rasters() override;
  std::string sql_rasters() override;
  void init_raster(pyramid_def& raster) override;

  std::string sql_columns(const identifier& tbl) override;
  std::string sql_indexed_columns(const identifier& tbl) override;
  std::string sql_spatial_detail(const table_def& tbl, const std::string& col) override;
  column_type get_type(const identifier& type_lcase, int scale) override;

  std::string sql_extent(const table_def& tbl, const std::string& col) override;

  std::string sql_schema() override;
  column_def fit_column(const column_def& col) override;
  std::string sql_srid(int epsg) override;

  std::string sql_column_def(const column_def& col) override;
  void sql_register_spatial_column(const table_def& tbl, const std::string& col, std::vector<std::string>& sql) override;
  void sql_unregister_spatial_column(const identifier& layer, std::vector<std::string>& sql) override;
  std::string sql_create_spatial_index(const table_def& tbl, const std::string& col) override;

  std::string sql_parameter(command* cmd, const column_def& param, size_t order) override;
  std::string sql_column(command* cmd, const column_def& col) override;
  void sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix) override;
  bool need_to_normalize_hemisphere(const column_def& col) override;
  std::string sql_intersect(const table_def& tbl, const std::string& col, const boost::box& box) override;
}; // dialect_postgres

inline std::string dialect_postgres::sql_tables()
{
  return "SELECT TABLE_SCHEMA scm, TABLE_NAME tbl FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA NOT SIMILAR TO E'(information\\_schema)|(pg\\_%)' AND TABLE_TYPE <> 'VIEW'";
}

inline std::string dialect_postgres::sql_geometries()
{
  return "\
SELECT g.* \
FROM (SELECT f_table_schema scm, f_table_name tbl, f_geometry_column col FROM geometry_columns UNION ALL SELECT f_table_schema scm, f_table_name tbl, f_geography_column col FROM geography_columns) g \
JOIN pg_namespace s ON g.scm = s.nspname \
JOIN pg_class t ON s.oid = t.relnamespace AND g.tbl = t.relname AND t.relkind = 'r' \
JOIN pg_attribute c ON t.oid = c.attrelid AND c.attname = g.col AND NOT c.attisdropped";
}

inline std::string dialect_postgres::sql_columns(const identifier& tbl)
{
  return "\
SELECT \
  COLUMN_NAME \
, (CASE DATA_TYPE WHEN 'USER-DEFINED' THEN DATA_TYPE ELSE '' END) \
, (CASE DATA_TYPE WHEN 'USER-DEFINED' THEN UDT_NAME ELSE DATA_TYPE END) \
, CHARACTER_MAXIMUM_LENGTH \
, NUMERIC_SCALE \
, (CASE IS_NULLABLE WHEN 'NO' THEN 1 ELSE 0 END) \
FROM INFORMATION_SCHEMA.COLUMNS \
WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' \
ORDER BY ORDINAL_POSITION";
}

inline std::string dialect_postgres::sql_indexed_columns(const identifier& tbl)
{
  return "\
SELECT scm, name, pri, unq, m.amname = 'gist', a.attname, opt & 1 FROM \
(SELECT y.*, y.keys[gs] AS key, y.opts[gs] AS opt FROM \
 (SELECT x.*, generate_series(x.lb, x.ub) AS gs FROM \
  (SELECT i.indisprimary AS pri, i.indisunique AS unq, i.indkey AS keys, i.indoption opts, array_lower(i.indkey, 1) AS lb, array_upper(i.indkey, 1) AS ub, o.relname AS name, o.relam AS mth, t.oid tbl, s.nspname AS scm FROM \
   pg_catalog.pg_index i, pg_catalog.pg_class o, pg_catalog.pg_class t, pg_catalog.pg_namespace s \
   WHERE i.indexrelid = o.oid AND i.indrelid = t.oid AND t.relnamespace = s.oid AND s.nspname = '" + tbl.schema + "' AND t.relname = '" + tbl.name + "' \
  ) AS x \
 ) AS y \
) AS z \
, pg_catalog.pg_am m \
, pg_catalog.pg_attribute a \
WHERE m.oid = mth AND a.attrelid = tbl AND a.attnum = key \
ORDER BY pri DESC, scm, name, gs";
}

inline std::string dialect_postgres::sql_spatial_detail(const table_def& tbl, const std::string& col)
{
  const std::string& dbms_type_name_lcase(tbl[col]->type_lcase.name);
  if (dbms_type_name_lcase.compare("raster") == 0)
    return "\
SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'EPSG' THEN s.AUTH_SRID ELSE NULL END) epsg \
FROM (SELECT SRID FROM PUBLIC.RASTER_COLUMNS WHERE R_TABLE_SCHEMA = '" + tbl.id.schema + "' AND R_TABLE_NAME = '" + tbl.id.name + "' AND R_RASTER_COLUMN = '" + col + "') c \
LEFT JOIN PUBLIC.SPATIAL_REF_SYS s ON c.SRID = s.SRID";
  if (dbms_type_name_lcase.compare("geography") == 0)
    return "\
SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'EPSG' THEN s.AUTH_SRID ELSE NULL END) epsg, c.TYPE \
FROM (SELECT SRID, TYPE FROM PUBLIC.GEOGRAPHY_COLUMNS WHERE F_TABLE_SCHEMA = '" + tbl.id.schema + "' AND F_TABLE_NAME = '" + tbl.id.name + "' AND F_GEOGRAPHY_COLUMN = '" + col + "') c \
LEFT JOIN PUBLIC.SPATIAL_REF_SYS s ON c.SRID = s.SRID";
  if (dbms_type_name_lcase.compare("geometry") == 0)
    return "\
SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'EPSG' THEN s.AUTH_SRID ELSE NULL END) epsg, c.TYPE \
FROM (SELECT SRID, TYPE FROM PUBLIC.GEOMETRY_COLUMNS WHERE F_TABLE_SCHEMA = '" + tbl.id.schema + "' AND F_TABLE_NAME = '" + tbl.id.name + "' AND F_GEOMETRY_COLUMN = '" + col + "') c \
LEFT JOIN PUBLIC.SPATIAL_REF_SYS s ON c.SRID = s.SRID";
  throw std::runtime_error("datatype error");
}

inline column_type dialect_postgres::get_type(const identifier& type_lcase, int scale)
{
  using namespace std;

  if ( type_lcase.schema.compare("user-defined") == 0
    && (type_lcase.name.compare("raster") == 0 || type_lcase.name.compare("geography") == 0 || type_lcase.name.compare("geometry") == 0)
    && (type_lcase.qualifier.empty() || is_ogc_type(type_lcase.qualifier))
     )
    return column_type::Geometry;
  if (!type_lcase.schema.empty()) return column_type::Void;
  if (type_lcase.name.find("serial") != string::npos) return column_type::Integer;
  if (type_lcase.name.compare("bytea") == 0) return column_type::Blob;
  if (type_lcase.name.find("array") != string::npos && type_lcase.name.find("char") == string::npos && type_lcase.name.find("text") == string::npos) return column_type::Void;
  return get_iso_type(type_lcase.name, scale);
}

inline std::string dialect_postgres::sql_extent(const table_def& tbl, const std::string& col)
{
  const std::string& dbms_type_name_lcase(tbl[col]->type_lcase.name);
  if (dbms_type_name_lcase.compare("raster") == 0)
    return "\
SELECT ST_XMin(t.r), ST_YMin(t.r), ST_XMax(t.r), ST_YMax(t.r) \
FROM (SELECT ST_Envelope(extent) r FROM raster_columns \
WHERE r_table_schema = '" + tbl.id.schema + "' AND r_table_name = '" + tbl.id.name + "' AND r_raster_column = '" + col + "') t";
  if (dbms_type_name_lcase.compare("geography") == 0)
    return "";
  if (dbms_type_name_lcase.compare("geometry") == 0)
    return "SELECT ST_XMin(t.r), ST_YMin(t.r), ST_XMax(t.r), ST_YMax(t.r) FROM (SELECT ST_Extent(" + sql_identifier(col) + ") r FROM " + sql_identifier(tbl.id) + ") t";
  throw std::runtime_error("datatype error");
}

inline std::string dialect_postgres::sql_schema()
{
  return "SELECT current_schema()";
}

inline column_def dialect_postgres::fit_column(const column_def& col)
{
  column_def res;
  res.name = fit_identifier(col.name);
  res.type = col.type;
  switch (res.type)
  {
  case column_type::Void: break;
  case column_type::Blob: res.type_lcase.name = "bytea"; break;
  case column_type::Double: res.type_lcase.name = "double precision"; break;
  case column_type::Geometry:
    res.type_lcase.schema = "user-defined";
    res.type_lcase.name = "geometry";
    res.epsg = col.epsg;
    break;
  case column_type::Integer: res.type_lcase.name = "bigint"; break;
  case column_type::String:
    res.chars = (col.chars > 0 && col.chars < CharsLimit)? col.chars: CharsLimit;
    res.type_lcase.name = "varchar(" + string_cast<char>(res.chars) + ")";
    break;
  }
  if (col.not_null) res.not_null = true;
  return res;
}

inline std::string dialect_postgres::sql_srid(int epsg)
{
  return "SELECT srid FROM public.spatial_ref_sys WHERE auth_name = 'EPSG' AND auth_srid = " + string_cast<char>(epsg) + " ORDER BY srid FETCH FIRST 1 ROWS ONLY";
}

inline std::string dialect_postgres::sql_column_def(const column_def& col)
{
  return column_type::Geometry == col.type? "": dialect::sql_column_def(col);
}

inline void dialect_postgres::sql_register_spatial_column(const table_def& tbl, const std::string& col, std::vector<std::string>& sql)
{
  sql.push_back("SELECT AddGeometryColumn('" + tbl.id.name + "', '" + col + "', " + string_cast<char>(tbl[col]->srid) + ", 'GEOMETRY', 2)");
}

inline void dialect_postgres::sql_unregister_spatial_column(const identifier& layer, std::vector<std::string>& sql)
{
  sql.push_back("SELECT DropGeometryColumn('" + layer.schema + "', '" + layer.name + "', '" + layer.qualifier + "')");
}

inline std::string dialect_postgres::sql_create_spatial_index(const table_def& tbl, const std::string& col)
{
  return "CREATE INDEX " + sql_identifier(tbl.rtree(col)->id.name) + " ON " + sql_identifier(tbl.id.name) + " USING GIST(" + sql_identifier(col) + ")";
}

inline std::string dialect_postgres::sql_test_rasters()
{
  return "SELECT s.nspname scm, t.relname tbl FROM pg_namespace s, pg_class t WHERE s.nspname = 'public' AND s.oid = t.relnamespace AND t.relname = 'raster_columns'";
}

inline std::string dialect_postgres::sql_rasters()
{
  return "\
SELECT \
  COALESCE(o.r_table_schema, r.r_table_schema) base_scm \
, COALESCE(o.r_table_name, r.r_table_name) base_tbl \
, COALESCE(o.r_raster_column, r.r_raster_column) base_col \
, abs(r.scale_x) res_x \
, abs(r.scale_y) res_y \
, r.r_table_schema \
, r.r_table_name \
, r.r_raster_column \
, r.r_raster_column \
FROM public.raster_columns r \
LEFT JOIN public.raster_overviews o ON r.r_table_schema = o.o_table_schema AND r.r_table_name = o.o_table_name AND r.r_raster_column = o.o_raster_column \
JOIN pg_namespace s ON r.r_table_schema = s.nspname \
JOIN pg_class t ON s.oid = t.relnamespace AND r.r_table_name = t.relname AND t.relkind = 'r' \
JOIN pg_attribute c ON t.oid = c.attrelid AND c.attname = r.r_raster_column AND NOT c.attisdropped \
ORDER BY base_scm, base_tbl, base_col, res_x, res_y";
}

inline void dialect_postgres::init_raster(pyramid_def& raster)
{
  using namespace std;

  raster.id.qualifier += "_as_png";
  for (auto lvl(begin(raster.levels)); lvl != end(raster.levels); ++lvl)
  {
    const std::string col_name(lvl->raster.name);
    lvl->raster.name = col_name + "_as_png";
    lvl->raster.query_expression = "ST_AsPNG(" + sql_identifier(col_name) + ")";
  }
}

inline std::string dialect_postgres::sql_parameter(command* cmd, const column_def& param, size_t order)
{
  using namespace std;

  const string marker(cmd->sql_param(order));
  if (column_type::Geometry == param.type && !cmd->writable_geom())
  {
    if (param.type_lcase.name.compare("geography") == 0)
    {
      if (param.srid != 4326) throw runtime_error("SRID error");
      return "ST_GeogFromWKB(" + marker + ")";
    }
    if (param.type_lcase.name.compare("geometry") == 0) return "ST_GeomFromWKB(" + marker + ", " + string_cast<char>(param.srid) + ")";
    throw runtime_error("datatype error");
  }
  return marker;
}

inline std::string dialect_postgres::sql_column(command* cmd, const column_def& col)
{
  using namespace std;

  const string id(sql_identifier(col.name));
  if (!col.query_expression.empty()) return col.query_expression + " AS " + id;
  if (column_type::String == col.type && col.type_lcase.name.find("time") != string::npos) return "(TO_CHAR(" + id + ", 'YYYY-MM-DD') || 'T' || TO_CHAR(" + id + ", 'HH24:MI:SS')) AS " + id;
  if (column_type::String == col.type && col.type_lcase.name.find("date") != string::npos) return "TO_CHAR(" + id + ", 'YYYY-MM-DD') AS " + id;
  if (column_type::Geometry == col.type && !cmd->readable_geom())
  {
    if (col.type_lcase.name.compare("raster") == 0) return "ST_AsBinary(ST_ConvexHull(" + id + ")) AS " + id;
    if (col.type_lcase.name.compare("geography") == 0 || col.type_lcase.name.compare("geometry") == 0) return "ST_AsBinary(" + id + ") AS " + id;
    throw runtime_error("datatype error");
  }
  return id;
}

inline void dialect_postgres::sql_limit(int rows, std::string&, std::string&, std::string& sql_suffix)
{
  sql_suffix = "FETCH FIRST " + string_cast<char>(rows) + " ROWS ONLY"; // SQL:2008
}

inline bool dialect_postgres::need_to_normalize_hemisphere(const column_def& col)
{
  return col.type_lcase.name.compare("geography") == 0;
}

inline std::string dialect_postgres::sql_intersect(const table_def& tbl, const std::string& col, const boost::box& box)
{
  using namespace std;

  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  auto col_def(tbl[col]);
  bool geography(false), raster(false);
  if (col_def->type_lcase.name.compare("raster") == 0) raster = true;
  else if (col_def->type_lcase.name.compare("geography") == 0) geography = true;
  else if (col_def->type_lcase.name.compare("geometry") != 0) throw runtime_error("datatype error");
  if (geography && col_def->srid != 4326) throw runtime_error("SRID error");
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);

  if (raster) stream << "ST_Envelope(";
  stream << sql_identifier(col);
  if (raster) stream << ")";
  stream << " && ";
  if (geography) stream << "ST_GeogFromWKB(ST_AsBinary(";
  stream << "ST_SetSRID(ST_MakeBox2D(ST_Point(" << xmin << ", " << ymin << "), ST_Point(" << xmax << ", " << ymax << ")), " << col_def->srid << ")";
  if (geography) stream << "))";
  return stream.str();
} // dialect_postgres::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_POSTGRES_HPP
