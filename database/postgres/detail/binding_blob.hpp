// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_BINDING_BLOB_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_BINDING_BLOB_HPP

#include <brig/blob_t.hpp>
#include <brig/database/postgres/detail/binding.hpp>
#include <brig/database/postgres/detail/lib.hpp>

namespace brig { namespace database { namespace postgres { namespace detail {

class binding_blob : public binding {
  const blob_t& m_blob;
public:
  binding_blob(const blob_t& blob) : m_blob(blob)  {}
  Oid type() override  { return PG_TYPE_BYTEA; }
  const char* value() override  { return (const char*)m_blob.data(); }
  int length() override  { return int(m_blob.size()); }
  int format() override  { return 1; }
}; // binding_blob

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_BINDING_BLOB_HPP
