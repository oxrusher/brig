// Andrew Naplavkov

#ifndef BRIG_DATABASE_COLUMN_HPP
#define BRIG_DATABASE_COLUMN_HPP

#include <brig/database/object.hpp>
#include <string>

namespace brig { namespace database {

struct column  { object table; std::string name; };

} } // brig::database

#endif // BRIG_DATABASE_COLUMN_HPP
