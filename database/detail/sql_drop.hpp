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

inline void sql_drop(DBMS sys, const table_definition& tbl, std::vector<std::string>& sql)
{
  if (SQLite == sys)
    for (auto idx(std::begin(tbl.indexes)); idx != std::end(tbl.indexes); ++idx)
      if (Spatial == idx->type)
      {
        if (1 != idx->columns.size()) throw std::runtime_error("SQL error");
        sql.push_back("SELECT DisableSpatialIndex('" + tbl.id.name + "', '" + idx->columns.front() + "')");
        sql.push_back("DROP TABLE " + sql_identifier(sys, "idx_" + tbl.id.name + "_" + idx->columns.front()));
      }

  for (auto col(std::begin(tbl.columns)); col != std::end(tbl.columns); ++col)
  {
    if (Geometry != col->type) continue;
    switch (sys)
    {
    case VoidSystem: throw std::runtime_error("SQL error");
    case MS_SQL:
    case MySQL:
    case Oracle: break;
    case DB2: sql.push_back("BEGIN ATOMIC DECLARE msg_code INTEGER; DECLARE msg_text VARCHAR(1024); call DB2GSE.ST_unregister_spatial_column('" + sql_identifier(sys, tbl.id.schema) + "', '" + sql_identifier(sys, tbl.id.name) + "', '" + sql_identifier(sys, col->name) + "', msg_code, msg_text); END"); break;
    case Postgres:
      if ("geometry" == col->lower_case_type.name)
        sql.push_back("SELECT DropGeometryColumn('" + tbl.id.schema + "', '" + tbl.id.name + "', '" + col->name + "')");
      break;
    case SQLite: sql.push_back("SELECT DiscardGeometryColumn('" + tbl.id.name + "', '" + col->name + "')"); break;
    }
  }

  sql.push_back("DROP TABLE " + sql_identifier(sys, tbl.id));
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_DROP_HPP
