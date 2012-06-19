// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_TYPE_HPP
#define BRIG_DATABASE_DETAIL_GET_TYPE_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/global.hpp>

namespace brig { namespace database { namespace detail {

inline column_type get_type(DBMS sys, const column_definition& col)
{
  if (is_geometry_type(sys, col)) return Geometry;

  if (!col.dbms_type_lcase.schema.empty())
    switch (sys)
    {
    default: return VoidColumn;
    case DB2: if ("sysibm" != col.dbms_type_lcase.schema) return VoidColumn; break;
    case Postgres: if ("user-defined" == col.dbms_type_lcase.schema) return VoidColumn; break;
    }

  switch (sys)
  {
  default: break;
  case CUBRID:
    if ("short" == col.dbms_type_lcase.name) return Integer;
    if (col.dbms_type_lcase.name.find("bit") != std::string::npos) return Blob;
    if ("string" == col.dbms_type_lcase.name) return String;
    break;
  case DB2:
    if (col.dbms_type_lcase.name.find("graphic") != std::string::npos) return String;
    break;
  case MS_SQL:
    if ("bit" == col.dbms_type_lcase.name) return Integer;
    else if ("image" == col.dbms_type_lcase.name) return Blob;
    break;
  case MySQL:
    if ("fixed" == col.dbms_type_lcase.name) return col.scale == 0? Integer: Double;
    break;
  case Oracle:
    if ("long" == col.dbms_type_lcase.name) return String;
    else if ("binary_float" == col.dbms_type_lcase.name
          || "binary_double" == col.dbms_type_lcase.name) return Double;
    else if ("bfile" == col.dbms_type_lcase.name
          || col.dbms_type_lcase.name.find("raw") != std::string::npos) return Blob;
    break;
  case Postgres:
    if (col.dbms_type_lcase.name.find("serial") != std::string::npos) return Integer;
    else if ("bytea" == col.dbms_type_lcase.name) return Blob;
    break;
  }

  if ((col.dbms_type_lcase.name.find("int") != std::string::npos && col.dbms_type_lcase.name.find("interval") == std::string::npos)
   || col.dbms_type_lcase.name.find("bool") == 0) return Integer;
  else if (col.dbms_type_lcase.name.find("date") != std::string::npos
        || col.dbms_type_lcase.name.find("time") != std::string::npos
        || col.dbms_type_lcase.name.find("char") != std::string::npos
        || col.dbms_type_lcase.name.find("clob") != std::string::npos
        || col.dbms_type_lcase.name.find("text") != std::string::npos) return String;
  else if (col.dbms_type_lcase.name.find("real") != std::string::npos
        || col.dbms_type_lcase.name.find("float") != std::string::npos
        || col.dbms_type_lcase.name.find("double") != std::string::npos) return Double;
  else if (col.dbms_type_lcase.name.find("dec") == 0
        || col.dbms_type_lcase.name.find("num") == 0) return col.scale == 0? Integer: Double;
  else if (col.dbms_type_lcase.name.find("binary") != std::string::npos
        || col.dbms_type_lcase.name.find("blob") != std::string::npos) return Blob;
  else return VoidColumn;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TYPE_HPP
