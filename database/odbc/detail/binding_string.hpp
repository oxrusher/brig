// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_STRING_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_STRING_HPP

#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/unicode/transform.hpp>
#include <string>

namespace brig { namespace database { namespace odbc { namespace detail {

class binding_string : public binding {
  std::basic_string<SQLWCHAR> m_str;
  SQLSMALLINT m_sql_type;
  SQLLEN m_ind;
public:
  binding_string(const std::string& str, SQLSMALLINT sql_type)
    : m_str(brig::unicode::transform<std::basic_string<SQLWCHAR>>(str))
    , m_sql_type(sql_type)
    { m_ind = m_str.size() * sizeof(SQLWCHAR); }
  virtual SQLSMALLINT c_type()  { return SQL_C_WCHAR; }
  virtual SQLSMALLINT sql_type()  { return m_sql_type; }
  virtual SQLULEN precision()  { return m_str.size(); }
  virtual SQLPOINTER val_ptr()  { return SQLPOINTER(m_str.c_str()); }
  virtual SQLLEN* ind()  { return &m_ind; }
}; // binding_string

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_STRING_HPP
