// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_TYPE_HPP
#define BRIG_DATABASE_DETAIL_GET_TYPE_HPP

#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>

namespace brig { namespace database { namespace detail {

inline column_type get_type(DBMS sys, const identifier& dbms_type_lcase, int scale)
{
  if (is_geometry_type(sys, dbms_type_lcase)) return Geometry;

  if (!dbms_type_lcase.schema.empty())
    switch (sys)
    {
    default: return VoidColumn;
    case DB2: if (dbms_type_lcase.schema.compare("sysibm") != 0) return VoidColumn; break;
    case Postgres: if (dbms_type_lcase.schema.compare("user-defined") == 0) return VoidColumn; break;
    }

  switch (sys)
  {
  default: break;
  case CUBRID:
    if (dbms_type_lcase.name.compare("short") == 0) return Integer;
    else if (dbms_type_lcase.name.find("bit") != std::string::npos) return Blob;
    else if (dbms_type_lcase.name.compare("string") == 0) return String;
    break;
  case DB2:
    if (dbms_type_lcase.name.find("graphic") != std::string::npos) return String;
    break;
  case Informix:
    if (dbms_type_lcase.name.find("serial") != std::string::npos) return Integer;
    else if (dbms_type_lcase.name.compare("byte") == 0) return Blob;
    break;
  case MS_SQL:
    if (dbms_type_lcase.name.compare("bit") == 0) return Integer;
    else if (dbms_type_lcase.name.compare("image") == 0) return Blob;
    break;
  case MySQL:
    if (dbms_type_lcase.name.compare("fixed") == 0) return scale == 0? Integer: Double;
    break;
  case Oracle:
    if (dbms_type_lcase.name.compare("long") == 0) return String;
    else if (dbms_type_lcase.name.compare("binary_float") == 0
          || dbms_type_lcase.name.compare("binary_double") == 0) return Double;
    else if (dbms_type_lcase.name.compare("bfile") == 0
          || dbms_type_lcase.name.find("raw") != std::string::npos) return Blob;
    break;
  case Postgres:
    if (dbms_type_lcase.name.find("serial") != std::string::npos) return Integer;
    else if (dbms_type_lcase.name.compare("bytea") == 0) return Blob;
    else if (dbms_type_lcase.name.find("array") != std::string::npos
          && dbms_type_lcase.name.find("char") == std::string::npos
          && dbms_type_lcase.name.find("text") == std::string::npos) return VoidColumn;
    break;
  }

  if ((dbms_type_lcase.name.find("int") != std::string::npos && dbms_type_lcase.name.find("interval") == std::string::npos)
   || dbms_type_lcase.name.find("bool") == 0) return Integer;
  else if (dbms_type_lcase.name.find("date") != std::string::npos
        || dbms_type_lcase.name.find("time") != std::string::npos
        || dbms_type_lcase.name.find("char") != std::string::npos
        || dbms_type_lcase.name.find("clob") != std::string::npos
        || dbms_type_lcase.name.find("text") != std::string::npos) return String;
  else if (dbms_type_lcase.name.find("real") != std::string::npos
        || dbms_type_lcase.name.find("float") != std::string::npos
        || dbms_type_lcase.name.find("double") != std::string::npos) return Double;
  else if (dbms_type_lcase.name.find("dec") == 0
        || dbms_type_lcase.name.find("num") == 0) return scale == 0? Integer: Double;
  else if (dbms_type_lcase.name.find("binary") != std::string::npos
        || dbms_type_lcase.name.find("blob") != std::string::npos) return Blob;
  else return VoidColumn;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TYPE_HPP
