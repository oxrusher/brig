// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_BLOB_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_BLOB_HPP

#include <brig/blob_t.hpp>
#include <brig/database/postgres/detail/get_value.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <cstring>

namespace brig { namespace database { namespace postgres { namespace detail {

struct get_value_blob : get_value {
  void operator()(PGresult* res, int row, int col, variant& var) override;
}; // get_value_blob

inline void get_value_blob::operator()(PGresult* res, int row, int col, variant& var)
{
  var = blob_t();
  blob_t& blob(::boost::get<blob_t>(var));
  blob.resize(lib::singleton().p_PQgetlength(res, row, col));
  memcpy(blob.data(), lib::singleton().p_PQgetvalue(res, row, col), blob.size());
} // get_value_blob::

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_BLOB_HPP
