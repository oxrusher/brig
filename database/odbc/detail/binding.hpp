// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_HPP

#include <boost/utility.hpp>
#include <brig/database/odbc/detail/lib.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

struct binding : ::boost::noncopyable {
  virtual ~binding()  {}
  virtual SQLSMALLINT c_type() = 0;
  virtual SQLSMALLINT sql_type() = 0;
  virtual SQLULEN column_size()
  {
    switch (c_type())
    {
    default: break;
    case SQL_C_BINARY:
    case SQL_C_WCHAR: return 1;
    }

    // If ParameterType is SQL_DECIMAL, SQL_NUMERIC, SQL_FLOAT, SQL_REAL, or SQL_DOUBLE, the SQL_DESC_PRECISION field of the IPD is set to the value of ColumnSize
    switch (sql_type())
    {
    default: return 0;
    case SQL_REAL: return 7;
    case SQL_DECIMAL:
    case SQL_NUMERIC:
    case SQL_FLOAT:
    case SQL_DOUBLE: return 15;
    }
  }
  virtual SQLPOINTER val_ptr() = 0;
  virtual SQLLEN* ind() = 0;
}; // binding

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_HPP
