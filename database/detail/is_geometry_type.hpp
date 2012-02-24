// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP

#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/database/global.hpp>

namespace brig { namespace database { namespace detail {

inline bool is_geometry_type(DBMS sys, const column_detail& col)
{
  switch (sys)
  {
  case VoidSystem: break;
  case DB2: return "db2gse" == col.case_folded_type.schema && is_ogc_type(col.case_folded_type.name);
  case MS_SQL: return "geometry" == col.case_folded_type.name || "geography" == col.case_folded_type.name;
  case MySQL:
  case SQLite: return is_ogc_type(col.case_folded_type.name);
  case Oracle: return "mdsys" == col.case_folded_type.schema && ("sdo_geometry" == col.case_folded_type.name || is_ogc_type(col.case_folded_type.name));
  case Postgres:
    return "user-defined" == col.case_folded_type.schema
      && ("geometry" == col.case_folded_type.name || "geography" == col.case_folded_type.name)
      && (col.case_folded_type_detail.empty() || is_ogc_type(col.case_folded_type_detail));
  }
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_GEOMETRY_TYPE_HPP
