// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_ORACLE_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_ORACLE_HPP

#include <algorithm>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_iso_type.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/detail/get_columns.hpp>
#include <brig/global.hpp>
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
  std::string sql_spatial_detail(const table_def& tbl, const std::string& col) override;
  column_type get_type(const identifier& type_lcase, int scale) override;

  std::string sql_extent(const table_def& tbl, const std::string& col) override;

  std::string sql_schema() override;
  std::string fit_identifier(const std::string& id)  override;
  column_def fit_column(const column_def& col) override;
  table_def fit_table(const table_def& tbl, const std::string& schema) override;
  std::string sql_srid(int epsg) override;

  void sql_register_spatial_column(const table_def& tbl, const std::string& col, std::vector<std::string>& sql) override;
  std::string sql_create_spatial_index(const table_def& tbl, const std::string& col) override;

  std::string sql_parameter(command* cmd, const column_def& param, size_t order) override;
  std::string sql_column(command* cmd, const column_def& col) override;
  void sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string& sql_suffix) override;
  bool need_to_normalize_hemisphere(const column_def& col) override;
  void sql_intersect(command* cmd, const table_def& tbl, const std::string& col, const std::vector<brig::boost::box>& boxes, std::string& sql, std::vector<column_def>& keys) override;
  std::string sql_intersect(const table_def& tbl, const std::string& col, const boost::box& box) override;
}; // dialect_oracle

inline std::string dialect_oracle::sql_tables()
{
  // sample schemas: HR,OE,PM,IX,SH
  return "\
SELECT OWNER scm, TABLE_NAME tbl \
FROM ALL_TABLES \
WHERE OWNER NOT IN ('ANONYMOUS','APPQOSSYS','CTXSYS','DBSNMP','DIP','DMSYS','EXFSYS','LBACSYS','MDDATA','MDSYS','MGMT_VIEW','OLAPSYS','ORDDATA','ORDPLUGINS','ORDSYS','OUTLN','OWBSYS','SI_INFORMTN_SCHEMA','SYS','SYSMAN','TSMSYS','WKSYS','WMSYS','XDB') \
AND OWNER NOT LIKE 'APEX_%' \
AND TABLE_NAME NOT LIKE '%$%' \
AND NOT (OWNER = 'SYSTEM' AND (TABLE_NAME LIKE 'LOGMNR%' OR TABLE_NAME LIKE 'SQLPLUS%' OR TABLE_NAME IN ('HELP','IMAGEREF')))";
}

