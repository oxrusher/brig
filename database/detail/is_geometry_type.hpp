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
  case DB2: return col.dbms_type_lcase.schema.compare("db2gse") == 0 && is_ogc_type(col.dbms_type_lcase.name);
  case Informix:
  case MySQL:
  case SQLite: return is_ogc_type(col.dbms_type_lcase.name);
  case MS_SQL: return col.dbms_type_lcase.name.compare("geometry") == 0 || col.dbms_type_lcase.name.compare("geography") == 0;
  case Oracle: return col.dbms_type_lcase.schema.compare("mdsys") == 0 && (col.dbms_type_lcase.name.compare("sdo_geometry") == 0 || is_ogc_type(col.dbms_type_lcase.name));
  case Postgres:
    return col.dbms_type_lcase.schema.compare("user-defined") == 0
       && (col.dbms_type_lcase.name.compare("raster") == 0 || col.dbms_type_lcase.name.compare("geography") == 0 || col.dbms_type_lcase.name.compare("geometry") == 0)
       && (col.dbms_type_lcase.qualifier.empty() || is_ogc_type(col.dbms_type_lcase.qualifier));
  }
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
