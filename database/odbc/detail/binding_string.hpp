// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_STRING_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_STRING_HPP

#include <algorithm>
#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/get_sql_type.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/unicode/transform.hpp>
#include <string>

namespace brig { namespace database { namespace odbc { namespace detail {

class binding_string : public binding {
  std::basic_string<SQLWCHAR> m_str;
  SQLLEN m_ind;
  DBMS m_sys;
public:
  binding_string(const std::string& str, DBMS sys)
    : m_str(brig::unicode::transform<decltype(m_str)>(str))
    , m_sys(sys)
    { m_ind = m_str.size() * sizeof(SQLWCHAR); }
  virtual SQLSMALLINT c_type()  { return SQL_C_WCHAR; }
  virtual SQLSMALLINT sql_type()  { return get_sql_type_string(m_sys); }
  virtual SQLULEN precision()  { return std::max<>(SQLULEN(m_str.size()), SQLULEN(1)); }
  virtual SQLPOINTER val_ptr()  { return SQLPOINTER(m_str.c_str()); }
  virtual SQLLEN* ind()  { return &m_ind; }
}; // binding_string

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_STRING_HPP
