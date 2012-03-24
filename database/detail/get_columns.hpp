// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP
#define BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP

#include <brig/database/column_definition.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<column_definition> get_columns(const std::vector<column_definition>& cols, const std::vector<std::string>& names)
{
  std::vector<column_definition> res;
  for (size_t i(0); i < names.size(); ++i)
  {
    auto col(std::find_if(std::begin(cols), std::end(cols), [&](const column_definition& c){ return c.name == names[i]; }));
    if (col == std::end(cols)) throw std::runtime_error("table error");
    res.push_back(*col);
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP
