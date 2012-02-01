// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_IDENTIFIER_HPP
#define BRIG_DATABASE_DETAIL_SQL_IDENTIFIER_HPP

#include <brig/database/global.hpp>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace detail {

inline void sql_identifier(DBMS sys, const std::string& id, std::ostringstream& stream)
{
  stream << (sys == MySQL? '`': '"') << id << (sys == MySQL? '`': '"');
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_IDENTIFIER_HPP
