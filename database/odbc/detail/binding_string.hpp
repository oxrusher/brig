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
  SQLSMALLINT c_type() override  { return SQL_C_WCHAR; }
  SQLSMALLINT sql_type() override  { return m_sql_type; }
  SQLULEN column_size() override  { return std::max<>(SQLULEN(m_str.size()), SQLULEN(1)); }
  SQLPOINTER val_ptr() override  { return SQLPOINTER(m_str.c_str()); }
  SQLLEN* ind() override  { return &m_ind; }
}; // binding_string

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_STRING_HPP
