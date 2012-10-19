// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_SELECT_HPP
#define BRIG_DATABASE_DETAIL_SQL_SELECT_HPP

#include <algorithm>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/database/column_definition.hpp>
#include <brig/database/detail/get_columns.hpp>
#include <brig/database/detail/is_geodetic_type.hpp>
#include <brig/database/detail/normalize_hemisphere.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_intersect.hpp>
#include <brig/database/detail/sql_limit.hpp>
#include <brig/database/detail/sql_select_list.hpp>
#include <brig/database/global.hpp>
#include <brig/database/index_definition.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/database/variant.hpp>
#include <ios>
#include <locale>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

template <typename Dialect>
void sql_select
  ( std::shared_ptr<Dialect> dct, const table_definition& tbl
  , std::string& sql, std::vector<column_definition>& params
  )
{
  using namespace std;

  const DBMS sys(dct->system());
  vector<column_definition> cols(tbl.columns);
  vector<column_definition> select_cols = tbl.query_columns.empty()? cols: get_columns(cols, tbl.query_columns);
  string sql_infix, sql_counter, sql_suffix, sql_conditions;
  sql_limit(sys, tbl.query_rows, sql_infix, sql_counter, sql_suffix);
  for (auto col(begin(tbl.columns)); col != end(tbl.columns); ++col)
    if (Geometry != col->type && typeid(null_t) != col->query_value.type())
    {
      if (!sql_conditions.empty()) sql_conditions += "AND ";
      sql_conditions += col->query_expression.empty()? col->name: col->query_expression;
      sql_conditions += " = (" + dct->sql_parameter(params.size(), *col) + ")"; // Oracle workaround
      params.push_back(*col);
    }

  // not spatial first
  auto geom_col(find_if(begin(cols), end(cols), [](const column_definition& col){ return Geometry == col.type && typeid(null_t) != col.query_value.type(); }));
  if (geom_col == end(cols))
  {
    if (!sql_counter.empty()) sql += "SELECT * FROM (";
    sql += "SELECT " + sql_infix + " " + sql_select_list(dct, select_cols) + " FROM " + sql_identifier(sys, tbl.id);
    if (!sql_conditions.empty()) sql += " WHERE " + sql_conditions;
    if (!sql_counter.empty()) sql += ") WHERE " + sql_counter;
    sql += " " + sql_suffix;
    return;
  }

  // spatial
  vector<brig::boost::box> boxes(1, brig::boost::envelope(brig::boost::geom_from_wkb(::boost::get<blob_t>(geom_col->query_value))));
  normalize_hemisphere(boxes, sys, is_geodetic_type(sys, *geom_col));
  auto spatial_idx(find_rtree(begin(tbl.indexes), end(tbl.indexes), geom_col->name));
  string sql_hint;
  if (MS_SQL == sys && spatial_idx != 0) sql_hint = "WITH(INDEX(" + sql_identifier(sys, spatial_idx->id) + "))";

  vector<column_definition> unique_cols;
  auto idx(find_if(begin(tbl.indexes), end(tbl.indexes), [&](const index_definition& i){ return Primary == i.type; }));
  if (idx == end(tbl.indexes)) idx = find_if(begin(tbl.indexes), end(tbl.indexes), [&](const index_definition& i){ return Unique == i.type; });
  if (idx != end(tbl.indexes)) unique_cols = get_columns(cols, idx->columns);

  // key table
  string sql_key_tbl, sql_tbl(sql_identifier(sys, tbl.id));
  if (MS_SQL == sys && boxes.size() > 1)
  {
    if (unique_cols.empty()) throw runtime_error("unique columns error");
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) sql_key_tbl += " UNION ";
      sql_key_tbl += "(SELECT " + sql_select_list(dct, unique_cols) + " FROM " + sql_tbl + " " + sql_hint + " WHERE (" + sql_intersect(sys, *geom_col, boxes[i], spatial_idx != 0) + "))";
    }
  }
  else if (Oracle == sys && boxes.size() > 1)
  {
    if (unique_cols.empty()) throw runtime_error("unique columns error");
    sql_key_tbl += "SELECT " + sql_infix + " DISTINCT * FROM (";
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) sql_key_tbl += " UNION ALL ";
      sql_key_tbl += "(SELECT " + sql_infix + " " + sql_select_list(dct, unique_cols) + " FROM " + sql_tbl + " WHERE (" + sql_intersect(sys, *geom_col, boxes[i], spatial_idx != 0) + ")";
      if (!sql_counter.empty()) sql_key_tbl += " AND " + sql_counter;
      sql_key_tbl += ")";
    }
    sql_key_tbl += ")";
  }
  else if (SQLite == sys && spatial_idx != 0)
  {
    if (unique_cols.empty())
    {
      column_definition col;
      col.name = "rowid";
      col.type = Integer;
      col.dbms_type.name = "int";
      col.dbms_type_lcase.name = "int";
      unique_cols.push_back(col);
    }
    if (unique_cols.size() != 1) throw runtime_error("unique columns error");
    sql_key_tbl += "SELECT pkid " + sql_identifier(sys, unique_cols[0].name) + " FROM " + sql_identifier(sys, "idx_" + tbl.id.name + "_" + geom_col->name) + " WHERE ";
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) sql_key_tbl += " OR ";
      const double xmin(boxes[i].min_corner().get<0>()), ymin(boxes[i].min_corner().get<1>()), xmax(boxes[i].max_corner().get<0>()), ymax(boxes[i].max_corner().get<1>());
      ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
      stream << "(xmax >= " << xmin << " AND xmin <= " << xmax << " AND ymax >= " << ymin << " AND ymin <= " << ymax << ")";
      sql_key_tbl += stream.str();
    }
  }

  // result
  if (!sql_counter.empty()) sql += "SELECT * FROM (";
  sql += "SELECT " + sql_infix + " ";
  if (sql_key_tbl.empty())
  {
    sql += sql_select_list(dct, select_cols) + " FROM " + sql_tbl + " " + sql_hint + " WHERE (";
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) sql += " OR ";
      sql += "(" + sql_intersect(sys, *geom_col, boxes[i], spatial_idx != 0) + ")";
    }
    sql += ")";
    if (!sql_conditions.empty()) sql += " AND " + sql_conditions;
  }
  else
  {
    for (size_t i(0); i < select_cols.size(); ++i)
    {
      if (i > 0) sql += ", ";
      const string id(sql_identifier(sys, select_cols[i].name));
      sql += "v." + id + " " + id;
    }
    sql += " FROM (" + sql_key_tbl + ") k JOIN (SELECT " + sql_select_list(dct, select_cols);
    for (size_t i(0); i < unique_cols.size(); ++i)
      if (!find_column(begin(select_cols), end(select_cols), unique_cols[i].name))
        sql += ", " + dct->sql_column(unique_cols[i]);
    sql += " FROM " + sql_tbl;
    if (!sql_conditions.empty()) sql += " WHERE " + sql_conditions;
    sql += ") v ON ";
    for (size_t i(0); i < unique_cols.size(); ++i)
    {
      if (i > 0) sql += " AND ";
      sql += "k." + sql_identifier(sys, unique_cols[i].name) + " = v." + sql_identifier(sys, unique_cols[i].name);
    }
  }
  sql += " " + sql_suffix;
  if (!sql_counter.empty()) sql += ") WHERE " + sql_counter;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_SELECT_HPP
