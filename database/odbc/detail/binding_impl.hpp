// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_IMPL_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_IMPL_HPP

#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/lib.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

template <typename T, SQLSMALLINT CType, SQLSMALLINT SqlType>
class binding_impl : public binding {
  T m_val;
  SQLLEN m_ind;
public:
  binding_impl(T val) : m_val(val), m_ind(sizeof(T))  {}
  SQLSMALLINT c_type() override  { return CType; }
  SQLSMALLINT sql_type() override  { return SqlType; }
  SQLPOINTER val_ptr() override  { return SQLPOINTER(&m_val); }
  SQLLEN* ind() override  { return &m_ind; }
}; // binding_impl

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_IMPL_HPP