inline std::string dialect_oracle::sql_geometries()
{
  return "\
SELECT c.scm scm, c.tbl tbl, c.col col \
FROM (SELECT OWNER scm, TABLE_NAME tbl, COLUMN_NAME col FROM MDSYS.ALL_SDO_GEOM_METADATA) g \
JOIN (SELECT OWNER scm, TABLE_NAME tbl, COLUMN_NAME col FROM ALL_TAB_COLUMNS) c \
ON g.scm = c.scm AND UPPER(g.tbl) = UPPER(c.tbl) AND UPPER(g.col) = UPPER(c.col)";
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

inline std::string dialect_oracle::sql_spatial_detail(const table_def& tbl, const std::string& col)
{
  return "\
SELECT c.SRID, (CASE s.DATA_SOURCE WHEN 'EPSG' THEN s.SRID ELSE NULL END) epsg, s.COORD_REF_SYS_KIND \
FROM (SELECT * FROM MDSYS.ALL_SDO_GEOM_METADATA WHERE OWNER = '" + tbl.id.schema + "' AND UPPER(TABLE_NAME) = UPPER('" + tbl.id.name + "') AND UPPER(COLUMN_NAME) = UPPER('" + col + "')) c \
LEFT JOIN MDSYS.SDO_COORD_REF_SYS s ON c.SRID = s.SRID";
}

inline column_type dialect_oracle::get_type(const identifier& type_lcase, int scale)
{
  if (type_lcase.schema.compare("mdsys") == 0 && (type_lcase.name.compare("sdo_geometry") == 0 || is_ogc_type(type_lcase.name))) return column_type::Geometry;
  if (!type_lcase.schema.empty()) return column_type::Void;
  if (type_lcase.name.compare("long") == 0) return column_type::String;
  if (type_lcase.name.compare("bfile") == 0 || type_lcase.name.find("raw") != std::string::npos) return column_type::Blob;
  return get_iso_type(type_lcase.name, scale);
}

inline std::string dialect_oracle::sql_extent(const table_def& tbl, const std::string& col)
{
  const std::string t("\
SELECT ROWNUM n, d.SDO_LB l, d.SDO_UB u \
FROM (SELECT * FROM ALL_SDO_GEOM_METADATA WHERE OWNER = '" + tbl.id.schema + "' AND UPPER(TABLE_NAME) = UPPER('" + tbl.id.name + "') AND UPPER(COLUMN_NAME) = UPPER('" + col + "')) m, TABLE(m.DIMINFO) d");
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
  u32string u32(transform<char32_t>(id, upper_case));
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
  return transform<char>(u32);
}

inline column_def dialect_oracle::fit_column(const column_def& col)
{
  column_def res;
  res.name = fit_identifier(col.name);
  res.type = col.type;
  switch (res.type)
  {
  case column_type::Void: break;
  case column_type::Blob: res.type_lcase.name = "blob"; break;
  case column_type::Double: res.type_lcase.name = "binary_double"; break;
  case column_type::Geometry:
    res.type_lcase.schema = "mdsys";
    res.type_lcase.name = "sdo_geometry";
    res.epsg = col.epsg;
    if (typeid(blob_t) == col.query_value.type()) res.query_value = col.query_value;
    else res.query_value = blob_t();
    break;
  case column_type::Integer: res.type_lcase.name = "number(19)"; break;
  case column_type::String:
    res.chars = (col.chars > 0 && col.chars < CharsLimit)? col.chars: CharsLimit;
    res.type_lcase.name = "nvarchar2(" + string_cast<char>(res.chars) + ")";
    break;
  }
  if (col.not_null) res.not_null = true;
  return res;
}

inline table_def dialect_oracle::fit_table(const table_def& tbl, const std::string& schema)
{
  using namespace std;

  table_def res(dialect::fit_table(tbl, schema));

  if (find_if(begin(res.indexes), end(res.indexes), [&](const index_def& idx){ return index_type::Primary == idx.type || index_type::Unique == idx.type; }) == end(res.indexes))
  {
    column_def col;
    col.name = fit_identifier("ID");
    col.type = column_type::String;
    col.type_lcase.name = "nvarchar2(32) default sys_guid()";
    col.chars = 32;
    col.not_null = true;
    res.columns.push_back(col);

    index_def idx;
    idx.type = index_type::Primary;
    idx.columns.push_back(fit_identifier("ID"));
    res.indexes.push_back(idx);
  }

  return res;
}

inline std::string dialect_oracle::sql_srid(int epsg)
{
  return "SELECT SRID FROM MDSYS.SDO_COORD_REF_SYS WHERE DATA_SOURCE = 'EPSG' AND SRID = " + string_cast<char>(epsg);
}

inline void dialect_oracle::sql_register_spatial_column(const table_def& tbl, const std::string& col, std::vector<std::string>& sql)
{
  using namespace std;
  using namespace brig::boost;

  auto col_def(tbl[col]);
  auto box(envelope(geom_from_wkb(::boost::get<blob_t>(col_def->query_value))));
  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
  stream << "\
BEGIN \
DELETE FROM MDSYS.USER_SDO_GEOM_METADATA WHERE TABLE_NAME = '" << tbl.id.name << "' AND COLUMN_NAME = '" << col << "'; \
INSERT INTO MDSYS.USER_SDO_GEOM_METADATA (TABLE_NAME, COLUMN_NAME, DIMINFO, SRID) VALUES \
( '" << tbl.id.name << "' \
, '" << col << "' \
, MDSYS.SDO_DIM_ARRAY(MDSYS.SDO_DIM_ELEMENT('X', " << xmin << ", " << xmax << ", 0.000001), MDSYS.SDO_DIM_ELEMENT('Y', " << ymin << ", " << ymax << ", 0.000001)) \
, " << col_def->srid << "); \
END;";
  sql.push_back(stream.str());
}

inline std::string dialect_oracle::sql_create_spatial_index(const table_def& tbl, const std::string& col)
{
  return "CREATE INDEX " + sql_identifier(tbl.rtree(col)->id.name) + " ON " + sql_identifier(tbl.id.name) + " (" + sql_identifier(col) + ") INDEXTYPE IS MDSYS.SPATIAL_INDEX";
}

inline std::string dialect_oracle::sql_parameter(command* cmd, const column_def& param, size_t order)
{
  using namespace std;

  const string marker(cmd->sql_param(order));
  if (column_type::Geometry == param.type && !cmd->writable_geom())
  {
    string sql;
    const bool conv(param.type_lcase.name.compare("sdo_geometry") != 0);
    if (conv) sql += param.type_lcase.to_string() + "(";
    sql += "MDSYS.SDO_GEOMETRY(TO_BLOB(" + marker + "), " + string_cast<char>(param.srid) + ")";
    if (conv) sql += ")";
    return sql;
  }
  return marker;
}

inline std::string dialect_oracle::sql_column(command* cmd, const column_def& col)
{
  using namespace std;

  const string id(sql_identifier(col.name));
  if (!col.query_expression.empty()) return col.query_expression + " AS " + id;
  if (column_type::String == col.type && col.type_lcase.name.find("time") != string::npos) return "(TO_CHAR(" + id + ", 'YYYY-MM-DD') || 'T' || TO_CHAR(" + id + ", 'HH24:MI:SS')) AS " + id;
  if (column_type::String == col.type && col.type_lcase.name.find("date") != string::npos) return "TO_CHAR(" + id + ", 'YYYY-MM-DD') AS " + id;
  if (column_type::Geometry == col.type && !cmd->readable_geom()) return col.type_lcase.to_string() + ".GET_WKB(" + id + ") AS " + id;
  return id;
}

inline void dialect_oracle::sql_limit(int rows, std::string& sql_infix, std::string& sql_counter, std::string&)
{
  const std::string sql_rows(string_cast<char>(rows));
  sql_infix = "/*+ FIRST_ROWS(" + sql_rows + ") */";
  sql_counter = "ROWNUM <= " + sql_rows;
}

inline bool dialect_oracle::need_to_normalize_hemisphere(const column_def& col)
{
  return col.type_lcase.qualifier.find("geographic") != std::string::npos;
}

inline void dialect_oracle::sql_intersect(command* cmd, const table_def& tbl, const std::string& col, const std::vector<brig::boost::box>& boxes, std::string& sql, std::vector<column_def>& keys)
{
  using namespace std;

  if (!tbl.rtree(col) || boxes.size() < 2) return;

  auto idx(find_if(begin(tbl.indexes), end(tbl.indexes), [&](const index_def& i){ return index_type::Primary == i.type; }));
  if (idx == end(tbl.indexes)) idx = find_if(begin(tbl.indexes), end(tbl.indexes), [&](const index_def& i){ return index_type::Unique == i.type; });
  if (idx == end(tbl.indexes)) throw runtime_error("unique columns error");
  keys = brig::detail::get_columns(tbl.columns, idx->columns);

  std::string sql_prefix, sql_infix, sql_counter, sql_suffix;
  if (tbl.query_rows >= 0) sql_limit(tbl.query_rows, sql_infix, sql_counter, sql_suffix);
  sql_prefix = "(SELECT " + sql_infix + " " + sql_select_list(this, cmd, keys) + " FROM " + sql_identifier(tbl.id) + " WHERE (";
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

inline std::string dialect_oracle::sql_intersect(const table_def& tbl, const std::string& col, const boost::box& box)
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
