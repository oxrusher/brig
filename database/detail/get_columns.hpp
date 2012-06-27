// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP
#define BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP

#include <algorithm>
#include <brig/database/column_definition.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<column_definition> get_columns(const std::vector<column_definition>& cols, const std::vector<std::string>& names)
{
  using namespace std;
  vector<column_definition> res;
  for (size_t i(0); i < names.size(); ++i)
    res.push_back( *find_if(begin(cols), end(cols), [&](const column_definition& c){ return c.name == names[i]; }) );
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP
