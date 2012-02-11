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
  std::string str;
  if (SQLite != sys && !obj.schema.empty())
    str += sql_identifier(sys, obj.schema) + '.';
  str += sql_identifier(sys, obj.name);
  return std::move(str);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_OBJECT_HPP
