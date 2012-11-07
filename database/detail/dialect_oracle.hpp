// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_ORACLE_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_ORACLE_HPP

#include <algorithm>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_columns.hpp>
#include <brig/database/detail/get_iso_type.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/database/detail/sql_regular_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/string_cast.hpp>
#include <brig/unicode/transform.hpp>
#include <brig/unicode/upper_case.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

struct dialect_oracle : dialect {
  std::string sql_tables() override;
  std::string sql_geometries() override;

  std::string sql_columns(const identifier& tbl) override;
  std::string sql_indexed_columns(const identifier& tbl) override;
  std::string sql_spatial_detail(const table_definition& tbl, const std::string& col) override;
  column_type get_type(const identifier& dbms_type_lcase, int scale) override;

  std::string sql_mbr(const table_definition& tbl, const std::string& col) override;

  std::string sql_schema() override;
  std::string fit_identifier(const std::string& id)  override;
  column_definition fit_column(const column_definition& col) override;
  table_definition fit_table(const table_definition& tbl, const std::string& schema) override;
  std::string sql_srid(int epsg) override;

  void sql_register_spatial_column(const table_definition& tbl, const std::string& col, std::vector<std::string>& sql) override;
  std::string sql_create_spatial_index(const table_definition& tbl, const std::string& col) override;

  std::string sql_parameter(const command_traits& trs, const column_definition& param, size_t order) override;
  std::string sql_column(const command_traits& trs, const column_definition& col) override;
  void sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix) override;
  bool need_to_normalize_hemisphere(const column_definition& col) override;
  void sql_intersect(const command_traits& trs, const table_definition& tbl, const std::string& col, const std::vector<brig::boost::box>& boxes, std::string& sql, std::vector<column_definition>& keys) override;
  std::string sql_intersect(const table_definition& tbl, const std::string& col, const boost::box& box) override;
}; // dialect_oracle

inline std::string dialect_oracle::sql_tables()
{
  // sample schemas: HR,OE,PM,IX,SH
  return "\
SELECT OWNER scm, TABLE_NAME tbl \
FROM ALL_TABLES \
WHERE OWNER NOT IN ('ANONYMOUS','APEX_040000','CTXSYS','DBSNMP','DIP','DMSYS','EXFSYS','MDDATA','MDSYS','MGMT_VIEW','OLAPSYS','ORDPLUGINS','ORDSYS','OUTLN','SI_INFORMTN_SCHEMA','SYS','SYSMAN','TSMSYS','WKSYS','WMSYS','XDB') \
AND TABLE_NAME NOT LIKE '%$%'";
}

inline std::string dialect_oracle::sql_geometries()
{
  return "\
SELECT g.scm scm, g.tbl tbl, g.col col \
FROM (SELECT OWNER scm, TABLE_NAME tbl, COLUMN_NAME col FROM MDSYS.ALL_SDO_GEOM_METADATA) g \
JOIN (SELECT OWNER scm, TABLE_NAME tbl, COLUMN_NAME col FROM ALL_TAB_COLUMNS) c \
ON g.scm = c.scm AND g.tbl = c.tbl AND g.col = c.col";
}

inline std::string dialect_oracle::sql_columns(const identifier& tbl)
{
  return "\
SELECT COLUMN_NAME, DATA_TYPE_OWNER, DATA_TYPE, CHAR_LENGTH, DATA_SCALE, (CASE NULLABLE WHEN 'N' THEN 1 ELSE 0 END) \
FROM ALL_TAB_COLUMNS \
WHERE OWNER = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "' \
ORDER BY COLUMN_ID";
}

