// Andrew Naplavkov

#ifndef BRIG_DATABASE_TABLE_DETAIL_HPP
#define BRIG_DATABASE_TABLE_DETAIL_HPP

#include <brig/database/index_detail.hpp>
#include <vector>

namespace brig { namespace database {

template <typename Column>
struct table_detail {
  std::vector<Column> cols;
  std::vector<index_detail> idxs;
}; // table_detail

} } // brig::database

#endif // BRIG_DATABASE_TABLE_DETAIL_HPP
