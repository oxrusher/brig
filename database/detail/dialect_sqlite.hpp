// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_SQLITE_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_SQLITE_HPP

#include <algorithm>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_iso_type.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/global.hpp>
#include <brig/numeric_cast.hpp>
#include <brig/string_cast.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

struct dialect_sqlite : dialect {
  std::string sql_identifier(const std::string& id) override  { return '[' + id + ']'; }

  std::string sql_tables() override;
  std::string sql_geometries() override;
  std::string sql_test_rasters() override;
  std::string sql_rasters() override;
  void init_raster(pyramid_def& raster) override;

  std::string sql_columns(const identifier&) override  { throw std::runtime_error("DBMS error"); }
  std::string sql_indexed_columns(const identifier&) override  { throw std::runtime_error("DBMS error"); }
  std::string sql_spatial_detail(const table_def&, const std::string&) override  { throw std::runtime_error("DBMS error"); }
  column_type get_type(const identifier&, int) override  { throw std::runtime_error("DBMS error"); }

  std::string sql_extent(const table_def& tbl, const std::string& col) override;

  std::string sql_schema() override  { return ""; }
  std::string fit_identifier(const std::string& id) override;
  column_def fit_column(const column_def& col) override;
  std::string sql_srid(int epsg) override;

  std::string sql_column_def(const column_def& col) override;
  void sql_register_spatial_column(const table_def& tbl, const std::string& col, std::vector<std::string>& sql) override;
  void sql_unregister_spatial_column(const identifier& layer, std::vector<std::string>& sql) override;
  std::string sql_create_spatial_index(const table_def& tbl, const std::string& col) override;
  void sql_drop_spatial_index(const identifier& layer, std::vector<std::string>& sql) override;

  std::string sql_parameter(command* cmd, const column_def& param, size_t order) override;
  std::string sql_column(command* cmd, const column_def& col) override;
  void sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix) override;
  void sql_intersect(command* cmd, const table_def& tbl, const std::string& col, const std::vector<brig::boost::box>& boxes, std::string& sql, std::vector<column_def>& keys) override;
  std::string sql_intersect(const table_def& tbl, const std::string& col, const boost::box& box) override;
}; // dialect_sqlite

inline std::string dialect_sqlite::sql_tables()
{
  return "\
SELECT  '' scm, name tbl FROM sqlite_master \
WHERE type = 'table' \
AND name NOT LIKE 'sqlite!_%' ESCAPE '!' \
AND name NOT LIKE 'idx!_%' ESCAPE '!' \
AND name NOT LIKE '%!_geometry_columns%' ESCAPE '!' \
AND name NOT LIKE '%geometry_columns!_%' ESCAPE '!' \
AND name NOT IN ('SpatialIndex','spatialite_history','sql_statements_log')";
}

inline std::string dialect_sqlite::sql_geometries()
{
  return "SELECT '' scm, t.name tbl, g.f_geometry_column col FROM geometry_columns g JOIN sqlite_master t ON LOWER(g.f_table_name) = LOWER(t.name)";
}

inline std::string dialect_sqlite::sql_extent(const table_def& tbl, const std::string& col)
{
  return "SELECT MbrMinX(t.r), MbrMinY(t.r), MbrMaxX(t.r), MbrMaxY(t.r) FROM (SELECT Extent(" + sql_identifier(col) + ") r FROM " + sql_identifier(tbl.id.name) + ") t";
}

inline std::string dialect_sqlite::fit_identifier(const std::string& id)
{
  return brig::unicode::transform<char>(id, brig::unicode::lower_case);
}

inline column_def dialect_sqlite::fit_column(const column_def& col)
{
  column_def res;
  res.name = fit_identifier(col.name);
  res.type = col.type;
  switch (res.type)
  {
  case column_type::Void: break;
  case column_type::Blob: res.type_lcase.name = "blob"; break;
  case column_type::Double: res.type_lcase.name = "real"; break; // real affinity
  case column_type::Geometry:
    res.type_lcase.name = "geometry";
    res.epsg = col.epsg;
    break;
  case column_type::Integer: res.type_lcase.name = "integer"; break; // integer affinity
  case column_type::String: res.type_lcase.name = "text"; break; // text affinity
  }
  if (col.not_null) res.not_null = true;
  return res;
}

inline std::string dialect_sqlite::sql_srid(int epsg)
{
  return "SELECT srid FROM spatial_ref_sys WHERE auth_name = 'epsg' AND auth_srid = " + string_cast<char>(epsg) + " ORDER BY srid LIMIT 1";
}

inline std::string dialect_sqlite::sql_column_def(const column_def& col)
{
  return column_type::Geometry == col.type? "": dialect::sql_column_def(col);
}

inline void dialect_sqlite::sql_register_spatial_column(const table_def& tbl, const std::string& col, std::vector<std::string>& sql)
{
  sql.push_back("SELECT AddGeometryColumn('" + tbl.id.name + "', '" + col + "', " + string_cast<char>(tbl[col]->srid) + ", 'GEOMETRY', 2)");
}