inline std::string dialect_oracle::sql_indexed_columns(const identifier& tbl)
{
  return "\
SELECT \
  i.OWNER \
, i.INDEX_NAME \
, (CASE cs.CONSTRAINT_TYPE WHEN 'P' THEN 1 ELSE 0 END) pri \
, (CASE i.UNIQUENESS WHEN 'NONUNIQUE' THEN 0 ELSE 1 END) unq \
, (CASE i.ITYP_OWNER || '.' || i.ITYP_NAME WHEN 'MDSYS.SPATIAL_INDEX' THEN 1 ELSE 0 END) sp \
, c.COLUMN_NAME \
, (CASE c.DESCEND WHEN 'DESC' THEN 1 ELSE 0 END) dsc \
FROM (SELECT * FROM ALL_INDEXES WHERE TABLE_OWNER = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "') i \
LEFT JOIN (SELECT * FROM ALL_CONSTRAINTS WHERE OWNER = '" + tbl.schema + "' AND TABLE_NAME = '" + tbl.name + "') cs ON i.INDEX_NAME = cs.INDEX_NAME \
JOIN ALL_IND_COLUMNS c ON i.OWNER = c.INDEX_OWNER AND i.INDEX_NAME = c.INDEX_NAME \
ORDER BY pri DESC, i.OWNER, i.INDEX_NAME, c.COLUMN_POSITION";
}

inline std::string dialect_oracle::sql_spatial_detail(const table_definition& tbl, const std::string& col)
{
  return "\
SELECT c.SRID, (CASE s.DATA_SOURCE WHEN 'EPSG' THEN s.SRID ELSE NULL END) epsg, s.COORD_REF_SYS_KIND \
FROM (SELECT * FROM MDSYS.ALL_SDO_GEOM_METADATA WHERE OWNER = '" + tbl.id.schema + "' AND TABLE_NAME = '" + tbl.id.name + "' AND COLUMN_NAME = '" + col + "') c \
LEFT JOIN MDSYS.SDO_COORD_REF_SYS s ON c.SRID = s.SRID";
}

inline column_type dialect_oracle::get_type(const identifier& dbms_type_lcase, int scale)
{
  if (dbms_type_lcase.schema.compare("mdsys") == 0 && (dbms_type_lcase.name.compare("sdo_geometry") == 0 || is_ogc_type(dbms_type_lcase.name))) return Geometry;
  if (!dbms_type_lcase.schema.empty()) return VoidColumn;
  if (dbms_type_lcase.name.compare("long") == 0) return String;
  if (dbms_type_lcase.name.compare("bfile") == 0 || dbms_type_lcase.name.find("raw") != std::string::npos) return Blob;
  return get_iso_type(dbms_type_lcase.name, scale);
}

inline std::string dialect_oracle::sql_mbr(const table_definition& tbl, const std::string& col)
{
  const std::string t("SELECT ROWNUM n, d.SDO_LB l, d.SDO_UB u FROM (SELECT * FROM ALL_SDO_GEOM_METADATA WHERE OWNER = '" + tbl.id.schema + "' AND TABLE_NAME = '" + tbl.id.name + "' AND COLUMN_NAME = '" + col + "') m, TABLE(m.DIMINFO) d");
  return "SELECT x.l, y.l, x.u, y.u FROM (SELECT * FROM (" + t + ") WHERE n = 1) x, (SELECT * FROM (" + t + ") WHERE n = 2) y";
}

inline std::string dialect_oracle::sql_schema()
{
  return "SELECT SYS_CONTEXT('USERENV','SESSION_SCHEMA') FROM DUAL";
}

inline std::string dialect_oracle::fit_identifier(const std::string& id)
{
  // The TABLE_NAME and COLUMN_NAME values are always converted to uppercase when you insert them into the USER_SDO_GEOM_METADATA view
  using namespace std;
  using namespace brig::unicode;
  u32string u32(transform<u32string>(id, upper_case));
  transform(begin(u32), end(u32), begin(u32), [](char32_t ch) -> char32_t
  {
    switch (ch)
    {
    default:
      return ch;
    case 0x20: // space
    case 0x2d: // minus
      return 0x5f; // underline
    }
  });
  return transform<string>(u32);
}

