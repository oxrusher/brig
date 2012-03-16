// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_DROP_HPP
#define BRIG_DATABASE_DETAIL_SQL_DROP_HPP

#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/table_definition.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<std::string> sql_drop(DBMS sys, const table_definition& tbl)
{
  std::vector<std::string> res;

  if (SQLite == sys)
    for (auto p_idx = tbl.indexes.begin(); p_idx != tbl.indexes.end(); ++p_idx)
      if (Spatial == p_idx->type)
      {
        if (1 != p_idx->columns.size()) throw std::runtime_error("SQL error");
        res.push_back("SELECT DisableSpatialIndex('" + tbl.id.name + "', '" + p_idx->columns.front() + "')");
        res.push_back("DROP TABLE " + sql_identifier(sys, "idx_" + tbl.id.name + "_" + p_idx->columns.front()));
      }

  for (auto p_col = tbl.columns.begin(); p_col != tbl.columns.end(); ++p_col)
  {
    if (Geometry != p_col->type) continue;
    switch (sys)
    {
    case VoidSystem: throw std::runtime_error("SQL error");
    case MS_SQL:
    case MySQL:
    case Oracle: break;
    case DB2: res.push_back("BEGIN ATOMIC DECLARE msg_code INTEGER; DECLARE msg_text VARCHAR(1024); call DB2GSE.ST_unregister_spatial_column('" + sql_identifier(sys, tbl.id.schema) + "', '" + sql_identifier(sys, tbl.id.name) + "', '" + sql_identifier(sys, p_col->name) + "', msg_code, msg_text); END"); break;
    case Postgres:
      if ("user-defined" == p_col->lower_case_type.schema && "geometry" == p_col->lower_case_type.name)
        res.push_back("SELECT DropGeometryColumn('" + tbl.id.schema + "', '" + tbl.id.name + "', '" + p_col->name + "')");
      break;
    case SQLite: res.push_back("SELECT DiscardGeometryColumn('" + tbl.id.name + "', '" + p_col->name + "')"); break;
    }
  }

  res.push_back("DROP TABLE " + sql_identifier(sys, tbl.id));
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_DROP_HPP
