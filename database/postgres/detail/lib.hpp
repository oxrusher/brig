// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_LIB_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_LIB_HPP

#include <brig/detail/dynamic_loading.hpp>
#include <libpq-fe.h>

namespace brig { namespace database { namespace postgres { namespace detail {

class lib {
  lib();

public:
  decltype(PQclear) *p_PQclear;
  decltype(PQerrorMessage) *p_PQerrorMessage;
  decltype(PQexec) *p_PQexec;
  decltype(PQexecParams) *p_PQexecParams;
  decltype(PQfinish) *p_PQfinish;
  decltype(PQfname) *p_PQfname;
  decltype(PQftype) *p_PQftype;
  decltype(PQgetisnull) *p_PQgetisnull;
  decltype(PQgetlength) *p_PQgetlength;
  decltype(PQgetvalue) *p_PQgetvalue;
  decltype(PQlibVersion) *p_PQlibVersion;
  decltype(PQnfields) *p_PQnfields;
  decltype(PQntuples) *p_PQntuples;
  decltype(PQresultStatus) *p_PQresultStatus;
  decltype(PQsetClientEncoding) *p_PQsetClientEncoding;
  decltype(PQsetdbLogin) *p_PQsetdbLogin;
  decltype(PQstatus) *p_PQstatus;

  bool empty() const  { return p_PQstatus == 0; }
  static lib& singleton()  { static lib s; return s; }
}; // lib

inline lib::lib() : p_PQstatus(0)
{
  auto handle = BRIG_DL_LIBRARY("libpq.dll", "libpq.so");
  if (  handle
    && (p_PQclear = BRIG_DL_FUNCTION(handle, PQclear))
    && (p_PQerrorMessage = BRIG_DL_FUNCTION(handle, PQerrorMessage))
    && (p_PQexec = BRIG_DL_FUNCTION(handle, PQexec))
    && (p_PQexecParams = BRIG_DL_FUNCTION(handle, PQexecParams))
    && (p_PQfinish = BRIG_DL_FUNCTION(handle, PQfinish))
    && (p_PQfname = BRIG_DL_FUNCTION(handle, PQfname))
    && (p_PQftype = BRIG_DL_FUNCTION(handle, PQftype))
    && (p_PQgetisnull = BRIG_DL_FUNCTION(handle, PQgetisnull))
    && (p_PQgetlength = BRIG_DL_FUNCTION(handle, PQgetlength))
    && (p_PQgetvalue = BRIG_DL_FUNCTION(handle, PQgetvalue))
    && (p_PQnfields = BRIG_DL_FUNCTION(handle, PQnfields))
    && (p_PQlibVersion = BRIG_DL_FUNCTION(handle, PQlibVersion))
    && (p_PQntuples = BRIG_DL_FUNCTION(handle, PQntuples))
    && (p_PQresultStatus = BRIG_DL_FUNCTION(handle, PQresultStatus))
    && (p_PQsetClientEncoding = BRIG_DL_FUNCTION(handle, PQsetClientEncoding))
    && (p_PQsetdbLogin = BRIG_DL_FUNCTION(handle, PQsetdbLogin))
     )  p_PQstatus = BRIG_DL_FUNCTION(handle, PQstatus);
} // lib::

enum pg_type {
  PG_TYPE_BOOL = 16,

  PG_TYPE_INT2 = 21,
  PG_TYPE_INT4 = 23,
  PG_TYPE_INT8 = 20,
  PG_TYPE_FLOAT4 = 700,
  PG_TYPE_FLOAT8 = 701,

  PG_TYPE_BPCHAR = 1042,
  PG_TYPE_BPCHARARRAY = 1014,
  PG_TYPE_NAME = 19,
  PG_TYPE_TEXT = 25,
  PG_TYPE_TEXTARRAY = 1009,
  PG_TYPE_VARCHAR = 1043,
  PG_TYPE_VARCHARARRAY = 1015,

  PG_TYPE_BYTEA = 17,
}; // pg_type

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_LIB_HPP
