// Andrew Naplavkov

#ifndef BRIG_DATABASE_COLUMN_DETAIL_HPP
#define BRIG_DATABASE_COLUMN_DETAIL_HPP

#include <brig/database/object.hpp>
#include <string>

namespace brig { namespace database {

struct column_detail {
  std::string name;
  object type, lower_case_type;
  std::string type_detail, lower_case_type_detail;
  int chars, precision, scale, srid, epsg;

  column_detail() : chars(-1), precision(-1), scale(-1), srid(-1), epsg(-1)  {}
}; // column_detail

} } // brig::database

#endif // BRIG_DATABASE_COLUMN_DETAIL_HPP
