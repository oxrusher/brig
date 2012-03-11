// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_NULL_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_NULL_HPP

#include <brig/database/global.hpp>
#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/get_sql_type.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <stdexcept>

namespace brig { namespace database { namespace odbc { namespace detail {

class binding_null : public binding {
  column_type m_type;
  SQLLEN m_ind;
  DBMS m_sys;
public:
  binding_null(column_type type, DBMS sys) : m_type(type), m_ind(SQL_NULL_DATA), m_sys(sys)  {}
  virtual SQLSMALLINT c_type();
  virtual SQLSMALLINT sql_type();
  virtual SQLULEN precision()  { return 0; }
  virtual SQLPOINTER val_ptr()  { return 0; }
  virtual SQLLEN* ind()  { return &m_ind; }
}; // binding_null

inline SQLSMALLINT binding_null::c_type()
{
  switch (m_type)
  {
    default: throw std::runtime_error("unsupported ODBC parameter");
    case Blob: return SQL_C_BINARY;
    case Double: return SQL_C_DOUBLE;
    case Geometry: return SQL_C_BINARY;
    case Integer: return SQL_C_SBIGINT;
    case String: return SQL_C_WCHAR;
  };
}

inline SQLSMALLINT binding_null::sql_type()
{
  switch (m_type)
  {
    default: throw std::runtime_error("unsupported ODBC parameter");
    case Blob: return get_sql_type_blob(m_sys);
    case Double: return SQL_DOUBLE;
    case Geometry: return SQL_VARBINARY;
    case Integer: return SQL_BIGINT;
    case String: return get_sql_type_string(m_sys);
  };
} // binding_null::

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_NULL_HPP
