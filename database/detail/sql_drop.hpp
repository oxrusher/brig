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
        sql.push_back("SELECT DisableSpatialIndex('" + tbl.name + "', '" + idx->columns.front() + "')");
        sql.push_back("DROP TABLE " + sql_identifier(sys, "idx_" + tbl.name + "_" + idx->columns.front()));
      }

  for (auto col(std::begin(tbl.columns)); col != std::end(tbl.columns); ++col)
  {
    if (Geometry != col->type) continue;
    switch (sys)
    {
    default: break;
    case DB2: sql.push_back("BEGIN ATOMIC DECLARE msg_code INTEGER; DECLARE msg_text VARCHAR(1024); call DB2GSE.ST_unregister_spatial_column('" + sql_identifier(sys, tbl.schema) + "', '" + sql_identifier(sys, tbl.name) + "', '" + sql_identifier(sys, col->name) + "', msg_code, msg_text); END"); break;
    case Postgres: if ("geometry" == col->dbms_type_lcase.name) sql.push_back("SELECT DropGeometryColumn('" + tbl.schema + "', '" + tbl.name + "', '" + col->name + "')"); break;
    case SQLite: sql.push_back("SELECT DiscardGeometryColumn('" + tbl.name + "', '" + col->name + "')"); break;
    }
  }

  sql.push_back("DROP TABLE " + sql_identifier(sys, tbl));
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_DROP_HPP
