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
  default: break;
  case DB2: return "db2gse" == col.dbms_type_lcase.schema && is_ogc_type(col.dbms_type_lcase.name);
  case MS_SQL: return "geometry" == col.dbms_type_lcase.name || "geography" == col.dbms_type_lcase.name;
  case MySQL:
  case SQLite: return is_ogc_type(col.dbms_type_lcase.name);
  case Oracle: return "mdsys" == col.dbms_type_lcase.schema && ("sdo_geometry" == col.dbms_type_lcase.name || is_ogc_type(col.dbms_type_lcase.name));
  case Postgres:
    return "user-defined" == col.dbms_type_lcase.schema
       && ("raster" == col.dbms_type_lcase.name || "geography" == col.dbms_type_lcase.name || "geometry" == col.dbms_type_lcase.name)
       && (col.dbms_type_lcase.qualifier.empty() || is_ogc_type(col.dbms_type_lcase.qualifier));
  }
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
