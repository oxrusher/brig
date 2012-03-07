// Andrew Naplavkov

#ifndef BRIG_DATABASE_TABLE_DEFINITION_HPP
#define BRIG_DATABASE_TABLE_DEFINITION_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/index_definition.hpp>
#include <brig/database/identifier.hpp>
#include <vector>

namespace brig { namespace database {

struct table_definition {
  identifier id;
  std::vector<column_definition> columns;
  std::vector<index_definition> indexes;
}; // table_definition

} } // brig::database

#endif // BRIG_DATABASE_TABLE_DEFINITION_HPP
