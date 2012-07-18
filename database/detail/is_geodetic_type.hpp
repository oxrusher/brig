// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_GEODETIC_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_GEODETIC_TYPE_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/global.hpp>

namespace brig { namespace database { namespace detail {

inline bool is_geodetic_type(DBMS sys, const column_definition& col)
{
  if (Geometry == col.type)
    switch (sys)
    {
    default: break;
    case DB2: return 2000000000 <= col.srid && col.srid <= 2000001000;
    case MS_SQL:
    case Postgres: return col.dbms_type_lcase.name.compare("geography") == 0;
    case Oracle: return col.dbms_type_lcase.qualifier.compare("geographic2d") == 0 || col.dbms_type_lcase.qualifier.compare("geographic3d") == 0;
    }
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_GEODETIC_TYPE_HPP
