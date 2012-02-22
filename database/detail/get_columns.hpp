// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP
#define BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP

#include <boost/algorithm/string.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/table_detail.hpp>
#include <locale>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<column_detail> get_columns(const table_detail<column_detail>& tbl, const std::vector<std::string>& cols)
{
  auto loc = std::locale::classic();
  std::vector<column_detail> res;
  for (size_t i(0); i < cols.size(); ++i)
  {
    auto p_col = std::find_if(tbl.columns.begin(), tbl.columns.end(), [&](const column_detail& col){ return ::boost::algorithm::iequals(col.name,  cols[i], loc); });
    if (p_col == tbl.columns.end()) throw std::runtime_error("table error");
    res.push_back(*p_col);
  }
  return std::move(res);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP
