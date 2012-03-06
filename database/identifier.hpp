// Andrew Naplavkov

#ifndef BRIG_DATABASE_IDENTIFIER_HPP
#define BRIG_DATABASE_IDENTIFIER_HPP

#include <string>

namespace brig { namespace database {

struct identifier  { std::string schema, name, qualifier; };

} } // brig::database

#endif // BRIG_DATABASE_IDENTIFIER_HPP
