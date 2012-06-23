// Andrew Naplavkov

#ifndef BRIG_DATABASE_COLUMN_DEFINITION_HPP
#define BRIG_DATABASE_COLUMN_DEFINITION_HPP

#include <brig/database/alias.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/variant.hpp>

namespace brig { namespace database {

struct column_definition : alias {
  column_type type;
  identifier dbms_type, dbms_type_lcase;
  int chars, precision, scale, srid, epsg;
  bool not_null;

  variant query_condition;

  column_definition() : type(VoidColumn), chars(-1), precision(-1), scale(-1), srid(-1), epsg(-1), not_null(false)  {}
}; // column_definition

} } // brig::database

#endif // BRIG_DATABASE_COLUMN_DEFINITION_HPP
