// Andrew Naplavkov

#ifndef BRIG_DATABASE_INDEX_DETAIL_HPP
#define BRIG_DATABASE_INDEX_DETAIL_HPP

#include <brig/database/global.hpp>
#include <brig/database/object.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct index_detail {
  object index;
  index_type type;
  std::vector<std::string> columns;

  index_detail() : type(VoidIndex)  {}
}; // index_detail

} } // brig::database

#endif // BRIG_DATABASE_INDEX_DETAIL_HPP
