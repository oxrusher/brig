// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_GEODETIC_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_GEODETIC_TYPE_HPP

#include <boost/algorithm/string.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/global.hpp>
#include <locale>

namespace brig { namespace database { namespace detail {

inline bool is_geodetic_type(DBMS sys, const column_detail& col)
{
  using namespace boost::algorithm;
  auto loc = std::locale::classic();
  if (is_geometry_type(sys, col))
    switch (sys)
    {
    case VoidSystem:
    case MySQL:
    case SQLite: break;
    case DB2: return 2000000000 <= col.srid && col.srid <= 2000001000;
    case MS_SQL: return iequals(col.type.name, "GEOGRAPHY", loc);
    case Oracle: return iequals(col.type_detail, "GEOGRAPHIC2D", loc) || iequals(col.type_detail, "GEOGRAPHIC3D", loc);
    case Postgres: return iequals(col.type.name, "GEOGRAPHY", loc);
    }
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_GEODETIC_TYPE_HPP
