// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_MS_SQL_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_MS_SQL_HPP

#include <algorithm>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_iso_type.hpp>
#include <brig/database/detail/sql_select_list.hpp>
#include <brig/detail/get_columns.hpp>
#include <brig/global.hpp>
#include <brig/string_cast.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

struct dialect_ms_sql : dialect {
  std::string sql_identifier(const std::string& id) override  { return '[' + id + ']'; }

  std::string sql_tables() override;
  std::string sql_geometries() override;

  std::string sql_columns(const identifier& tbl) override;
  std::string sql_indexed_columns(const identifier& tbl) override;
  std::string sql_spatial_detail(const table_definition& tbl, const std::string& col) override;
  column_type get_type(const identifier& type_lcase, int scale) override;

  std::string sql_mbr(const table_definition& tbl, const std::string& col) override;

  std::string sql_schema() override;
  column_definition fit_column(const column_definition& col) override;
  table_definition fit_table(const table_definition& tbl, const std::string& schema) override;
  std::string sql_srid(int) override  { return ""; }

  std::string sql_create_spatial_index(const table_definition& tbl, const std::string& col) override;

  std::string sql_parameter(const command_traits& trs, const column_definition& param, size_t order) override;
  std::string sql_column(const command_traits& trs, const column_definition& col) override;
  void sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix) override;
  std::string sql_hint(const table_definition& tbl, const std::string& col) override;
  bool need_to_normalize_hemisphere(const column_definition& col) override;
  void sql_intersect(const command_traits& trs, const table_definition& tbl, const std::string& col, const std::vector<brig::boost::box>& boxes, std::string& sql, std::vector<column_definition>& keys) override;
  std::string sql_intersect(const table_definition& tbl, const std::string& col, const boost::box& box) override;
}; // dialect_ms_sql

inline std::string dialect_ms_sql::sql_tables()
{
  return "SELECT TABLE_SCHEMA scm, TABLE_NAME tbl FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA NOT IN ('guest','INFORMATION_SCHEMA','sys') AND TABLE_SCHEMA NOT LIKE 'db[_]%' AND TABLE_SCHEMA NOT LIKE 'sys[_]%'";
}

inline std::string dialect_ms_sql::sql_geometries()
{
  return "SELECT TABLE_SCHEMA scm, TABLE_NAME tbl, COLUMN_NAME col FROM INFORMATION_SCHEMA.COLUMNS WHERE DATA_TYPE IN ('geometry','geography')";
}

inline std::string dialect_ms_sql::sql_columns(const identifier& tbl)
{
  return "\
SELECT COLUMN_NAME, '', DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, NUMERIC_SCALE, (CASE IS_NULLABLE WHEN 'NO' THEN 1 ELSE 0 END) \
FROM INFORMATION_SCHEMA.COLUMNS \
WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' \
ORDER BY ORDINAL_POSITION";
}

inline std::string dialect_ms_sql::sql_indexed_columns(const identifier& tbl)
{
  return "\
SELECT '', i.name, i.is_primary_key, i.is_unique, (CASE i.type WHEN 4 THEN 1 ELSE 0 END) sp, COL_NAME(c.object_id, c.column_id) col, c.is_descending_key \
FROM sys.indexes i, sys.index_columns c \
WHERE i.object_id = OBJECT_ID('\"" + tbl.schema + "\".\"" + tbl.name + "\"') AND i.object_id = c.object_id AND i.index_id = c.index_id \
ORDER BY i.is_primary_key DESC, i.name, c.key_ordinal";
}

inline std::string dialect_ms_sql::sql_spatial_detail(const table_definition& tbl, const std::string& col)
{
  return "SELECT TOP 1 " + sql_identifier(col) + ".STSrid FROM " + dialect::sql_identifier(tbl.id);
}

inline column_type dialect_ms_sql::get_type(const identifier& type_lcase, int scale)
{
  if (!type_lcase.schema.empty()) return VoidColumn;
  if (type_lcase.name.compare("geometry") == 0 || type_lcase.name.compare("geography") == 0) return Geometry;
  if (type_lcase.name.compare("bit") == 0) return Integer;
  if (type_lcase.name.compare("image") == 0) return Blob;
  return get_iso_type(type_lcase.name, scale);
}

inline std::string dialect_ms_sql::sql_mbr(const table_definition& tbl, const std::string& col)
{
  if (tbl[col]->type_lcase.name.compare("geography") == 0) return "";
  return "\
SELECT a.bounding_box_xmin, a.bounding_box_ymin, a.bounding_box_xmax, a.bounding_box_ymax \
FROM sys.spatial_index_tessellations a, sys.index_columns b \
WHERE a.object_id = OBJECT_ID('" + dialect::sql_identifier(tbl.id) + "') AND COL_NAME(a.object_id, b.column_id) = '" + col + "' AND a.index_id = b.index_id";
}

inline std::string dialect_ms_sql::sql_schema()
{
  return "SELECT SCHEMA_NAME()";
}

