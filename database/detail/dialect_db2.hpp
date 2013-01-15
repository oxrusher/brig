// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_DB2_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_DB2_HPP

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

struct dialect_db2 : dialect {
  std::string sql_tables() override;
  std::string sql_geometries() override;

  std::string sql_columns(const identifier& tbl) override;
  std::string sql_indexed_columns(const identifier& tbl) override;
  std::string sql_spatial_detail(const table_definition& tbl, const std::string& col) override;
  column_type get_type(const identifier& type_lcase, int scale) override;

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
}; // dialect_db2

inline std::string dialect_db2::sql_tables()
{
  return "SELECT RTRIM(TABSCHEMA) scm, TABNAME tbl FROM SYSCAT.TABLES WHERE TYPE = 'T' AND RTRIM(TABSCHEMA) NOT IN ('APP','DB2GSE','DB2QP','NULLID','SQLJ','ST_INFORMTN_SCHEMA') AND TABSCHEMA NOT LIKE 'SYS%'";
}

inline std::string dialect_db2::sql_geometries()
{
  return "SELECT TABLE_SCHEMA scm, TABLE_NAME tbl, COLUMN_NAME col FROM DB2GSE.ST_GEOMETRY_COLUMNS";
}

inline std::string dialect_db2::sql_columns(const identifier& tbl)
{
  return "SELECT COLNAME, RTRIM(TYPESCHEMA), TYPENAME, LENGTH, SCALE, (CASE NULLS WHEN 'N' THEN 1 ELSE 0 END) FROM SYSCAT.COLUMNS WHERE TABSCHEMA = '" + tbl.schema + "' AND TABNAME = '" + tbl.name + "' ORDER BY COLNO";
}

inline std::string dialect_db2::sql_indexed_columns(const identifier& tbl)
{
  return "\
SELECT \
  RTRIM(i.INDSCHEMA) \
, i.INDNAME \
, (CASE i.UNIQUERULE WHEN 'P' THEN 1 ELSE 0 END) pri \
, (CASE i.UNIQUERULE WHEN 'D' THEN 0 ELSE 1 END) unq \
, (CASE RTRIM(i.IESCHEMA) || '.' || i.IENAME WHEN 'DB2GSE.SPATIAL_INDEX' THEN 1 ELSE 0 END) sp \
, c.COLNAME \
, (CASE c.COLORDER WHEN 'D' THEN 1 ELSE 0 END) dsc \
FROM SYSCAT.INDEXES i, SYSCAT.INDEXCOLUSE c \
WHERE i.INDSCHEMA = c.INDSCHEMA AND i.INDNAME = c.INDNAME AND i.TABSCHEMA = '" + tbl.schema + "' AND i.TABNAME = '" + tbl.name + "' \
ORDER BY pri DESC, i.INDSCHEMA, i.INDNAME, c.COLSEQ";
}

inline std::string dialect_db2::sql_spatial_detail(const table_definition& tbl, const std::string& col)
{
  return "\
SELECT c.SRS_ID, (CASE s.ORGANIZATION WHEN 'EPSG' THEN s.ORGANIZATION_COORDSYS_ID ELSE NULL END) epsg \
FROM (SELECT SRS_ID FROM DB2GSE.ST_GEOMETRY_COLUMNS WHERE TABLE_SCHEMA = '" + tbl.id.schema + "' AND TABLE_NAME = '" + tbl.id.name + "' AND COLUMN_NAME = '" + col + "') c \
LEFT JOIN DB2GSE.ST_SPATIAL_REFERENCE_SYSTEMS s ON c.SRS_ID = s.SRS_ID";
}

inline column_type dialect_db2::get_type(const identifier& type_lcase, int scale)
{
  using namespace std;

  if (type_lcase.schema.compare("db2gse") == 0 && is_ogc_type(type_lcase.name)) return Geometry;
  if (!type_lcase.schema.empty() && type_lcase.schema.compare("sysibm") != 0) return VoidColumn;
  if (type_lcase.name.find("graphic") != string::npos) return String;
  return get_iso_type(type_lcase.name, scale);
}

inline std::string dialect_db2::sql_mbr(const table_definition& tbl, const std::string& col)
{
  using namespace std;

  const int srid(tbl[col]->srid);
  if (2000000000 <= srid && srid <= 2000001000) return "";
  const string c(sql_identifier(col));
  return "SELECT Min(DB2GSE.ST_MinX(" + c + ")), Min(DB2GSE.ST_MinY(" + c + ")), Max(DB2GSE.ST_MaxX(" + c + ")), Max(DB2GSE.ST_MaxY(" + c + ")) FROM " + sql_identifier(tbl.id);
}

inline std::string dialect_db2::sql_schema()
{
  return "VALUES RTRIM(CURRENT_SCHEMA)";
}

