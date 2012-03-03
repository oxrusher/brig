// Andrew Naplavkov

#ifndef BRIG_DATABASE_OBJECT_HPP
#define BRIG_DATABASE_OBJECT_HPP

#include <string>

namespace brig { namespace database {

struct object  { std::string schema, name; };
struct subobject : object  { std::string qualifier; };

} } // brig::database

#endif // BRIG_DATABASE_OBJECT_HPP
