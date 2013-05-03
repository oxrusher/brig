// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_LIB_HPP
#define BRIG_DATABASE_ODBC_DETAIL_LIB_HPP

#include <brig/detail/dynamic_loading.hpp>
#include <brig/global.hpp>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

namespace brig { namespace database { namespace odbc { namespace detail {

class lib {
  lib();

public:
  decltype(SQLAllocHandle) *p_SQLAllocHandle;
  decltype(SQLBindParameter) *p_SQLBindParameter;
  decltype(SQLColAttributeW) *p_SQLColAttributeW;
  decltype(SQLDataSourcesW) *p_SQLDataSourcesW;
  decltype(SQLDisconnect) *p_SQLDisconnect;
  decltype(SQLDriverConnectW) *p_SQLDriverConnectW;
  decltype(SQLDriversW) *p_SQLDriversW;
  decltype(SQLEndTran) *p_SQLEndTran;
  decltype(SQLExecute) *p_SQLExecute;
  decltype(SQLFetch) *p_SQLFetch;
  decltype(SQLFreeHandle) *p_SQLFreeHandle;
  decltype(SQLFreeStmt) *p_SQLFreeStmt;
  decltype(SQLGetConnectAttr) *p_SQLGetConnectAttr;
  decltype(SQLGetData) *p_SQLGetData;
  decltype(SQLGetDiagRecW) *p_SQLGetDiagRecW;
  decltype(SQLGetInfoW) *p_SQLGetInfoW;
  decltype(SQLMoreResults) *p_SQLMoreResults;
  decltype(SQLNumResultCols) *p_SQLNumResultCols;
  decltype(SQLPrepareW) *p_SQLPrepareW;
  decltype(SQLSetConnectAttr) *p_SQLSetConnectAttr;
  decltype(SQLSetEnvAttr) *p_SQLSetEnvAttr;

  bool empty() const  { return p_SQLSetEnvAttr == 0; }
  static lib& singleton()  { static lib s; return s; }
}; // lib

inline lib::lib() : p_SQLSetEnvAttr(0)
{
  auto handle = BRIG_DL_LIBRARY(LibOdbcWin, LibOdbcLin);
  if (  handle
    && (p_SQLAllocHandle = BRIG_DL_FUNCTION(handle, SQLAllocHandle))
    && (p_SQLBindParameter = BRIG_DL_FUNCTION(handle, SQLBindParameter))
    && (p_SQLColAttributeW = BRIG_DL_FUNCTION(handle, SQLColAttributeW))
    && (p_SQLDataSourcesW = BRIG_DL_FUNCTION(handle, SQLDataSourcesW))
    && (p_SQLDisconnect = BRIG_DL_FUNCTION(handle, SQLDisconnect))
    && (p_SQLDriverConnectW = BRIG_DL_FUNCTION(handle, SQLDriverConnectW))
    && (p_SQLDriversW = BRIG_DL_FUNCTION(handle, SQLDriversW))
    && (p_SQLEndTran = BRIG_DL_FUNCTION(handle, SQLEndTran))
    && (p_SQLExecute = BRIG_DL_FUNCTION(handle, SQLExecute))
    && (p_SQLFetch = BRIG_DL_FUNCTION(handle, SQLFetch))
    && (p_SQLFreeHandle = BRIG_DL_FUNCTION(handle, SQLFreeHandle))
    && (p_SQLFreeStmt = BRIG_DL_FUNCTION(handle, SQLFreeStmt))
    && (p_SQLGetConnectAttr = BRIG_DL_FUNCTION(handle, SQLGetConnectAttr))
    && (p_SQLGetData = BRIG_DL_FUNCTION(handle, SQLGetData))
    && (p_SQLGetDiagRecW = BRIG_DL_FUNCTION(handle, SQLGetDiagRecW))
    && (p_SQLGetInfoW = BRIG_DL_FUNCTION(handle, SQLGetInfoW))
    && (p_SQLMoreResults = BRIG_DL_FUNCTION(handle, SQLMoreResults))
    && (p_SQLNumResultCols = BRIG_DL_FUNCTION(handle, SQLNumResultCols))
    && (p_SQLPrepareW = BRIG_DL_FUNCTION(handle, SQLPrepareW))
    && (p_SQLSetConnectAttr = BRIG_DL_FUNCTION(handle, SQLSetConnectAttr))
     )  p_SQLSetEnvAttr = BRIG_DL_FUNCTION(handle, SQLSetEnvAttr);
} // lib::

enum ext {
  SQL_DB2_UNICODE_CHAR = -95,
  SQL_DB2_UNICODE_VARCHAR = -96,
  SQL_DB2_UNICODE_LONGVARCHAR = -97,
  SQL_DB2_BLOB = -98,
  SQL_DB2_CLOB = -99,
  SQL_INFX_UDT_VARYING = -101,
  SQL_INFX_UDT_LVARCHAR = -104,
  SQL_INFX_BIGINT = -114,
  SQL_INFX_ATTR_DELIMIDENT = 2273,
}; // ext

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_LIB_HPP