inline void dialect_sqlite::sql_unregister_spatial_column(const identifier& layer, std::vector<std::string>& sql)
{
  sql.push_back("SELECT DiscardGeometryColumn('" + layer.name + "', '" + layer.qualifier + "')");
}

inline std::string dialect_sqlite::sql_create_spatial_index(const table_def& tbl, const std::string& col)
{
  return "SELECT CreateSpatialIndex('" + tbl.id.name + "', '" + col + "')";
}

inline void dialect_sqlite::sql_drop_spatial_index(const identifier& layer, std::vector<std::string>& sql)
{
  sql.push_back("SELECT DisableSpatialIndex('" + layer.name + "', '" + layer.qualifier + "')");
  sql.push_back("DROP TABLE " + sql_identifier("idx_" + layer.name + "_" + layer.qualifier));
}

inline std::string dialect_sqlite::sql_test_rasters()
{
  return sql_tables() + " AND name = 'raster_pyramids'";
}

inline std::string dialect_sqlite::sql_rasters()
{
  return "\
SELECT '', (r.table_prefix || '_rasters') base_tbl, 'raster' base_col, r.pixel_x_size res_x, r.pixel_y_size res_y, '', (r.table_prefix || '_metadata'), 'geometry', 'raster' \
FROM raster_pyramids r JOIN geometry_columns g ON LOWER(g.f_table_name) = LOWER(r.table_prefix || '_metadata') \
ORDER BY base_tbl, base_col, res_x, res_y";
}

inline void dialect_sqlite::init_raster(pyramid_def& raster)
{
  const std::string tbl(sql_identifier(raster.id.name));
  for (size_t i(0); i < raster.levels.size(); ++i)
  {
    const bool hint((i * 6) < raster.levels.size());
    tilemap_def& lvl(raster.levels[i]);
    lvl.raster.query_expression = "(SELECT r FROM (SELECT id i, raster r FROM " + tbl + ") t WHERE t.i = " + "id)";
    {
      column_def col;
      col.name = "pixel_x_size";
      col.type = column_type::Double;
      col.query_expression = hint? "+pixel_x_size": "";
      col.query_value = lvl.resolution_x;
      lvl.query_conditions.push_back(col);
    }
    {
      column_def col;
      col.name = "pixel_y_size";
      col.type = column_type::Double;
      col.query_expression = hint? "+pixel_y_size": "";
      col.query_value = lvl.resolution_y;
      lvl.query_conditions.push_back(col);
    }
  }
}

inline std::string dialect_sqlite::sql_parameter(command* cmd, const column_def& param, size_t order)
{
  const std::string marker(cmd->sql_param(order));
  if (column_type::Geometry == param.type && !cmd->writable_geom()) return "GeomFromWKB(" + marker + ", " + string_cast<char>(param.srid) + ")";
  return marker;
}

inline std::string dialect_sqlite::sql_column(command* cmd, const column_def& col)
{
  using namespace std;

  const string id(sql_identifier(col.name));
  if (!col.query_expression.empty()) return col.query_expression + " AS " + id;
  if (column_type::Geometry == col.type && !cmd->readable_geom()) return "AsBinary(" + id + ") AS " + id;
  return id;
}

inline void dialect_sqlite::sql_limit(int rows, std::string&, std::string&, std::string& sql_suffix)
{
  sql_suffix = "LIMIT " + string_cast<char>(rows);
}

inline void dialect_sqlite::sql_intersect(command*, const table_def& tbl, const std::string& col, const std::vector<brig::boost::box>& boxes, std::string& sql, std::vector<column_def>& keys)
{
  using namespace std;

  if (!tbl.rtree(col)) return;

  column_def key;
  key.name = "rowid";
  key.type = column_type::Integer;
  key.type_lcase.name = "int";
  keys.push_back(key);

  sql += "SELECT pkid AS " + sql_identifier(key.name) + " FROM " + sql_identifier("idx_" + tbl.id.name + "_" + col) + " WHERE ";
  for (auto box(begin(boxes)); box != end(boxes); ++box)
  {
    if (box != begin(boxes)) sql += " OR ";
    const double xmin(box->min_corner().get<0>()), ymin(box->min_corner().get<1>()), xmax(box->max_corner().get<0>()), ymax(box->max_corner().get<1>());
    ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
    stream << "(xmax >= " << xmin << " AND xmin <= " << xmax << " AND ymax >= " << ymin << " AND ymin <= " << ymax << ")";
    sql += stream.str();
  }
}

inline std::string dialect_sqlite::sql_intersect(const table_def& tbl, const std::string& col, const boost::box& box)
{
  using namespace std;

  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
  stream << "MbrIntersects(" << sql_identifier(col) << ", BuildMbr(" << xmin << ", " << ymin << ", " << xmax << ", " << ymax << ", " << tbl[col]->srid << ")) = 1"; // no index
  return stream.str();
} // dialect_sqlite::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_SQLITE_HPP
