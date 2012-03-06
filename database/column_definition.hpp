// Andrew Naplavkov

#ifndef BRIG_DATABASE_COLUMN_DEFINITION_HPP
#define BRIG_DATABASE_COLUMN_DEFINITION_HPP

#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/database/global.hpp>
#include <string>

namespace brig { namespace database {

struct column_definition {
  std::string name;
  column_type type;
  int epsg;
  ::boost::variant<bool, brig::boost::box> mbr_need;

  column_definition() : type(VoidColumn), epsg(-1), mbr_need(false)  {}
}; // column_definition

} } // brig::database

#endif // BRIG_DATABASE_COLUMN_DEFINITION_HPP
