// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_INSERT_HPP
#define BRIG_DATABASE_DETAIL_SQL_INSERT_HPP

#include <brig/database/command_traits.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_columns.hpp>
#include <brig/database/table_definition.hpp>
#include <iterator>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::string sql_insert(dialect* dct, const command_traits& trs, const table_definition& tbl)
{
  using namespace std;

  vector<column_definition> cols = tbl.query_columns.empty()? tbl.columns: get_columns(tbl.columns, tbl.query_columns);
  string prefix, suffix;
  size_t order(0);

  prefix += "INSERT INTO " + dct->sql_identifier(tbl.id) + "(";
  suffix += "VALUES(";
  for (auto col(begin(cols)); col != end(cols); ++col)
  {
    if (col != begin(cols))
    {
      prefix += ", ";
      suffix += ", ";
    }
    prefix += dct->sql_identifier(col->name);
    suffix += dct->sql_parameter(trs, *col, order++);
  }
  prefix += ")";
  suffix += ")";

  return prefix + " " + suffix;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_INSERT_HPP
