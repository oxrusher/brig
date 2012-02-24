// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_TYPE_HPP
#define BRIG_DATABASE_DETAIL_GET_TYPE_HPP

#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/global.hpp>

namespace brig { namespace database { namespace detail {

inline column_type get_type(DBMS sys, const column_detail& col)
{
  if (is_geometry_type(sys, col)) return Geometry;
  else
    switch (sys)
    {
    case VoidSystem:
    case SQLite:
      break;

    case DB2:
      if (!col.case_folded_type.schema.empty() && "sysibm" != col.case_folded_type.schema) return VoidColumn;
      else if (col.case_folded_type.name.find("graphic") != std::string::npos) return String;
      break;

    case MS_SQL:
      if ("bit" == col.case_folded_type.name) return Integer;
      else if ("datetime" == col.case_folded_type.name
            || "datetime2" == col.case_folded_type.name
            || "smalldatetime" == col.case_folded_type.name) return DateTime;
      break;

    case MySQL:
      if ("fixed" == col.case_folded_type.name) return col.scale == 0? Integer: Double;
      else if ("datetime" == col.case_folded_type.name) return DateTime;
      break;

    case Oracle:
      if (!col.case_folded_type.schema.empty()) return VoidColumn;
      else if ("long" == col.case_folded_type.name) return String;
      break;

    case Postgres:
      if ("user-defined" == col.case_folded_type.schema) return VoidColumn;
      else if (col.case_folded_type.name.find("serial") != std::string::npos) return Integer;
      break;
    }

  if ("date" == col.case_folded_type.name) return Date;
  else if (col.case_folded_type.name.find("timestamp") == 0) return DateTime;
  else if (col.case_folded_type.name.find("int") != std::string::npos
        || col.case_folded_type.name.find("bool") == 0) return Integer;
  else if (col.case_folded_type.name.find("char") != std::string::npos
        || col.case_folded_type.name.find("clob") != std::string::npos
        || col.case_folded_type.name.find("text") != std::string::npos) return String;
  else if (col.case_folded_type.name.find("real") != std::string::npos
        || col.case_folded_type.name.find("float") != std::string::npos
        || col.case_folded_type.name.find("double") != std::string::npos) return Double;
  else if (col.case_folded_type.name.find("dec") == 0
        || col.case_folded_type.name.find("num") == 0) return col.scale == 0? Integer: Double;
  else return VoidColumn;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TYPE_HPP
