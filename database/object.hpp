// Andrew Naplavkov

#ifndef BRIG_DATABASE_OBJECT_HPP
#define BRIG_DATABASE_OBJECT_HPP

#include <string>

namespace brig { namespace database {

struct object {
  std::string schema, name;

  bool operator==(const object& r) const  { return schema == r.schema && name == r.name; }
}; // object

} } // brig::database

#endif // BRIG_DATABASE_OBJECT_HPP
