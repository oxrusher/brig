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
  else
    switch (sys)
    {
    case VoidSystem:
    case SQLite:
      break;

    case DB2:
      if (!col.lower_case_type.schema.empty() && "sysibm" != col.lower_case_type.schema) return VoidColumn;
      else if (col.lower_case_type.name.find("graphic") != std::string::npos) return String;
      break;

    case MS_SQL:
      if ("bit" == col.lower_case_type.name) return Integer;
      else if ("datetime" == col.lower_case_type.name
            || "datetime2" == col.lower_case_type.name
            || "smalldatetime" == col.lower_case_type.name) return DateTime;
      else if ("image" == col.lower_case_type.name) return Blob;
      break;

    case MySQL:
      if ("fixed" == col.lower_case_type.name) return col.scale == 0? Integer: Double;
      else if ("datetime" == col.lower_case_type.name) return DateTime;
      break;

    case Oracle:
      if (!col.lower_case_type.schema.empty()) return VoidColumn;
      else if ("long" == col.lower_case_type.name) return String;
      else if ("binary_float" == col.lower_case_type.name
            || "binary_double" == col.lower_case_type.name) return Double;
      else if ("bfile" == col.lower_case_type.name
            || col.lower_case_type.name.find("raw") != std::string::npos) return Blob;
      break;

    case Postgres:
      if ("user-defined" == col.lower_case_type.schema) return VoidColumn;
      else if (col.lower_case_type.name.find("serial") != std::string::npos) return Integer;
      else if ("bytea" == col.lower_case_type.name) return Blob;
      break;
    }

  if ("date" == col.lower_case_type.name) return Date;
  else if (col.lower_case_type.name.find("timestamp") == 0) return DateTime;
  else if (col.lower_case_type.name.find("int") != std::string::npos
        || col.lower_case_type.name.find("bool") == 0) return Integer;
  else if (col.lower_case_type.name.find("char") != std::string::npos
        || col.lower_case_type.name.find("clob") != std::string::npos
        || col.lower_case_type.name.find("text") != std::string::npos) return String;
  else if (col.lower_case_type.name.find("real") != std::string::npos
        || col.lower_case_type.name.find("float") != std::string::npos
        || col.lower_case_type.name.find("double") != std::string::npos) return Double;
  else if (col.lower_case_type.name.find("dec") == 0
        || col.lower_case_type.name.find("num") == 0) return col.scale == 0? Integer: Double;
  else if (col.lower_case_type.name.find("binary") != std::string::npos
        || col.lower_case_type.name.find("blob") != std::string::npos) return Blob;
  else return VoidColumn;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TYPE_HPP
