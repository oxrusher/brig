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
  virtual SQLRETURN operator()(SQLHSTMT stmt, int col, variant& var);
}; // get_data_string

inline SQLRETURN get_data_string::operator()(SQLHSTMT stmt, int col, variant& var)
{
  SQLWCHAR buf[SQL_MAX_MESSAGE_LENGTH];
  const SQLLEN reserved(sizeof(buf) - sizeof(SQLWCHAR));
  std::basic_string<SQLWCHAR> str;

  while (true)
  {
    SQLLEN ind(SQL_NULL_DATA);
    SQLRETURN r(lib::singleton().p_SQLGetData(stmt, col + 1, SQL_C_WCHAR, SQLPOINTER(buf), reserved, &ind));

    if (r == SQL_NO_DATA)  break;
    else if (!SQL_SUCCEEDED(r)) return r;

    if (ind == SQL_NULL_DATA)  { var = null_t(); return SQL_SUCCESS; }
    else if (ind == SQL_NO_TOTAL) ind = reserved;
    else if (ind < 0) return SQL_ERROR;

    buf[std::min<SQLLEN>(ind, reserved) / sizeof(SQLWCHAR)] = 0;
    str += buf;

    if (ind <= reserved) break;
  }

  var = brig::unicode::transform<std::string>(str);
  return SQL_SUCCESS;
} // get_data_string::

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_GET_DATA_STRING_HPP
