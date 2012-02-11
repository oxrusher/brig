// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_IDENTIFIER_HPP
#define BRIG_DATABASE_DETAIL_SQL_IDENTIFIER_HPP

#include <brig/database/global.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_identifier(DBMS sys, const std::string& id)
{
  return (sys == MySQL? '`': '"') + id + (sys == MySQL? '`': '"');
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_IDENTIFIER_HPP
