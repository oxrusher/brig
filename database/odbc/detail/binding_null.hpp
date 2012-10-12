// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_NULL_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_NULL_HPP

#include <brig/database/global.hpp>
#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/lib.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

class binding_null : public binding {
  SQLSMALLINT m_c_type, m_sql_type;
  SQLLEN m_ind;
public:
  binding_null(SQLSMALLINT c_type, SQLSMALLINT sql_type) : m_c_type(c_type), m_sql_type(sql_type), m_ind(SQL_NULL_DATA)  {}
  virtual SQLSMALLINT c_type()  { return m_c_type; }
  virtual SQLSMALLINT sql_type()  { return m_sql_type; }
  virtual SQLPOINTER val_ptr()  { return 0; }
  virtual SQLLEN* ind()  { return &m_ind; }
}; // binding_null

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_NULL_HPP
