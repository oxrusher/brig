// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP

#include <boost/algorithm/string.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/database/global.hpp>
#include <locale>

namespace brig { namespace database { namespace detail {

inline bool is_geometry_type(DBMS sys, const column_detail& col)
{
  using namespace boost::algorithm;
  auto loc = std::locale::classic();
  switch (sys)
  {
  case UnknownSystem: break;
  case DB2: return iequals(col.type.schema, "DB2GSE", loc) && is_ogc_type(col.type.name);
  case MS_SQL: return iequals(col.type.name, "GEOMETRY", loc) || iequals(col.type.name, "GEOGRAPHY", loc);
  case MySQL:
  case SQLite: return is_ogc_type(col.type.name);
  case Oracle: return iequals(col.type.schema, "MDSYS", loc) && (iequals(col.type.name, "SDO_GEOMETRY", loc) || is_ogc_type(col.type.name));
  case Postgres:
    return iequals(col.type.schema, "USER-DEFINED", loc)
      && (iequals(col.type.name, "GEOMETRY", loc) || iequals(col.type.name, "GEOGRAPHY", loc))
      && (col.type_detail.empty() || is_ogc_type(col.type_detail));
  }
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
