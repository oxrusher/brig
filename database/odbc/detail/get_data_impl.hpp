// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_GET_DATA_IMPL_HPP
#define BRIG_DATABASE_ODBC_DETAIL_GET_DATA_IMPL_HPP

#include <brig/database/odbc/detail/get_data.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/variant.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

template <typename T, SQLSMALLINT TargetType>
struct get_data_impl : get_data {
  SQLRETURN operator()(SQLHSTMT stmt, size_t col, variant& var) override;
}; // get_data_impl

template <typename T, SQLSMALLINT TargetType>
SQLRETURN get_data_impl<T, TargetType>::operator()(SQLHSTMT stmt, size_t col, variant& var)
{
  T val;
  SQLLEN ind(SQL_NULL_DATA);
  const SQLRETURN r(lib::singleton().p_SQLGetData(stmt, SQLUSMALLINT(col + 1), TargetType, SQLPOINTER(&val), sizeof(val), &ind));
  if (!SQL_SUCCEEDED(r) || SQL_NULL_DATA == ind) var = null_t();
  else var = val;
  return r;
} // get_data_impl::

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_GET_DATA_IMPL_HPP
