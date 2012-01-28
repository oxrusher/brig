// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_NULL_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_NULL_HPP

#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/database/type_identifier.hpp>
#include <stdexcept>

namespace brig { namespace database { namespace odbc { namespace detail {

class binding_null : public binding {
  TypeIdentifier m_type;
  SQLLEN m_ind;
public:
  binding_null(TypeIdentifier type) : m_type(type), m_ind(SQL_NULL_DATA)  {}
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
    case UnknownType: break;
    case Date: return SQL_C_TYPE_DATE;
    case DateTime: return SQL_C_TYPE_TIMESTAMP;
    case Double: return SQL_C_DOUBLE;
    case Geometry: return SQL_C_BINARY;
    case Integer: return SQL_C_SBIGINT;
    case String: return SQL_C_WCHAR;
  };
  throw std::runtime_error("unsupported ODBC parameter");
}

inline SQLSMALLINT binding_null::sql_type()
{
  switch (m_type)
  {
    case UnknownType: break;
    case Date: return SQL_DATE;
    case DateTime: return SQL_TIMESTAMP;
    case Double: return SQL_DOUBLE;
    case Geometry: return SQL_VARBINARY;
    case Integer: return SQL_BIGINT;
    case String: return SQL_WVARCHAR;
  };
  throw std::runtime_error("unsupported ODBC parameter");
} // binding_null::

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_NULL_HPP
