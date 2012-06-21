// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_GET_DATA_BLOB_HPP
#define BRIG_DATABASE_ODBC_DETAIL_GET_DATA_BLOB_HPP

#include <brig/blob_t.hpp>
#include <brig/database/odbc/detail/get_data.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/database/variant.hpp>
#include <vector>

namespace brig { namespace database { namespace odbc { namespace detail {

struct get_data_blob : get_data {
  virtual SQLRETURN operator()(SQLHSTMT stmt, size_t col, variant& var);
}; // get_data_blob

inline SQLRETURN get_data_blob::operator()(SQLHSTMT stmt, size_t col, variant& var)
{
  var = blob_t();
  blob_t& blob = ::boost::get<blob_t>(var);

  SQLLEN ind(SQL_NULL_DATA), reserved(0);
  SQLRETURN r(lib::singleton().p_SQLGetData(stmt, SQLUSMALLINT(col + 1), SQL_C_BINARY, SQLPOINTER(1), 0, &ind));

  while (true)
  {
    if (r == SQL_NO_DATA)  { ind = 0; break; }
    else if (!SQL_SUCCEEDED(r)) return r;

    if (ind == SQL_NULL_DATA)  { var = null_t(); return SQL_SUCCESS; }
    else if (ind == SQL_NO_TOTAL) ind = reserved + SQL_MAX_MESSAGE_LENGTH;
    else if (ind < 0) return SQL_ERROR;
    else if (ind <= reserved) break;

    reserved = ind - reserved;
    const size_t offset(blob.size());
    blob.resize(blob.size() + size_t(reserved));
    r = lib::singleton().p_SQLGetData(stmt, SQLUSMALLINT(col + 1), SQL_C_BINARY, SQLPOINTER(blob.data() + offset), reserved, &ind);
  }

  if (reserved > ind) blob.resize(blob.size() - size_t(reserved - ind));
  return SQL_SUCCESS;
} // get_data_blob::

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_GET_DATA_BLOB_HPP
