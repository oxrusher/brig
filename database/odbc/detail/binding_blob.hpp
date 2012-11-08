// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_BLOB_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_BLOB_HPP

#include <algorithm>
#include <brig/blob_t.hpp>
#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/lib.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

class binding_blob : public binding {
  SQLSMALLINT m_sql_type;
  SQLPOINTER m_ptr;
  SQLLEN m_ind;
public:
  binding_blob(SQLSMALLINT sql_type, const blob_t& blob) : m_sql_type(sql_type), m_ptr((void*)blob.data()), m_ind(blob.size())  {}
  SQLSMALLINT c_type() override  { return SQL_C_BINARY; }
  SQLSMALLINT sql_type() override  { return m_sql_type; }
  SQLULEN column_size() override  { return std::max<>(SQLULEN(m_ind), SQLULEN(1)); }
  SQLPOINTER val_ptr() override  { return m_ptr; }
  SQLLEN* ind() override  { return &m_ind; }
}; // binding_blob

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_BLOB_HPP
