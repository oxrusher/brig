// Andrew Naplavkov

#ifndef BRIG_DATABASE_TABLE_DEFINITION_HPP
#define BRIG_DATABASE_TABLE_DEFINITION_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/index_definition.hpp>
#include <iterator>
#include <string>
#include <vector>

namespace brig { namespace database {

struct table_definition {
  identifier id;
  std::vector<column_definition> columns;
  std::vector<index_definition> indexes;

  std::vector<std::string> query_columns;
  int query_rows;

  table_definition() : query_rows(-1)  {}
  const column_definition* operator [](const std::string& col_name) const
    { return find_column(std::begin(columns), std::end(columns), col_name); }
  column_definition* operator [](const std::string& col_name)
    { return find_column(std::begin(columns), std::end(columns), col_name); }
}; // table_definition

} } // brig::database

#endif // BRIG_DATABASE_TABLE_DEFINITION_HPP
