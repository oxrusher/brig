// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_HPP

#include <brig/database/command_traits.hpp>
#include <brig/database/detail/sql_regular_identifier.hpp>
#include <brig/database/detail/to_lcase.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/string_cast.hpp>
#include <iterator>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

struct dialect {
  virtual ~dialect()  {}
  virtual std::string sql_identifier(const std::string& id)  { return '"' + id + '"'; }
  std::string sql_identifier(const identifier& id);

  virtual std::string sql_tables() = 0;
  virtual std::string sql_geometries() = 0;
  virtual std::string sql_test_rasters()  { return ""; }
  virtual std::string sql_rasters()  { return ""; }
  virtual void init_raster(raster_pyramid& /*raster*/)  {}

  virtual std::string sql_columns(const identifier& tbl) = 0;
  virtual std::string sql_indexed_columns(const identifier& tbl) = 0;
  virtual std::string sql_spatial_detail(const table_definition& tbl, const std::string& col) = 0;
  virtual column_type get_type(const identifier& dbms_type_lcase, int scale) = 0;

  virtual std::string sql_mbr(const table_definition& tbl, const std::string& col) = 0; // 1 - metadata, 2 - geodetic (no sql), 3 - aggregate

  virtual std::string sql_schema() = 0; // empty is returned if not supported
  virtual std::string fit_identifier(const std::string& id)  { return id; }
  virtual column_definition fit_column(const column_definition& col) = 0;
  virtual table_definition fit_table(const table_definition& tbl, const std::string& schema);
  virtual std::string sql_srid(int epsg) = 0; // empty is returned if equals

  virtual std::string sql_column_definition(const column_definition& col);
  virtual std::string sql_table_options()  { return ""; }
  virtual void sql_register_spatial_column(const table_definition& /*tbl*/, const std::string& /*col*/, std::vector<std::string>& /*sql*/)  {}
  virtual void sql_unregister_spatial_column(const identifier& /*layer*/, std::vector<std::string>& /*sql*/)  {}
  virtual std::string sql_create_spatial_index(const table_definition& tbl, const std::string& col) = 0;
  virtual void sql_drop_spatial_index(const identifier& /*layer*/, std::vector<std::string>& /*sql*/)  {}

  virtual std::string sql_parameter(const command_traits& trs, const column_definition& param, size_t order) = 0;
  virtual std::string sql_column(const command_traits& trs, const column_definition& col) = 0;
  virtual void sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix) = 0;
  virtual std::string sql_hint(const table_definition& /*tbl*/, const std::string& /*col*/)  { return ""; }
  virtual bool need_to_normalize_hemisphere(const column_definition& /*col*/)  { return false; }
  virtual void sql_intersect
    ( const command_traits& /*trs*/, const table_definition& /*tbl*/, const std::string& /*col*/, const std::vector<brig::boost::box>& /*boxes*/
    , std::string& /*sql*/, std::vector<column_definition>& /*keys*/
    )
    {}
  virtual std::string sql_intersect(const table_definition& tbl, const std::string& col, const boost::box& box) = 0;
}; // dialect

inline std::string dialect::sql_identifier(const identifier& id)
{
  return id.schema.empty()? sql_identifier(id.name): (sql_identifier(id.schema) + "." + sql_identifier(id.name));
}

inline table_definition dialect::fit_table(const table_definition& tbl, const std::string& schema)
{
  using namespace std;

  table_definition res;
  res.id.schema = schema;
  res.id.name = fit_identifier(tbl.id.name);
  res.indexes = tbl.indexes;
  for (auto col_iter(begin(tbl.columns)); col_iter != end(tbl.columns); ++col_iter)
  {
    res.columns.push_back(fit_column(*col_iter));
    if (Geometry == col_iter->type && !tbl.rtree(col_iter->name))
    {
      index_definition idx;
      idx.type = Spatial;
      idx.columns.push_back(col_iter->name);
      res.indexes.push_back(idx);
    }
  }

  size_t suffix(0);
  for (auto idx_iter(begin(res.indexes)); idx_iter != end(res.indexes); ++idx_iter)
  {
    idx_iter->id = identifier();
    if (Primary != idx_iter->type) idx_iter->id.name = fit_identifier(res.id.name + "_idx_" + string_cast<char>(++suffix));
    for (auto col_iter(begin(idx_iter->columns)); col_iter != end(idx_iter->columns); ++col_iter)
    {
      *col_iter = fit_identifier(*col_iter);
      // DB2: When UNIQUE is used, null values are treated as any other values. For example, if the key is a single column that may contain null values, that column may contain no more than one null value.
      // Informix: Null values are never allowed in a primary-key column
      // Ingres: All columns in a UNIQUE constraint MUST be created as NOT NULL
      if (Primary == idx_iter->type || Unique == idx_iter->type) res[*col_iter]->not_null = true;
    }
  }

  return res;
}

inline std::string dialect::sql_column_definition(const column_definition& col)
{
  std::string str;
  str += sql_identifier(col.name) + " " + sql_regular_identifier(col.dbms_type_lcase);
  if (col.not_null) str += " NOT NULL";
  return str;
} // dialect::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_HPP
