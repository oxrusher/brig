// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_REGULAR_IDENTIFIER_HPP
#define BRIG_DATABASE_DETAIL_SQL_REGULAR_IDENTIFIER_HPP

#include <brig/database/identifier.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_regular_identifier(const identifier& id)
{
  return id.schema.empty()? id.name: (id.schema + "." + id.name);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_REGULAR_IDENTIFIER_HPP
