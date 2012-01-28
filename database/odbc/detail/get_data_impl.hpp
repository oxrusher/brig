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
  virtual SQLRETURN operator()(SQLHSTMT stmt, int col, variant& var);
  static void assign(const T& from, variant& to)  { to = from; }
}; // get_data_impl

template <typename T, SQLSMALLINT TargetType>
SQLRETURN get_data_impl<T, TargetType>::operator()(SQLHSTMT stmt, int col, variant& var)
{
  T val;
  SQLLEN ind(SQL_NULL_DATA);
  SQLRETURN r(lib::singleton().p_SQLGetData(stmt, col + 1, TargetType, (SQLPOINTER)&val, sizeof(val), &ind));
  if (SQL_SUCCEEDED(r) && ind != SQL_NULL_DATA)
    assign(val, var);
  else
    var = null_t();
  return r;
}

template <>
inline void get_data_impl<DATE_STRUCT, SQL_C_TYPE_DATE>::assign(const DATE_STRUCT& from, variant& to)
{
  to = boost::gregorian::date(from.year, from.month, from.day);
}

template <>
inline void get_data_impl<TIMESTAMP_STRUCT, SQL_C_TYPE_TIMESTAMP>::assign(const TIMESTAMP_STRUCT& from, variant& to)
{
  to = boost::posix_time::ptime
    ( boost::gregorian::date(from.year, from.month, from.day)
    , boost::posix_time::time_duration(from.hour, from.minute, from.second)
    );
} // get_data_impl::

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_GET_DATA_IMPL_HPP
