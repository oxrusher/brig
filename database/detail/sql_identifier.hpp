// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_IDENTIFIER_HPP
#define BRIG_DATABASE_DETAIL_SQL_IDENTIFIER_HPP

#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_identifier(DBMS sys, const std::string& id)
{
  return (sys == MySQL? '`': '"') + id + (sys == MySQL? '`': '"');
}

inline std::string sql_identifier(DBMS sys, const identifier& id)
{
  if (id.schema.empty()) return sql_identifier(sys, id.name);
  else return sql_identifier(sys, id.schema) + "." + sql_identifier(sys, id.name);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_IDENTIFIER_HPP
