// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_INGRES_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_INGRES_HPP

#include <algorithm>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_iso_type.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/global.hpp>
#include <brig/string_cast.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

struct dialect_ingres : dialect {
  std::string sql_tables() override;
  std::string sql_geometries() override;

  std::string sql_columns(const identifier& tbl) override;
  std::string sql_indexed_columns(const identifier& tbl) override;
  std::string sql_spatial_detail(const table_def& tbl, const std::string& col) override;
  column_type get_type(const identifier& type_lcase, int scale) override;

  std::string sql_extent(const table_def& tbl, const std::string& col) override;

  std::string sql_schema() override;
  std::string fit_identifier(const std::string& id) override;
  column_def fit_column(const column_def& col) override;
  table_def fit_table(const table_def& tbl, const std::string& schema) override;
  std::string sql_srid(int epsg) override;

  std::string sql_column_def(const column_def& col) override;
  std::string sql_create_spatial_index(const table_def& tbl, const std::string& col) override;

  std::string sql_parameter(command* cmd, const column_def& param, size_t order) override;
  std::string sql_column(command* cmd, const column_def& col) override;
  void sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix) override;
  std::string sql_intersect(const table_def& tbl, const std::string& col, const boost::box& box) override;
}; // dialect_ingres

inline std::string dialect_ingres::sql_tables()
{
  return "SELECT RTRIM(table_owner) scm, RTRIM(table_name) tbl FROM iitables WHERE table_owner <> '$ingres' AND table_type = 'T' AND system_use = 'U' AND table_name NOT LIKE 'iietab_%'";
}

inline std::string dialect_ingres::sql_geometries()
{
  return "\
SELECT g.scm scm, g.tbl tbl, g.col col \
FROM (SELECT f_table_schema scm, f_table_name tbl, f_geometry_column col FROM geometry_columns) g \
JOIN (SELECT table_owner scm, table_name tbl, column_name col FROM iicolumns) c \
ON g.scm = c.scm AND g.tbl = c.tbl AND g.col = c.col";
}

inline std::string dialect_ingres::sql_columns(const identifier& tbl)
{
  return "\
SELECT RTRIM(column_name), '',  RTRIM(column_datatype), column_length, column_scale, (CASE column_nulls WHEN 'N' THEN 1 ELSE 0 END) \
FROM iicolumns \
WHERE table_owner = '" + tbl.schema + "' AND table_name = '" + tbl.name + "' \
ORDER BY column_sequence";
}

inline std::string dialect_ingres::sql_indexed_columns(const identifier& tbl)
{
  return "\
SELECT \
  RTRIM(csi.schema_name) scm \
, RTRIM(csi.index_name) idx \
, 1 pri \
, 1 unq \
, 0 sp \
, RTRIM(c.column_name) col \
, 0 dsc \
, c.key_position pos \
FROM (SELECT * FROM iiconstraints WHERE constraint_type = 'P' AND schema_name = '" + tbl.schema + "' AND table_name = '" + tbl.name + "') cs \
JOIN iiconstraint_indexes csi ON cs.schema_name = csi.schema_name AND cs.constraint_name = csi.constraint_name \
JOIN iikeys c ON cs.schema_name = c.schema_name AND cs.constraint_name = c.constraint_name \
UNION ALL \
SELECT \
  RTRIM(i.index_owner) scm \
, RTRIM(i.index_name) idx \
, 0 pri \
, (CASE i.unique_rule WHEN 'U' THEN 1 ELSE 0 END) unq \
, (CASE i.storage_structure WHEN 'RTREE' THEN 1 ELSE 0 END) sp \
, RTRIM(c.column_name) \
, (CASE ic.sort_direction WHEN 'A' THEN 0 ELSE 1 END) dsc \
, ic.key_sequence pos \
FROM (SELECT * FROM iiindexes WHERE base_owner = '" + tbl.schema + "' AND base_name = '" + tbl.name + "') i \
JOIN iiindex_columns ic ON i.index_owner = ic.index_owner AND i.index_name = ic.index_name \
JOIN iicolumns c ON i.base_owner = c.table_owner AND i.base_name = c.table_name AND ic.column_name = c.column_name \
ORDER BY \
  pri DESC \
, scm \
, idx \
, pos";
}

inline std::string dialect_ingres::sql_spatial_detail(const table_def& tbl, const std::string& col)
{
  return "\
SELECT c.srid, (CASE s.auth_name WHEN 'EPSG' THEN s.auth_srid ELSE NULL END) epsg \
FROM (SELECT srid FROM geometry_columns WHERE f_table_schema = '" + tbl.id.schema + "' AND f_table_name = '" + tbl.id.name + "' AND f_geometry_column = '" + col + "') c \
LEFT JOIN spatial_ref_sys s ON c.srid = s.srid";
}

inline column_type dialect_ingres::get_type(const identifier& type_lcase, int scale)
{
  if (!type_lcase.schema.empty()) return VoidColumn;
  if (is_ogc_type(type_lcase.name)) return Geometry;
  if (type_lcase.name.find("byte") != std::string::npos) return Blob;
  if (type_lcase.name.compare("c") == 0) return String;
  return get_iso_type(type_lcase.name, scale);
}

