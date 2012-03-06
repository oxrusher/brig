// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_INSERT_HPP
#define BRIG_DATABASE_DETAIL_SQL_INSERT_HPP

#include <brig/database/column_detail.hpp>
#include <brig/database/detail/get_columns.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/table_definition.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

template <typename Dialect>
std::string sql_insert(Dialect* dct, const table_definition<column_detail>& tbl, const std::vector<std::string>& cols)
{
  const DBMS sys(dct->system());
  std::vector<column_detail> insert_cols = cols.empty()? tbl.columns: get_columns(tbl.columns, cols);
  std::string prefix, suffix;

  prefix += "INSERT INTO " + sql_identifier(sys, tbl.table) + "(";
  suffix += "VALUES(";
  for (size_t i(0); i < insert_cols.size(); ++i)
  {
    if (i > 0)
    {
      prefix += ", ";
      suffix += ", ";
    }
    prefix += sql_identifier(sys, insert_cols[i].name);
    suffix += dct->sql_parameter(i, insert_cols[i]);
  }
  prefix += ")";
  suffix += ")";

  return prefix + " " + suffix;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_INSERT_HPP
