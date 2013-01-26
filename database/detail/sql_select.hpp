// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_SELECT_HPP
#define BRIG_DATABASE_DETAIL_SQL_SELECT_HPP

#include <algorithm>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/normalize_hemisphere.hpp>
#include <brig/database/detail/sql_select_list.hpp>
#include <brig/detail/get_columns.hpp>
#include <brig/global.hpp>
#include <brig/table_def.hpp>
#include <iterator>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_select(dialect* dct, command* cmd, const table_def& tbl, std::string& sql, std::vector<column_def>& params)
{
  using namespace std;

  vector<column_def> cols = tbl.query_columns.empty()? tbl.columns: brig::detail::get_columns(tbl.columns, tbl.query_columns);
  string sql_infix, sql_counter, sql_suffix, sql_conditions;
  if (tbl.query_rows >= 0) dct->sql_limit(tbl.query_rows, sql_infix, sql_counter, sql_suffix);
  for (auto col(begin(tbl.columns)); col != end(tbl.columns); ++col)
    if (Geometry != col->type && typeid(null_t) != col->query_value.type())
    {
      if (!sql_conditions.empty()) sql_conditions += "AND ";
      sql_conditions += col->query_expression.empty()? col->name: col->query_expression;
      sql_conditions += " = (" + dct->sql_parameter(cmd, *col, params.size()) + ")"; // Oracle workaround
      params.push_back(*col);
    }

  // not spatial first
  auto geom_col(find_if(begin(tbl.columns), end(tbl.columns), [](const column_def& col){ return Geometry == col.type && typeid(null_t) != col.query_value.type(); }));
  if (geom_col == end(tbl.columns))
  {
    if (!sql_counter.empty()) sql += "SELECT * FROM (";
    sql += "SELECT " + sql_infix + " " + sql_select_list(dct, cmd, cols) + " FROM " + dct->sql_identifier(tbl.id);
    if (!sql_conditions.empty()) sql += " WHERE " + sql_conditions;
    if (!sql_counter.empty()) sql += ") WHERE " + sql_counter;
    sql += " " + sql_suffix;
    return;
  }

  // spatial
  vector<brig::boost::box> boxes(1, brig::boost::envelope(brig::boost::geom_from_wkb(::boost::get<blob_t>(geom_col->query_value))));
  if (dct->need_to_normalize_hemisphere(*geom_col)) normalize_hemisphere(boxes);
  string sql_keys;
  vector<column_def> keys;
  dct->sql_intersect(cmd, tbl, geom_col->name, boxes, sql_keys, keys);
  const string sql_tbl(dct->sql_identifier(tbl.id));

  if (!sql_counter.empty()) sql += "SELECT * FROM (";
  sql += "SELECT " + sql_infix + " ";
  if (sql_keys.empty())
  {
    sql += sql_select_list(dct, cmd, cols) + " FROM " + sql_tbl + " " + dct->sql_hint(tbl, geom_col->name) + " WHERE (";
    for (auto box(begin(boxes)); box != end(boxes); ++box)
    {
      if (box != begin(boxes)) sql += " OR ";
      sql += "(" + dct->sql_intersect(tbl, geom_col->name, *box) + ")";
    }
    sql += ")";
    if (!sql_conditions.empty()) sql += " AND " + sql_conditions;
  }
  else
  {
    for (auto col(begin(cols)); col != end(cols); ++col)
    {
      if (col != begin(cols)) sql += ", ";
      const string id(dct->sql_identifier(col->name));
      sql += "v." + id + " AS " + id;
    }
    sql += " FROM (" + sql_keys + ") k JOIN (SELECT " + sql_select_list(dct, cmd, cols);
    for (auto key(begin(keys)); key != end(keys); ++key)
      if (!find_column(begin(cols), end(cols), key->name))
        sql += ", " + dct->sql_column(cmd, *key);
    sql += " FROM " + sql_tbl;
    if (!sql_conditions.empty()) sql += " WHERE " + sql_conditions;
    sql += ") v ON ";
    for (auto key(begin(keys)); key != end(keys); ++key)
    {
      if (key != begin(keys)) sql += " AND ";
      const string id(dct->sql_identifier(key->name));
      sql += "k." + id + " = v." + id;
    }
  }
  sql += " " + sql_suffix;
  if (!sql_counter.empty()) sql += ") WHERE " + sql_counter;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_SELECT_HPP
