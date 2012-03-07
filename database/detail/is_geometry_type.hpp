// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/database/global.hpp>

namespace brig { namespace database { namespace detail {

inline bool is_geometry_type(DBMS sys, const column_definition& col)
{
  switch (sys)
  {
  case VoidSystem: break;
  case DB2: return "db2gse" == col.lower_case_type.schema && is_ogc_type(col.lower_case_type.name);
  case MS_SQL: return "geometry" == col.lower_case_type.name || "geography" == col.lower_case_type.name;
  case MySQL:
  case SQLite: return is_ogc_type(col.lower_case_type.name);
  case Oracle: return "mdsys" == col.lower_case_type.schema && ("sdo_geometry" == col.lower_case_type.name || is_ogc_type(col.lower_case_type.name));
  case Postgres:
    return "user-defined" == col.lower_case_type.schema
       && ("raster" == col.lower_case_type.name || "geography" == col.lower_case_type.name || "geometry" == col.lower_case_type.name)
       && (col.lower_case_type.qualifier.empty() || is_ogc_type(col.lower_case_type.qualifier));
  }
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
