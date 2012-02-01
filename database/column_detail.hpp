// Andrew Naplavkov

#ifndef BRIG_DATABASE_COLUMN_DETAIL_HPP
#define BRIG_DATABASE_COLUMN_DETAIL_HPP

#include <brig/database/object.hpp>
#include <string>

namespace brig { namespace database {

struct column_detail {
  std::string name;
  object type;
  std::string type_detail;
  int chars, precision, scale, srid, epsg;

  column_detail() : chars(-1), precision(-1), scale(-1), srid(-1), epsg(-1)  {}
  explicit column_detail(const std::string& name_) : name(name_), chars(-1), precision(-1), scale(-1), srid(-1), epsg(-1)  {}
  bool operator==(const column_detail& r) const  { return name == r.name; }
}; // column_detail

} } // brig::database

#endif // BRIG_DATABASE_COLUMN_DETAIL_HPP
