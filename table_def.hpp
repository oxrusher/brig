// Andrew Naplavkov

#ifndef BRIG_TABLE_DEF_HPP
#define BRIG_TABLE_DEF_HPP

#include <algorithm>
#include <brig/column_def.hpp>
#include <brig/global.hpp>
#include <brig/identifier.hpp>
#include <brig/index_def.hpp>
#include <iterator>
#include <string>
#include <vector>

namespace brig {

struct table_def {
  identifier id;
  std::vector<column_def> columns;
  std::vector<index_def> indexes;

  std::vector<std::string> query_columns;
  int query_rows;

  table_def() : query_rows(-1)  {}
  column_def* operator [](const std::string& col_name);
  const column_def* operator [](const std::string& col_name) const;
  const index_def* rtree(const std::string& col_name) const;
}; // table_def

inline column_def* table_def::operator [](const std::string& col_name)
{
  using namespace std;
  return find_column(begin(columns), end(columns), col_name);
}

inline const column_def* table_def::operator [](const std::string& col_name) const
{
  using namespace std;
  return find_column(begin(columns), end(columns), col_name);
}

inline const index_def* table_def::rtree(const std::string& col_name) const
{
  using namespace std;
  auto idx_iter(find_if(begin(indexes), end(indexes), [&col_name](const index_def& idx){ return Spatial == idx.type && idx.columns.size() == 1 && col_name.compare(idx.columns.front()) == 0; }));
  return idx_iter == end(indexes)? 0: &*idx_iter;
} // table_def::

} // brig

#endif // BRIG_TABLE_DEF_HPP