inline column_definition dialect_db2::fit_column(const column_definition& col)
{
  column_definition res;
  res.name = fit_identifier(col.name);
  res.type = col.type;
  switch (res.type)
  {
  case VoidColumn: break;
  case Blob: res.type_lcase.name = "blob"; break;
  case Double: res.type_lcase.name = "double"; break;
  case Geometry:
    res.type_lcase.schema = "db2gse";
    res.type_lcase.name = "st_geometry";
    res.epsg = col.epsg;
    break;
  case Integer: res.type_lcase.name = "bigint"; break;
  case String:
    res.chars = (col.chars > 0 && col.chars < CharsLimit)? col.chars: CharsLimit;
    res.type_lcase.name = "vargraphic(" + string_cast<char>(res.chars) + ")";
    break;
  }
  if (col.not_null) res.not_null = true;
  return res;
}

inline std::string dialect_db2::sql_srid(int epsg)
{
  return "SELECT SRS_ID FROM DB2GSE.ST_SPATIAL_REFERENCE_SYSTEMS WHERE ORGANIZATION LIKE 'EPSG' AND ORGANIZATION_COORDSYS_ID = " + string_cast<char>(epsg) + " ORDER BY SRS_ID FETCH FIRST 1 ROWS ONLY";
}

inline void dialect_db2::sql_register_spatial_column(const table_definition& tbl, const std::string& col, std::vector<std::string>& sql)
{
  sql.push_back("\
BEGIN ATOMIC \
DECLARE msg_code INTEGER; \
DECLARE msg_text VARCHAR(1024); \
call DB2GSE.ST_register_spatial_column( \
  NULL \
, '" + sql_identifier(tbl.id.name) + "' \
, '" + sql_identifier(col) + "' \
, (SELECT SRS_NAME FROM DB2GSE.ST_SPATIAL_REFERENCE_SYSTEMS WHERE SRS_ID = " + string_cast<char>(tbl[col]->srid) + ") \
, msg_code \
, msg_text \
); \
END");
}

inline void dialect_db2::sql_unregister_spatial_column(const identifier& layer, std::vector<std::string>& sql)
{
  sql.push_back("\
BEGIN ATOMIC \
DECLARE msg_code INTEGER; \
DECLARE msg_text VARCHAR(1024); \
call DB2GSE.ST_unregister_spatial_column('" + sql_identifier(layer.schema) + "', '" + sql_identifier(layer.name) + "', '" + sql_identifier(layer.qualifier) + "', msg_code, msg_text); \
END");
}

inline std::string dialect_db2::sql_create_spatial_index(const table_definition& tbl, const std::string& col)
{
  return "CREATE INDEX " + sql_identifier(tbl.rtree(col)->id.name) + " ON " + sql_identifier(tbl.id.name) + " (" + sql_identifier(col) + ") EXTEND USING DB2GSE.SPATIAL_INDEX (1, 0, 0)";
}

inline std::string dialect_db2::sql_parameter(const command_traits& trs, const column_definition& param, size_t order)
{
  const std::string marker(trs.sql_parameter_marker(order));
  if (Geometry == param.type && !trs.writable_geometry) return param.type_lcase.to_string() + "(CAST(" + marker + " AS BLOB (100M)), " + string_cast<char>(param.srid) + ")";
  return marker;
}

inline std::string dialect_db2::sql_column(const command_traits& trs, const column_definition& col)
{
  using namespace std;

  const string id(sql_identifier(col.name));
  if (!col.query_expression.empty()) return col.query_expression + " AS " + id;
  if (String == col.type && col.type_lcase.name.find("time") != string::npos) return "(TO_CHAR(" + id + ", 'YYYY-MM-DD') || 'T' || TO_CHAR(" + id + ", 'HH24:MI:SS')) AS " + id;
  if (String == col.type && col.type_lcase.name.find("date") != string::npos) return "TO_CHAR(" + id + ", 'YYYY-MM-DD') AS " + id;
  if (Geometry == col.type && !trs.readable_geometry) return "DB2GSE.ST_AsBinary(" + id + ") AS " + id;
  return id;
}

inline void dialect_db2::sql_limit(int rows, std::string&, std::string&, std::string& sql_suffix)
{
  const std::string sql_rows(string_cast<char>(rows));
  sql_suffix = "FETCH FIRST " + sql_rows + " ROWS ONLY OPTIMIZE FOR " + sql_rows + " ROWS";
}

inline std::string dialect_db2::sql_intersect(const table_definition& tbl, const std::string& col, const boost::box& box)
{
  using namespace std;

  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
  stream << "DB2GSE.EnvelopesIntersect(" << sql_identifier(col) << ", " << xmin << ", " << ymin << ", " << xmax << ", " << ymax << ", " << tbl[col]->srid << ") = 1";
  return stream.str();
} // dialect_db2::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_DB2_HPP
