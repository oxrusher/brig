// Andrew Naplavkov

#ifndef BRIG_DATABASE_TABLE_DETAIL_HPP
#define BRIG_DATABASE_TABLE_DETAIL_HPP

#include <brig/database/index_detail.hpp>
#include <brig/database/object.hpp>
#include <vector>

namespace brig { namespace database {

template <typename Column>
struct table_detail {
  object table;
  std::vector<Column> columns;
  std::vector<index_detail> indexes;
}; // table_detail

} } // brig::database

#endif // BRIG_DATABASE_TABLE_DETAIL_HPP
