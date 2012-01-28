// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_GET_DATA_IMPL_HPP
#define BRIG_DATABASE_ODBC_DETAIL_GET_DATA_IMPL_HPP

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <brig/database/odbc/detail/get_data.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/database/variant.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

template <typename T, SQLSMALLINT TargetType>
struct get_data_impl : get_data {
  virtual SQLRETURN operator()(SQLHSTMT stmt, size_t col, variant& var);
}; // get_data_impl

template <typename T>
void assign(const T& from, variant& to)
{
  to = from;
}

template <>
inline void assign<DATE_STRUCT>(const DATE_STRUCT& from, variant& to)
{
  to = boost::gregorian::date(from.year, from.month, from.day);
}

template <>
inline void assign<TIMESTAMP_STRUCT>(const TIMESTAMP_STRUCT& from, variant& to)
{
  to = boost::posix_time::ptime
    ( boost::gregorian::date(from.year, from.month, from.day)
    , boost::posix_time::time_duration(from.hour, from.minute, from.second)
    );
}

template <typename T, SQLSMALLINT TargetType>
SQLRETURN get_data_impl<T, TargetType>::operator()(SQLHSTMT stmt, size_t col, variant& var)
{
  T val;
  SQLLEN ind(SQL_NULL_DATA);
  const SQLRETURN r(lib::singleton().p_SQLGetData(stmt, SQLUSMALLINT(col + 1), TargetType, SQLPOINTER(&val), sizeof(val), &ind));
  if (!SQL_SUCCEEDED(r) || SQL_NULL_DATA == ind) var = null_t();
  else assign(val, var);
  return r;
} // get_data_impl::

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_GET_DATA_IMPL_HPP
