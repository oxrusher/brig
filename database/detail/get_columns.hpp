// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP
#define BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP

#include <brig/database/column_definition.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<column_definition> get_columns(const std::vector<column_definition>& cols, const std::vector<std::string>& names)
{
  using namespace std;
  vector<column_definition> res;
  for (auto name(begin(names)); name != end(names); ++name)
    res.push_back( *find_column(begin(cols), end(cols), *name) );
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_COLUMNS_HPP
