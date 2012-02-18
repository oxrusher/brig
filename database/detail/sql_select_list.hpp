// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_SELECT_LIST_HPP
#define BRIG_DATABASE_DETAIL_SQL_SELECT_LIST_HPP

#include <brig/database/column_detail.hpp>
#include <sstream>
#include <vector>

namespace brig { namespace database { namespace detail {

template <typename Dialect>
void sql_select_list(Dialect* dct, const std::vector<column_detail>& cols, std::ostringstream& stream)
{
  for (size_t i(0); i < cols.size(); ++i)
  {
    if (i > 0) stream << ", ";
    dct->sql_column(cols[i], stream);
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_SELECT_LIST_HPP
