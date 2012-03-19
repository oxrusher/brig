// Andrew Naplavkov

#ifndef BRIG_DATABASE_TABLE_DEFINITION_HPP
#define BRIG_DATABASE_TABLE_DEFINITION_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/column_definition.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/index_definition.hpp>
#include <brig/database/variant.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct table_definition {
  identifier id;
  std::vector<column_definition> columns;
  std::vector<index_definition> indexes;

  std::string box_column;
  brig::boost::box box;

  std::string sql_filter;
  std::vector<variant> parameters;

  std::vector<std::string> select_columns;
  int rows;

  table_definition() : box(brig::boost::point(0, 0), brig::boost::point(0, 0)), rows(-1)  {}
}; // table_definition

} } // brig::database

#endif // BRIG_DATABASE_TABLE_DEFINITION_HPP
