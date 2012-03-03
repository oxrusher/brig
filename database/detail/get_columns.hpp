// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP
#define BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP

#include <brig/database/column_detail.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<column_detail> get_columns(const std::vector<column_detail>& cols, const std::vector<std::string>& names)
{
  std::vector<column_detail> res;
  for (size_t i(0); i < names.size(); ++i)
  {
    auto p_col = std::find_if(cols.begin(), cols.end(), [&](const column_detail& col){ return col.name == names[i]; });
    if (p_col == cols.end()) throw std::runtime_error("table error");
    res.push_back(*p_col);
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP
