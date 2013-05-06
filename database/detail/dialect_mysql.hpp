// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_MYSQL_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_MYSQL_HPP

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

struct dialect_mysql : dialect {
  std::string sql_identifier(const std::string& id) override  { return '`' + id + '`'; }

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
  std::string sql_srid(int) override  { return ""; }

  std::string sql_table_options() override;
  std::string sql_create_spatial_index(const table_def& tbl, const std::string& col) override;

  std::string sql_parameter(command* cmd, const column_def& param, size_t order) override;
  std::string sql_column(command* cmd, const column_def& col) override;
  void sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix) override;
  std::string sql_intersect(const table_def& tbl, const std::string& col, const boost::box& box) override;
}; // dialect_mysql

inline std::string dialect_mysql::sql_tables()
{
  return "SELECT TABLE_SCHEMA scm, TABLE_NAME tbl FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA NOT IN ('information_schema','mysql','performance_schema')";
}

inline std::string dialect_mysql::sql_geometries()
{
  return "\
SELECT table_schema scm, table_name tbl, column_name col \
FROM information_schema.columns \
WHERE data_type IN ('geometry','point','curve','linestring','surface','polygon','geometrycollection','multipoint','multicurve','multilinestring','multisurface','multipolygon')";
}

inline std::string dialect_mysql::sql_columns(const identifier& tbl)
{
  return "\
SELECT COLUMN_NAME, '', DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, NUMERIC_SCALE, (CASE IS_NULLABLE WHEN 'NO' THEN 1 ELSE 0 END) \
FROM INFORMATION_SCHEMA.COLUMNS \
WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' \
ORDER BY ORDINAL_POSITION";
}

inline std::string dialect_mysql::sql_indexed_columns(const identifier& tbl)
{
  // ASC or DESC are permitted for future extensions - currently (5.6) they are ignored
  return "\
SELECT '', INDEX_NAME, (INDEX_NAME = 'PRIMARY') pri, NOT NON_UNIQUE, INDEX_TYPE = 'SPATIAL', COLUMN_NAME, COLLATION = 'D' \
FROM INFORMATION_SCHEMA.STATISTICS \
WHERE TABLE_SCHEMA = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' \
ORDER BY pri DESC, INDEX_NAME, SEQ_IN_INDEX";
}

inline std::string dialect_mysql::sql_spatial_detail(const table_def& tbl, const std::string& col)
{
  return "SELECT SRID(" + sql_identifier(col) + ") FROM " + dialect::sql_identifier(tbl.id) + " LIMIT 1";
}

inline column_type dialect_mysql::get_type(const identifier& type_lcase, int scale)
{
  if (!type_lcase.schema.empty()) return column_type::Void;
  if (is_ogc_type(type_lcase.name)) return column_type::Geometry;
  if (type_lcase.name.compare("fixed") == 0) return scale == 0? column_type::Integer: column_type::Double;
  return get_iso_type(type_lcase.name, scale);
}

inline std::string dialect_mysql::sql_extent(const table_def& tbl, const std::string& col)
{
  return "\
SELECT Min(X(PointN(t.r, 1))), Min(Y(PointN(t.r, 1))), Max(X(PointN(t.r, 3))), Max(Y(PointN(t.r, 3))) \
FROM (SELECT ExteriorRing(Envelope(" + sql_identifier(col) + ")) r FROM " + dialect::sql_identifier(tbl.id) + ") t";
}

inline std::string dialect_mysql::sql_schema()
{
  return "SELECT schema()";
}

inline std::string dialect_mysql::fit_identifier(const std::string& id)
{
  return brig::unicode::transform<char>(id, brig::unicode::lower_case);
}

inline column_def dialect_mysql::fit_column(const column_def& col)
{
  column_def res;
  res.name = fit_identifier(col.name);
  res.type = col.type;
  switch (res.type)
  {
  case column_type::Void: break;
  case column_type::Blob: res.type_lcase.name = "longblob"; break;
  case column_type::Double: res.type_lcase.name = "double"; break;
  case column_type::Geometry:
    res.type_lcase.name = "geometry";
    res.srid = col.epsg;
    res.epsg = col.epsg;
    res.not_null = true; // columns in spatial indexes must be declared NOT NULL
    break;
  case column_type::Integer: res.type_lcase.name = "bigint"; break;
  case column_type::String:
    res.chars = (col.chars > 0 && col.chars < CharsLimit)? col.chars: CharsLimit;
    res.type_lcase.name = "nvarchar(" + string_cast<char>(res.chars) + ")";
    break;
  }
  if (col.not_null) res.not_null = true;
  return res;
}

inline std::string dialect_mysql::sql_table_options()
{
  return "ENGINE = MyISAM";
}

inline std::string dialect_mysql::sql_create_spatial_index(const table_def& tbl, const std::string& col)
{
  return "CREATE SPATIAL INDEX " + sql_identifier(tbl.rtree(col)->id.name) + " ON " + sql_identifier(tbl.id.name) + " (" + sql_identifier(col) + ")";
}

inline std::string dialect_mysql::sql_parameter(command* cmd, const column_def& param, size_t order)
{
  const std::string marker(cmd->sql_param(order));
  if (column_type::Geometry == param.type && !cmd->writable_geom()) return "GeomFromWKB(" + marker + ", " + string_cast<char>(param.srid) + ")";
  return marker;
}

inline std::string dialect_mysql::sql_column(command* cmd, const column_def& col)
{
  using namespace std;

  const string id(sql_identifier(col.name));
  if (!col.query_expression.empty()) return col.query_expression + " AS " + id;
  if (column_type::String == col.type && col.type_lcase.name.find("time") != string::npos) return "DATE_FORMAT(" + id + ", '%Y-%m-%dT%T') AS " + id;
  if (column_type::String == col.type && col.type_lcase.name.find("date") != string::npos) return "DATE_FORMAT(" + id + ", '%Y-%m-%d') AS " + id;
  if (column_type::Geometry == col.type && !cmd->readable_geom()) return "AsBinary(" + id + ") AS " + id;
  return id;
}

inline void dialect_mysql::sql_limit(int rows, std::string&, std::string&, std::string& sql_suffix)
{
  sql_suffix = "LIMIT " + string_cast<char>(rows);
}

inline std::string dialect_mysql::sql_intersect(const table_def&, const std::string& col, const boost::box& box)
{
  using namespace std;

  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
  stream << "MBRIntersects(Envelope(LineString(Point(" << xmin << ", " << ymin << "), Point(" << xmax << ", " << ymax << "))), " << sql_identifier(col) << ")";
  return stream.str();
} // dialect_mysql::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_MYSQL_HPP