inline column_definition dialect_oracle::fit_column(const column_definition& col)
{
  using namespace std;

  column_definition res;
  res.name = fit_identifier(col.name);
  res.type = col.type;
  switch (res.type)
  {
  case VoidColumn: throw runtime_error("datatype error");
  case Blob: res.dbms_type_lcase.name = "blob"; break;
  case Double: res.dbms_type_lcase.name = "binary_double"; break;
  case Geometry:
    res.dbms_type_lcase.schema = "mdsys";
    res.dbms_type_lcase.name = "sdo_geometry";
    res.epsg = col.epsg;
    res.query_value = (typeid(blob_t) == col.query_value.type())? col.query_value: blob_t();
    break;
  case Integer: res.dbms_type_lcase.name = "number(19)"; break;
  case String:
    res.chars = (col.chars > 0 && col.chars < CharsLimit)? col.chars: CharsLimit;
    res.dbms_type_lcase.name = "nvarchar2(" + string_cast<char>(res.chars) + ")";
    break;
  }
  if (col.not_null) res.not_null = true;
  return res;
}

inline table_definition dialect_oracle::fit_table(const table_definition& tbl, const std::string& schema)
{
  using namespace std;

  table_definition res(dialect::fit_table(tbl, schema));

  if (find_if(begin(res.indexes), end(res.indexes), [&](const index_definition& idx){ return Primary == idx.type || Unique == idx.type; }) == end(res.indexes))
  {
    column_definition col;
    col.name = fit_identifier("ID");
    col.type = String;
    col.dbms_type_lcase.name = "nvarchar2(32) default sys_guid()";
    col.chars = 32;
    col.not_null = true;
    res.columns.push_back(col);

    index_definition idx;
    idx.type = Primary;
    idx.columns.push_back(fit_identifier("ID"));
    res.indexes.push_back(idx);
  }

  return res;
}

inline std::string dialect_oracle::sql_srid(int epsg)
{
  return "SELECT SRID FROM MDSYS.SDO_COORD_REF_SYS WHERE DATA_SOURCE = 'EPSG' AND SRID = " + string_cast<char>(epsg);
}

inline void dialect_oracle::sql_register_spatial_column(const table_definition& tbl, const std::string& col, std::vector<std::string>& sql)
{
  using namespace std;
  using namespace brig::boost;

  sql.push_back("DELETE FROM MDSYS.USER_SDO_GEOM_METADATA WHERE TABLE_NAME = '" + tbl.id.name + "' AND COLUMN_NAME = '" + col + "'");
  auto col_def(tbl[col]);
  auto box(envelope(geom_from_wkb(::boost::get<blob_t>(col_def->query_value))));
  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
  stream << "\
INSERT INTO MDSYS.USER_SDO_GEOM_METADATA (TABLE_NAME, COLUMN_NAME, DIMINFO, SRID) VALUES \
( '" << tbl.id.name << "' \
, '" << col << "' \
, MDSYS.SDO_DIM_ARRAY(MDSYS.SDO_DIM_ELEMENT('X', " << xmin << ", " << xmax << ", 0.000001), MDSYS.SDO_DIM_ELEMENT('Y', " << ymin << ", " << ymax << ", 0.000001)) \
, " << col_def->srid << ")";
  sql.push_back(stream.str());
}

inline std::string dialect_oracle::sql_create_spatial_index(const table_definition& tbl, const std::string& col)
{
  return "CREATE INDEX " + sql_identifier(tbl.rtree(col)->id.name) + " ON " + sql_identifier(tbl.id.name) + " (" + sql_identifier(col) + ") INDEXTYPE IS MDSYS.SPATIAL_INDEX";
}

inline std::string dialect_oracle::sql_parameter(const command_traits& trs, const column_definition& param, size_t order)
{
  using namespace std;

  const string marker(trs.sql_parameter_marker(order));
  if (Geometry == param.type && !trs.writable_geometry)
  {
    string sql;
    const bool conv(param.dbms_type_lcase.name.compare("sdo_geometry") != 0);
    if (conv) sql += sql_regular_identifier(param.dbms_type_lcase) + "(";
    sql += "MDSYS.SDO_GEOMETRY(TO_BLOB(" + marker + "), " + string_cast<char>(param.srid) + ")";
    if (conv) sql += ")";
    return sql;
  }
  return marker;
}