inline column_definition dialect_ms_sql::fit_column(const column_definition& col)
{
  column_definition res;
  res.name = fit_identifier(col.name);
  res.type = col.type;
  switch (res.type)
  {
  case VoidColumn: break;
  case Blob: res.type_lcase.name = "varbinary(max)"; break;
  case Double: res.type_lcase.name = "float"; break;
  case Geometry:
    res.type_lcase.name = "geometry";
    res.srid = col.epsg;
    res.epsg = col.epsg;
    res.query_value = (typeid(blob_t) == col.query_value.type())? col.query_value: blob_t();
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

inline table_definition dialect_ms_sql::fit_table(const table_definition& tbl, const std::string& schema)
{
  using namespace std;

  table_definition res(dialect::fit_table(tbl, schema));

  if (find_if(begin(res.indexes), end(res.indexes), [&](const index_definition& idx){ return Primary == idx.type; }) == end(res.indexes))
  {
    auto unq_idx(find_if(begin(res.indexes), end(res.indexes), [&](const index_definition& idx){ return Unique == idx.type; }));
    if (unq_idx == end(res.indexes))
    {
      column_definition col;
      col.name = fit_identifier("id");
      col.type = Integer;
      col.type_lcase.name = "bigint identity";
      col.not_null = true;
      res.columns.push_back(col);

      index_definition idx;
      idx.type = Primary;
      idx.columns.push_back(fit_identifier("id"));
      res.indexes.push_back(idx);
    }
    else
      unq_idx->type = Primary;
  }

  return res;
}

inline std::string dialect_ms_sql::sql_create_spatial_index(const table_definition& tbl, const std::string& col)
{
  using namespace std;
  using namespace brig::boost;

  auto box(envelope(geom_from_wkb(::boost::get<blob_t>(tbl[col]->query_value))));
  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
  stream << "\
CREATE SPATIAL INDEX " << sql_identifier(tbl.rtree(col)->id.name) << " ON " << sql_identifier(tbl.id.name) << " (" << sql_identifier(col) << ") \
USING GEOMETRY_GRID WITH (BOUNDING_BOX = (" << xmin << ", " << ymin << ", " << xmax << ", " << ymax << "))";
  return stream.str();
}

inline std::string dialect_ms_sql::sql_parameter(const command_traits& trs, const column_definition& param, size_t order)
{
  const std::string marker(trs.sql_parameter_marker(order));
  if (Geometry == param.type && !trs.writable_geometry) return param.type_lcase.name + "::STGeomFromWKB(" + marker + ", " + string_cast<char>(param.srid) + ").MakeValid()";
  return marker;
}

inline std::string dialect_ms_sql::sql_column(const command_traits& trs, const column_definition& col)
{
  using namespace std;

  const string id(sql_identifier(col.name));
  if (!col.query_expression.empty()) return col.query_expression + " AS " + id;
  if (String == col.type && col.type_lcase.name.find("time") != string::npos) return "CONVERT(CHAR(19), " + id + ", 126) AS " + id;
  if (String == col.type && col.type_lcase.name.find("date") != string::npos) return "CONVERT(CHAR(10), " + id + ", 126) AS " + id;
  if (Geometry == col.type && !trs.readable_geometry) return id + ".STAsBinary() AS " + id;
  return id;
}

inline void dialect_ms_sql::sql_limit(int rows, std::string& sql_infix, std::string&, std::string&)
{
  sql_infix = "TOP " + string_cast<char>(rows);
}

inline std::string dialect_ms_sql::sql_hint(const table_definition& tbl, const std::string& col)
{
  auto idx(tbl.rtree(col));
  if (idx == 0) return "";
  else return "WITH(INDEX(" + sql_identifier(idx->id.name) + "))";
}

inline bool dialect_ms_sql::need_to_normalize_hemisphere(const column_definition& col)
{
  return col.type_lcase.name.compare("geography") == 0;
}

inline void dialect_ms_sql::sql_intersect(const command_traits& trs, const table_definition& tbl, const std::string& col, const std::vector<brig::boost::box>& boxes, std::string& sql, std::vector<column_definition>& keys)
{
  using namespace std;

  if (!tbl.rtree(col) || boxes.size() < 2) return;

  auto idx(find_if(begin(tbl.indexes), end(tbl.indexes), [&](const index_definition& i){ return Primary == i.type; }));
  if (idx == end(tbl.indexes)) throw runtime_error("unique columns error");
  keys = brig::detail::get_columns(tbl.columns, idx->columns);
  
  const string sql_prefix("(SELECT " + sql_select_list(this, trs, keys) + " FROM " + dialect::sql_identifier(tbl.id) + " " + sql_hint(tbl, col) + " WHERE (");
  const string sql_suffix("))");
  
  for (auto box(begin(boxes)); box != end(boxes); ++box)
  {
    if (box != begin(boxes)) sql += " UNION ";
    sql += sql_prefix + sql_intersect(tbl, col, *box) + sql_suffix;
  }
}

inline std::string dialect_ms_sql::sql_intersect(const table_definition& tbl, const std::string& col, const boost::box& box)
{
  using namespace std;

  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  auto col_def(tbl[col]);
  const bool geography(col_def->type_lcase.name.compare("geography") == 0);
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);

  stream << "" << sql_identifier(col) << ".Filter(";
  if (geography) stream << "geography::STGeomFromWKB(";
  stream << "geometry::Point(" << xmin << ", " << ymin << ", " << col_def->srid << ").STUnion(geometry::Point(" << xmax << ", " << ymax << ", " << col_def->srid << ")).STEnvelope()";
  if (geography) stream << ".STAsBinary(), " << col_def->srid << ")";
  stream << ") = 1";
  return stream.str();
} // dialect_ms_sql::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_MS_SQL_HPP
