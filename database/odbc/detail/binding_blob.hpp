// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_BLOB_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_BLOB_HPP

#include <algorithm>
#include <brig/blob_t.hpp>
#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/lib.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

class binding_blob : public binding {
  SQLPOINTER m_ptr;
  SQLLEN m_ind;
  SQLSMALLINT m_sql_type;
public:
  binding_blob(const blob_t& blob, SQLSMALLINT sql_type) : m_ptr((void*)blob.data()), m_ind(blob.size()), m_sql_type(sql_type)  {}
  virtual SQLSMALLINT c_type()  { return SQL_C_BINARY; }
  virtual SQLSMALLINT sql_type()  { return m_sql_type; }
  virtual SQLULEN precision()  { return std::max<>(SQLULEN(m_ind), SQLULEN(1)); }
  virtual SQLPOINTER val_ptr()  { return m_ptr; }
  virtual SQLLEN* ind()  { return &m_ind; }
}; // binding_blob

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_BLOB_HPP
