// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_DROP_HPP
#define BRIG_DATABASE_DETAIL_SQL_DROP_HPP

#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/table_definition.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_drop(DBMS sys, const table_definition& tbl, std::vector<std::string>& sql)
{
  if (SQLite == sys)
    for (auto idx(std::begin(tbl.indexes)); idx != std::end(tbl.indexes); ++idx)
      if (Spatial == idx->type)
      {
        sql.push_back("SELECT DisableSpatialIndex('" + tbl.id.name + "', '" + idx->columns.front() + "')");
        sql.push_back("DROP TABLE " + sql_identifier(sys, "idx_" + tbl.id.name + "_" + idx->columns.front()));
      }

  for (auto col(std::begin(tbl.columns)); col != std::end(tbl.columns); ++col)
  {
    if (Geometry != col->type) continue;
    switch (sys)
    {
    default: break;
    case DB2: sql.push_back("BEGIN ATOMIC DECLARE msg_code INTEGER; DECLARE msg_text VARCHAR(1024); call DB2GSE.ST_unregister_spatial_column('" + sql_identifier(sys, tbl.id.schema) + "', '" + sql_identifier(sys, tbl.id.name) + "', '" + sql_identifier(sys, col->name) + "', msg_code, msg_text); END"); break;
    case Informix: sql.push_back("DELETE FROM sde.geometry_columns WHERE RTRIM(f_table_catalog) = (SELECT RTRIM(ODB_DBName) DB FROM sysmaster:SysOpenDB WHERE CAST(ODB_SessionID AS INT) = CAST(DBINFO('sessionid') AS INT) AND ODB_IsCurrent = 'Y') AND RTRIM(f_table_schema) = '" + tbl.id.schema + "' AND f_table_name = '" + tbl.id.name + "'"); break;
    case Postgres: if (col->dbms_type_lcase.name.compare("geometry") == 0) sql.push_back("SELECT DropGeometryColumn('" + tbl.id.schema + "', '" + tbl.id.name + "', '" + col->name + "')"); break;
    case SQLite: sql.push_back("SELECT DiscardGeometryColumn('" + tbl.id.name + "', '" + col->name + "')"); break;
    }
  }

  sql.push_back("DROP TABLE " + sql_identifier(sys, tbl.id));
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_DROP_HPP
