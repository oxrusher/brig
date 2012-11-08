// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_BINDING_NULL_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_BINDING_NULL_HPP

#include <brig/database/postgres/detail/binding.hpp>
#include <brig/database/postgres/detail/lib.hpp>

namespace brig { namespace database { namespace postgres { namespace detail {

struct binding_null : binding {
  binding_null()  {}
  Oid type() override  { return PG_TYPE_TEXT; }
  const char* value() override  { return 0; }
  int length() override  { return 0; }
  int format() override  { return 0; }
}; // binding_null

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_BINDING_NULL_HPP
