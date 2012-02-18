// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_OBJECT_HPP
#define BRIG_DATABASE_DETAIL_SQL_OBJECT_HPP

#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/object.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_object(DBMS sys, const object& obj)
{
  return (obj.schema.empty()? std::string(): sql_identifier(sys, obj.schema) + '.') + sql_identifier(sys, obj.name);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_OBJECT_HPP
