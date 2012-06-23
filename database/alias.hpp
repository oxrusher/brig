// Andrew Naplavkov

#ifndef BRIG_DATABASE_ALIAS_HPP
#define BRIG_DATABASE_ALIAS_HPP

#include <string>

namespace brig { namespace database {

struct alias  { std::string name, query_expression; };

} } // brig::database

#endif // BRIG_DATABASE_ALIAS_HPP