inline std::string dialect_ingres::sql_extent(const table_def& tbl, const std::string& col)
{
  return "SELECT X(PointN(t.r, 1)), Y(PointN(t.r, 1)), X(PointN(t.r, 3)), Y(PointN(t.r, 3)) FROM (SELECT ExteriorRing(Extent(" + sql_identifier(col) + ")) r FROM " + sql_identifier(tbl.id) + ") t";
}

inline std::string dialect_ingres::sql_schema()
{
  return "SELECT dbmsinfo('session_user')";
}

inline std::string dialect_ingres::fit_identifier(const std::string& id)
{
  return brig::unicode::transform<char>(id, brig::unicode::lower_case);
}

inline column_def dialect_ingres::fit_column(const column_def& col)
{
  column_def res;
  res.name = fit_identifier(col.name);
  res.type = col.type;
  switch (res.type)
  {
  case VoidColumn: break;
  case Blob: res.type_lcase.name = "long byte"; break;
  case Double: res.type_lcase.name = "double precision"; break;
  case Geometry:
    res.type_lcase.name = "geometry";
    res.epsg = col.epsg;
    // todo: invalid spatial index
    // res.query_value = (typeid(blob_t) == col.query_value.type())? col.query_value: blob_t();
    break;
  case Integer: res.type_lcase.name = "bigint"; break;
  case String:
    res.chars = (col.chars > 0 && col.chars < CharsLimit)? col.chars: CharsLimit;
    res.type_lcase.name = "nvarchar(" + string_cast<char>(res.chars) + ")";
    break;
  }
  if (col.not_null) res.not_null = true;
  return res;
}

inline table_def dialect_ingres::fit_table(const table_def& tbl, const std::string& schema)
{
  using namespace std;

  table_def res(dialect::fit_table(tbl, schema));

  // todo: invalid spatial index
  auto new_end(remove_if(begin(res.indexes), end(res.indexes), [](const index_def& idx){ return Spatial == idx.type; }));
  res.indexes.resize(distance(begin(res.indexes), new_end));

  return res;
}

inline std::string dialect_ingres::sql_srid(int epsg)
{
  return "SELECT TOP 1 srid FROM spatial_ref_sys WHERE auth_name = 'EPSG' AND auth_srid = " + string_cast<char>(epsg) + " ORDER BY srid";
}

inline std::string dialect_ingres::sql_column_def(const column_def& col)
{
  std::string str;
  str += sql_identifier(col.name) + " " + col.type_lcase.to_string();
  if (Geometry == col.type) str += " SRID " + string_cast<char>(col.srid);
  if (col.not_null) str += " NOT NULL";
  return str;
}

inline std::string dialect_ingres::sql_create_spatial_index(const table_def& tbl, const std::string& col)
{
  using namespace std;
  using namespace brig::boost;

  auto box(envelope(geom_from_wkb(::boost::get<blob_t>(tbl[col]->query_value))));
  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
  stream << "\
CREATE INDEX " << sql_identifier(tbl.rtree(col)->id.name) << " ON " << sql_identifier(tbl.id.name) << " (" << sql_identifier(col) << ") \
WITH STRUCTURE=RTREE, RANGE=((" << xmin << ", " << ymin << "), (" << xmax << ", " << ymax << "))";
  return stream.str();
}

inline std::string dialect_ingres::sql_parameter(command* cmd, const column_def& param, size_t order)
{
  const std::string marker(cmd->sql_param(order));
  if (Geometry == param.type && !cmd->writable_geom()) return "GeomFromWKB(" + marker + ", " + string_cast<char>(param.srid) + ")";
  return marker;
}

inline std::string dialect_ingres::sql_column(command* cmd, const column_def& col)
{
  using namespace std;

  const string id(sql_identifier(col.name));
  if (!col.query_expression.empty()) return col.query_expression + " AS " + id;
  if (String == col.type && (col.type_lcase.name.find("time") != string::npos || col.type_lcase.name.compare("ingresdate") == 0)) return "DATE_FORMAT(" + id + ", '%Y-%m-%dT%T') AS " + id;
  if (String == col.type && col.type_lcase.name.find("date") != string::npos) return "DATE_FORMAT(" + id + ", '%Y-%m-%d') AS " + id;
  if (Geometry == col.type && !cmd->readable_geom()) return "AsBinary(" + id + ") AS " + id;
  return id;
}

inline void dialect_ingres::sql_limit(int rows, std::string& sql_infix, std::string&, std::string&)
{
  sql_infix = "TOP " + string_cast<char>(rows);
}

inline std::string dialect_ingres::sql_intersect(const table_def& tbl, const std::string& col, const boost::box& box)
{
  using namespace std;

  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
  stream << "Intersects(" << sql_identifier(col) << ", Envelope(LineFromText('LINESTRING(" << xmin << " " << ymin << ", " << xmax << " " << ymax << ")', " << tbl[col]->srid << "))) = 1";
  return stream.str();
} // dialect_ingres::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_INGRES_HPP
