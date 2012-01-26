// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_GEODETIC_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_GEODETIC_TYPE_HPP

#include <brig/database/column_detail.hpp>
#include <brig/database/dbms.hpp>
#include <brig/database/detail/is_vector_type.hpp>

namespace brig { namespace database { namespace detail {

inline bool is_geodetic_type(DBMS sys, const column_detail& col)
{
  if (is_vector_type(sys, col))
    switch (sys)
    {
    case UnknownSystem:
    case MySQL:
    case SQLite: break;
    case DB2: return 2000000000 <= col.srid && col.srid <= 2000001000;
    case MS_SQL: return col.type_name == "GEOGRAPHY";
    case Oracle: return col.type_detail == "GEOGRAPHIC2D" || col.type_detail == "GEOGRAPHIC3D";
    case Postgres: return col.type_name == "GEOGRAPHY";
    }
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_GEODETIC_TYPE_HPP
