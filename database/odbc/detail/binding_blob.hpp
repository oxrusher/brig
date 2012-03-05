// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_BLOB_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_BLOB_HPP

#include <brig/blob_t.hpp>
#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/get_sql_type.hpp>
#include <brig/database/odbc/detail/lib.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

class binding_blob : public binding {
  SQLPOINTER m_ptr;
  SQLLEN m_ind;
  DBMS m_sys;
public:
  binding_blob(const blob_t& blob, DBMS sys) : m_ptr((void*)blob.data()), m_ind(blob.size()), m_sys(sys)  {}
  virtual SQLSMALLINT c_type()  { return SQL_C_BINARY; }
  virtual SQLSMALLINT sql_type()  { return get_sql_type_blob(m_sys); }
  virtual SQLULEN precision()  { return m_ind; }
  virtual SQLPOINTER val_ptr()  { return m_ptr; }
  virtual SQLLEN* ind()  { return &m_ind; }
}; // binding_blob

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_BLOB_HPP
