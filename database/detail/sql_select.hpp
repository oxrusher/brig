// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_SELECT_HPP
#define BRIG_DATABASE_DETAIL_SQL_SELECT_HPP

#include <algorithm>
#include <boost/geometry/algorithms/covered_by.hpp>
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
std::string sql_select(std::shared_ptr<Dialect> dct, const table_definition& tbl)
{
  std::string sql;

  const DBMS sys(dct->system());
  std::vector<column_definition> cols(tbl.columns);
  std::vector<column_definition> select_cols = tbl.select_columns.empty()? cols: get_columns(cols, tbl.select_columns);
  std::string sql_infix, sql_condition, sql_suffix;
  sql_limit(sys, tbl.select_rows, sql_infix, sql_condition, sql_suffix);

  // try no spatial first
  auto geom_col(std::find_if(std::begin(cols), std::end(cols), [&](const column_definition& c){ return c.name == tbl.select_box_column; }));
  if ( tbl.select_box_column.empty() ||
       geom_col != std::end(cols) && geom_col->mbr.type() == typeid(brig::boost::box) && ::boost::geometry::covered_by(::boost::get<brig::boost::box>(geom_col->mbr), tbl.select_box)
     )
  {
    if (!sql_condition.empty()) sql += "SELECT * FROM (";
    sql += "SELECT " + sql_infix + " " + sql_select_list(dct, select_cols) + " FROM " + sql_identifier(sys, tbl.id);
    if (!tbl.select_sql_condition.empty()) sql += " WHERE " + tbl.select_sql_condition;
    if (!sql_condition.empty()) sql += ") WHERE " + sql_condition;
    sql += " " + sql_suffix;
    return sql;
  }

  // spatial
  if (geom_col == std::end(cols)) throw std::runtime_error("SQL error");
  std::vector<brig::boost::box> boxes(1, tbl.select_box);
  normalize_hemisphere(boxes, sys, is_geodetic_type(sys, *geom_col));

  std::string sql_hint;
  if (MS_SQL == sys)
  {
    auto idx(std::find_if(std::begin(tbl.indexes), std::end(tbl.indexes), [&](const index_definition& i){ return Spatial == i.type && i.columns.front() == tbl.select_box_column; }));
    if (idx == std::end(tbl.indexes)) throw std::runtime_error("SQL error");
    sql_hint = "WITH(INDEX(" + sql_identifier(sys, idx->id) + "))";
  }

  std::vector<column_definition> unique_cols;
  auto idx(std::find_if(std::begin(tbl.indexes), std::end(tbl.indexes), [&](const index_definition& i){ return Primary == i.type; }));
  if (idx == std::end(tbl.indexes)) idx = std::find_if(std::begin(tbl.indexes), std::end(tbl.indexes), [&](const index_definition& i){ return Unique == i.type; });
  if (idx != std::end(tbl.indexes)) unique_cols = get_columns(cols, idx->columns);

  // key table
  std::string sql_key_tbl, sql_tbl(sql_identifier(sys, tbl.id));
  if (MS_SQL == sys && boxes.size() > 1)
  {
    if (unique_cols.empty()) throw std::runtime_error("SQL error");
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) sql_key_tbl += " UNION ";
      sql_key_tbl += "(SELECT " + sql_select_list(dct, unique_cols) + " FROM " + sql_tbl + " " + sql_hint + " WHERE " + sql_box_filter(sys, *geom_col, boxes[i]) + ")";
    }
  }
  else if (Oracle == sys && boxes.size() > 1)
  {
    if (unique_cols.empty()) throw std::runtime_error("SQL error");
    sql_key_tbl += "SELECT " + sql_infix + " DISTINCT * FROM (";
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) sql_key_tbl += " UNION ALL ";
      sql_key_tbl += "(SELECT " + sql_infix + " " + sql_select_list(dct, unique_cols) + " FROM " + sql_tbl + " WHERE " + sql_box_filter(sys, *geom_col, boxes[i]);
      if (!sql_condition.empty()) sql_key_tbl += " AND " + sql_condition;
      sql_key_tbl += ")";
    }
    sql_key_tbl += ")";
  }
  else if (SQLite == sys)
  {
    if (unique_cols.size() != 1) throw std::runtime_error("SQL error");
    sql_key_tbl += "SELECT pkid " + sql_identifier(sys, unique_cols[0].name) + " FROM " + sql_identifier(sys, "idx_" + tbl.id.name + "_" + tbl.select_box_column) + " WHERE ";
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) sql_key_tbl += " OR ";
      sql_key_tbl += sql_box_filter(sys, *geom_col, boxes[i]);
    }
  }

  // result
  if (!sql_condition.empty()) sql += "SELECT * FROM (";
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
    if (!tbl.select_sql_condition.empty()) sql += " AND " + tbl.select_sql_condition;
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
    if (!tbl.select_sql_condition.empty()) sql += " WHERE " + tbl.select_sql_condition;
    sql += ") v ON ";
    for (size_t i(0); i < unique_cols.size(); ++i)
    {
      if (i > 0) sql += " AND ";
      sql += "k." + sql_identifier(sys, unique_cols[i].name) + " = v." + sql_identifier(sys, unique_cols[i].name);
    }
  }
  sql += " " + sql_suffix;
  if (!sql_condition.empty()) sql += ") WHERE " + sql_condition;
  return sql;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_SELECT_HPP
