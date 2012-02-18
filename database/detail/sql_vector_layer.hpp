// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_VECTOR_LAYER_HPP
#define BRIG_DATABASE_DETAIL_SQL_VECTOR_LAYER_HPP

#include <algorithm>
#include <brig/boost/geometry.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_geodetic_type.hpp>
#include <brig/database/detail/normalize_hemisphere.hpp>
#include <brig/database/detail/sql_box_filter.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_limit.hpp>
#include <brig/database/detail/sql_object.hpp>
#include <brig/database/detail/sql_select_list.hpp>
#include <brig/database/global.hpp>
#include <brig/database/index_detail.hpp>
#include <brig/database/table_detail.hpp>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

template <typename Dialect>
inline std::string sql_vector_layer
  ( Dialect* dct
  , const table_detail<column_detail>& tbl, const std::string& lr, const boost::box& box
  , const std::vector<std::string> cols, int rows
  )
{
  auto lr_col = std::find_if(tbl.columns.begin(), tbl.columns.end(), [&](const column_detail& col){ return col.name == lr; });
  if (lr_col == tbl.columns.end()) throw std::runtime_error("SQL error");

  const DBMS sys(dct->system());
  std::vector<boost::box> boxes(1, box);
  normalize_hemisphere(boxes, sys, is_geodetic_type(sys, *lr_col));

  std::vector<column_detail> select_cols;
  for (size_t i(0); i < cols.size(); ++i)
  {
    auto p_col = std::find_if(tbl.columns.begin(), tbl.columns.end(), [&](const column_detail& col){ return col.name == cols[i]; });
    if (p_col == tbl.columns.end()) throw std::runtime_error("SQL error");
    select_cols.push_back(*p_col);
  }
  if (select_cols.empty()) select_cols.push_back(*lr_col);

  std::string sql_prefix, sql_infix, sql_suffix, sql_postfix;
  sql_limit(sys, rows, sql_prefix, sql_infix, sql_suffix, sql_postfix);

  std::string sql_hint;
  if (MS_SQL == sys)
  {
    auto p_idx = std::find_if(tbl.indexes.begin(), tbl.indexes.end(), [&](const index_detail& idx){ return idx.type == Spatial && idx.columns.front() == lr; });
    if (p_idx == tbl.indexes.end()) throw std::runtime_error("SQL error");
    sql_hint = "WITH(INDEX(" + sql_object(sys, p_idx->index) + "))";
  }

  std::vector<column_detail> unique_cols;
  auto p_idx = std::find_if(tbl.indexes.begin(), tbl.indexes.end(), [&](const index_detail& idx){ return idx.type == Primary; });
  if (p_idx == tbl.indexes.end()) p_idx = std::find_if(tbl.indexes.begin(), tbl.indexes.end(), [&](const index_detail& idx){ return idx.type == Unique; });
  if (p_idx != tbl.indexes.end())
    for (size_t i(0); i < p_idx->columns.size(); ++i)
    {
      auto p_col = std::find_if(tbl.columns.begin(), tbl.columns.end(), [&](const column_detail& col){ return col.name ==  p_idx->columns[i]; });
      if (p_col == tbl.columns.end()) throw std::runtime_error("SQL error");
      unique_cols.push_back(*p_col);
    }

  std::ostringstream stream;
  stream.imbue(std::locale::classic());
  if (MS_SQL == sys && boxes.size() > 1)
  {
    if (unique_cols.empty()) throw std::runtime_error("SQL error");
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) stream << " UNION ";
      stream << "(SELECT ";
      sql_select_list(dct, unique_cols, stream);
      stream << " FROM " << sql_object(sys, tbl.table) << " " << sql_hint << " WHERE " << sql_box_filter(sys, *lr_col, boxes[i]) << ")";
    }
  }
  else if (Oracle == sys && boxes.size() > 1)
  {
    if (unique_cols.empty()) throw std::runtime_error("SQL error");
    stream << "SELECT " << sql_infix << " DISTINCT * FROM (";
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) stream << " UNION ALL ";
      stream << "(SELECT " << sql_infix << " ";
      sql_select_list(dct, unique_cols, stream);
      stream << " FROM " << sql_object(sys, tbl.table) << " WHERE " << sql_box_filter(sys, *lr_col, boxes[i]);
      if (rows >= 0) stream << " AND ROWNUM <= " << rows;
      stream << ")";
    }
    stream << ")";
  }
  else if (SQLite == sys)
  {
    if (unique_cols.size() != 1) throw std::runtime_error("SQL error");
    stream << "SELECT pkid " << sql_identifier(sys, unique_cols[0].name) << " FROM " << sql_identifier(sys, "idx_" + tbl.table.name + "_" + lr) << " WHERE ";
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) stream << " OR ";
      stream << sql_box_filter(sys, *lr_col, boxes[i]);
    }
  }
  const std::string sql_key_tbl(stream.str());

  stream = std::ostringstream();
  stream << sql_prefix << " ";
  if (sql_key_tbl.empty())
  {
    stream << "SELECT " << sql_infix << " ";
    sql_select_list(dct, select_cols, stream);
    stream << " FROM " << sql_object(sys, tbl.table) << " " << sql_hint << " WHERE ";
    for (size_t i(0); i < boxes.size(); ++i)
    {
      if (i > 0) stream << " OR ";
      stream << sql_box_filter(sys, *lr_col, boxes[i]);
    }
    stream << " " << sql_suffix;
  }
  else
  {
    stream << "SELECT " << sql_infix << " v.* FROM (" << sql_key_tbl << ") k INNER JOIN (SELECT ";
    sql_select_list(dct, select_cols, stream);
    for (size_t i(0); i < unique_cols.size(); ++i)
      if (std::find_if(select_cols.begin(), select_cols.end(), [&](const column_detail& col){ return col.name == unique_cols[i].name; }) == select_cols.end())
      {
        stream << ", ";
        dct->sql_column(unique_cols[i], stream);
      }
    stream << " FROM " << sql_object(sys, tbl.table) << ") v ON ";
    for (size_t i(0); i < unique_cols.size(); ++i)
    {
      if (i > 0) stream << " AND ";
      stream << "k." << sql_identifier(sys, unique_cols[i].name) << " = v." << sql_identifier(sys, unique_cols[i].name);
    }
    stream << " " << sql_suffix;
  }
  stream << " " << sql_postfix;
  return stream.str();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_VECTOR_LAYER_HPP
