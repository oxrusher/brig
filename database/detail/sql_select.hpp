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
#include <brig/database/detail/sql_box_filter.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_limit.hpp>
#include <brig/database/detail/sql_select_list.hpp>
#include <brig/database/global.hpp>
#include <brig/database/index_definition.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/database/variant.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

template <typename Dialect>
void sql_select
  ( std::shared_ptr<Dialect> dct, const table_definition& tbl
  , std::string& sql, std::vector<variant>& params
  )
{
  const DBMS sys(dct->system());
  std::vector<column_definition> cols(tbl.columns);
  std::vector<column_definition> select_cols = tbl.query_columns.empty()? cols: get_columns(cols, tbl.query_columns);
  std::string sql_infix, sql_counter, sql_suffix, sql_conditions;
  sql_limit(sys, tbl.query_rows, sql_infix, sql_counter, sql_suffix);
  for (auto col(std::begin(tbl.columns)); col != std::end(tbl.columns); ++col)
    if (Geometry != col->type && typeid(null_t) != col->query_condition.type())
    {
      if (!sql_conditions.empty()) sql_conditions += "AND ";
      sql_conditions += col->query_expression.empty()? col->name: col->query_expression;
      sql_conditions += " = (" + dct->sql_parameter(params.size(), *col) + ")"; // Oracle workaround
      params.push_back(col->query_condition);
    }

  // not spatial first
  auto geom_col(std::find_if(std::begin(cols), std::end(cols), [](const column_definition& col){ return Geometry == col.type && typeid(null_t) != col.query_condition.type(); }));
  if (geom_col == std::end(cols))
  {
    if (!sql_counter.empty()) sql += "SELECT * FROM (";
    sql += "SELECT " + sql_infix + " " + sql_select_list(dct, select_cols) + " FROM " + sql_identifier(sys, tbl);
    if (!sql_conditions.empty()) sql += " WHERE " + sql_conditions;
    if (!sql_counter.empty()) sql += ") WHERE " + sql_counter;
    sql += " " + sql_suffix;
    return;
  }

  // spatial
  std::vector<brig::boost::box> boxes(1, brig::boost::envelope(brig::boost::geom_from_wkb(::boost::get<blob_t>(geom_col->query_condition))));
  normalize_hemisphere(boxes, sys, is_geodetic_type(sys, *geom_col));

  std::string sql_hint;
  if (MS_SQL == sys)
  {
    auto idx(std::find_if(std::begin(tbl.indexes), std::end(tbl.indexes), [&](const index_definition& i){ return Spatial == i.type && i.columns.front() == geom_col->name; }));
    if (idx == std::end(tbl.indexes)) throw std::runtime_error("SQL error");
    sql_hint = "WITH(INDEX(" + sql_identifier(sys, *idx) + "))";
  }

  std::vector<column_definition> unique_cols;
  auto idx(std::find_if(std::begin(tbl.indexes), std::end(tbl.indexes), [&](const index_definition& i){ return Primary == i.type; }));
  if (idx == std::end(tbl.indexes)) idx = std::find_if(std::begin(tbl.indexes), std::end(tbl.indexes), [&](const index_definition& i){ return Unique == i.type; });
  if (idx != std::end(tbl.indexes)) unique_cols = get_columns(cols, idx->columns);

  // key table
  std::string sql_key_tbl, sql_tbl(sql_identifier(sys, tbl));
  if (MS_SQL == sys && boxes.size() > 1)
  {
    if (unique_cols.empty()) throw std::runtime_error("unique columns error");
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) sql_key_tbl += " UNION ";
      sql_key_tbl += "(SELECT " + sql_select_list(dct, unique_cols) + " FROM " + sql_tbl + " " + sql_hint + " WHERE " + sql_box_filter(sys, *geom_col, boxes[i]) + ")";
    }
  }
  else if (Oracle == sys && boxes.size() > 1)
  {
    if (unique_cols.empty()) throw std::runtime_error("unique columns error");
    sql_key_tbl += "SELECT " + sql_infix + " DISTINCT * FROM (";
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) sql_key_tbl += " UNION ALL ";
      sql_key_tbl += "(SELECT " + sql_infix + " " + sql_select_list(dct, unique_cols) + " FROM " + sql_tbl + " WHERE " + sql_box_filter(sys, *geom_col, boxes[i]);
      if (!sql_counter.empty()) sql_key_tbl += " AND " + sql_counter;
      sql_key_tbl += ")";
    }
    sql_key_tbl += ")";
  }
  else if (SQLite == sys)
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
    if (unique_cols.size() != 1) throw std::runtime_error("SQL error");
    sql_key_tbl += "SELECT pkid " + sql_identifier(sys, unique_cols[0].name) + " FROM " + sql_identifier(sys, "idx_" + tbl.name + "_" + geom_col->name) + " WHERE ";
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) sql_key_tbl += " OR ";
      sql_key_tbl += sql_box_filter(sys, *geom_col, boxes[i]);
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
      sql += sql_box_filter(sys, *geom_col, boxes[i]);
    }
    sql += ")";
    if (!sql_conditions.empty()) sql += " AND " + sql_conditions;
  }
  else
  {
    for (size_t i(0); i < select_cols.size(); ++i)
    {
      if (i > 0) sql += ", ";
      const std::string id(sql_identifier(sys, select_cols[i].name));
      sql += "v." + id + " " + id;
    }
    sql += " FROM (" + sql_key_tbl + ") k JOIN (SELECT " + sql_select_list(dct, select_cols);
    for (size_t i(0); i < unique_cols.size(); ++i)
      if (std::find_if(std::begin(select_cols), std::end(select_cols), [&](const column_definition& c){ return c.name == unique_cols[i].name; }) == std::end(select_cols))
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
