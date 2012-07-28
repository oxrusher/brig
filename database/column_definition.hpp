// Andrew Naplavkov

#ifndef BRIG_DATABASE_COLUMN_DEFINITION_HPP
#define BRIG_DATABASE_COLUMN_DEFINITION_HPP

#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/variant.hpp>
#include <string>

namespace brig { namespace database {

struct column_definition {
  std::string name;
  column_type type;
  identifier dbms_type, dbms_type_lcase;
  int chars, srid, epsg;
  bool not_null;

  std::string query_expression;
  variant query_value;

  column_definition() : type(VoidColumn), chars(-1), srid(-1), epsg(-1), not_null(false)  {}
}; // column_definition

} } // brig::database

#endif // BRIG_DATABASE_COLUMN_DEFINITION_HPP
