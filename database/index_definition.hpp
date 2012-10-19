// Andrew Naplavkov

#ifndef BRIG_DATABASE_INDEX_DEFINITION_HPP
#define BRIG_DATABASE_INDEX_DEFINITION_HPP

#include <algorithm>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <iterator>
#include <string>
#include <vector>

namespace brig { namespace database {

struct index_definition {
  identifier id;
  index_type type;
  std::vector<std::string> columns;

  index_definition() : type(VoidIndex)  {}
}; // index_definition

template <typename Iterator>
typename std::iterator_traits<Iterator>::pointer find_rtree(Iterator beg, Iterator end, const std::string& col_name)
{
  auto idx_iter(std::find_if(beg, end, [&col_name](const index_definition& idx){ return Spatial == idx.type && idx.columns.size() == 1 && col_name.compare(idx.columns.front()) == 0; }));
  return idx_iter == end? 0: &*idx_iter;
}

} } // brig::database

#endif // BRIG_DATABASE_INDEX_DEFINITION_HPP
