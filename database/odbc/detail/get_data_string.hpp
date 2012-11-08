// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_GET_DATA_STRING_HPP
#define BRIG_DATABASE_ODBC_DETAIL_GET_DATA_STRING_HPP

#include <algorithm>
#include <brig/database/odbc/detail/get_data.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/database/variant.hpp>
#include <brig/unicode/transform.hpp>
#include <string>

namespace brig { namespace database { namespace odbc { namespace detail {

struct get_data_string : get_data {
  SQLRETURN operator()(SQLHSTMT stmt, size_t col, variant& var) override;
}; // get_data_string

inline SQLRETURN get_data_string::operator()(SQLHSTMT stmt, size_t col, variant& var)
{
  using namespace std;

  SQLWCHAR buf[SQL_MAX_MESSAGE_LENGTH];
  const SQLLEN reserved(sizeof(buf) - sizeof(SQLWCHAR));
  basic_string<SQLWCHAR> str;

  while (true)
  {
    SQLLEN ind(SQL_NULL_DATA);
    const SQLRETURN r(lib::singleton().p_SQLGetData(stmt, SQLUSMALLINT(col + 1), SQL_C_WCHAR, SQLPOINTER(buf), reserved, &ind));

    if (r == SQL_NO_DATA)  break;
    else if (!SQL_SUCCEEDED(r)) return r;

    if (ind == SQL_NULL_DATA)  { var = null_t(); return SQL_SUCCESS; }
    else if (ind == SQL_NO_TOTAL) ind = reserved;
    else if (ind < 0) return SQL_ERROR;

    buf[min<SQLLEN>(ind, reserved) / sizeof(SQLWCHAR)] = 0;
    str += buf;

    if (ind <= reserved) break;
  }

  var = brig::unicode::transform<string>(str);
  return SQL_SUCCESS;
} // get_data_string::

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_GET_DATA_STRING_HPP
