// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP

#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>

namespace brig { namespace database { namespace detail {

inline bool is_geometry_type(DBMS sys, const identifier& dbms_type_lcase)
{
  switch (sys)
  {
  default: break;
  case DB2: return dbms_type_lcase.schema.compare("db2gse") == 0 && is_ogc_type(dbms_type_lcase.name);
  case Informix:
  case MySQL:
  case SQLite: return is_ogc_type(dbms_type_lcase.name);
  case MS_SQL: return dbms_type_lcase.name.compare("geometry") == 0 || dbms_type_lcase.name.compare("geography") == 0;
  case Oracle: return dbms_type_lcase.schema.compare("mdsys") == 0 && (dbms_type_lcase.name.compare("sdo_geometry") == 0 || is_ogc_type(dbms_type_lcase.name));
  case Postgres:
    return dbms_type_lcase.schema.compare("user-defined") == 0
       && (dbms_type_lcase.name.compare("raster") == 0 || dbms_type_lcase.name.compare("geography") == 0 || dbms_type_lcase.name.compare("geometry") == 0)
       && (dbms_type_lcase.qualifier.empty() || is_ogc_type(dbms_type_lcase.qualifier));
  }
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
