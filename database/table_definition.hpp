// Andrew Naplavkov

#ifndef BRIG_DATABASE_TABLE_DEFINITION_HPP
#define BRIG_DATABASE_TABLE_DEFINITION_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/index_definition.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct table_definition : identifier {
  std::vector<column_definition> columns;
  std::vector<index_definition> indexes;

  std::vector<std::string> query_columns;
  int query_rows;

  table_definition() : query_rows(-1)  {}
}; // table_definition

} } // brig::database

#endif // BRIG_DATABASE_TABLE_DEFINITION_HPP
