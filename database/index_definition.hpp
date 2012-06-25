// Andrew Naplavkov

#ifndef BRIG_DATABASE_INDEX_DEFINITION_HPP
#define BRIG_DATABASE_INDEX_DEFINITION_HPP

#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct index_definition {
  identifier id;
  index_type type;
  std::vector<std::string> columns;

  index_definition() : type(VoidIndex)  {}
}; // index_definition

} } // brig::database

#endif // BRIG_DATABASE_INDEX_DEFINITION_HPP
