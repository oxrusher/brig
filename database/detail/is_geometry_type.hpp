// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP

#include <brig/database/column_detail.hpp>
#include <brig/database/dbms.hpp>
#include <brig/database/detail/is_ogc_type.hpp>

namespace brig { namespace database { namespace detail {

inline bool is_geometry_type(DBMS sys, const column_detail& col)
{
  switch (sys)
  {
  case UnknownSystem: break;
  case DB2: return col.type_schema == "DB2GSE" && is_ogc_type(col.type_name);
  case MS_SQL: return col.type_name == "GEOMETRY" || col.type_name == "GEOGRAPHY";
  case MySQL:
  case SQLite: return is_ogc_type(col.type_name);
  case Oracle: return col.type_schema == "MDSYS" && (col.type_name == "SDO_GEOMETRY" || is_ogc_type(col.type_name));
  case Postgres:
    return col.type_schema == "USER-DEFINED"
      && (col.type_name == "GEOMETRY" || col.type_name == "GEOGRAPHY")
      && (col.type_detail.empty() || is_ogc_type(col.type_detail));
  }
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
