// Andrew Naplavkov

#ifndef BRIG_DATABASE_COLUMN_DEFINITION_HPP
#define BRIG_DATABASE_COLUMN_DEFINITION_HPP

#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <string>

namespace brig { namespace database {

struct column_definition {
  std::string name, sql_expression;
  identifier dbms_type, lower_case_type;
  column_type type;
  int chars, precision, scale, srid, epsg;
  ::boost::variant<bool, brig::boost::box> mbr;

  column_definition() : type(VoidColumn), chars(-1), precision(-1), scale(-1), srid(-1), epsg(-1), mbr(false)  {}
}; // column_definition

} } // brig::database

#endif // BRIG_DATABASE_COLUMN_DEFINITION_HPP
