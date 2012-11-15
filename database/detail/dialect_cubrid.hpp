// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_CUBRID_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_CUBRID_HPP

#include <algorithm>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_iso_type.hpp>
#include <brig/database/global.hpp>
#include <brig/string_cast.hpp>
#include <iterator>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

struct dialect_cubrid : dialect {
  std::string sql_tables() override;
  std::string sql_geometries() override  { throw std::runtime_error("DBMS error"); }

  std::string sql_columns(const identifier& tbl) override;
  std::string sql_indexed_columns(const identifier& tbl) override;
  std::string sql_spatial_detail(const table_definition&, const std::string&)  { throw std::runtime_error("DBMS error"); }
  column_type get_type(const identifier& dbms_type_lcase, int scale) override;

  std::string sql_mbr(const table_definition&, const std::string&) override  { throw std::runtime_error("DBMS error"); }

  std::string sql_schema() override;
  column_definition fit_column(const column_definition& col) override;
  std::string sql_srid(int) override  { throw std::runtime_error("DBMS error"); }

  std::string sql_create_spatial_index(const table_definition&, const std::string&) override  { throw std::runtime_error("DBMS error"); }

  std::string sql_parameter(const command_traits& trs, const column_definition& param, size_t order) override;
  std::string sql_column(const command_traits& trs, const column_definition& col) override;
  void sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix) override;
  std::string sql_intersect(const table_definition&, const std::string&, const boost::box&) override  { throw std::runtime_error("DBMS error"); }
}; // dialect_cubrid

inline std::string dialect_cubrid::sql_tables()
{
  return "SELECT owner.name scm, class_name tbl FROM _db_class WHERE class_type = 0 AND is_system_class = 0";
}

inline std::string dialect_cubrid::sql_columns(const identifier& tbl)
{
  return "\
SELECT a.attr_name, '', t.type_name, d.prec, d.scale, (CASE a.is_nullable WHEN 0 THEN 1 ELSE 0 END) \
FROM _db_class c, _db_attribute a, _db_domain d, _db_data_type t \
WHERE c.owner.name = '" + tbl.schema + "' AND c.class_name = '" + tbl.name + "' AND a.class_of = c AND d.object_of = a AND d.data_type = t.type_id \
ORDER BY a.def_order";
}

inline std::string dialect_cubrid::sql_indexed_columns(const identifier& tbl)
{
  return "\
SELECT '', i.index_name, i.is_primary_key, i.is_unique, 0, k.key_attr_name, k.asc_desc \
FROM _db_class c, _db_index i, _db_index_key k \
WHERE c.owner.name = '" + tbl.schema + "' AND c.class_name = '" + tbl.name + "' AND i.class_of = c AND k.index_of = i \
ORDER BY i.is_primary_key DESC, i.index_name, k.key_order";
}

inline column_type dialect_cubrid::get_type(const identifier& dbms_type_lcase, int scale)
{
  if (!dbms_type_lcase.schema.empty()) return VoidColumn;
  if (dbms_type_lcase.name.compare("short") == 0) return Integer;
  if (dbms_type_lcase.name.find("bit") != std::string::npos) return Blob;
  if (dbms_type_lcase.name.compare("string") == 0) return String;
  return get_iso_type(dbms_type_lcase.name, scale);
}

inline std::string dialect_cubrid::sql_schema()
{
  return "SELECT current_user";
}

inline column_definition dialect_cubrid::fit_column(const column_definition& col)
{
  using namespace std;

  column_definition res;
  res.name = fit_identifier(col.name);
  res.type = col.type;
  switch (res.type)
  {
  case VoidColumn:
  case Geometry: throw runtime_error("datatype error");
  case Blob: res.dbms_type_lcase.name = "bit varying"; break;
  case Double: res.dbms_type_lcase.name = "double"; break;
  case Integer: res.dbms_type_lcase.name = "bigint"; break;
  case String: res.dbms_type_lcase.name = "string"; break;
  }
  if (col.not_null) res.not_null = true;
  return res;
}

inline std::string dialect_cubrid::sql_parameter(const command_traits& trs, const column_definition& param, size_t order)
{
  if (Geometry == param.type && !trs.writable_geometry) throw std::runtime_error("datatype error");
  return trs.sql_parameter_marker(order);
}

inline std::string dialect_cubrid::sql_column(const command_traits& trs, const column_definition& col)
{
  using namespace std;

  const string id(sql_identifier(col.name));
  if (!col.query_expression.empty()) return col.query_expression + " AS " + id;
  if (String == col.type && col.dbms_type_lcase.name.find("time") != string::npos) return "(TO_CHAR(" + id + ", 'YYYY-MM-DD') || 'T' || TO_CHAR(" + id + ", 'HH24:MI:SS')) AS " + id;
  if (String == col.type && col.dbms_type_lcase.name.find("date") != string::npos) return "TO_CHAR(" + id + ", 'YYYY-MM-DD') AS " + id;
  if (Geometry == col.type && !trs.readable_geometry) throw runtime_error("datatype error");
  return id;
}

inline void dialect_cubrid::sql_limit(int rows, std::string&, std::string&, std::string& sql_suffix)
{
  sql_suffix = "LIMIT " + string_cast<char>(rows);
} // dialect_cubrid::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_CUBRID_HPP