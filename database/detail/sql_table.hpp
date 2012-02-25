// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_TABLE_HPP
#define BRIG_DATABASE_DETAIL_SQL_TABLE_HPP

#include <brig/database/column_detail.hpp>
#include <brig/database/detail/get_columns.hpp>
#include <brig/database/detail/sql_limit.hpp>
#include <brig/database/detail/sql_object.hpp>
#include <brig/database/detail/sql_select_list.hpp>
#include <brig/database/global.hpp>
#include <brig/database/table_detail.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

template <typename Dialect>
std::string sql_table(Dialect* dct, const table_detail<column_detail>& tbl, const std::vector<std::string>& cols, int rows)
{
  const DBMS sys(dct->system());
  std::vector<column_detail> select_cols = cols.empty()? tbl.columns: get_columns(tbl, cols);
  std::string sql_infix, sql_condition, sql_suffix;
  sql_limit(sys, rows, sql_infix, sql_condition, sql_suffix);
  std::string res;

  res += "SELECT " + sql_infix + " " + sql_select_list(dct, select_cols) + " FROM " + sql_object(sys, tbl.table);
  if (!sql_condition.empty()) res += " WHERE " + sql_condition;
  res += " " + sql_suffix;

  return std::move(res);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_TABLE_HPP
