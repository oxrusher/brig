// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_GET_DATA_FACTORY_HPP
#define BRIG_DATABASE_ODBC_DETAIL_GET_DATA_FACTORY_HPP

#include <brig/database/odbc/detail/get_data.hpp>
#include <brig/database/odbc/detail/get_data_blob.hpp>
#include <brig/database/odbc/detail/get_data_impl.hpp>
#include <brig/database/odbc/detail/get_data_string.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <cstdint>
#include <stdexcept>

namespace brig { namespace database { namespace odbc { namespace detail {

inline get_data* get_data_factory(SQLSMALLINT sql_type)
{
  const SQLSMALLINT SQL_DB2_UNICODE_CHAR = -95;
  const SQLSMALLINT SQL_DB2_UNICODE_VARCHAR = -96;
  const SQLSMALLINT SQL_DB2_UNICODE_LONGVARCHAR = -97;
  const SQLSMALLINT SQL_DB2_BLOB = -98;
  const SQLSMALLINT SQL_DB2_CLOB = -99;

  switch (sql_type)
  {
  // arithmetic
  case SQL_BIT:
  case SQL_TINYINT:
  case SQL_SMALLINT: return new get_data_impl<int16_t, SQL_C_SSHORT>();
  case SQL_INTEGER: return new get_data_impl<int32_t, SQL_C_SLONG>();
  case SQL_BIGINT: return new get_data_impl<int64_t, SQL_C_SBIGINT>();
  case SQL_REAL: return new get_data_impl<float, SQL_C_FLOAT>();
  case SQL_DECIMAL:
  case SQL_DOUBLE:
  case SQL_FLOAT:
  case SQL_NUMERIC: return new get_data_impl<double, SQL_C_DOUBLE>();

  // date
  case SQL_DATE:
  case SQL_TYPE_DATE: return new get_data_impl<DATE_STRUCT, SQL_C_TYPE_DATE>();

  // timestamp
  case SQL_TIMESTAMP:
  case SQL_TYPE_TIMESTAMP: return new get_data_impl<TIMESTAMP_STRUCT, SQL_C_TYPE_TIMESTAMP>();

  // string
  case SQL_CHAR:
  case SQL_VARCHAR:
  case SQL_LONGVARCHAR:
  case SQL_WCHAR:
  case SQL_WVARCHAR:
  case SQL_WLONGVARCHAR:
  case SQL_DB2_UNICODE_CHAR:
  case SQL_DB2_UNICODE_VARCHAR:
  case SQL_DB2_UNICODE_LONGVARCHAR:
  case SQL_DB2_CLOB: return new get_data_string();

  // blob
  case SQL_BINARY:
  case SQL_VARBINARY:
  case SQL_LONGVARBINARY:
  case SQL_DB2_BLOB: return new get_data_blob();

  // SQL_UNKNOWN_TYPE
  default: throw std::runtime_error("unsupported ODBC column");
  }
} // get_data_factory

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_GET_DATA_FACTORY_HPP
