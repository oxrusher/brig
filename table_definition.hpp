// Andrew Naplavkov

#ifndef BRIG_TABLE_DEFINITION_HPP
#define BRIG_TABLE_DEFINITION_HPP

#include <algorithm>
#include <brig/column_definition.hpp>
#include <brig/global.hpp>
#include <brig/identifier.hpp>
#include <brig/index_definition.hpp>
#include <iterator>
#include <string>
#include <vector>

namespace brig {

struct table_definition {
  identifier id;
  std::vector<column_definition> columns;
  std::vector<index_definition> indexes;

  std::vector<std::string> query_columns;
  int query_rows;

  table_definition() : query_rows(-1)  {}
  column_definition* operator [](const std::string& col_name);
  const column_definition* operator [](const std::string& col_name) const;
  const index_definition* rtree(const std::string& col_name) const;
}; // table_definition

inline column_definition* table_definition::operator [](const std::string& col_name)
{
  using namespace std;
  return find_column(begin(columns), end(columns), col_name);
}

inline const column_definition* table_definition::operator [](const std::string& col_name) const
{
  using namespace std;
  return find_column(begin(columns), end(columns), col_name);
}

inline const index_definition* table_definition::rtree(const std::string& col_name) const
{
  using namespace std;
  auto idx_iter(find_if(begin(indexes), end(indexes), [&col_name](const index_definition& idx){ return Spatial == idx.type && idx.columns.size() == 1 && col_name.compare(idx.columns.front()) == 0; }));
  return idx_iter == end(indexes)? 0: &*idx_iter;
} // table_definition::

} // brig

#endif // BRIG_TABLE_DEFINITION_HPP
