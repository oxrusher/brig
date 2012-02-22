// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_DROP_HPP
#define BRIG_DATABASE_DETAIL_SQL_DROP_HPP

#include <boost/algorithm/string.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_object.hpp>
#include <brig/database/global.hpp>
#include <brig/database/table_detail.hpp>
#include <locale>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_drop(DBMS sys, const table_detail<column_detail>& tbl, std::vector<std::string>& sqls)
{
  using namespace ::boost::algorithm;
  auto loc = std::locale::classic();

  if (SQLite == sys)
    for (auto p_idx = tbl.indexes.begin(); p_idx != tbl.indexes.end(); ++p_idx)
      if (Spatial == p_idx->type)
      {
        if (1 != p_idx->columns.size()) throw std::runtime_error("SQL error");
        sqls.push_back("SELECT DisableSpatialIndex('" + tbl.table.name + "', '" + p_idx->columns.front() + "')");
        sqls.push_back("DROP TABLE " + sql_identifier(sys, "idx_" + tbl.table.name + "_" + p_idx->columns.front()));
      }

  for (auto p_col = tbl.columns.begin(); p_col != tbl.columns.end(); ++p_col)
  {
    if (!is_geometry_type(sys, *p_col)) continue;
    switch (sys)
    {
    case VoidSystem: throw std::runtime_error("SQL error");
    case MS_SQL:
    case MySQL:
    case Oracle: break;
    case DB2: sqls.push_back("BEGIN ATOMIC DECLARE msg_code INTEGER; DECLARE msg_text VARCHAR(1024); call DB2GSE.ST_unregister_spatial_column('" + sql_identifier(sys, tbl.table.schema) + "', '" + sql_identifier(sys, tbl.table.name) + "', '" + sql_identifier(sys, p_col->name) + "', msg_code, msg_text); END"); break;
    case Postgres:
      if (iequals(p_col->type.schema, "USER-DEFINED", loc) && iequals(p_col->type.name, "GEOMETRY", loc))
        sqls.push_back("SELECT DropGeometryColumn('" + tbl.table.schema + "', '" + tbl.table.name + "', '" + p_col->name + "')");
      break;
    case SQLite: sqls.push_back("SELECT DiscardGeometryColumn('" + tbl.table.name + "', '" + p_col->name + "')"); break;
    }
  }

  sqls.push_back("DROP TABLE " + sql_object(sys, tbl.table));
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_DROP_HPP
