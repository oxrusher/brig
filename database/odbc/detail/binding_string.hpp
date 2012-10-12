// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_STRING_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_STRING_HPP

#include <algorithm>
#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/unicode/transform.hpp>
#include <string>

namespace brig { namespace database { namespace odbc { namespace detail {

class binding_string : public binding {
  SQLSMALLINT m_sql_type;
  std::basic_string<SQLWCHAR> m_str;
  SQLLEN m_ind;
public:
  binding_string(SQLSMALLINT sql_type, const std::string& str)
    : m_sql_type(sql_type)
    , m_str(brig::unicode::transform<decltype(m_str)>(str))
    { m_ind = m_str.size() * sizeof(SQLWCHAR); }
  virtual SQLSMALLINT c_type()  { return SQL_C_WCHAR; }
  virtual SQLSMALLINT sql_type()  { return m_sql_type; }
  virtual SQLULEN column_size()  { return std::max<>(SQLULEN(m_str.size()), SQLULEN(1)); }
  virtual SQLPOINTER val_ptr()  { return SQLPOINTER(m_str.c_str()); }
  virtual SQLLEN* ind()  { return &m_ind; }
}; // binding_string

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_STRING_HPP
