// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_SELECT_LIST_HPP
#define BRIG_DATABASE_DETAIL_SQL_SELECT_LIST_HPP

#include <brig/database/column_definition.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

template <typename Dialect>
std::string sql_select_list(Dialect* dct, const std::vector<column_definition>& cols)
{
  std::string res;
  for (size_t i(0); i < cols.size(); ++i)
  {
    if (i > 0) res += ", ";
    res += dct->sql_column(cols[i]);
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_SELECT_LIST_HPP