inline std::string dialect_oracle::sql_column(const command_traits& trs, const column_definition& col)
{
  using namespace std;

  const string id(sql_identifier(col.name));
  if (!col.query_expression.empty()) return col.query_expression + " AS " + id;
  if (String == col.type && col.dbms_type_lcase.name.find("time") != string::npos) return "(TO_CHAR(" + id + ", 'YYYY-MM-DD') || 'T' || TO_CHAR(" + id + ", 'HH24:MI:SS')) AS " + id;
  if (String == col.type && col.dbms_type_lcase.name.find("date") != string::npos) return "TO_CHAR(" + id + ", 'YYYY-MM-DD') AS " + id;
  if (Geometry == col.type && !trs.readable_geometry) return sql_regular_identifier(col.dbms_type_lcase) + ".GET_WKB(" + id + ") AS " + id;
  return id;
}

inline void dialect_oracle::sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string&)
{
  const std::string sql_rows(string_cast<char>(rows));
  sql_infix = "/*+ FIRST_ROWS(" + sql_rows + ") */";
  sql_counter = "ROWNUM <= " + sql_rows;
}

inline bool dialect_oracle::need_to_normalize_hemisphere(const column_definition& col)
{
  return col.dbms_type_lcase.qualifier.find("geographic") != std::string::npos;
}

inline void dialect_oracle::sql_intersect(const command_traits& trs, const table_definition& tbl, const std::string& col, const std::vector<brig::boost::box>& boxes, std::string& sql, std::vector<column_definition>& keys)
{
  using namespace std;

  if (!tbl.rtree(col) || boxes.size() < 2) return;

  auto idx(find_if(begin(tbl.indexes), end(tbl.indexes), [&](const index_definition& i){ return Primary == i.type; }));
  if (idx == end(tbl.indexes)) idx = find_if(begin(tbl.indexes), end(tbl.indexes), [&](const index_definition& i){ return Unique == i.type; });
  if (idx == end(tbl.indexes)) throw runtime_error("unique columns error");
  keys = get_columns(tbl.columns, idx->columns);

  std::string sql_prefix, sql_infix, sql_counter, sql_suffix;
  if (tbl.query_rows >= 0) sql_limit(tbl.query_rows, sql_infix, sql_counter, sql_suffix);
  sql_prefix = "(SELECT " + sql_infix + " " + sql_select_list(this, trs, keys) + " FROM " + sql_identifier(tbl.id) + " WHERE (";
  sql_suffix = ")";
  if (!sql_counter.empty()) sql_suffix += " AND " + sql_counter;
  sql_suffix += ")";

  sql += "SELECT " + sql_infix + " DISTINCT * FROM (";
  for (auto box(begin(boxes)); box != end(boxes); ++box)
  {
    if (box != begin(boxes)) sql += " UNION ALL ";
    sql += sql_prefix + sql_intersect(tbl, col, *box) + sql_suffix;
  }
  sql += ")";
}

inline std::string dialect_oracle::sql_intersect(const table_definition& tbl, const std::string& col, const boost::box& box)
{
  using namespace std;

  const bool indexed(tbl.rtree(col) != 0);
  const string id(sql_identifier(col));
  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
  stream << "MDSYS.SDO_GEOMETRY(2003, " << tbl[col]->srid << ", NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1, 1003, 3), MDSYS.SDO_ORDINATE_ARRAY(" << xmin << ", " << ymin << ", " << xmax << ", " << ymax << "))";
  if (indexed)
    return "MDSYS.SDO_FILTER(" + id + ", " + stream.str() + ") = 'TRUE'";
  else
    return "MDSYS.SDO_GEOM.RELATE(" + id + ", 'anyinteract'" + ", " + stream.str() + ", 0.000001) = 'TRUE'";
} // dialect_oracle::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_ORACLE_HPP
