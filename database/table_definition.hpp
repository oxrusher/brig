// Andrew Naplavkov

#ifndef BRIG_DATABASE_TABLE_DEFINITION_HPP
#define BRIG_DATABASE_TABLE_DEFINITION_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/index_definition.hpp>
#include <stdexcept>
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
  column_definition* operator [](const std::string& col_name);
}; // table_definition

inline column_definition* table_definition::operator [](const std::string& col_name)
{
  for (size_t i(0); i < columns.size(); ++i)
    if (col_name.compare(columns[i].name) == 0) return &columns[i];
  throw std::runtime_error("table error");
} // table_definition::

} } // brig::database

#endif // BRIG_DATABASE_TABLE_DEFINITION_HPP
